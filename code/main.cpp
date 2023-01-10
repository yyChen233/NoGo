//不围棋(NoGo) 
//版本号：5.2.2
//日期：2021/1/17
#include<iostream>
#include<cstdio>
#include<algorithm>
#include<string>
#include<cstring>
#include<cmath>
#include<ctime>
#include<easyx.h>
#include<graphics.h>
#include<conio.h>
#include<iomanip>
#include<Windows.h>
#include<fstream>
#include<vector>
#pragma comment(lib,"Winmm.lib")
#define High 623
#define Width 600
#define inf 10000000
using namespace std;

//函数签名---------------------------------------------------------------------------------------------------------------

int startMenu();//函数功能：游戏开始界面以及菜单界面

void chessboard();//函数功能：利用easyX绘制棋盘

void updateWithInput();//函数功能：实现鼠标交互

int findLocation(int x, int mod);//函数功能：找到最近的格点(mod==0表示找横坐标x，mod==1表示找纵坐标y)

int startUp();//函数功能：数据的初始化

void rules();//函数功能：介绍规则

void show();//函数功能：显示画面

void chooseDifficulty();//函数功能：选择游戏难度

bool inBoard(int x, int y);//函数功能：判断点是否在棋盘内部

bool haveAir(int fx, int fy);//函数功能：判断是否有“气”，true：有“气”

void decideByComputer();//函数功能：电脑决策并落子（低阶随机版）

int winOrLose();//函数功能：判断是否胜利

void endGame(int mod);//函数功能：游戏的结束界面

void readRecordFile(int(*board)[10]);//函数功能：读取游戏数据文件存档

void writeRecordFile(int n, int(*board)[10]);//函数功能：存储游戏数据文件存档

//全局变量---------------------------------------------------------------------------------------------------------------

int cnt = 0, cnt_w = 0, cnt_b = 0;//用于计数
int gamemode = 0;//游戏模式
int state;//游戏状态
int difficulty = 0;//游戏难度
int locations_x[10][10] = { 0 };//用于储存格点位置x
int locations_y[10][10] = { 0 };//用于储存格点位置y
int sum = 0;//用于计算“气”
int table[10][10] = { 0 };//用于记录棋子，以便接下来的判断，0表示空，1表示黑子，-1表示白子
bool haveAir_visited[10][10] = { false };//用于递归
int air[10][10] = { 0 };//用于储存“气”的计算值
int flag = 0, judge = 0;//用于判断
IMAGE background;//定义背景图片
int nowx, nowy, nx, ny;//辅助记忆
int di[4] = { 0,0,1,-1 };//用于搜索
int dj[4] = { 1,-1,0,0 };//用于搜索

//函数的具体实现------------------------------------------------------------------------------------------------------------

int startMenu()
{
	//easyX代码
	//显示界面
	setbkmode(TRANSPARENT);
	settextcolor(BLACK);
	settextstyle(70, 0, _T("楷体"));
	outtextxy(Width * 0.2, High * 0.3, _T("不围棋 NoGo"));
	settextstyle(44, 0, _T("楷体"));
	outtextxy(Width * 0.32, High * 0.55, _T("1.开始新游戏"));
	outtextxy(Width * 0.32, High * 0.65, _T("2.继续游戏"));
	outtextxy(Width * 0.32, High * 0.75, _T("3.退出游戏"));
	MOUSEMSG m;//定义鼠标信息
	while (1)
	{
		m = GetMouseMsg();//读取鼠标信息
		if (m.uMsg == WM_LBUTTONDOWN)//按下鼠标左键
		{
			mciSendString(_T("close jpmusic"), NULL, 0, NULL);
			mciSendString(_T("open ..\\music\\落子.wav alias jpmusic"), NULL, 0, NULL);
			mciSendString(_T("play jpmusic"), NULL, 0, NULL);
			if (m.x >= Width * 0.32 && m.y <= High * 0.61 && m.y >= High * 0.55 && m.x <= Width * 0.75)
			{
				rules();//显示界面
			}
			else if (m.y >= High * 0.65 && m.x >= Width * 0.32 && m.y <= High * 0.71 && m.x <= Width * 0.7)
			{
				readRecordFile(table);//读档
				state = 1;
				if (winOrLose() != 0)
					state = 2;
				return state;
			}
			else if (m.y >= High * 0.75 && m.x >= Width * 0.32 && m.y <= High * 0.81 && m.x <= Width * 0.7)
				exit(0);
			else
				continue;
			return state;
		}
	}

}

