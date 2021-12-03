#pragma once

#define CHAR_AVG_HEIGHT 50 // 字符的平均高度, 在settextstyle中会使用
#define CHAR_AVG_WIDHT 0 // 字符的平均宽度(0自适应)
#define CHAR_FONT "黑体" // 字体名称
#define COLOR_BG RGB(187, 173, 160) // 背景颜色
#define COLOR_SOID_DEFAULT RGB(205, 193, 180, 0) // 格子填充初始颜色
#define GRID_SIZE 100 // 格子大小
#define GRID_ROUND_SIZE 10 // 圆角大小
#define GRID_ITEMS 4 // 每组格子数量
#define GRID_INTERVAL 15 // 格子之间的间隔
#define WIN_SIZE (GRID_SIZE * GRID_ITEMS + GRID_INTERVAL * (GRID_ITEMS + 1)) // 窗口大小
#define NUM_WINNER 2048