#include <iostream>
#include <graphics.h> // easyx图形库，需要安装之后才能使用
#include <conio.h> // 不是标准头文件，不是所有编译器都支持
#include "resource.h"
#include "config.h"

/**
 * 游戏-2048(4x4)(play2048.co)
 * 玩法介绍：按上（格子上面有空格则移动）下左（合并相邻相同项目）右，胜利（棋盘上出现2048）
 * 界面(n=4)：n*n小格（格间隔 n+1 15），格子大小(100*100)
 * 数据：二维数组
 */


enum GridBGColor {
	BG_WIN		 = RGB(187, 173, 160),	// 窗口背景颜色
	GBG_NUM_0    = RGB(205, 193, 180),	// 数字0格子背景
	GBG_NUM_2    = RGB(238, 228, 218),	// 数字2格子背景
	GBG_NUM_4    = RGB(238, 225, 201),	// 数字4格子背景
	GBG_NUM_8    = RGB(243, 178, 122),	// 数字8格子背景
	GBG_NUM_16   = RGB(246, 150, 100),	// 数字16格子背景
	GBG_NUM_32	 = RGB(247, 124, 95),	// 数字32格子背景
	GBG_NUM_64	 = RGB(247, 95, 59),	// 数字64格子背景
	GBG_NUM_128	 = RGB(242, 177, 121),	// 数字128格子背景
	GBG_NUM_256	 = RGB(237, 204, 97),	// 数字256格子背景
	GBG_NUM_512	 = RGB(255, 0, 128),	// 数字512格子背景
	GBG_NUM_1024 = RGB(145, 0, 72),		// 数字1024格子背景
	GBG_NUM_2048 = RGB(242, 17, 158),	// 数字2048格子背景
};

GridBGColor colorArr[13] = {GBG_NUM_0, GBG_NUM_2, GBG_NUM_4, GBG_NUM_8, GBG_NUM_16, GBG_NUM_32, GBG_NUM_64, GBG_NUM_128, GBG_NUM_256, GBG_NUM_512, GBG_NUM_1024, GBG_NUM_2048, BG_WIN};
// 存储数据，全局变量，自动初始化为0
unsigned short dataMap[GRID_ITEMS][GRID_ITEMS]{};
unsigned char freqCountMerged = 0; // 每次操作后数字合并次数
struct FlagControl {
	bool is_playing = false; // 是否在玩
	bool is_failed = false; // 是否失败
	bool is_exit = false; // 是否退出
} flagControl;


// 随机产生整数 2|4 2概率大于4 需要调用方自行播种
unsigned short randNumGenerator()
{
	return (rand()%6 == 0)?4:2;
}

void dataMapFill(unsigned short freqFill)
{
	for (unsigned char i = 0; i < freqFill; )
	{
		unsigned char x = rand() % GRID_ITEMS;
		unsigned char y = rand() % GRID_ITEMS;
		if (dataMap[x][y] == 0) {
			dataMap[x][y] = randNumGenerator();
			i++;
		}
	}
}

bool checkerFail()
{
	unsigned char numLoad = 0; // 非0负荷
	for (unsigned char row = 0; row < GRID_ITEMS; row++)
	{
		for (unsigned char col = 0; col < GRID_ITEMS; col++)
		{

			if (dataMap[row][col] == NUM_WINNER) return true;
			if (dataMap[row][col] != 0) numLoad++;
		}
	}
	if (numLoad == (GRID_ITEMS * GRID_ITEMS)) return true;
	return false;
}

void GameInit()
{
	srand((unsigned int)time(NULL) + clock()); // 播种: 设置随机数种子
	initgraph(WIN_SIZE, WIN_SIZE); // 初始化窗口

	flagControl.is_playing = false;
	flagControl.is_failed = false;
	flagControl.is_exit = false;

	dataMapFill(2);
}

void GameDraw()
{
	BeginBatchDraw();
	// 设置背景颜色(RGB) & 清屏
	setbkcolor(COLOR_BG);
	cleardevice();

	// 实心矩形(solidrectangle)绘制, 空心矩形(rectangle) rect 直角 round 圆角
	for (unsigned char row = 0; row < GRID_ITEMS; row++) // 行
	{
		for (unsigned char col = 0; col < GRID_ITEMS; col++) // 列
		{
			// 左顶点坐标
			unsigned short currentGridLTAxisX = row * GRID_SIZE + (row + 1) * GRID_INTERVAL;
			unsigned short currentGridLTAxisY = col * GRID_SIZE + (col + 1) * GRID_INTERVAL;
			unsigned short currentDataNum = dataMap[col][row]; // 当前格子数字
			unsigned char tmpIdx = (unsigned char)log2((double)currentDataNum); // 临时索引
			COLORREF tColor = colorArr[tmpIdx];
			setfillcolor(tColor); // 设置填充颜色
			solidroundrect(currentGridLTAxisX, currentGridLTAxisY, currentGridLTAxisX + GRID_SIZE, currentGridLTAxisY + GRID_SIZE, GRID_ROUND_SIZE, GRID_ROUND_SIZE);
			// 绘制dataMap到图形窗口
			if (currentDataNum) { // 非0绘制
				settextstyle(CHAR_AVG_HEIGHT, CHAR_AVG_WIDHT, CHAR_FONT); // 设置文字样式
				setbkmode(TRANSPARENT); // 跟随父级
				settextcolor(RGB(119, 110, 101)); // 设置文字颜色
				char tmpStr[5]; // 2048+\0
				sprintf_s(tmpStr, "%d", currentDataNum); // sprintf的安全版本(safe)
				// 项目属性 -> 属性 -> 高级 -> 字符集 -> 多字节字符集
				// 计算字符串的宽度(textwidth)和高度(textheight)
				outtextxy(currentGridLTAxisX + GRID_SIZE / 2 - textwidth(tmpStr) / 2, currentGridLTAxisY + GRID_SIZE / 2 - textheight(tmpStr) / 2, tmpStr);
			}
		}
	}

	EndBatchDraw();
}
// 上移 数据从上开始循环
void directUp()
{
	for (unsigned short row = 0; row < GRID_ITEMS; row++) // 行
	{
		unsigned short tmpRow = 0; // 默认第一行 [0, 0-3] 逐列检测
		for (unsigned short col = 1;  col < GRID_ITEMS;  col++) // 列
		{
			if (dataMap[col][row] != 0) { // 当前元素非0 [col(1-3), row] != 0
				if (dataMap[tmpRow][row] == 0) { // 上移
					dataMap[tmpRow][row] = dataMap[col][row];
					dataMap[col][row] = 0;
				} else if (dataMap[tmpRow][row] == dataMap[col][row]) { // 相等即合并
					dataMap[tmpRow][row] += dataMap[col][row];
					dataMap[col][row] = 0;
					tmpRow++; // 向下偏移，每次操作一步
					freqCountMerged++;
				} else { // 非0且不等
					if (dataMap[tmpRow + 1][row] == 0) {
						dataMap[tmpRow + 1][row] = dataMap[col][row];
						if ((tmpRow + 1) != col) {
							dataMap[col][row] = 0;
							freqCountMerged++;
						}
					}
					tmpRow++;
				}
			}
		}
	}
}