void draw()
{
	//在读档之后恢复棋盘界面
	for (int i = 0; i < 10; i++)
		for (int j = 0; j < 10; j++)
		{
			if (table[i][j] == 1)
			{
				setfillcolor(BLACK);
				setlinecolor(BLACK);
				fillcircle(i * 60, j * 60, 27);
			}
			else if (table[i][j] == -1)
			{
				setfillcolor(WHITE);
				setlinecolor(WHITE);
				fillcircle(i * 60, j * 60, 27);
			}
		}
}

void chessboard()
{
	cleardevice();//清屏
	putimage(0, 0, &background);
	int step = 60;//用于定位
	setlinestyle(PS_SOLID, 2);//画实线，宽度为2个像素
	setcolor(RGB(0, 0, 0));//设置为黑色

	for (int i = 1; i <= 9; i++)//画横线、竖线
	{
		line(i * step, 1 * step, i * step, 9 * step);
		line(1 * step, i * step, 9 * step, i * step);
	}
	//记录每一个点的坐标
	for (int i = 1; i <= 9; i++)
	{
		for (int j = 1; j <= 9; j++)
		{
			locations_x[i][j] = i * step;
			locations_y[i][j] = j * step;
			setfillcolor(BLACK);
			fillcircle(locations_x[i][j], locations_y[i][j], 2);

		}
	}
	setbkmode(TRANSPARENT);
	settextcolor(BLACK);
	settextstyle(29, 0, _T("仿宋"), 0, 0, 1000, 0, 0, 0);
	outtextxy(Width * 0.18, High * 0.94, _T("不围棋"));
	settextstyle(29, 0, _T("黑体"), 0, 0, 1000, 0, 0, 0);
	outtextxy(Width * 0.38, High * 0.94, _T("保存并退出"));
	outtextxy(Width * 0.7, High * 0.94, _T("NoGo"));
}

void updateWithInput()
{
	MOUSEMSG m;//定义鼠标信息
	while (1)
	{
		m = GetMouseMsg();//读取鼠标信息
		if (m.uMsg == WM_LBUTTONDOWN)
		{
			if (m.x >= Width * 0.35 && m.x <= Width * 0.65 && m.y >= High * 0.94)
			{
				writeRecordFile(cnt, table);//存档
				exit(0);//退出游戏
			}
			else
			{
				if (cnt % 2 == 1)
				{
					//绘制棋子
					setfillcolor(WHITE);
					setlinecolor(WHITE);
					int mmx = findLocation(m.x, 0);
					int mmy = findLocation(m.y, 1);
					if (table[mmx / 60][mmy / 60] != 0)//位置不合法（已被占用）
					{
						flag = 1;
						return;
					}
					//落子音效
					mciSendString(_T("close jpmusic"), NULL, 0, NULL);
					mciSendString(_T("open ..\\music\\落子.wav alias jpmusic"), NULL, 0, NULL);
					mciSendString(_T("play jpmusic"), NULL, 0, NULL);
					fillcircle(mmx, mmy, 27);
					table[mmx / 60][mmy / 60] = -1;
					nowx = mmx / 60; nowy = mmy / 60;
				}
				else
				{
					//同上
					int mmx = findLocation(m.x, 0);
					int mmy = findLocation(m.y, 1);
					if (table[mmx / 60][mmy / 60] != 0)
					{
						flag = 1;
						return;
					}
					if (mmx == locations_x[5][5] && mmy == locations_y[5][5] && cnt == 0)
					{
						flag = 1;
						return;
					}
					mciSendString(_T("close jpmusic"), NULL, 0, NULL);
					mciSendString(_T("open ..\\music\\落子.wav alias jpmusic"), NULL, 0, NULL);
					mciSendString(_T("play jpmusic"), NULL, 0, NULL);

					if (gamemode == 1)
					{
						setfillcolor(BLACK);
						setlinecolor(BLACK); table[mmx / 60][mmy / 60] = 1;
					}
					else if (gamemode == 2)
					{
						setfillcolor(WHITE);
						setlinecolor(WHITE); table[mmx / 60][mmy / 60] = -1;
					}
					fillcircle(mmx, mmy, 27);
					nowx = mmx / 60; nowy = mmy / 60;
				}
			}
			return;
		}
	}
}

int findLocation(int x, int mod)
{
	//定位
	int mini = 1000000;
	int mini_x = 0, mini_y = 0;
	if (mod == 0)
	{
		for (int i = 1; i <= 9; i++)
			for (int j = 1; j <= 9; j++)
			{
				if (abs(locations_x[i][j] - x) < mini)
				{
					//寻找最近的格点（x坐标）
					mini = abs(locations_x[i][j] - x);
					mini_x = locations_x[i][j];
				}
			}
		return mini_x;
	}
	if (mod == 1)
	{
		for (int i = 1; i <= 9; i++)
			for (int j = 1; j <= 9; j++)
			{
				if (abs(locations_y[i][j] - x) < mini)
				{
					//寻找最近的格点（y坐标）
					mini = abs(locations_y[i][j] - x);
					mini_y = locations_y[i][j];
				}
			}
		return mini_y;
	}
}

