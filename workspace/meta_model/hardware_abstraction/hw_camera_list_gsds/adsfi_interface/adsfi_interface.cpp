#include "adsfi_interface.h"
#include <sys/syscall.h>
#include <unistd.h>
#include "custom_stack.h"
#include "log/avos_node.h"

mdc::visual::Times GetCurrentTime()
{
    const auto now = std::chrono::high_resolution_clock::now().time_since_epoch();
    uint32_t sec = std::chrono::duration_cast<std::chrono::seconds>(now).count();
    uint32_t nsec = std::chrono::duration_cast<std::chrono::nanoseconds>(now).count() % 1000000000UL;
    return mdc::visual::Times { sec, nsec };
}

// ara::adsfi::MsgHafTime -> double
inline double MsgHafTimeToSeconds(const ara::adsfi::MsgHafTime& time) {
    return static_cast<double>(time.sec) + static_cast<double>(time.nsec) * 1e-9;
}

// double -> ara::adsfi::MsgHafTime (简化版)
inline ara::adsfi::MsgHafTime SecondsToMsgHafTime(double seconds) {
    ara::adsfi::MsgHafTime result;
    result.sec = static_cast<::uint32_t>(seconds);
    // +0.5 实现四舍五入，避免 0.999999999 变成 999999998
    result.nsec = static_cast<::uint32_t>((seconds - result.sec) * 1e9 + 0.5);
    return result;
}