void directDown()
{
	// row && col < GRID_ITEMS 防止负数溢出 65535
	for (unsigned short row = GRID_ITEMS - 1; row >=0 && row < GRID_ITEMS; row--)
	{
		unsigned short tmpRow = GRID_ITEMS - 1;
		for (unsigned short col = GRID_ITEMS - 2; col >= 0 && col < GRID_ITEMS; col--)
		{
			if (dataMap[col][row] != 0) {
				if (dataMap[tmpRow][row] == 0) { // 下移
					dataMap[tmpRow][row] = dataMap[col][row];
					dataMap[col][row] = 0;
				} else if (dataMap[tmpRow][row] == dataMap[col][row]) { // 相等即合并
					dataMap[tmpRow][row] += dataMap[col][row];
					dataMap[col][row] = 0;
					tmpRow--; // 向上偏移，每次操作一步
					freqCountMerged++;
				} else { // 非0且不等
					if (dataMap[tmpRow - 1][row] == 0) {
						dataMap[tmpRow - 1][row] = dataMap[col][row];
						if ((tmpRow - 1) != col) {
							dataMap[col][row] = 0;
							freqCountMerged++;
						}
					}
					tmpRow--;
				}
			}
		}
	}
}

void directLeft()
{
	for (unsigned short row = 0; row < GRID_ITEMS; row++)
	{
		unsigned short tmpCol = 0; // 默认第一列 [0, 0-3] 逐列检测
		for (unsigned short col = 1; col < GRID_ITEMS; col++)
		{
			if (dataMap[row][col] != 0) {
				if (dataMap[row][tmpCol] == 0) { // 左移
					dataMap[row][tmpCol] = dataMap[row][col];
					dataMap[row][col] = 0;
				} else if (dataMap[row][tmpCol] == dataMap[row][col]) { // 相等即合并
					dataMap[row][tmpCol] += dataMap[row][col];
					dataMap[row][col] = 0;
					tmpCol++; // 向右偏移
					freqCountMerged++;
				} else { // 非0且不等
					if (dataMap[row][tmpCol + 1] == 0) {
						dataMap[row][tmpCol + 1] = dataMap[row][col];
						if ((tmpCol + 1) != col) {
							dataMap[row][col] = 0;
							freqCountMerged++;
						}
					}
					tmpCol++;
				}
			}
		}
	}
}

void directRight()
{
	for (unsigned short row = GRID_ITEMS - 1; row >= 0 && row < GRID_ITEMS; row--)
	{
		unsigned short tmpCol = GRID_ITEMS - 1;
		for (unsigned short col = GRID_ITEMS - 2; col >= 0 && col < GRID_ITEMS; col--)
		{
			if (dataMap[row][col] != 0) {
				if (dataMap[row][tmpCol] == 0) { // 左移
					dataMap[row][tmpCol] = dataMap[row][col];
					dataMap[row][col] = 0;
				}
				else if (dataMap[row][tmpCol] == dataMap[row][col]) { // 相等即合并
					dataMap[row][tmpCol] += dataMap[row][col];
					dataMap[row][col] = 0;
					tmpCol--; // 向左偏移
					freqCountMerged++;
				} else { // 非0且不等
					if (dataMap[row][tmpCol - 1] == 0) {
						dataMap[row][tmpCol - 1] = dataMap[row][col];
						if ((tmpCol - 1) != col) {
							dataMap[row][col] = 0;
							freqCountMerged++;
						}
					}
					tmpCol--;
				}
			}
		}
	}
}

void KeyListener()
{
	freqCountMerged = 0; // 格式化合并次数
	char key = _getch();
	switch (key)
	{
		case 'w': // up
		case 'W':
		case 72:
			directUp();
			break;
		case 's': // down
		case 'S':
		case 80:
			directDown();
			break;
		case 'a': // left
		case 'A':
		case 75:
			directLeft();
			break;
		case 'd': // right
		case 'D':
		case 77:
			directRight();
			break;
	}
	if (freqCountMerged) dataMapFill(1);
}

void GameUpdate()
{
	flagControl.is_failed = checkerFail();
	if (!flagControl.is_failed) {
		KeyListener();
	}
}

int main()
{
	GameInit();
	while (1) {
		GameDraw();
		GameUpdate();
	}
	closegraph();
}