int startUp()
{
	initgraph(Width, High);//初始化界面
	mciSendString(_T("open ..\\music\\bgm.mp3 alias bkmusic"), NULL, 0, NULL);
	mciSendString(_T("play bkmusic repeat"), NULL, 0, NULL);
	//获取窗口句柄
	HWND hwnd = GetHWnd();
	//设置窗口标题文字
	SetWindowText(hwnd, _T("NoGo 不围棋 By 陈奕阳"));
	loadimage(&background, _T("..\\picture\\chessboard.png"));//载入背景
	putimage(0, 0, &background);
	int a = startMenu();
	if (a == 1)
	{
		cleardevice();
		putimage(0, 0, &background);
		chessboard();
		draw();
	}
	else if (a == 2)
	{
		cleardevice();
		putimage(0, 0, &background);
		chessboard();
		draw();
		MOUSEMSG s;
		while (1)
		{
			s = GetMouseMsg();
			if (s.uMsg == WM_LBUTTONDOWN)
			{
				if (cnt % 2 == 0 && haveAir(nx, ny) == false)
				{
					endGame(0);
					_getch();
					closegraph();
					return 1;
				}
				else
				{
					endGame(winOrLose());
					_getch();
					closegraph();
					return 1;
				}
			}
		}
	}
	return 0;
}

void rules()
{
	cleardevice();//清屏
	loadimage(&background, _T("..\\picture\\chessboard.png"));//载入背景
	putimage(0, 0, &background);
	setbkmode(TRANSPARENT);
	settextcolor(BLACK);
	settextstyle(50, 0, _T("楷体"));
	outtextxy(Width * 0.26, High * 0.12, _T("游戏规则简介"));
	settextstyle(23, 0, _T("楷体"));
	outtextxy(Width * 0.05, High * 0.25, _T("1.黑子先手，双方轮流落子，落子后棋子不可移动；"));
	outtextxy(Width * 0.05, High * 0.30, _T("2.对弈的目标不是吃掉对方的棋子，不是占领地盘；"));
	outtextxy(Width * 0.05, High * 0.35, _T("3.若一方落子后吃掉了对方的棋子，落子一方判负；"));
	outtextxy(Width * 0.05, High * 0.40, _T("4.对弈禁止自杀，落子自杀一方判负；"));
	outtextxy(Width * 0.05, High * 0.45, _T("5.对弈禁止空手(pass)，空手一方判负；"));
	outtextxy(Width * 0.05, High * 0.50, _T("6.对弈结果只有胜负，没有和棋。"));
	outtextxy(Width * 0.40, High * 0.76, _T("特别鸣谢："));
	outtextxy(Width * 0.25, High * 0.80, _T("北京大学信息科学技术学院"));
	outtextxy(Width * 0.26, High * 0.84, _T("Microsoft Visual Studio"));
	outtextxy(Width * 0.37, High * 0.88, _T("CSDN、中国知网"));
	settextstyle(50, 0, _T("楷体"));
	outtextxy(Width * 0.11, High * 0.60, _T("点击此处以开始游戏！"));
	settextstyle(23, 0, _T("楷体"));
	MOUSEMSG m;//定义鼠标信息
	while (1)
	{
		m = GetMouseMsg();
		if (m.uMsg == WM_LBUTTONDOWN)
		{
			mciSendString(_T("close jpmusic"), NULL, 0, NULL);
			mciSendString(_T("open ..\\music\\落子.wav alias jpmusic"), NULL, 0, NULL);
			mciSendString(_T("play jpmusic"), NULL, 0, NULL);
			if (m.x >= Width * 0.11 && m.x <= Width * 0.90 && m.y >= High * 0.60 && m.y <= High * 0.68)
			{
				show();
				break;
			}
		}
	}
	
}