void AdsfiInterface::Init()  {
    SetScheduleInfo("message");
	auto ptr = CustomStack::Instance();

    const char *CM_CONFIG_FILE_PATH = std::getenv("CM_CONFIG_FILE_PATH");
    printf("CM_CONFIG_FILE_PATH = %s\n", CM_CONFIG_FILE_PATH);
    ptr->GetConfig("HwCameraList","hw_fps", hw_fps);
    ptr->GetConfig("HwCameraList","format", format);
    ptr->GetConfig("HwCameraList","channel_id", channelId);
    ptr->GetConfig("HwCameraList","width", width);
    ptr->GetConfig("HwCameraList","height", height);
    ptr->GetConfig("HwCameraList","camera_name", camera_name);
    ptr->GetConfig("HwCameraList","debug", debug);
    ptr->GetConfig("HwCameraList","n_out_1", n_out_1);

    ApInfo("init") << "================== n_out_1 is " << n_out_1 << " ==================";
    ApInfo("init") << "================== debug is " << debug << " ==================";
    ApInfo("init") << "================== format is " << format << " ==================";
    ApInfo("init") << "================== hw_fps is " << hw_fps << " ==================";
    ApInfo("init") << "[HZ] init START";
    ApInfo("init") << "channelId str is " << channelId;
    ApInfo("init") << "width str is " << width;
    ApInfo("init") << "height str is " << height;
    ApInfo("init") << "camera_name str is " << camera_name;

    std::vector<std::string> channelId_str_v = split(channelId, ',');
    std::vector<int>         channelId_int_v;
    for (const auto &token : channelId_str_v) {
        channelId_int_v.push_back(atoi(token.c_str()));
    }

    std::vector<std::string> camera_name_str_v = split(camera_name, ',');

    std::vector<std::string> width_str_v = split(width, ',');
    std::vector<int>         width_int_v;
    for (const auto &token : width_str_v) {
        width_int_v.push_back(atoi(token.c_str()));
    }

    std::vector<std::string> height_str_v = split(height, ',');
    std::vector<int>         height_int_v;
    for (const auto &token : height_str_v) {
        height_int_v.push_back(atoi(token.c_str()));
    }
    camera_data_ptr = std::make_shared<avos::idpware::SCameraDataList>();
    compressed_camera_data_ptr = std::make_shared<avos::idpware::SCameraDataList>();
    mdc_camera_interface_ptr = std::make_shared<MdcCameraInterface>();

    // std::make_shared<MdcCameraInterface>("/opt/usr/zxz/aarch64-linux-gnu/idp/libcamera_recv_v8.so");
    std::string project_config_path = ptr->GetNodeConfigPath();
    std::string so_name = "";
    int         main_idx = 0;
    ptr->GetConfig( "HwCameraList","main_idx", main_idx);
    ptr->GetConfig( "HwCameraList","nv_cam_pos", mdc_camera_interface_ptr->nv_cam_pos);
    ptr->GetConfig( "HwCameraList","node_name", mdc_camera_interface_ptr->node_name);
    ptr->GetConfig( "HwCameraList","so_name", so_name);
    if (-1 == mdc_camera_interface_ptr->nv_cam_pos) {
        mdc_camera_interface_ptr->enable_nv_cam = false;
    }

    ApInfo("init") << "================== main_idx is " << main_idx << "==================";
    ApInfo("init") << "================== nv_cam_pos is " << mdc_camera_interface_ptr->nv_cam_pos << "==================";
    ApInfo("init") << "================== enable_nv_cam is " << mdc_camera_interface_ptr->enable_nv_cam << "==================";
    ApInfo("init") << "================== node_name is " << mdc_camera_interface_ptr->node_name << "==================";
    ApInfo("init") << "================== so_name is " << so_name << "==================";
    mdc_camera_interface_ptr->so_path = project_config_path + "/config/" + so_name;
    // printf("================== so_path is %s  ==================\n", mdc_camera_interface_ptr->so_path.c_str());
    ApInfo("init") << "=========================================== so_path: " << mdc_camera_interface_ptr->so_path
            << " ===========================================";

    for (int i = 0; i < channelId_int_v.size(); i++) {
        if (mdc_camera_interface_ptr->channel_en_.size() > channelId_int_v[i]) {
            mdc_camera_interface_ptr->channel_en_[channelId_int_v[i]] = '1';
            mdc_camera_interface_ptr->channel_en_mask_ =
                (mdc_camera_interface_ptr->channel_en_mask_ | (1 << channelId_int_v[i]));


            std::string frameId = "mdc_camera_instance_" + std::to_string(70+channelId_int_v[i]);
            viz_h265_images[channelId_int_v[i]].header.frameId = frameId;
            viz_h265_images_pub[channelId_int_v[i]] = mdc::visual::Publisher::Advertise<mdc::visual::Image>(frameId);
            ApInfo("init") << "frameId = " << frameId;
        }
    }
    ApInfo("init") << "================== channel_en_ is " << mdc_camera_interface_ptr->channel_en_\
                 <<", str_len is " << mdc_camera_interface_ptr->channel_en_.size() << ", channel_en_mask_ is: "\
                 << mdc_camera_interface_ptr->channel_en_mask_ << " ==================";
    CameraShmParam::SetCameraModuleParam(mdc_camera_interface_ptr->node_name, main_idx,
                                            mdc_camera_interface_ptr->nv_cam_pos,
                                            mdc_camera_interface_ptr->channel_en_);

    mdc_camera_interface_ptr->Init(channelId_int_v, camera_name_str_v, width_int_v, height_int_v, n_out_1,
                                    main_idx);
    mdc_camera_interface_ptr->debug_ = debug;
    if (hw_fps > 0) {
        mdc_camera_interface_ptr->interval_s_ = 1.0 / hw_fps;
    }

    std::thread([&]() {
        int             main_idx_read = -1;
        int             nv_cam_pos_read = -1;
        std::string     channel_en_read = "none";
        static uint64_t get_para_cnt = 1;
        while (true) {
            usleep(1000 * 1500); // sleep 1s
            std::unique_lock<std::mutex> locker(mdc_camera_interface_ptr->ioctl_mutex_);
            CameraShmParam::GetCameraModuleParam(mdc_camera_interface_ptr->node_name, main_idx_read,
                                                    nv_cam_pos_read, channel_en_read);
            if (mdc_camera_interface_ptr->enable_nv_cam) {
                if ((nv_cam_pos_read != mdc_camera_interface_ptr->nv_cam_pos)&&(-1 != nv_cam_pos_read)) {
                    mdc_camera_interface_ptr->nv_cam_pos = nv_cam_pos_read;
                    mdc_camera_interface_ptr->mdc_camera_ptr->ioctl(avos::idpware::IOCTL_SET_TOGGLE_LLC_MASTER,
                                                                    &(mdc_camera_interface_ptr->nv_cam_pos));

                    printf("get nv_cam_pos_read val is: %d, exec fucn IOCTL_SET_TOGGLE_LLC_MASTER\n",
                            nv_cam_pos_read);
                }
            }
            if ((main_idx_read != mdc_camera_interface_ptr->main_idx_)&&(-1 != main_idx_read)) {
                if((0<=main_idx_read)&&(main_idx_read<=15))
                {
                    mdc_camera_interface_ptr->main_idx_ = main_idx_read;
                    mdc_camera_interface_ptr->mdc_camera_ptr->ioctl(avos::idpware::IOCTL_SET_KEYCHANNELID_PARAM,
                            &(main_idx_read));

                    printf("get main_idx_read val is: %d, exec fucn IOCTL_SET_KEYCHANNELID_PARAM\n", main_idx_read);
                }
                else
                {
                    printf("get main_idx_read val is: %d, which is invalid[0,15]", main_idx_read);
                }

            }

            if ((channel_en_read != mdc_camera_interface_ptr->channel_en_)&&("none" != channel_en_read)) {
                std::string para = mdc_camera_interface_ptr->node_name + "_channel_en";
                printf("================== get para %s, value is [%s], str_len is: %d ==================\n",
                        para.c_str(), channel_en_read.c_str(), channel_en_read.size());
                int  channel_en_mask_read = 0;
                int  old_bit_int;
                char new_bit_char;
                for (int i = 0; i < MAX_CAM_NUM; i++) {
                    if (i < channel_en_read.size()) {
                        new_bit_char = channel_en_read[i];
                        if (new_bit_char == '1') {
                            printf("\tchannel_%d enabled\n", i);
                            channel_en_mask_read = (channel_en_mask_read | (1 << i));
                        } else {
                            printf("\tchannel_%d disabled\n", i);
                        }
                    } else {
                        old_bit_int = (mdc_camera_interface_ptr->channel_en_mask_ & (1 << i));
                        if (old_bit_int) {
                            printf("\tchannel_%d enabled\n", i);
                            channel_en_mask_read = (channel_en_mask_read | (1 << i));
                        } else {
                            printf("\tchannel_%d disabled\n", i);
                        }
                    }
                }
                mdc_camera_interface_ptr->channel_en_mask_ = channel_en_mask_read;
                mdc_camera_interface_ptr->channel_en_ = channel_en_read;

                mdc_camera_interface_ptr->mdc_camera_ptr->ioctl(avos::idpware::IOCTL_SET_CAMERA_ENABLE,
                                                                &(mdc_camera_interface_ptr->channel_en_mask_));
            }
            if (0 == (get_para_cnt % 30)) {
                printf("==================get para channel_en_mask_ is %d ==================\n",
                        mdc_camera_interface_ptr->channel_en_mask_);
                for (int i = 0; i < MAX_CAM_NUM; i++) {
                    int is_enable = (mdc_camera_interface_ptr->channel_en_mask_ & (1 << i));
                    if (is_enable) {
                        printf("\tchannel_%d enabled\n", i);
                    } else {
                        printf("\tchannel_%d disabled\n", i);
                    }
                }
            }
            get_para_cnt++;
        }
    }).detach();

    for (int i = 0; i < channelId_int_v.size(); i++) {
        camera_data_ptr->initBit(channelId_int_v[i]);
    }
    flag_ptr = std::make_shared<int>();
    ApInfo("init") << "[HZ] init ALL DONE ";

}

