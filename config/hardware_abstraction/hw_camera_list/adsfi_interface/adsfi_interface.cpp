#include "adsfi_interface.h"
#include <sys/syscall.h>
#include <unistd.h>
#include "custom_stack.h"
#include "log/avos_node.h"

// #ifndef CONFIG_PATH
// #   error "CONFIG_PATH MUST BE DEFINED"
// #endif


void printCurrentTimeMillis(int id, const char* file, int line, std::string context = "") 
{
    auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch());
    pid_t threadId = syscall(SYS_gettid); // 获取当前线程的系统线程ID
    // 打印文件名、行号、线程ID和格式化的日期时间
    std::cout << "[" << file << ":" << line << ":" << threadId << ":"
            << std::fixed << std::setprecision(6) << static_cast<double>(ns.count()) / 1e9f << "] " << context << std::endl;
}

std::vector<std::string> split(const std::string &s, char delimiter) {
    std::vector<std::string> tokens;
    std::string              token;
    std::stringstream        ss(s);
    while (std::getline(ss, token, delimiter)) {
        if (!token.empty())
            tokens.push_back(token); // 忽略空字符串
    }
    return tokens;
}

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

int AdsfiInterface::InitCameraData(const std::vector<int> &channel_id, const std::vector<std::string> &camera_name,
             const std::vector<int> &width, const std::vector<int> &height, int freq, int main_idx) 
{
    if ((channel_id.size() != width.size()) || (channel_id.size() != height.size()) ||
            (channel_id.size() != camera_name.size())) {
        printf(
            "error:======================== input vec size of channel_id[%d]/camera_name[%d]/widht[%d]/height[%d] "
            "unequal ========================\n",
            channel_id.size(), camera_name.size(), width.size(), height.size());
        return -1;
    }
    printCurrentTimeMillis(0,__FILE__, __LINE__);
    mdc_camera_ptr = std::make_shared<avos::idpware::CIdpAbstractPlatformInterface>();
    if(mdc_camera_ptr == nullptr)
    {
        std::cout << "mdc_camera_ptr == nullptr\n";
    }
    printf("m_so_name = %s,mdc_camera_ptr\n",m_so_name.c_str());
    printCurrentTimeMillis(0,__FILE__, __LINE__);
    mdc_camera_ptr->open(m_so_name.c_str(), RTLD_LAZY);
    printCurrentTimeMillis(0,__FILE__, __LINE__);
    adsfi_camera_ptr = std::make_shared<ara::adsfi::MsgImageDataList>();
    adsfi_camera_ptr->images.resize(channel_id.size());
	adsfi_camera_ptr->header.frameID = "/base_link";

    adsfi_compressed_camera_ptr = std::make_shared<ara::adsfi::MsgImageDataList>();
    adsfi_compressed_camera_ptr->images.resize(channel_id.size());
	adsfi_compressed_camera_ptr->header.frameID = "/base_link";

    for (int i = 0; i < channel_id.size(); i++) {
		// adsfi_camera_ptr->images[i].header.module_name = "MDCCameraDriver";
        // adsfi_camera_ptr->images[i].header.data_name = camera_name[i];
        adsfi_camera_ptr->images[i].frameID = camera_name[i];
    	// adsfi_camera_ptr->images[i].encoding = "yuv420";
        adsfi_camera_ptr->images[i].imageType = 2;
        adsfi_camera_ptr->images[i].bufferType = 1;
        adsfi_camera_ptr->images[i].height = height[i];
        adsfi_camera_ptr->images[i].width = width[i];
        // adsfi_camera_ptr->images[i].step = width[i]*3/2;


        adsfi_compressed_camera_ptr->images[i].frameID = camera_name[i];
        adsfi_compressed_camera_ptr->images[i].imageType = 53;
        adsfi_compressed_camera_ptr->images[i].bufferType = 0;
        adsfi_compressed_camera_ptr->images[i].height = height[i];
        adsfi_compressed_camera_ptr->images[i].width = width[i];
        adsfi_compressed_camera_ptr->images[i].rawData.resize(512*1024);//默认h265数据不超过512K


        src_data_list.m_camera_data_list[i].m_ptr = (char*)malloc(3840*2160*3/2);//(char*)adsfi_camera_ptr->images[i].frame_data.data();//(char*)malloc(3840*2160*3/2);
        h264_data_list.m_camera_data_list[i].m_ptr = reinterpret_cast<char*>(adsfi_compressed_camera_ptr->images[i].rawData.data());

        std::string frameId = "mdc_camera_instance_" + std::to_string(70+i);
        viz_h265_images[i].header.frameId = frameId;
        viz_h265_images_pub[i] = mdc::visual::Publisher::Advertise<mdc::visual::Image>(frameId);
        std::cout << "frameId = " << frameId << std::endl;
        mdc_camera_ptr->ioctl(avos::idpware::IOCTL_SET_CHANNELID_PARAM,
                                (void *) & (channel_id[i]));
        m_channel_en_mask = m_channel_en_mask|(1<<channel_id[i]);
        m_cam_num++;
    }
    printCurrentTimeMillis(0,__FILE__, __LINE__);

    // mdc_camera_ptr->ioctl(avos::idpware::IOCTL_SET_KEYCHANNELID_PARAM, &(data_idx_[main_idx_]));
    mdc_camera_ptr->ioctl(avos::idpware::IOCTL_SET_KEYCHANNELID_PARAM, &main_idx);
    if (freq > 0) {
        mdc_camera_ptr->ioctl(avos::idpware::IOCTL_SET_H264CAMERADATA_FREQ, &freq);
    }

    printCurrentTimeMillis(0,__FILE__, __LINE__);

    mdc_camera_ptr->ioctl(avos::idpware::IOCTL_SET_CAMERA_ENABLE, &m_channel_en_mask);

    avos::idpware::SCameraInfo testparam;
    testparam.width = 0;
    testparam.height = 0;
    if (mdc_camera_ptr->ioctl(avos::idpware::IOCTL_INIT_CAMERA, &testparam) != 0) // yf
    {
        std::cerr << "Failed to IOCTL_INIT_CAMERA" << std::endl;
        return -1;
    }
    printCurrentTimeMillis(0,__FILE__, __LINE__);

    mdc_camera_ptr->ioctl(avos::idpware::IOCTL_START_SERVICE, NULL); // yf

    printCurrentTimeMillis(0,__FILE__, __LINE__);

    return 0;
}