void show()
{
	cleardevice();//清屏
	loadimage(&background, _T("..\\picture\\chessboard.png"));//载入背景
	putimage(0, 0, &background);
	setbkmode(TRANSPARENT);
	settextcolor(BLACK);
	
	MOUSEMSG m;
	settextstyle(60, 0, _T("楷体"));
	outtextxy(Width * 0.09, High * 0.3, _T("请选择先后手顺序:"));
	settextstyle(44, 0, _T("楷体"));
	outtextxy(Width * 0.32, High * 0.55, _T("1.玩家先手"));
	outtextxy(Width * 0.32, High * 0.65, _T("2.玩家后手"));
	while (1)
	{
		m = GetMouseMsg();
		if (m.uMsg == WM_LBUTTONDOWN)
		{
			mciSendString(_T("close jpmusic"), NULL, 0, NULL);
			mciSendString(_T("open ..\\music\\落子.wav alias jpmusic"), NULL, 0, NULL);
			mciSendString(_T("play jpmusic"), NULL, 0, NULL);
			if (m.x >= Width * 0.32 && m.y <= High * 0.61 && m.y >= High * 0.55 && m.x <= Width * 0.75)
			{
				cleardevice();
				putimage(0, 0, &background);
				cnt = 0; gamemode = 1;
				chooseDifficulty();
				
			}
			else if (m.y >= High * 0.65 && m.x >= Width * 0.32 && m.y <= High * 0.71 && m.x <= Width * 0.7)
			{
				cleardevice();
				putimage(0, 0, &background);
				cnt = 1; gamemode = 2;
				chooseDifficulty();
			}
			else
				continue;
			return;
		}
	}
}

void chooseDifficulty()
{
	setbkmode(TRANSPARENT);
	settextcolor(BLACK);
	settextstyle(60, 0, _T("楷体"));
	outtextxy(Width * 0.15, High * 0.3, _T("请选择游戏难度:"));
	settextstyle(44, 0, _T("楷体"));
	outtextxy(Width * 0.38, High * 0.55, _T("1.初阶"));
	outtextxy(Width * 0.38, High * 0.65, _T("2.中阶"));
	outtextxy(Width * 0.38, High * 0.75, _T("3.高阶"));
	MOUSEMSG m;//定义鼠标信息
	while (1)
	{
		m = GetMouseMsg();
		if (m.uMsg == WM_LBUTTONDOWN)
		{
			mciSendString(_T("close jpmusic"), NULL, 0, NULL);
			mciSendString(_T("open ..\\music\\落子.wav alias jpmusic"), NULL, 0, NULL);
			mciSendString(_T("play jpmusic"), NULL, 0, NULL);
			if (m.x >= Width * 0.32 && m.y <= High * 0.61 && m.y >= High * 0.55 && m.x <= Width * 0.75)
			{
				difficulty = 1;
				chessboard();
			}
			else if (m.y >= High * 0.65 && m.x >= Width * 0.32 && m.y <= High * 0.71 && m.x <= Width * 0.7)
			{
				difficulty = 2;
				chessboard();
			}
			else if (m.y >= High * 0.75 && m.x >= Width * 0.32 && m.y <= High * 0.81 && m.x <= Width * 0.7)
			{
				difficulty = 3;
				chessboard();
			}
			else
				continue;
			return;
		}
	}
}

void decideByComputer()
{
	//随机播种
	srand((unsigned)time(NULL));
	int mmx = 0, mmy = 0;
	while (1)
	{
		mmx = rand() % 9 + 1;
		mmy = rand() % 9 + 1;
		if (table[mmx][mmy] == 0)
		{
			table[mmx][mmy] = (gamemode == 1) ? -1 : 1;
			if (haveAir(mmx, mmy) == 1)
			{
				table[mmx][mmy] = 0;
				break;
			}

		}
	}
	Sleep(400);
	mciSendString(_T("close jpmusic"), NULL, 0, NULL);
	mciSendString(_T("open ..\\music\\落子.wav alias jpmusic"), NULL, 0, NULL);
	mciSendString(_T("play jpmusic"), NULL, 0, NULL);
	if (gamemode == 1)
	{
		setfillcolor(WHITE);
		setlinecolor(WHITE); table[mmx][mmy] = -1;
	}
	else if (gamemode == 2)
	{
		setfillcolor(BLACK);
		setlinecolor(BLACK); table[mmx][mmy] = 1;
	}
	fillcircle(mmx * 60, mmy * 60, 27);
}

