#include <iostream>
#include <graphics.h> // easyxͼ�ο⣬��Ҫ��װ֮�����ʹ��
#include <conio.h> // ���Ǳ�׼ͷ�ļ����������б�������֧��
#include "resource.h"
#include "config.h"

/**
 * ��Ϸ-2048(4x4)(play2048.co)
 * �淨���ܣ����ϣ����������пո����ƶ������󣨺ϲ�������ͬ��Ŀ���ң�ʤ���������ϳ���2048��
 * ����(n=4)��n*nС�񣨸��� n+1 15�������Ӵ�С(100*100)
 * ���ݣ���ά����
 */


enum GridBGColor {
	BG_WIN		 = RGB(187, 173, 160),	// ���ڱ�����ɫ
	GBG_NUM_0    = RGB(205, 193, 180),	// ����0���ӱ���
	GBG_NUM_2    = RGB(238, 228, 218),	// ����2���ӱ���
	GBG_NUM_4    = RGB(238, 225, 201),	// ����4���ӱ���
	GBG_NUM_8    = RGB(243, 178, 122),	// ����8���ӱ���
	GBG_NUM_16   = RGB(246, 150, 100),	// ����16���ӱ���
	GBG_NUM_32	 = RGB(247, 124, 95),	// ����32���ӱ���
	GBG_NUM_64	 = RGB(247, 95, 59),	// ����64���ӱ���
	GBG_NUM_128	 = RGB(242, 177, 121),	// ����128���ӱ���
	GBG_NUM_256	 = RGB(237, 204, 97),	// ����256���ӱ���
	GBG_NUM_512	 = RGB(255, 0, 128),	// ����512���ӱ���
	GBG_NUM_1024 = RGB(145, 0, 72),		// ����1024���ӱ���
	GBG_NUM_2048 = RGB(242, 17, 158),	// ����2048���ӱ���
};

GridBGColor colorArr[13] = {GBG_NUM_0, GBG_NUM_2, GBG_NUM_4, GBG_NUM_8, GBG_NUM_16, GBG_NUM_32, GBG_NUM_64, GBG_NUM_128, GBG_NUM_256, GBG_NUM_512, GBG_NUM_1024, GBG_NUM_2048, BG_WIN};
// �洢���ݣ�ȫ�ֱ������Զ���ʼ��Ϊ0
unsigned short dataMap[GRID_ITEMS][GRID_ITEMS]{};
unsigned char freqCountMerged = 0; // ÿ�β��������ֺϲ�����
struct FlagControl {
	bool is_playing = false; // �Ƿ�����
	bool is_failed = false; // �Ƿ�ʧ��
	bool is_exit = false; // �Ƿ��˳�
} flagControl;


// ����������� 2|4 2���ʴ���4 ��Ҫ���÷����в���
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
	unsigned char numLoad = 0; // ��0����
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
	srand((unsigned int)time(NULL) + clock()); // ����: �������������
	initgraph(WIN_SIZE, WIN_SIZE); // ��ʼ������

	flagControl.is_playing = false;
	flagControl.is_failed = false;
	flagControl.is_exit = false;

	dataMapFill(2);
}

