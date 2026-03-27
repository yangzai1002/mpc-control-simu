#include "XDrawDrivableArea.h"
#include "ap_log/ap_log_interface.h"
#include "custom_stack.h"
#include "shm_util/zero_copy_shm_mem.h"
#include <vector>
#include <cstring>

// 可通行区域像素值（语义分割中road类别=1）
constexpr uint8_t DRIVABLE_PIXEL_VALUE = 1;

// 默认绿色叠加参数（YUV格式），可被配置覆盖
// 纯绿色 RGB(0,255,0) -> YUV: Y=150, U=44, V=21
static uint8_t g_overlay_y   = 150;
static uint8_t g_overlay_u   = 44;
static uint8_t g_overlay_v   = 21;
static float   g_overlay_alpha = 0.4f;
static bool    g_config_loaded = false;

// RLE解压（参考fusion_follow_src/data_exchange.cpp）
static void RLEDecompress(const std::vector<uint8_t>& input,
                           std::vector<uint8_t>& output, int output_size) {
    output.clear();
    output.resize(output_size);
    uint8_t* data = output.data();
    for (int i = 0; i < static_cast<int>(input.size());) {
        int count = 0;
        for (int k = 0; k < static_cast<int>(sizeof(int)); k++) {
            count |= (input[i + k] << (8 * k));
        }
        memset(data, input[i + sizeof(int)], count);
        data += count;
        i += 1 + sizeof(int);
    }
}

// 加载配置（只加载一次）
static void loadConfigOnce() {
    if (g_config_loaded){
        return;
    }
    auto ptr = CustomStack::Instance();
    if (!ptr) {
        return;
    }
    std::string prefix = ptr->GetProjectConfigPath() + "/";

    float alpha = 0.4f;
    if (ptr->GetProjectConfigValue(prefix + "video/edit/drivable_area_alpha", alpha)) {
        g_overlay_alpha = alpha;
    }
    int y_val = 150, u_val = 44, v_val = 21;
    ptr->GetProjectConfigValue(prefix + "video/edit/drivable_area_yuv_y", y_val);
    ptr->GetProjectConfigValue(prefix + "video/edit/drivable_area_yuv_u", u_val);
    ptr->GetProjectConfigValue(prefix + "video/edit/drivable_area_yuv_v", v_val);
    g_overlay_y = static_cast<uint8_t>(y_val);
    g_overlay_u = static_cast<uint8_t>(u_val);
    g_overlay_v = static_cast<uint8_t>(v_val);
    g_config_loaded = true;
}

/**
 * @brief 在YUV420(NV12)图像上叠加可通行区域
 * 优化：整数运算+预计算映射表，避免每帧重新分配
 */
static void overlayDrivableArea(void* yuv_buffer,
                                 int img_width, int img_height,
                                 const std::vector<uint8_t>& seg_mask,
                                 int seg_width, int seg_height) {
    if (!yuv_buffer || seg_mask.empty() || seg_width <= 0 || seg_height <= 0) {
        return;
    }
    if (static_cast<int>(seg_mask.size()) < seg_width * seg_height) {
        return;
    }

    uint8_t* y_plane  = static_cast<uint8_t*>(yuv_buffer);
    uint8_t* uv_plane = y_plane + img_width * img_height;

    // 预计算映射表（尺寸变化时才重建，通常只建一次）
    static std::vector<int> row_map, col_map;
    static int last_iw = 0, last_ih = 0, last_sw = 0, last_sh = 0;
    if (last_iw != img_width || last_ih != img_height || last_sw != seg_width || last_sh != seg_height) {
        row_map.resize(img_height);
        col_map.resize(img_width);
        for (int y = 0; y < img_height; ++y){
            row_map[y] = std::min(y * seg_height / img_height, seg_height - 1);
        }
        for (int x = 0; x < img_width; ++x){
            col_map[x] = std::min(x * seg_width / img_width, seg_width - 1);
        }
        last_iw = img_width; last_ih = img_height;
        last_sw = seg_width; last_sh = seg_height;
    }

    // 整数定点alpha混合（避免每像素浮点）
    int alpha_int     = static_cast<int>(g_overlay_alpha * 256);
    int inv_alpha_int = 256 - alpha_int;
    int y_premul      = g_overlay_y * alpha_int;
    int u_premul      = g_overlay_u * alpha_int;
    int v_premul = g_overlay_v * alpha_int;

    for (int iy = 0; iy < img_height; ++iy) {
        const uint8_t* seg_row = seg_mask.data() + row_map[iy] * seg_width;
        uint8_t*       y_row   = y_plane + iy * img_width;

        for (int ix = 0; ix < img_width; ++ix) {
            if (seg_row[col_map[ix]] != DRIVABLE_PIXEL_VALUE) {
                continue;
            }

            y_row[ix] = static_cast<uint8_t>((y_row[ix] * inv_alpha_int + y_premul) >> 8);

            // NV12: UV平面每2x2像素共享一对UV
            if ((iy & 1) == 0 && (ix & 1) == 0) {
                int uv_idx = (iy >> 1) * img_width + (ix & ~1);
                uv_plane[uv_idx]     = static_cast<uint8_t>((uv_plane[uv_idx]     * inv_alpha_int + u_premul) >> 8);
                uv_plane[uv_idx + 1] = static_cast<uint8_t>((uv_plane[uv_idx + 1] * inv_alpha_int + v_premul) >> 8);
            }
        }
    }
}

