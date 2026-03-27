#ifndef __CAMERA_LIST_HPP__
#define __CAMERA_LIST_HPP__
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <cstddef>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "ShmBufferInfo.h"
#include "config_loader/custom_stack.h"
#include "faulthandle_api.hpp"
#include "log/log.h"
#include "../protocol/cameraplatform2usrprotocol.h"
#include "cidpabstractplatforminterface.h"
#include "cam_shm_param.hpp"
#include "zero_copy_shm_mem.h"

inline void printCurrentTimeMillis(const char *file, int line) {
    auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch());

    // 打印文件名、行号和格式化的日期时间
    std::cout << "[" << file << ":" << line << "] "
              << "当前系统时间（精确到毫秒）: " << static_cast<double>(ns.count()) / 1e6f << std::endl;
}

// // 宏定义，用于简化函数调用
#define PRINT_CURRENT_TIME(file, line) printCurrentTimeMillis((file), (line))

inline std::vector<std::string> split(const std::string &s, char delimiter) {
    std::vector<std::string> tokens;
    std::string              token;
    std::stringstream        ss(s);
    while (std::getline(ss, token, delimiter)) {
        if (!token.empty())
            tokens.push_back(token); // 忽略空字符串
    }
    return tokens;
}

typedef struct SHM_DATA {
    int              channelId;
    std::string      channelId_str;
    std::string      cam_name;
    std::string      format;
    int              width;
    int              height;
    int              dtype;
    ShmBufferInfo    private_shm_buffer_info;
    std::atomic<int> write_seq;
    int              max_queue_size;
    std::mutex       mutex_;
    ShmBufferInfo    swap_shm_buffer_info;
    char *           comp_buf;
    char *           shm_addr;
    char *           img_buf;
    int              img_buf_size;
    int              no_data_cnt;
    int              lost_data_cnt;
    double           last_img_ts;
    int              opened;

    SHM_DATA() {
        dtype = 0;
        comp_buf = nullptr;
        img_buf = nullptr;
        img_buf_size = 0;
        no_data_cnt = 0;
        lost_data_cnt = 0;
        shm_addr = nullptr;
    }
} shm_data_t;

class MdcCameraInterface {
public:
    MdcCameraInterface() {
    }

    ~MdcCameraInterface() {
        for (int i = 0; i < data_idx_.size(); i++) {
            if (0 != channal_shm_buf_m.count(data_idx_[i])) {
                if (channal_shm_buf_m[data_idx_[i]].comp_buf) {
                    free(channal_shm_buf_m[data_idx_[i]].comp_buf);
                }
                if (channal_shm_buf_m[data_idx_[i]].img_buf) {
                    free(channal_shm_buf_m[data_idx_[i]].img_buf);
                }
            }
        }
    }