struct decide_more
{
	//模拟落子
	bool goStep(int x, int y, int c)
	{
		table[x][y] = c;
		if (getQi(x, y) == 0) return false;
		return true;
	}
	//检查落子后是否吃子（就是查一下上下左右不同颜色棋的气数，没气了就被提了）
	bool checkQi(int x, int y)
	{
		for (int t = 0; t < 4; t++)
		{
			int tx = x + di[t], ty = y + dj[t];
			if (tx >= 1 && tx <= 9 && ty >= 1 && ty <= 9)
			{
				if (table[tx][ty] != 0 && getQi(tx, ty) == 0) return false;
			}
		}
		return true;
	}
	bool checkStep(int x, int y)
	{
		if (!inBoard(x, y)) return false;
		if (table[x][y] != 0) return false;
		if (x == 5 && y == 5 && cnt == 0) return false;
		return true;
	}
	//查这个点是否可以落子
	bool testgoStep(int x, int y, int c)
	{
		if (!checkStep(x, y)) return false;
		bool flag = goStep(x, y, c) && checkQi(x, y);
		ungoStep(x, y);
		return flag;
	}
	void ungoStep(int x, int y)
	{
		table[x][y] = 0;
	}
	int evaluate1(int colour)
	{
		int ans = 0;
		int opp_colour = (colour == 1) ? -1 : 1;
		for (int x = 1; x < 10; x++)
		{
			for (int y = 1; y < 10; y++)
			{
				if (table[x][y] != 0)
					continue;
				else
				{
					if (testgoStep(x, y, colour) == 1)
					{
						ans++;
						ungoStep(x, y);
					}
					if (testgoStep(x, y, opp_colour) == 1)
					{
						ans--;
						ungoStep(x, y);
					}
				}
			}
		}
		return ans;
	}
	int chk[15][15] = { 0 };
	int dfs(int x, int y, int colour)
	{
		if (chk[x][y] == -1)
			return 0;
		chk[x][y] = -1;
		int res = 0;
		for (int i = 0; i < 4; i++)
		{
			int tx = x + di[i], ty = y + dj[i];
			if (inBoard(tx, ty) == 1)
			{
				if (table[tx][ty] == 0 && !chk[tx][ty])
				{
					res++;
					chk[tx][ty] = 1;
				}
				if (table[tx][ty] == colour)
				{
					res += dfs(tx, ty, colour);
				}
			}
		}
		return res;
	}
	int getQi(int x, int y)
	{
		memset(chk, 0, sizeof(chk));
		return dfs(x, y, table[x][y]);
	}
	int decideByComputer_more()
	{
		int decideList_x[81], decideList_y[81];
		int valueList[10][10];
		int col;
		int mmx, mmy;
		int maxValue = -1000000;
		int maxNum = 0;
		if (gamemode == 1)col = -1;
		else col = 1;
		int Threshold = 18;
		for (int j = 1; j < 10; j++)
			for (int i = 1; i < 10; i++)
			{
				if (testgoStep(i, j, col) == 1)
				{
					table[i][j] = col;
					int qi = -getQi(i, j);
					if (cnt <= 20)
						valueList[i][j] = 3 * evaluate1(col) + qi;
					else
					{
						valueList[i][j] = evaluate1(col);
					}
					if (valueList[i][j] >= maxValue)
					{
						maxValue = valueList[i][j];
					}
					table[i][j] = 0;
				}
			}
		for (int i = 1; i < 10; i++)
			for (int j = 1; j < 10; j++)
			{
				if (valueList[i][j] == maxValue)
				{
					decideList_x[maxNum] = i;
					decideList_y[maxNum] = j;
					maxNum++;
				}
			}
		if (maxNum == 0)
			return 0;
		srand((unsigned)time(NULL));
		int position = rand() % maxNum;
		mmx = decideList_x[position];
		mmy = decideList_y[position];
		Sleep(300);

		mciSendString(_T("close jpmusic"), NULL, 0, NULL);
		mciSendString(_T("open ..\\music\\落子.wav alias jpmusic"), NULL, 0, NULL);
		mciSendString(_T("play jpmusic"), NULL, 0, NULL);
		if (gamemode == 1)
		{
			setfillcolor(WHITE);
			setlinecolor(WHITE); table[mmx][mmy] = -1;
		}
		else if (gamemode == 2)
		{
			setfillcolor(BLACK);
			setlinecolor(BLACK); table[mmx][mmy] = 1;
		}
		fillcircle(mmx * 60, mmy * 60, 27);
		return 1;
	}
}S;

