#pragma once
#include <vector>
#include <array>
#include <algorithm>
#include <stdexcept>

struct SelfRect {
    int x, y;           // 左上角
    int width, height;  // 宽高
};

/**
 * find_inner_rect
 *
 * 已知图像上 4 个不相交矩形围成一个中间空洞，返回该空洞矩形。
 *
 * 原理：
 *   将所有矩形的左边(x)和右边(x+width)合并排序，中间两个值为内矩形的左右边界；
 *   将所有矩形的上边(y)和下边(y+height)合并排序，中间两个值为内矩形的上下边界。
 *
 * @param rects  4 个矩形（顺序任意）
 * @return       中间被围住的矩形
 * @throws       std::invalid_argument 若计算结果无效（无封闭空洞）
 */
inline SelfRect find_inner_rect(const std::vector<SelfRect>& rects)
{
    std::array<int, 8> all_x, all_y;
    for (int i = 0; i < 4; ++i) {
        all_x[i]     = rects[i].x;
        all_x[i + 4] = rects[i].x + rects[i].width;
        all_y[i]     = rects[i].y;
        all_y[i + 4] = rects[i].y + rects[i].height;
    }
    std::sort(all_x.begin(), all_x.end());
    std::sort(all_y.begin(), all_y.end());

    // 排序后 index 3、4 即为内矩形的边界
    int inner_x1 = all_x[3];
    int inner_x2 = all_x[4];
    int inner_y1 = all_y[3];
    int inner_y2 = all_y[4];

    if (inner_x1 >= inner_x2 || inner_y1 >= inner_y2)
        throw std::invalid_argument("find_inner_rect: rects do not form a valid enclosure");

    SelfRect inner;
    inner.x      = inner_x1;
    inner.y      = inner_y1;
    inner.width  = inner_x2 - inner_x1;
    inner.height = inner_y2 - inner_y1;
    return inner;
}