    int Init(const std::vector<int> &channel_id, const std::vector<std::string> &camera_name,
             const std::vector<int> &width, const std::vector<int> &height, int freq, int main_idx) {
        main_idx_ = main_idx; // idx of data_idx_
        if ((channel_id.size() != width.size()) || (channel_id.size() != height.size()) ||
            (channel_id.size() != camera_name.size())) {
            printf(
                "======================== input vec size of channel_id[%d]/camera_name[%d]/widht[%d]/height[%d] "
                "unequal ========================\n",
                channel_id.size(), camera_name.size(), width.size(), height.size());
        }

        for (int i = 0; i < channel_id.size(); i++) {
            if (0 == channal_shm_buf_m.count(channel_id[i])) {
                channal_shm_buf_m[channel_id[i]].channelId = channel_id[i];
                std::string channelId_str = std::to_string(channel_id[i]);
                while (channelId_str.length() < 2) {
                    channelId_str = "0" + channelId_str;
                }
                channal_shm_buf_m[channel_id[i]].channelId_str = channelId_str;
                channal_shm_buf_m[channel_id[i]].cam_name = camera_name[i];
                channal_shm_buf_m[channel_id[i]].width = width[i];
                channal_shm_buf_m[channel_id[i]].height = height[i];
                channal_shm_buf_m[channel_id[i]].dtype = 0;
                channal_shm_buf_m[channel_id[i]].write_seq.store(0);
                channal_shm_buf_m[channel_id[i]].max_queue_size = 10;
                channal_shm_buf_m[channel_id[i]].private_shm_buffer_info.shm_key =
                    "data_type" + std::to_string(0) + "_channel" + std::to_string(channel_id[i]);
                channal_shm_buf_m[channel_id[i]].private_shm_buffer_info.buffer_size = width[i] * height[i] * 3;
                channal_shm_buf_m[channel_id[i]].private_shm_buffer_info.queue_len = 10;
                channal_shm_buf_m[channel_id[i]].comp_buf = (char *)malloc(width[i] * height[i] * 3);
                channal_shm_buf_m[channel_id[i]].img_buf = (char *)malloc(width[i] * height[i] * 3);
                channal_shm_buf_m[channel_id[i]].no_data_cnt = 0;
                channal_shm_buf_m[channel_id[i]].lost_data_cnt = 0;
                channal_shm_buf_m[channel_id[i]].opened = 0;
                channal_shm_buf_m[channel_id[i]].last_img_ts = -1.1;
                cam_num_++;
                data_idx_.push_back(channel_id[i]);
            } else {
                printf(
                    "======================== channel_id[%d] exist, witch match widht[%d] height[%d] "
                    "========================\n",
                    channel_id[i], width[i], height[i]);
            }
        }

        mdc_camera_ptr = std::make_shared<avos::idpware::CIdpAbstractPlatformInterface>();
        mdc_camera_ptr->open(so_path.c_str(), RTLD_LAZY);
        for (auto it = channal_shm_buf_m.begin(); it != channal_shm_buf_m.end(); ++it) {
            printf("IOCTL_SET_CHANNELID_PARAM for channel %d\n", it->first);
            mdc_camera_ptr->ioctl(avos::idpware::IOCTL_SET_CHANNELID_PARAM,
                                  (void *)&(it->first)); // yf, 多个相机调用多次，若打开全部相机可以只调用一次参数传递14
        }
        // mdc_camera_ptr->ioctl(avos::idpware::IOCTL_SET_KEYCHANNELID_PARAM, &(data_idx_[main_idx_]));
        mdc_camera_ptr->ioctl(avos::idpware::IOCTL_SET_KEYCHANNELID_PARAM, &main_idx);
        if (freq > 0) {
            mdc_camera_ptr->ioctl(avos::idpware::IOCTL_SET_H264CAMERADATA_FREQ, &freq);
        }

        mdc_camera_ptr->ioctl(avos::idpware::IOCTL_SET_CAMERA_ENABLE, &channel_en_mask_);

        avos::idpware::SCameraInfo testparam;
        testparam.width = 0;
        testparam.height = 0;
        if (mdc_camera_ptr->ioctl(avos::idpware::IOCTL_INIT_CAMERA, &testparam) != 0) // yf
        {
            std::cerr << "Failed to IOCTL_INIT_CAMERA" << std::endl;
            return -1;
        }

        mdc_camera_ptr->ioctl(avos::idpware::IOCTL_START_SERVICE, NULL); // yf

        return 0;
    }