struct decide_most
{
	int chk[15][15] = { 0 };
	int col[15][15], colcnt;
	//看当前位置或者当前位置所处同种颜色的棋块的“气”数
	int dfs(int x, int y, int colour)
	{
		if (chk[x][y] == -1)
			return 0;
		chk[x][y] = -1;
		int res = 0;
		for (int i = 0; i < 4; i++)
		{
			int tx = x + di[i], ty = y + dj[i];
			if (inBoard(tx, ty) == 1)
			{
				if (table[tx][ty] == 0 && !chk[tx][ty])
				{
					res++;
					chk[tx][ty] = 1;
				}
				if (table[tx][ty] == colour)
				{
					res += dfs(tx, ty, colour);
				}
			}
		}
		return res;
	}
	int getQi(int x, int y)
	{
		memset(chk, 0, sizeof(chk));
		return dfs(x, y, table[x][y]);
	}
	//模拟落子
	bool goStep(int x, int y, int c) 
	{
		table[x][y] = c;
		if (getQi(x, y) == 0) return false;
		return true;
	}
    //取消模拟落子，恢复状态
	void ungoStep(int x, int y) 
	{
		table[x][y] = 0;
	}
	//检查落子后是否吃子（就是查一下上下左右不同颜色棋的气数，没气了就被提了）
	bool checkQi(int x, int y) 
	{ 
		for (int t = 0; t < 4; t++) 
		{
			int tx = x + di[t], ty = y + dj[t];
			if (tx >= 1 && tx <= 9 && ty >= 1 && ty <= 9) 
			{
				if (table[tx][ty] != 0 && getQi(tx, ty) == 0) return false;
			}
		}
		return true;
	}
	bool checkStep(int x, int y) 
	{
		if (!inBoard(x,y)) return false;
		if (table[x][y] != 0) return false;
		if (x == 5 && y == 5 && cnt == 0) return false;
		return true;
	}
	//查这个点是否可以落子
	bool testgoStep(int x, int y, int c) 
	{
		if (!checkStep(x, y)) return false;
		bool flag = goStep(x, y, c) && checkQi(x, y);
		ungoStep(x, y);
		return flag;
	}
	void color(int x, int y) 
	{
		if (col[x][y]) return;
		col[x][y] = colcnt;
		for (int i = 0; i < 4; i++) 
		{
			int tx = x + di[i], ty = y + dj[i];
			if (tx >= 1 && tx <= 9 && ty >= 1 && ty <= 9 && table[tx][ty] == table[x][y]) 
				color(tx, ty);
		}
	}
	//统计棋局中同种颜色但不相邻色块个数
	int calc(int c)
	{
		colcnt = 0;
		memset(col, 0, sizeof(col));
		for (int i = 1; i <= 9; i++) 
		{
			for (int j = 1; j <= 9; j++) 
			{
				if (table[i][j] == c && col[i][j] == 0) 
				{
					++colcnt;
					color(i, j);
				}
			}
		}
		return colcnt;
	}
	//评估局面，求“净”可下手数
	int evaluate(int c) 
	{
		int res = 0;
		for (int j = 1; j <= 9; j++) 
		{
			for (int i = 1; i <= 9; i++) 
			{
				if (testgoStep(i, j, c *(-1))) --res;
				else if (testgoStep(i, j, c)) ++res;
			}
		}
		if (c == 1) res = -res;//若执黑棋就翻转一下，有利于后面搜索、计算
		return res;
	}
	//评估局面，与上一个函数相仿，但是不翻转
	int evaluate1(int c) 
	{
		int res = 0;
		for (int j = 1; j <= 9; j++)
		{
			for (int i = 1; i <= 9; i++)
			{
				if (testgoStep(i, j, c*(-1))) --res;
				if (testgoStep(i, j, c)) ++res;
			}
		}
		return res;
	}
	vector<pair<int, int> > vec, vec1;
	int bx, by, prevtime;
	//残局搜索函数
	int canItWin(int c, int dep) 
	{
		int th = evaluate(c);  //此时的Evaluate是计算当前未试下局面的优势手数
		int num = 0;
		if (clock() - prevtime > 1000) return -1;
		for (int j = 1; j <= 9; j++) 
		{
			for (int i = 1; i <= 9; i++) 
			{
				if (!checkStep(i, j)) continue;
				if (goStep(i, j, c) && checkQi(i, j)) 
				{
					if ((c == -1 && evaluate(c*(-1)) >= th) || (c == 1 && evaluate(c*(-1)) <= th)) 
					{
						num++;
						int r = canItWin(c *(-1), dep + 1);
						if (r == 0) 
						{
							if (dep != 0) ungoStep(i, j);
							return 1;
						}
						if (r == -1) 
						{
							ungoStep(i, j); return -1;
						}
					}
				}
				ungoStep(i, j);
				if (num > 7) return 0;
			}
		}
		return 0;
	}
	int my; int qi[100] = { 0 };
	//选择下一步
	bool chooseNextStep() 
	{
		vec.clear(); vec1.clear();
		for (int j = 1; j <= 9; j++) 
		{
			for (int i = 1; i <= 9; i++) 
			{
				if (!checkStep(i, j)) continue;
				if (goStep(i, j, my)) 
				{
					if (checkQi(i, j)) vec.push_back(make_pair(i, j));
				}
				ungoStep(i, j);
			}
		}
		if (vec.size() == 0) return false;
		for (int t = 0; t < vec.size(); ++t) 
		{
			goStep(vec[t].first, vec[t].second, my);
			qi[t] = (cnt <= 20 ? -getQi(vec[t].first, vec[t].second) : calc(my));
			ungoStep(vec[t].first, vec[t].second);
		}
		int ch = 0; int mm = 0; int now = -inf; int ddx, ddy;
		for (int t = 0; t < vec.size(); ++t) 
		{
			if (qi[t] > qi[ch]) 
			{
				vec1.clear(); ch = t;
				vec1.push_back(make_pair(vec[t].first, vec[t].second));
			}
			else if (qi[t] == qi[ch])
			{
				vec1.push_back(make_pair(vec[t].first, vec[t].second));
			}
		}
		int p = rand() % vec1.size();
		if (cnt <= 55) 
		{
			for (int t = 0; t < vec1.size(); t++)
			{
				goStep(vec1[t].first, vec1[t].second, my);
				if (evaluate1(my) > now)
				{
					now = evaluate1(my);
					ddx = vec1[t].first; ddy = vec1[t].second;
				}
				ungoStep(vec1[t].first, vec1[t].second);
			}
			goStep(ddx, ddy, my);
			Sleep(500);
			mciSendString(_T("close jpmusic"), NULL, 0, NULL);
			mciSendString(_T("open ..\\music\\落子.wav alias jpmusic"), NULL, 0, NULL);
			mciSendString(_T("play jpmusic"), NULL, 0, NULL);
			if (gamemode == 1)
			{
				setfillcolor(WHITE);
				setlinecolor(WHITE); table[ddx][ddy] = -1;
			}
			else if (gamemode == 2)
			{
				setfillcolor(BLACK);
				setlinecolor(BLACK); table[ddx][ddy] = 1;
			}
			fillcircle(ddx * 60, ddy * 60, 27);
		}
		else {
			prevtime = clock();
			if (canItWin(my, 0) != 1) 
			{
				goStep(vec1[p].first, vec1[p].second, my);
				mciSendString(_T("close jpmusic"), NULL, 0, NULL);
				mciSendString(_T("open ..\\music\\落子.wav alias jpmusic"), NULL, 0, NULL);
				mciSendString(_T("play jpmusic"), NULL, 0, NULL);
				if (gamemode == 1)
				{
					setfillcolor(WHITE);
					setlinecolor(WHITE); table[vec1[p].first][vec1[p].second] = -1;
				}
				else if (gamemode == 2)
				{
					setfillcolor(BLACK);
					setlinecolor(BLACK); table[vec1[p].first][vec1[p].second] = 1;
				}
				fillcircle(vec1[p].first * 60, vec1[p].second * 60, 27);
			}
		}
		return true;
	}
}SS;