void AdsfiInterface::Init()  {
	SetScheduleInfo("message");
	std::cout << "init" << std::endl;

    avos::common::AvosNode::Init("HwCameraList",true,"/opt/usr/zxz/log/","/opt/usr/zxz/log/","/opt/usr/zxz/log/");

    // std::string usr_config_path = CONFIG_PATH;
    // AINFO << "usr_config_path = " << usr_config_path << std::endl;

    auto ptr = CustomStack::Instance();
    // ptr->LoadModuleConfig(usr_config_path);

    ptr->GetConfig("HwCameraList","hw_fps", m_hw_fps);
    ptr->GetConfig("HwCameraList","format", m_format);
    ptr->GetConfig("HwCameraList","channel_id", m_channelId);
    ptr->GetConfig("HwCameraList","width", m_width);
    ptr->GetConfig("HwCameraList","height", m_height);
    ptr->GetConfig("HwCameraList","camera_name", m_camera_name);
    ptr->GetConfig("HwCameraList","debug", m_debug);
    ptr->GetConfig("HwCameraList","n_out_1", m_n_out_1);

    ptr->GetConfig("HwCameraList","main_idx", m_main_idx);
    ptr->GetConfig("HwCameraList","nv_cam_pos", m_nv_cam_pos);
    ptr->GetConfig("HwCameraList","node_name", m_node_name);
    ptr->GetConfig("HwCameraList","so_name", m_so_name);

    std::string usr_node_path = ptr->GetNodeConfigPath();
    m_so_name = usr_node_path + "/config/" + m_so_name;
    
    std::vector<std::string> channelId_str_v = split(m_channelId, ',');
    
    for (const auto &token : channelId_str_v) {
        channelId_int_v.push_back(atoi(token.c_str()));
    }

    std::vector<std::string> camera_name_str_v = split(m_camera_name, ',');

    std::vector<std::string> width_str_v = split(m_width, ',');
    std::vector<int>         width_int_v;
    for (const auto &token : width_str_v) {
        width_int_v.push_back(atoi(token.c_str()));
    }

    std::vector<std::string> height_str_v = split(m_height, ',');
    std::vector<int>         height_int_v;
    for (const auto &token : height_str_v) {
        height_int_v.push_back(atoi(token.c_str()));
    }

    
    InitCameraData(channelId_int_v,camera_name_str_v,width_int_v,height_int_v,m_n_out_1,m_main_idx);

    // 在程序入口处调用mdc::visual::Connect()接口初始化viz_lib本地资源和向上位机发起连接
    bool result = mdc::visual::Connect();
    if (result) {
        std::cout << "The MViz server address is parsed and the visualization client is created successfully.\n";
    } else {
    	std::cout << "Failed to parse the MViz server address, or a visualization client has been created.\n";
    }


}