    void GetShmImage(std::shared_ptr<avos::idpware::SCameraDataList> &camera_data_ptr) {
        for (auto it = channal_shm_buf_m.begin(); it != channal_shm_buf_m.end(); ++it) {
            it->second.private_shm_buffer_info.current_index = it->second.write_seq.load() + 1;

            auto shm_ptr = zero_shm_mem_map[it->second.private_shm_buffer_info.shm_key].GetAllocShmMem(
                it->second.private_shm_buffer_info.shm_key, it->second.private_shm_buffer_info.buffer_size,
                it->second.private_shm_buffer_info.current_index, it->second.private_shm_buffer_info.queue_len);
            it->second.shm_addr = (char *)shm_ptr;

            if (debug_ & 4) {
                std::cout << it->second.private_shm_buffer_info.shm_key
                          << "  write private_shm_buffer_info.current_index: "
                          << it->second.private_shm_buffer_info.current_index << "," << it->second.write_seq.load()
                          << "\n";
            }
            // memcpy(shm_ptr, camera_data_ptr->m_camera_data_list[it->first].m_ptr,
            // camera_data_ptr->m_camera_data_list[it->first].m_ptr_len);
            camera_data_ptr->m_camera_data_list[it->first].m_ptr = (char *)shm_ptr;
            it->second.write_seq.fetch_add(1, std::memory_order_relaxed); ///////////////

            it->second.private_shm_buffer_info.current_index = it->second.write_seq.load(); /////////////////////
            // it->second.private_shm_buffer_info.buffer_size =
            // camera_data_ptr->m_camera_data_list[it->first].m_ptr_len;
            // SetShmBufferInfo(it->second.private_shm_buffer_info, it->first);
        }
        camera_data_ptr->mask = 0;
        mdc_camera_ptr->ioctl(avos::idpware::IOCTL_GET_CAMERADATA, camera_data_ptr.get());
        int main_idx_ioctl = 0;
        mdc_camera_ptr->ioctl(avos::idpware::IOCTL_GET_KEYCHANNELID_PARAM, &main_idx_ioctl);
        if(main_idx_ioctl != main_idx_)
        {
            main_idx_ = main_idx_ioctl;
            printf("get IOCTL_GET_KEYCHANNELID_PARAM val is: %d, update main_idx_\n", main_idx_ioctl);
            std::unique_lock<std::mutex> locker(ioctl_mutex_);
            CameraShmParam::SetCameraModuleParam(node_name, main_idx_ioctl, nv_cam_pos, channel_en_);
        }

        for (auto it = channal_shm_buf_m.begin(); it != channal_shm_buf_m.end(); ++it) {
            it->second.private_shm_buffer_info.buffer_size = camera_data_ptr->m_camera_data_list[it->first].m_ptr_len;
            // SetShmBufferInfo(it->second.private_shm_buffer_info, it->first);

            // if((camera_data_ptr->mask) & (1<<(it->first)))
            if (debug_ & 4) {
                printf("channel_%d's img last_img_ts: %.3f, m_fsyncTimeStamp: %.3f\n", it->first,
                       it->second.last_img_ts, camera_data_ptr->m_camera_data_list[it->first].m_fsyncTimeStamp);
            }
            if ((fabs(it->second.last_img_ts - camera_data_ptr->m_camera_data_list[it->first].m_fsyncTimeStamp) >
                 0.001)) // new frame
            {
                SetShmBufferInfo(it->second.private_shm_buffer_info, it->first);
                // it->second.img_buf_size = camera_data_ptr->m_camera_data_list[it->first].m_ptr_len;
                // memcpy(it->second.img_buf, camera_data_ptr->m_camera_data_list[it->first].m_ptr,
                //        it->second.img_buf_size);
                if (it->second.last_img_ts > -0.1)
                {
                    it->second.no_data_cnt = 0;
                    it->second.lost_data_cnt = 0;
                    if (0 == it->second.opened) {
                        it->second.opened = 1;
                    }
                    ec_remove("1042"+it->second.channelId_str+"3", it->second.cam_name+" open device failed");
                    ec_remove("1042"+it->second.channelId_str+"4", it->second.cam_name+" lost frame", 5);
                }

                if (it->second.last_img_ts > 0) {
                    if ((camera_data_ptr->m_camera_data_list[it->first].m_fsyncTimeStamp - it->second.last_img_ts) >
                            (interval_s_ * 1.7)) {
                        ec_add("1042"+it->second.channelId_str+"6", it->second.cam_name+" frame interval exceed", 20);
                    } else {
                        ec_remove("1042"+it->second.channelId_str+"6", it->second.cam_name+" frame interval exceed", 5);
                    }
                    timeval tv{};
                    gettimeofday(&tv, nullptr);
                    double cur_ts = tv.tv_sec + tv.tv_usec / 1000000.0;
                    if (fabs(cur_ts - camera_data_ptr->m_camera_data_list[it->first].m_fsyncTimeStamp) > 0.5) {
                        ec_add("1042"+it->second.channelId_str+"5", it->second.cam_name+" time sync failed", 20);
                    } else {
                        ec_remove("1042"+it->second.channelId_str+"5", it->second.cam_name+" time sync failed", 5);
                    }
                }
                it->second.last_img_ts = camera_data_ptr->m_camera_data_list[it->first].m_fsyncTimeStamp;
            } else {
                it->second.write_seq.fetch_sub(1, std::memory_order_relaxed);
                it->second.private_shm_buffer_info.current_index = it->second.write_seq.load();
                SetShmBufferInfo(it->second.private_shm_buffer_info, it->first);
                // if (it->second.img_buf_size > 0) // use last image
                // {
                //     memcpy(it->second.shm_addr, it->second.img_buf, it->second.img_buf_size);
                // }
                if (debug_ & 4) {
                    printf("lost channel_%d's frame\n", it->first);
                }
                if (0 == it->second.opened) {
                    it->second.no_data_cnt++;
                    if (it->second.no_data_cnt > 300) // open device failed
                    {
                        ec_add("1042"+it->second.channelId_str+"3", it->second.cam_name+" open device failed");
                    }
                }

                if (1 == it->second.opened) // lost frame at 13v_list
                {
                    it->second.lost_data_cnt++;
                    // if (it->second.lost_data_cnt > 100) {
                    //     ec_add("1081" + it->second.channelId_str + "2");
                    // }
                    ec_add("1042"+it->second.channelId_str+"4", it->second.cam_name+" lost frame", 20);
                }
            }
        }
        if (debug_ & 8) {
            printf("camera_data_ptr->mask=%x\n", camera_data_ptr->mask);
        }
    }