bool inBoard(int x, int y)
{
	if (x >= 1 && y >= 1 && x <= 9 && y <= 9)//在棋盘之内
		return true;
	return false;
}

bool haveAir(int fx, int fy)
{
	haveAir_visited[fx][fy] = true;
	bool flag = false;
	for (int dir = 0; dir < 4; dir++)
	{
		//递归
		int dx = fx + di[dir], dy = fy + dj[dir];
		if (inBoard(dx, dy) == true)
		{
			if (table[dx][dy] == 0)
				flag = true;
			if (table[dx][dy] == table[fx][fy] && haveAir_visited[dx][dy] == false)
				if (haveAir(dx, dy) == true)
					flag = true;
		}
	}
	return flag;
}

int winOrLose()
{
	for (int i = 1; i <= 9; i++)
	{
		for (int j = 1; j <= 9; j++)
		{
			memset(haveAir_visited, false, sizeof(haveAir_visited));
			if (haveAir(i, j) == false)//如果有一个位置没有气，则结束游戏
			{
				if (table[i][j] == 1)
				{
					return 1;
				}
				else if (table[i][j] == -1)
				{
					return -1;
				}
			}
		}
	}
	return 0;
}

void endGame(int mod)
{
	writeRecordFile(cnt, table);//存档
	if (mod == 1)
	{
		cleardevice();
		putimage(0, 0, &background);
		setbkmode(TRANSPARENT);
		settextcolor(BLACK);
		settextstyle(70, 0, _T("华文新魏"), 0, 0, 0, 0, 0, 0);
		outtextxy(Width * 0.25, High * 0.3, _T("游戏结束！"));
		if (gamemode == 1)
			outtextxy(Width * 0.25, High * 0.4, _T("你胜利了！"));
		else if (gamemode == 2)
			outtextxy(Width * 0.13, High * 0.4, _T("吃子！你输了！"));
		settextstyle(30, 0, _T("仿宋"));
		mciSendString(_T("close bkmusic"), NULL, 0, NULL);
		if (gamemode == 1)
		{
			mciSendString(_T("close winmusic"), NULL, 0, NULL);
			mciSendString(_T("open ..\\music\\win.wav alias winmusic"), NULL, 0, NULL);
			mciSendString(_T("play winmusic"), NULL, 0, NULL);
		}
		else if (gamemode == 2)
		{
			mciSendString(_T("close losemusic"), NULL, 0, NULL);
			mciSendString(_T("open ..\\music\\lose.wav alias losemusic"), NULL, 0, NULL);
			mciSendString(_T("play losemusic"), NULL, 0, NULL);
		}
		outtextxy(Width * 0.33, High * 0.8, _T("按任意键以退出"));
	}
	else if (mod == -1)
	{
		cleardevice();
		putimage(0, 0, &background);
		setbkmode(TRANSPARENT);
		settextcolor(BLACK);
		settextstyle(70, 0, _T("华文新魏"), 0, 0, 0, 0, 0, 0);
		outtextxy(Width * 0.25, High * 0.3, _T("游戏结束！"));
		if (gamemode == 1)
			outtextxy(Width * 0.13, High * 0.4, _T("吃子！你输了！"));
		else if (gamemode == 2)
			outtextxy(Width * 0.25, High * 0.4, _T("你胜利了！"));
		mciSendString(_T("close bkmusic"), NULL, 0, NULL);
		if (gamemode == 1)
		{
			mciSendString(_T("close losemusic"), NULL, 0, NULL);
			mciSendString(_T("open ..\\music\\lose.wav alias losemusic"), NULL, 0, NULL);
			mciSendString(_T("play losemusic"), NULL, 0, NULL);
		}
		else if (gamemode == 2)
		{
			mciSendString(_T("close winmusic"), NULL, 0, NULL);
			mciSendString(_T("open ..\\music\\win.wav alias winmusic"), NULL, 0, NULL);
			mciSendString(_T("play winmusic"), NULL, 0, NULL);

		}
		settextstyle(30, 0, _T("仿宋"));
		outtextxy(Width * 0.33, High * 0.8, _T("按任意键以退出"));
	}
	else if (mod == 0)
	{
		cleardevice();
		putimage(0, 0, &background);
		setbkmode(TRANSPARENT);
		settextcolor(BLACK);
		settextstyle(70, 0, _T("华文新魏"), 0, 0, 0, 0, 0, 0);
		outtextxy(Width * 0.25, High * 0.3, _T("游戏结束！"));
		settextstyle(60, 0, _T("华文新魏"));
		outtextxy(Width * 0.08, High * 0.44, _T("落子自杀，你输了！"));
		settextstyle(30, 0, _T("仿宋"));
		mciSendString(_T("close bkmusic"), NULL, 0, NULL);
		mciSendString(_T("close losemusic"), NULL, 0, NULL);
		mciSendString(_T("open ..\\music\\lose.wav alias losemusic"), NULL, 0, NULL);
		mciSendString(_T("play losemusic"), NULL, 0, NULL);

		outtextxy(Width * 0.33, High * 0.8, _T("按任意键以退出"));
	}
}