void AdsfiInterface::DeInit()  {
	std::cout << "deinit" << std::endl;
}


int AdsfiInterface::Process(const std::string &topic,std::shared_ptr<ara::adsfi::MsgImageDataList> &ptr) {

    if("image_list" == topic)
    {
        std::cout << "topic = " << topic << std::endl;
        static int i = 0;
        ptr->header.seq = i++;

        printCurrentTimeMillis(0,__FILE__, __LINE__);
        for(int i=0; i<channelId_int_v.size(); i++)
        {
            src_data_list.initBit(channelId_int_v[i]);
        }
        src_data_list.mask=0;
        mdc_camera_ptr->ioctl(avos::idpware::IOCTL_GET_CAMERADATA, &src_data_list);
        printCurrentTimeMillis(0,__FILE__, __LINE__);
        std::cout << "src_data_list.mask = " << src_data_list.mask << std::endl;
        adsfi_camera_ptr->header.seq = m_seq++;

        for(int i = 0; i < m_cam_num; i++ )
        {
            if(src_data_list.m_camera_data_list[i].m_fsyncTimeStamp != 0 && m_main_idx == i)
            {
                adsfi_camera_ptr->header.timestamp = SecondsToMsgHafTime(src_data_list.m_camera_data_list[i].m_fsyncTimeStamp);
            }
            
            adsfi_camera_ptr->images[i].dataSize = src_data_list.m_camera_data_list[i].m_ptr_len;
            adsfi_camera_ptr->images[i].timestamp = SecondsToMsgHafTime(src_data_list.m_camera_data_list[i].m_fsyncTimeStamp);
            // adsfi_camera_ptr->images[i].shm_buffer_info.buffer_size = src_data_list.m_camera_data_list[i].m_ptr_len;
            std::cout << "src_data_list.m_camera_data_list[i].m_ptr_len = " << src_data_list.m_camera_data_list[i].m_ptr_len << std::endl;
        }

        ptr = adsfi_camera_ptr;
    }
    else if ("compressed_image_list" == topic) 
    {
        adsfi_compressed_camera_ptr->header.seq++;
        mdc_camera_ptr->ioctl(avos::idpware::IOCTL_GET_H264CAMERADATA, &h264_data_list);
        for(int i = 0; i < m_cam_num; i++ )
        {
            if(h264_data_list.m_camera_data_list[i].m_ptr_len > 3840*2160 || h264_data_list.m_camera_data_list[i].m_ptr_len == 0)
                continue;
            std::cout << "h264_data_list.m_camera_data_list["<< i <<"].m_ptr_len = " << h264_data_list.m_camera_data_list[i].m_ptr_len << std::endl;

            viz_h265_images[i].header.stamp = GetCurrentTime();
            viz_h265_images[i].frameData.resize(h264_data_list.m_camera_data_list[i].m_ptr_len);
            memcpy(viz_h265_images[i].frameData.data(), h264_data_list.m_camera_data_list[i].m_ptr, h264_data_list.m_camera_data_list[i].m_ptr_len);
            viz_h265_images[i].header.seq++;
            std::cout << "viz_h265_images[i].header.stamp.sec = " << viz_h265_images[i].header.stamp.sec <<std::endl;
            std::cout << "viz_h265_images[i].header.stamp.nsec = " << viz_h265_images[i].header.stamp.nsec <<std::endl;

            std::cout << "viz_h265_images[i].header.frameId = " << viz_h265_images[i].header.frameId <<std::endl;

            adsfi_compressed_camera_ptr->images[i].dataSize = h264_data_list.m_camera_data_list[i].m_ptr_len;
            adsfi_compressed_camera_ptr->images[i].timestamp = SecondsToMsgHafTime(src_data_list.m_camera_data_list[i].m_fsyncTimeStamp);

            viz_h265_images_pub[i].Publish(viz_h265_images[i]);
        }
        ptr = adsfi_compressed_camera_ptr;
    }
    else
    {
        sleep(1);
        return -1;
    }
    return 0;
}


// int AdsfiInterface::Process(std::shared_ptr<ara::adsfi::IdpSensorCompressedImageList> &ptr) {
// 	static int i = 0;
// 	ptr->header.seq = i++;
//     return 0;
// }
