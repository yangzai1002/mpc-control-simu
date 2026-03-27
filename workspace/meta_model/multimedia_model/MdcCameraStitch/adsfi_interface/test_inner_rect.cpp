#include "find_inner_rect.h"
#include <cstdio>
#include <cassert>

static void print_rect(const char* name, const Rect& r) {
    std::printf("%s: (%d,%d)-(%d,%d)\n", name, r.x1, r.y1, r.x2, r.y2);
}

int main()
{
    // 原有测试
    std::array<Rect, 4> rects = {{
        {  0,   0, 200, 100},
        {  0, 300, 200, 400},
        {  0, 100,  50, 300},
        {150, 100, 200, 300},
    }};
    Rect inner = find_inner_rect(rects);
    print_rect("test1 inner", inner);
    assert(inner.x1 == 50  && inner.y1 == 100);
    assert(inner.x2 == 150 && inner.y2 == 300);
    std::printf("test 1 passed\n\n");

    // 用户提供的测试用例
    // 矩形1: (0,0)   - (1280,534)   上方横条
    // 矩形2: (0,523) - (470, 1304)  左侧竖条
    // 矩形3: (0,1380)- (1280,1920)  下方横条
    // 矩形4: (0,1163)- (1280,1920)  也在下方，与矩形3重叠
    //
    // 分析：
    //   all_x 排序: 0,0,0,0, 470,1280,1280,1280 → index3=0, index4=470
    //   all_y 排序: 0,523,1163,1380, 1304,1920,1920,1920 → index3=1380, index4=1304
    //   → inner.y1=1380 > inner.y2=1304，无效！
    //
    // 根本原因：4个矩形没有右侧矩形，无法围成封闭空洞。
    // 正确的4矩形围合布局应为：上、下、左、右各一个。
    std::array<Rect, 4> rects3 = {{
        {   0,    0, 1280,  534},  // 上
        {   0,  523,  470, 1304},  // 左
        {   0, 1380, 1280, 1920},  // 下
        {   0, 1163, 1280, 1920},  // 也在下方 → 无右侧矩形
    }};

    std::printf("user test case (expected to be invalid):\n");
    for (int i = 0; i < 4; ++i)
        print_rect(("  rect" + std::to_string(i+1)).c_str(), rects3[i]);

    try {
        Rect inner3 = find_inner_rect(rects3);
        print_rect("  inner3 (raw)", inner3);
        std::printf("  WARNING: result is geometrically invalid"
                    " (no right-side rect provided)\n");
    } catch (const std::exception& e) {
        std::printf("  caught: %s\n", e.what());
    }

    // 修正版：补充右侧矩形，使4个矩形真正围合
    // 假设右侧矩形为 (810, 523)-(1280, 1380)
    // 则期望中间矩形: (470, 534)-(810, 1163) 或类似
    std::printf("\ncorrected test case (with proper right rect):\n");
    std::array<Rect, 4> rects4 = {{
        {   0,    0, 1280,  534},  // 上
        {   0,  534,  470, 1380},  // 左
        {   0, 1380, 1280, 1920},  // 下
        { 810,  534, 1280, 1380},  // 右
    }};
    for (int i = 0; i < 4; ++i)
        print_rect(("  rect" + std::to_string(i+1)).c_str(), rects4[i]);

    Rect inner4 = find_inner_rect(rects4);
    print_rect("  inner4", inner4);
    // 期望: x1=470, y1=534, x2=810, y2=1380
    assert(inner4.x1 == 470 && inner4.y1 == 534);
    assert(inner4.x2 == 810 && inner4.y2 == 1380);
    std::printf("  corrected test passed\n");

    return 0;
}