bool XDrawDrivableArea(const ara::adsfi::MsgImageData& image_data, const ara::adsfi::MsgCameraVPResult& vp_result,
                       const compatible::VideoParameter& video_parameter) {
    // 开关检查
    if (video_parameter.editor_parameter.show_drivable_area != 1) {
        return true;
    }

    // 仅处理前视相机
    if (image_data.frameID != "put_front") {
        return true;
    }

    // 加载配置（只加载一次）
    loadConfigOnce();

    // 从共享内存获取图像buffer
    static ZeroCopyShmMem zero_shm_mem_image;
    constexpr int SHM_MAX_W = 3840, SHM_MAX_H = 2160;
    auto                  shm_image_ptr =
        zero_shm_mem_image.GetAllocShmMem(image_data.frameID, SHM_MAX_W * SHM_MAX_H * 3 / 2, image_data.seq);
    if (!shm_image_ptr) {
        ApError("video_editor") << "XDrawDrivableArea: GetAllocShmMem failed";
        return false;
    }

    // 直接从MsgCameraVPResult取数据，不转换PerceptionVPResult
    // 结构：MsgCameraVPResult.vp_result_vec -> MsgCameraDetectionResult[]
    //        每个元素.header.frameID 是相机名
    //        每个元素.image_segmentation_vec -> MsgImageData[]
    //        其中 frameID=="seg" 的是语义分割数据

    // 缓存最后一次有效的seg数据（解决10hz topic vs 30fps视频的闪烁问题）
    static std::vector<uint8_t> cached_seg_data;
    static int                  cached_seg_w = 0, cached_seg_h = 0;
    static bool cached_valid = false;

    cached_seg_data.clear();

    // 检查是否需要RLE解压（根据MsgCameraVPResult.header.frameID判断）
    bool is_compressed = (vp_result.header.frameID.find("segment_compressed") != std::string::npos);
    // 遍历vp_result_vec，找front相机的seg数据
    bool got_new_data = false;
    for (const auto& cam_res : vp_result.vp_result_vec) {
        // front相机判断
        if (cam_res.header.frameID.find("front_h100") == std::string::npos) {
            continue;
        }

        for (const auto& seg_img : cam_res.image_segmentation_vec) {
            if (seg_img.frameID != "seg") continue;
            if (seg_img.dataSize == 0 || seg_img.rawData.empty()) {
                continue;
            }

            int seg_w = static_cast<int>(seg_img.width);
            int seg_h = static_cast<int>(seg_img.height);
            if (seg_w <= 0 || seg_h <= 0) continue;

            if (is_compressed) {
                // RLE解压：先复制压缩数据，再解压到缓存
                std::vector<uint8_t> compressed(seg_img.rawData.begin(),
                                                 seg_img.rawData.begin() + seg_img.dataSize);
                RLEDecompress(compressed, cached_seg_data, seg_w * seg_h);
            } else {
                cached_seg_data.assign(seg_img.rawData.begin(),
                                        seg_img.rawData.begin() + seg_img.dataSize);
            }
            cached_seg_w = seg_w;
            cached_seg_h = seg_h;
            cached_valid = true;
            got_new_data = true;
            break;
        }
        if (got_new_data) {
            break;
        }
    }

    // 无有效缓存则跳过（不叠加，不报错）
    if (!cached_valid || cached_seg_data.empty()) {
        return true;
    }

    // 叠加可通行区域
    overlayDrivableArea(shm_image_ptr,
                        image_data.width, image_data.height,
                        cached_seg_data, cached_seg_w, cached_seg_h);
    return true;
}