void AdsfiInterface::DeInit()  {
	ApInfo("init") << "deinit";
}


int AdsfiInterface::Process(const std::string &topic, std::shared_ptr<ara::adsfi::MsgImageDataList> &ptr) {
    if("image_list" == topic)
    {
        mdc_camera_interface_ptr->GetShmImage(camera_data_ptr);
        for (int i = 0; i < mdc_camera_interface_ptr->data_idx_.size(); i++) {
            if (debug & 1) {
                printf("channel_%d's original image, ts: %.3f, size: %d\n", mdc_camera_interface_ptr->data_idx_[i],
                       camera_data_ptr->m_camera_data_list[mdc_camera_interface_ptr->data_idx_[i]].m_fsyncTimeStamp,
                       camera_data_ptr->m_camera_data_list[mdc_camera_interface_ptr->data_idx_[i]].m_ptr_len);
            }
            if (debug & 2) {
                printf("channel_%d's original image, ts: %.3f, size: %d\n", mdc_camera_interface_ptr->data_idx_[i],
                       camera_data_ptr->m_camera_data_list[mdc_camera_interface_ptr->data_idx_[i]].m_fsyncTimeStamp,
                       camera_data_ptr->m_camera_data_list[mdc_camera_interface_ptr->data_idx_[i]].m_ptr_len);
                break;
            }
        }

        ptr->images.resize(mdc_camera_interface_ptr->cam_num_);
        ptr->header.seq = seq++;
        ptr->header.frameID = "/base_link";
        int main_idx_in_vec = 0;
        for (int i = 0; i < mdc_camera_interface_ptr->cam_num_; i++) {
            ptr->images[i].frameID = mdc_camera_interface_ptr->channal_shm_buf_m[mdc_camera_interface_ptr->data_idx_[i]].cam_name;
            ptr->images[i].imageType = 2;
            ptr->images[i].height = mdc_camera_interface_ptr->channal_shm_buf_m[mdc_camera_interface_ptr->data_idx_[i]].height;
            ptr->images[i].width = mdc_camera_interface_ptr->channal_shm_buf_m[mdc_camera_interface_ptr->data_idx_[i]].width;
            ptr->images[i].bufferType = 1;
            ptr->images[i].dataSize = camera_data_ptr->m_camera_data_list[mdc_camera_interface_ptr->data_idx_[i]].m_ptr_len;
            ptr->images[i].timestamp = SecondsToMsgHafTime(camera_data_ptr->m_camera_data_list[mdc_camera_interface_ptr->data_idx_[i]].m_fsyncTimeStamp);
            ptr->images[i].seq = (uint32_t)(mdc_camera_interface_ptr->channal_shm_buf_m[mdc_camera_interface_ptr->data_idx_[i]].write_seq.load());
            if(mdc_camera_interface_ptr->main_idx_ == mdc_camera_interface_ptr->data_idx_[i])
            {
                main_idx_in_vec = i;
            }
        }

        {
            std::lock_guard<std::mutex> lock(ts_mutex_);
            for(int i = 0; i < mdc_camera_interface_ptr->cam_num_; i++)
            {
                ts_map_[i] = ptr->images[i].timestamp;
            }
        }
        ptr->header.timestamp = ptr->images[main_idx_in_vec].timestamp;
        safebuf_flag_info_.SetNotify(flag_ptr);

        return 0;
    }
    else if ("compressed_image_list" == topic)
    {
        std::shared_ptr<int>  temp;
        safebuf_flag_info_.WaitForGet(temp);
        mdc_camera_interface_ptr->GetSensorImage(compressed_camera_data_ptr);
        compressed_seq++;
        if (0 == n_out_1) {
            for (int i = 0; i < mdc_camera_interface_ptr->data_idx_.size(); i++) {
                if (debug & 16) {
                    printf(
                        "channel_%d's compress image, ts: %.3f, size: %d\n", mdc_camera_interface_ptr->data_idx_[i],
                        compressed_camera_data_ptr->m_camera_data_list[mdc_camera_interface_ptr->data_idx_[i]]
                            .m_fsyncTimeStamp,
                        compressed_camera_data_ptr->m_camera_data_list[mdc_camera_interface_ptr->data_idx_[i]].m_ptr_len);
                }
                if (debug & 32) {
                    printf(
                        "channel_%d's compress image, ts: %.3f, size: %d\n", mdc_camera_interface_ptr->data_idx_[i],
                        compressed_camera_data_ptr->m_camera_data_list[mdc_camera_interface_ptr->data_idx_[i]]
                            .m_fsyncTimeStamp,
                        compressed_camera_data_ptr->m_camera_data_list[mdc_camera_interface_ptr->data_idx_[i]].m_ptr_len);
                    break;
                }
            }
            ptr->images.resize(mdc_camera_interface_ptr->cam_num_);
            ptr->header.seq = compressed_seq;
            ptr->header.frameID = "/base_link";
            int main_idx_in_vec = 0;
            for (int i = 0; i < mdc_camera_interface_ptr->cam_num_; i++) {
                ptr->images[i].frameID = mdc_camera_interface_ptr->channal_shm_buf_m[mdc_camera_interface_ptr->data_idx_[i]].cam_name;
                ptr->images[i].imageType = 2;
                ptr->images[i].height = mdc_camera_interface_ptr->channal_shm_buf_m[mdc_camera_interface_ptr->data_idx_[i]].height;
                ptr->images[i].width = mdc_camera_interface_ptr->channal_shm_buf_m[mdc_camera_interface_ptr->data_idx_[i]].width;
                ptr->images[i].bufferType = 1;

                ptr->images[i].dataSize = compressed_camera_data_ptr->m_camera_data_list[mdc_camera_interface_ptr->data_idx_[i]].m_ptr_len;
                ptr->images[i].rawData.resize(compressed_camera_data_ptr->m_camera_data_list[mdc_camera_interface_ptr->data_idx_[i]].m_ptr_len);
                memcpy(ptr->images[i].rawData.data(), compressed_camera_data_ptr->m_camera_data_list[mdc_camera_interface_ptr->data_idx_[i]].m_ptr,
                             compressed_camera_data_ptr->m_camera_data_list[mdc_camera_interface_ptr->data_idx_[i]].m_ptr_len);
                ptr->images[i].timestamp = SecondsToMsgHafTime(compressed_camera_data_ptr->m_camera_data_list[mdc_camera_interface_ptr->data_idx_[i]].m_fsyncTimeStamp);
                ptr->images[i].seq = (uint32_t)(mdc_camera_interface_ptr->channal_shm_buf_m[mdc_camera_interface_ptr->data_idx_[i]].write_seq.load());
                if(mdc_camera_interface_ptr->main_idx_ == mdc_camera_interface_ptr->data_idx_[i])
                {
                    main_idx_in_vec = i;
                }
            }
            {
                std::lock_guard<std::mutex> lock(ts_mutex_);
                for(int i = 0; i < mdc_camera_interface_ptr->cam_num_; i++)
                {
                    if(ts_map_.count(i) > 0)
                    {
                        ptr->images[i].timestamp = ts_map_[i];
                    }
                }
            }
            ptr->header.timestamp = ptr->images[main_idx_in_vec].timestamp;
        } else if (0 < n_out_1) {
            if (compressed_seq % n_out_1 == 0) {
                for (int i = 0; i < mdc_camera_interface_ptr->data_idx_.size(); i++) {
                    if (debug & 16) {
                        printf(
                            "channel_%d's compress image, ts: %.3f, size: %d\n", mdc_camera_interface_ptr->data_idx_[i],
                            compressed_camera_data_ptr->m_camera_data_list[mdc_camera_interface_ptr->data_idx_[i]]
                                .m_fsyncTimeStamp,
                            compressed_camera_data_ptr->m_camera_data_list[mdc_camera_interface_ptr->data_idx_[i]].m_ptr_len);
                    }
                    if (debug & 32) {
                        printf(
                            "channel_%d's compress image, ts: %.3f, size: %d\n", mdc_camera_interface_ptr->data_idx_[i],
                            compressed_camera_data_ptr->m_camera_data_list[mdc_camera_interface_ptr->data_idx_[i]]
                                .m_fsyncTimeStamp,
                            compressed_camera_data_ptr->m_camera_data_list[mdc_camera_interface_ptr->data_idx_[i]].m_ptr_len);
                        break;
                    }
                }
                ptr->images.resize(mdc_camera_interface_ptr->cam_num_);
                ptr->header.seq = compressed_seq;
                ptr->header.frameID = "/base_link";
                int main_idx_in_vec = 0;
                for (int i = 0; i < mdc_camera_interface_ptr->cam_num_; i++) {
                    ptr->images[i].frameID = mdc_camera_interface_ptr->channal_shm_buf_m[mdc_camera_interface_ptr->data_idx_[i]].cam_name;
                    ptr->images[i].imageType = 2;
                    ptr->images[i].height = mdc_camera_interface_ptr->channal_shm_buf_m[mdc_camera_interface_ptr->data_idx_[i]].height;
                    ptr->images[i].width = mdc_camera_interface_ptr->channal_shm_buf_m[mdc_camera_interface_ptr->data_idx_[i]].width;
                    ptr->images[i].bufferType = 1;

                    ptr->images[i].dataSize = compressed_camera_data_ptr->m_camera_data_list[mdc_camera_interface_ptr->data_idx_[i]].m_ptr_len;
                    ptr->images[i].rawData.resize(compressed_camera_data_ptr->m_camera_data_list[mdc_camera_interface_ptr->data_idx_[i]].m_ptr_len);
                    memcpy(ptr->images[i].rawData.data(), compressed_camera_data_ptr->m_camera_data_list[mdc_camera_interface_ptr->data_idx_[i]].m_ptr,
                                compressed_camera_data_ptr->m_camera_data_list[mdc_camera_interface_ptr->data_idx_[i]].m_ptr_len);
                    ptr->images[i].timestamp = SecondsToMsgHafTime(compressed_camera_data_ptr->m_camera_data_list[mdc_camera_interface_ptr->data_idx_[i]].m_fsyncTimeStamp);
                    ptr->images[i].seq = (uint32_t)(mdc_camera_interface_ptr->channal_shm_buf_m[mdc_camera_interface_ptr->data_idx_[i]].write_seq.load());
                    if(mdc_camera_interface_ptr->main_idx_ == mdc_camera_interface_ptr->data_idx_[i])
                    {
                        main_idx_in_vec = i;
                    }



                    viz_h265_images[i].header.stamp = GetCurrentTime();
                    viz_h265_images[i].frameData.resize(ptr->images[i].rawData.size());
                    memcpy(viz_h265_images[i].frameData.data(), ptr->images[i].rawData.data(), ptr->images[i].rawData.size());
                    viz_h265_images[i].header.seq++;
                    std::cout << "viz_h265_images[i].header.stamp.sec = " << viz_h265_images[i].header.stamp.sec <<std::endl;
                    std::cout << "viz_h265_images[i].header.stamp.nsec = " << viz_h265_images[i].header.stamp.nsec <<std::endl;

                    std::cout << "viz_h265_images[i].header.frameId = " << viz_h265_images[i].header.frameId <<std::endl;
                }

                {
                    std::lock_guard<std::mutex> lock(ts_mutex_);
                    for(int i = 0; i < mdc_camera_interface_ptr->cam_num_; i++)
                    {
                        if(ts_map_.count(i) > 0)
                        {
                            ptr->images[i].timestamp = ts_map_[i];
                        }
                    }
                }
                ptr->header.timestamp = ptr->images[main_idx_in_vec].timestamp;

            } else {
                return -1;
            }
        }
        return 0;
    }
    else
    {
        sleep(1);
        return -1;
    }
}