void writeRecordFile(int n, int(*board)[10])
{
	ofstream outfile;//定义流与文件
	outfile.open("..\\data\\data.txt");
	outfile << n << ' ';
	for (int i = 0; i < 10; i++)
	{
		for (int j = 0; j < 10; j++)
		{
			outfile << board[i][j] << ' ';//写入棋盘情况
		}
	}
	outfile << nowx << ' ' << nowy;//写入当前落子情况
	outfile << ' ' << gamemode;//写入游戏难度
	outfile << ' ' << difficulty;
	outfile.close();
}

void readRecordFile(int(*board)[10])
{
	int n;
	ifstream infile("..\\data\\data.txt");
	infile >> n;
	cnt = n;//回合数
	for (int i = 0; i < 10; i++)
		for (int j = 0; j < 10; j++)
			infile >> board[i][j];//读入棋盘情况
	infile >> nx >> ny;//读入当前落子情况
	infile >> gamemode;//读入游戏难度
	infile >> difficulty;
}

//main函数---------------------------------------------------------------------------------------------------------------

int main()
{
	int mode = startUp();
	if (mode == 1)
		return 0;
	while (true)
	{
		if (cnt % 2 == 0)
			updateWithInput();
		else
		{
			int result = -1;
			if (difficulty == 1)
				decideByComputer();
			else if (difficulty == 2)
				result = S.decideByComputer_more();
			else if (difficulty == 3)
			{
				if (gamemode == 1)SS.my = -1;
				else SS.my = 1;
				result = SS.chooseNextStep();
			}
			if (result == 0)
			{
				Sleep(500);
				gamemode = 1;
				endGame(1);
				_getch();
				closegraph();
				break;
			}
		}
		if (flag == 1)
		{
			flag = 0;
			continue;
		}
		if (cnt % 2 == 0 && haveAir(nowx, nowy) == false)
		{
			Sleep(500);
			endGame(0);
			_getch();
			closegraph();
			break;
		}
		else if (winOrLose() == 0)
		{
			cnt++;
			continue;
		}
		else
		{
			Sleep(500);
			endGame(winOrLose());
			_getch();
			closegraph();
			break;
		}
	}
	return 0;
}