void GameDraw()
{
	BeginBatchDraw();
	// ���ñ�����ɫ(RGB) & ����
	setbkcolor(COLOR_BG);
	cleardevice();

	// ʵ�ľ���(solidrectangle)����, ���ľ���(rectangle) rect ֱ�� round Բ��
	for (unsigned char row = 0; row < GRID_ITEMS; row++) // ��
	{
		for (unsigned char col = 0; col < GRID_ITEMS; col++) // ��
		{
			// �󶥵�����
			unsigned short currentGridLTAxisX = row * GRID_SIZE + (row + 1) * GRID_INTERVAL;
			unsigned short currentGridLTAxisY = col * GRID_SIZE + (col + 1) * GRID_INTERVAL;
			unsigned short currentDataNum = dataMap[col][row]; // ��ǰ��������
			unsigned char tmpIdx = (unsigned char)log2((double)currentDataNum); // ��ʱ����
			COLORREF tColor = colorArr[tmpIdx];
			setfillcolor(tColor); // ���������ɫ
			solidroundrect(currentGridLTAxisX, currentGridLTAxisY, currentGridLTAxisX + GRID_SIZE, currentGridLTAxisY + GRID_SIZE, GRID_ROUND_SIZE, GRID_ROUND_SIZE);
			// ����dataMap��ͼ�δ���
			if (currentDataNum) { // ��0����
				settextstyle(CHAR_AVG_HEIGHT, CHAR_AVG_WIDHT, CHAR_FONT); // ����������ʽ
				setbkmode(TRANSPARENT); // ���游��
				settextcolor(RGB(119, 110, 101)); // ����������ɫ
				char tmpStr[5]; // 2048+\0
				sprintf_s(tmpStr, "%d", currentDataNum); // sprintf�İ�ȫ�汾(safe)
				// ��Ŀ���� -> ���� -> �߼� -> �ַ��� -> ���ֽ��ַ���
				// �����ַ����Ŀ��(textwidth)�͸߶�(textheight)
				outtextxy(currentGridLTAxisX + GRID_SIZE / 2 - textwidth(tmpStr) / 2, currentGridLTAxisY + GRID_SIZE / 2 - textheight(tmpStr) / 2, tmpStr);
			}
		}
	}

	EndBatchDraw();
}
// ���� ���ݴ��Ͽ�ʼѭ��
void directUp()
{
	for (unsigned short row = 0; row < GRID_ITEMS; row++) // ��
	{
		unsigned short tmpRow = 0; // Ĭ�ϵ�һ�� [0, 0-3] ���м��
		for (unsigned short col = 1;  col < GRID_ITEMS;  col++) // ��
		{
			if (dataMap[col][row] != 0) { // ��ǰԪ�ط�0 [col(1-3), row] != 0
				if (dataMap[tmpRow][row] == 0) { // ����
					dataMap[tmpRow][row] = dataMap[col][row];
					dataMap[col][row] = 0;
				} else if (dataMap[tmpRow][row] == dataMap[col][row]) { // ��ȼ��ϲ�
					dataMap[tmpRow][row] += dataMap[col][row];
					dataMap[col][row] = 0;
					tmpRow++; // ����ƫ�ƣ�ÿ�β���һ��
					freqCountMerged++;
				} else { // ��0�Ҳ���
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
	// row && col < GRID_ITEMS ��ֹ������� 65535
	for (unsigned short row = GRID_ITEMS - 1; row >=0 && row < GRID_ITEMS; row--)
	{
		unsigned short tmpRow = GRID_ITEMS - 1;
		for (unsigned short col = GRID_ITEMS - 2; col >= 0 && col < GRID_ITEMS; col--)
		{
			if (dataMap[col][row] != 0) {
				if (dataMap[tmpRow][row] == 0) { // ����
					dataMap[tmpRow][row] = dataMap[col][row];
					dataMap[col][row] = 0;
				} else if (dataMap[tmpRow][row] == dataMap[col][row]) { // ��ȼ��ϲ�
					dataMap[tmpRow][row] += dataMap[col][row];
					dataMap[col][row] = 0;
					tmpRow--; // ����ƫ�ƣ�ÿ�β���һ��
					freqCountMerged++;
				} else { // ��0�Ҳ���
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
		unsigned short tmpCol = 0; // Ĭ�ϵ�һ�� [0, 0-3] ���м��
		for (unsigned short col = 1; col < GRID_ITEMS; col++)
		{
			if (dataMap[row][col] != 0) {
				if (dataMap[row][tmpCol] == 0) { // ����
					dataMap[row][tmpCol] = dataMap[row][col];
					dataMap[row][col] = 0;
				} else if (dataMap[row][tmpCol] == dataMap[row][col]) { // ��ȼ��ϲ�
					dataMap[row][tmpCol] += dataMap[row][col];
					dataMap[row][col] = 0;
					tmpCol++; // ����ƫ��
					freqCountMerged++;
				} else { // ��0�Ҳ���
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
				if (dataMap[row][tmpCol] == 0) { // ����
					dataMap[row][tmpCol] = dataMap[row][col];
					dataMap[row][col] = 0;
				}
				else if (dataMap[row][tmpCol] == dataMap[row][col]) { // ��ȼ��ϲ�
					dataMap[row][tmpCol] += dataMap[row][col];
					dataMap[row][col] = 0;
					tmpCol--; // ����ƫ��
					freqCountMerged++;
				} else { // ��0�Ҳ���
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
	freqCountMerged = 0; // ��ʽ���ϲ�����
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