    void GetSensorImage(std::shared_ptr<avos::idpware::SCameraDataList> &camera_data_ptr) {
        // std::unique_lock<std::mutex> locker(comp_mutex_);
        for (auto it = channal_shm_buf_m.begin(); it != channal_shm_buf_m.end(); ++it) {
            camera_data_ptr->m_camera_data_list[it->first].m_ptr = it->second.comp_buf;
            // memcpy(it->second.comp_buf, camera_data_ptr->m_camera_data_list[it->first].m_ptr,
            // camera_data_ptr->m_camera_data_list[it->first].m_ptr_len);
        }
        mdc_camera_ptr->ioctl(avos::idpware::IOCTL_GET_H264CAMERADATA, camera_data_ptr.get());
        // std::cout << "IOCTL_GET_H264CAMERADATA length: " << camera_data_ptr->m_ptr_len << std::endl;
    }

    void SetShmBufferInfo(const ShmBufferInfo &temp, int channel_id) {
        std::unique_lock<std::mutex> locker(channal_shm_buf_m[channel_id].mutex_);
        channal_shm_buf_m[channel_id].swap_shm_buffer_info = temp;
    }

    void GetShmBufferInfo(ShmBufferInfo &temp, int channel_id) {
        std::unique_lock<std::mutex> locker(channal_shm_buf_m[channel_id].mutex_);
        temp = channal_shm_buf_m[channel_id].swap_shm_buffer_info;
    }

    void ec_add(const std::string &ec, const std::string &extar_info, uint8_t threshold=1) {
        std::lock_guard<std::mutex> lock(_mtx);
        static timeval tv = {};
        gettimeofday(&tv, nullptr);

        auto ptr = ec_map.find(ec);
        if (ptr == ec_map.end()) {
            ec_map[ec] = ErrCodeState();
        }
        ec_map[ec].normal_cnt = 0;
        ec_map[ec].abnormal_cnt++;
        ec_map[ec].abnormal_ts = tv.tv_sec*1000 + tv.tv_usec / 1000.0;
        if (debug_ & 4) {
            printf("ec_add_%s, normal_cnt:%d, last_state_is_normal:%d,   abnormal_cnt:%d, last_state_is_abnormal:%d\n", ec.c_str(),
                ec_map[ec].normal_cnt, ec_map[ec].last_state_is_normal, ec_map[ec].abnormal_cnt, ec_map[ec].last_state_is_abnormal);
        }
        if((ec_map[ec].abnormal_cnt >= threshold)&&(!ec_map[ec].last_state_is_abnormal))
        {
            ec_map[ec].last_state_is_normal =  false;
            ec_map[ec].last_state_is_abnormal = true;

            std::string info = std::to_string(ec_map[ec].abnormal_ts)+" ms: "+extar_info;
            std::cout << "ec_add: " << ec << ", extra_info: " << info <<"\n";
            FaultHandle::FaultApi::Instance()->SetFaultCode(ec, info);
        }
    }

    void ec_remove(const std::string &ec, const std::string &extar_info, uint8_t threshold=1) {
        std::lock_guard<std::mutex> lock(_mtx);
        static timeval tv = {};
        gettimeofday(&tv, nullptr);

        auto ptr = ec_map.find(ec);
        if (ptr == ec_map.end()) {
            ec_map[ec] = ErrCodeState();
        }
        ec_map[ec].abnormal_cnt = 0;
        ec_map[ec].normal_cnt++;
        ec_map[ec].normal_ts = tv.tv_sec*1000 + tv.tv_usec / 1000.0;
        if (debug_ & 4) {
            printf("ec_remove_%s, normal_cnt:%d, last_state_is_normal:%d,   abnormal_cnt:%d, last_state_is_abnormal:%d\n", ec.c_str(),
                ec_map[ec].normal_cnt, ec_map[ec].last_state_is_normal, ec_map[ec].abnormal_cnt, ec_map[ec].last_state_is_abnormal);
        }
        if((ec_map[ec].normal_cnt >= threshold)&&(!ec_map[ec].last_state_is_normal))
        {
            ec_map[ec].last_state_is_normal =  true;
            ec_map[ec].last_state_is_abnormal = false;
            std::string info = std::to_string(ec_map[ec].abnormal_ts)+" ms: "+extar_info;
            std::cout << "ec_remove: " << ec << ", extra_info: " << info <<"\n";
            FaultHandle::FaultApi::Instance()->ResetFaultCode(ec);
        }
    }

    void ec_clear() {
        std::lock_guard<std::mutex> lock(_mtx);
        for (const auto &ec : ec_map) {
            FaultHandle::FaultApi::Instance()->ResetFaultCode(ec.first);
        }
        ec_map.clear();
    }

private:
    std::mutex                           _mtx;
    typedef struct ErrCodeState
    {
        uint8_t normal_cnt;
        uint8_t abnormal_cnt;
        uint64_t normal_ts;//ms
        uint64_t abnormal_ts;//ms
        bool     last_state_is_normal;
        bool     last_state_is_abnormal;
        ErrCodeState()
        {
            normal_cnt = 0;
            abnormal_cnt = 0;
            normal_ts = 0;
            abnormal_ts = 0;
            last_state_is_normal = false;
            last_state_is_abnormal = false;
        }
    }ErrCodeState;
    std::map<std::string, ErrCodeState> ec_map; //<ecc_code, state>

private:
    std::map<std::string, ZeroCopyShmMem> zero_shm_mem_map;

public:
    std::shared_ptr<avos::idpware::CIdpAbstractPlatformInterface> mdc_camera_ptr;
    std::map<int, shm_data_t>                                     channal_shm_buf_m; // <channel_id, shm_data_t>
    int                                                           debug_ = 0;
    int                                                           cam_num_ = 0;
    std::mutex                                                    comp_mutex_;
    std::vector<int>                                              data_idx_;
    std::atomic<int>                                              main_idx_ = 0;
    int                                                           nv_cam_pos = 0;
    std::string                                                   channel_en_ = "00000000000000";
    int                                                           channel_en_mask_ = 0;
    bool                                                          enable_nv_cam = true;
    double                                                        interval_s_ = 0.033; // 30hz
    std::string                                                   node_name = "mdc_camera";
    std::string                                                   so_path = "";
    std::mutex                                                    ioctl_mutex_;
};

#endif
