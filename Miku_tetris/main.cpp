#include<graphics.h>//引入图形库
#include<conio.h>//关于键盘的操作
#include<time.h>
#include<stdio.h>
#include <Windows.h>
#include<mmsystem.h>//BGM
#pragma comment(lib,"winmm.lib")
//////////////////////////////////////////////////
//定义常量、全局变量、结构体、枚举
//////////////////////////////////////////////////
#define WIDTH 10 //宽为10个单元
#define HEIGHT 20 //高 20
#define UNIT 30 //方块大小
struct block {
	int dir[4];//方块的四个旋转状态
	COLORREF color;//颜色
}blocks[7] = {
	{0x0F00, 0x4444, 0x0F00, 0x4444,RGB(222,206,206)},		// I
	{0x0660, 0x0660, 0x0660, 0x0660, RGB(219,226,36)},		// O
	{0x4460, 0x02E0, 0x0622, 0x0740, RGB(201, 191, 211)},   //L
	{0x2260, 0x0E20, 0x0644, 0x0470, RGB(248, 235, 216)},	// J
	{0x4E00, 0x4C40, 0x0E40, 0x4640, RGB(191, 191, 191)},		// T
	{0x0360, 0x4620, 0x0360, 0x4620,RGB(150, 164, 138)},	// S
	{0x0C60, 0x2640, 0x0C60, 0x2640, RGB(150, 83, 84)}	// Z
};//将16进制转换成二进制看看，代表方块的各个状态
struct BlockID
{
	int id;//方块id
	char x, y;//方块在游戏中的坐标
	unsigned int dir : 2;//方向	dir:2 表示dir这个成员只用2个bit.
}NowBlock, NextBlock;
enum CMD
{
	ROTATE, LEFT, RIGHT, DOWN, SINK, QUIT
};//键盘操作

long int old_time, new_time;//时间变量
char FixBlock[WIDTH][HEIGHT] = { 0 };//固定方块区
int score, max, level, speed = 70;//分数,等级,速度

/*函数声明*/
void Init();
void UI_WelcomeView();//欢迎界面
void ChooseLevel(int x);//改变速度
void NewGame();//初始化游戏
void XhMusic();//消行音乐
void CreatBlock();//创建新方块
void DrawBlock(BlockID block);//画方块
void DrawUnit(int x, int y, COLORREF color);//移动过程中的更新
void ClearBlock(BlockID block); //移动过程中的更新
void ClearUnit(int x, int y, COLORREF color = BLACK);//移动过程中的更新
CMD GetCommand();//键盘操作
void SendCommand(CMD cmd);//反馈键盘操作
void Go_Down();//↓
void Go_Left();//←
void Go_Right();//→
void Go_Sink();//沉底+得分更新+消行
void Go_Rotate();//方块的旋转
bool CheckBlock(BlockID temp);//边界
void ReadGrade();//保存最高分
void WriteGrade();//更新最高分
void GameOver();
void Quit();

//开始界面
void UI_WelcomeView()
{
	mciSendString("open ./Music/千本樱.mp3 alias bgm_time", NULL, 0, 0);
	mciSendString("play  bgm_time from 0", NULL, 0, 0);
	initgraph(500, 645);
	IMAGE BeginImage;
	loadimage(&BeginImage, "BK1.jpg", 500, 645);
	putimage(0, 0, &BeginImage);
	setbkcolor(TRANSPARENT);
	setbkmode(TRANSPARENT);
	setcolor(BLACK);
	settextstyle(30, 0, _T("Algerian"));
	outtextxy(100, 100, "Welcome to the Game!");
	settextstyle(20, 0, _T("华文琥珀"));
	setcolor(BLACK);
	outtextxy(200, 500, "点击数字键1/2/3选择游戏难度！");
	outtextxy(200, 525, "①高速模式②中速模式③低速模式");
	settextstyle(18, 0, _T("华文彩云"));
	setcolor(BLACK);
	outtextxy(150, 550, "敢按其他键的话可是默认为超高速模式了哦！");
	settextstyle(18, 0, _T("华文彩云"));
	setcolor(BLACK);
	outtextxy(100, 300, "俄罗斯方块是一个有着悠长历史的游戏...");
	char ch = _getch();
	mciSendString("close bgm_time", NULL, 0, NULL);
	switch (ch)
	{
		case 49:
		case 97:ChooseLevel(1); break;
		case 50:
		case 98: ChooseLevel(2); break;
		case 51:
		case 99: ChooseLevel(3); break;
	}
	Init();
}
//建立游戏窗口(美化游戏窗口√
void Init()
{
	mciSendString("open  ./Music/BKmusic.mp3  alias gamerun", NULL, 0, 0);
	mciSendString("play gamerun repeat", NULL, 0, 0);
	initgraph(500, 645);
	IMAGE img;
	loadimage(&img, "test.jpg", 500, 645);
	putimage(0, 0, &img);
	setbkcolor(WHITE);
	setbkmode(TRANSPARENT);//去除文字黑底（解决√
	setorigin(20, 20);	// 设置坐标原点
	// 绘制游戏区边界
	rectangle(-1, -1, WIDTH * UNIT, HEIGHT * UNIT);		//大矩形
	line((WIDTH + 1) * UNIT - 1, -1, (WIDTH + 1) * UNIT - 1, HEIGHT * UNIT);	//竖直线
	line((WIDTH + 1) * UNIT - 1, -1, 550, -1);	//横线1
	line((WIDTH + 1) * UNIT - 1, 4 * UNIT, 550, 4 * UNIT);//横线2
	line((WIDTH + 1) * UNIT - 1, 9 * UNIT, 550, 9 * UNIT);//横线3
	line((WIDTH + 1) * UNIT - 1, HEIGHT * UNIT, 500, HEIGHT * UNIT);//横线4
	settextstyle(18, 0, _T("黑体"));
	outtextxy((WIDTH + 2) * UNIT, 10 * UNIT, _T("操作说明"));
	outtextxy((WIDTH + 2) * UNIT, 11 * UNIT, _T("↑：旋转"));
	outtextxy((WIDTH + 2) * UNIT, 12.5 * UNIT, _T("←：左移"));
	outtextxy((WIDTH + 2) * UNIT, 14 * UNIT, _T("→：右移"));
	outtextxy((WIDTH + 2) * UNIT, 15.5 * UNIT, _T("↓：加速"));
	outtextxy((WIDTH + 2) * UNIT - 8, 17 * UNIT, _T("空格：沉底"));
	outtextxy((WIDTH + 2) * UNIT - 4, 18.5 * UNIT, _T("ESC：退出"));
	settextstyle(30, 0, _T("华文行楷"));
	outtextxy((WIDTH + 2.5) * UNIT, 4 * UNIT, _T("得分"));
	outtextxy((WIDTH + 1.5) * UNIT, 6 * UNIT, _T("最高记录"));
	NewGame();
}
//速度
void ChooseLevel(int x)
{
	level = x;
	speed = 2 * level * 100;
}
//初始化游戏
void NewGame()
{
	setfillcolor(RGB(181, 196, 177));
	solidrectangle(0, 0, WIDTH * UNIT - 1, HEIGHT * UNIT - 1);//画一个实心矩形
	ZeroMemory(FixBlock, WIDTH * HEIGHT);//FixBlock[WID][HIG] = 0;ZeroMemory函数用0来填充这一块内存区域
	score = 0;//初始化分数
	char s[10], m[10];
	setbkcolor(TRANSPARENT);
	setbkmode(TRANSPARENT);
	sprintf_s(s, "%d", score);
	sprintf_s(m, "%d", max);
	settextstyle(30, 0, _T("黑体"));
	outtextxy((WIDTH + 2.5) * UNIT + 25, 5 * UNIT, s);
	outtextxy((WIDTH + 2.5) * UNIT + 25, 7 * UNIT, m);
	//初始化下一个方块
	srand((unsigned int)time(NULL));
	NextBlock.id = rand() % 7;
	NextBlock.x = WIDTH + 1;
	NextBlock.y = HEIGHT - 1;
	NextBlock.dir = rand() % 4;
	CreatBlock();
}
//创建新的方块
void CreatBlock()
{
	NowBlock.x = 3;
	NowBlock.y = HEIGHT + 2;
	NowBlock.id = NextBlock.id;
	NowBlock.dir = NextBlock.dir;
	srand((unsigned int)time(NULL));
	NextBlock.id = rand() % 7;
	NextBlock.dir = rand() % 4;
	DrawBlock(NowBlock);
	setfillcolor(RGB(181, 196, 177));
	solidrectangle((WIDTH + 1) * UNIT, 0, (WIDTH + 5) * UNIT - 1, 4 * UNIT - 1);
	DrawBlock(NextBlock);
	old_time = GetTickCount();// 设置计时器，用于判断自动下落
}
//画方块
void DrawBlock(BlockID block)
{
	int b = blocks[block.id].dir[block.dir];
	int x, y;
	int i;

	for (i = 0; i < 16; i++, b <<= 1)
	{
		if (b & 0x8000)		//和0x8000做与运算是为了测试最高位是否为1.
		{
			x = block.x + i % 4;
			y = block.y - i / 4;
			if (y < HEIGHT)
				DrawUnit(x, y, blocks[block.id].color);//找到1画方块
		}
	}
}
void ClearBlock(BlockID block)
{
	int b = blocks[block.id].dir[block.dir];
	int x, y;
	int i;

	for (i = 0; i < 16; i++, b <<= 1)
	{
		if (b & 0x8000)		//和0x8000做与运算是为了测试最高位是否为1.
		{
			x = block.x + i % 4;
			y = block.y - i / 4;
			if (y < HEIGHT)
				ClearUnit(x, y, blocks[block.id].color);
		}
	}
}

void DrawUnit(int x, int y, COLORREF color)
{
	// 计算单元方块对应的屏幕坐标
	int left = x * UNIT;
	int top = (HEIGHT - y - 1) * UNIT;
	int right = (x + 1) * UNIT - 1;
	int bottom = (HEIGHT - y) * UNIT - 1;

	// 画单元方块
	setfillcolor(color);
	setlinecolor(WHITE);
	fillrectangle(left + 2, top + 2, right - 2, bottom - 2);
}
void ClearUnit(int x, int y, COLORREF color)
{
	// 计算单元方块对应的屏幕坐标
	int left = x * UNIT;
	int top = (HEIGHT - y - 1) * UNIT;
	int right = (x + 1) * UNIT - 1;
	int bottom = (HEIGHT - y) * UNIT - 1;

	setfillcolor(RGB(181, 196, 177));
	solidrectangle(x * UNIT, (HEIGHT - y - 1) * UNIT, (x + 1) * UNIT - 1, (HEIGHT - y) * UNIT - 1);
}
CMD GetCommand()
{
	CMD cmd;
	while (true)
	{
		new_time = GetTickCount();// 不控制，默认自动下落一格		
		if (new_time - old_time > speed)//控制下落速度，选择难度
		{
			old_time = new_time;
			return DOWN;
		}
		// 如果有按键，返回按键对应的功能
		if (_kbhit())
		{
			switch (_getch())
			{
			case 72:
				cmd = ROTATE;
				return cmd;
			case 80:
				cmd = DOWN;
				return cmd;
			case 75:
				cmd = LEFT;
				return cmd;
			case 77:
				cmd = RIGHT;
				return cmd;
			case 27:	//ESC键
				cmd = QUIT;
				return cmd;
			case ' ':
				cmd = SINK;
				return cmd;
			}
		}
	}
}
//控制操作
void SendCommand(CMD cmd)
{
	switch (cmd)
	{
	case DOWN:
		Go_Down();
		break;
	case LEFT:
		Go_Left();
		break;
	case RIGHT:
		Go_Right();
		break;
	case ROTATE:
		Go_Rotate();
		break;
	case SINK:
		Go_Sink();
		break;
	case QUIT:
		// 按退出时，显示对话框咨询用户是否退出
		HWND wnd = GetHWnd();
		if (MessageBox(wnd, _T("您确定要退出游戏吗？"), _T("提醒"), MB_OKCANCEL | MB_ICONQUESTION) == IDOK)
			Quit();
	}
}

void Go_Down()
{
	BlockID temp = NowBlock;
	temp.y--;

	if (CheckBlock(temp))
	{
		ClearBlock(NowBlock);
		NowBlock.y--;
		DrawBlock(NowBlock);
	}
	else
		Go_Sink();	//不能下移时，执行沉淀操作
}
void Go_Left()
{
	BlockID temp = NowBlock;
	temp.x--;

	if (CheckBlock(temp))
	{
		ClearBlock(NowBlock);
		NowBlock.x--;
		DrawBlock(NowBlock);
	}
}
void Go_Right()
{
	BlockID temp = NowBlock;
	temp.x++;

	if (CheckBlock(temp))
	{
		ClearBlock(NowBlock);
		NowBlock.x++;
		DrawBlock(NowBlock);
	}
}

//沉淀、消行
void Go_Sink()
{
	int x, y, i, n;
	int m = 0;	//可消的行数

	ClearBlock(NowBlock);
	BlockID temp = NowBlock;
	temp.y--;
	while (CheckBlock(temp))
	{
		NowBlock.y--;
		temp.y--;
	}

	DrawBlock(NowBlock);

	//固定方块
	int b = blocks[NowBlock.id].dir[NowBlock.dir];
	for (i = 0; i < 16; i++, b = b << 1)
	{
		if (b & 0x8000)
		{
			x = NowBlock.x + i % 4;
			y = NowBlock.y - i / 4;

			if (y >= HEIGHT)
			{
				if (score > max)
				{
					WriteGrade();
				}
				GameOver();
				return;
			}
			else
				FixBlock[x][y] = 1;
		}
	}

	//4位用来标记方块涉及的 4 行是否有消除行为
	char remove = 0;		//标记是否要消掉行
	for (y = NowBlock.y; y >= 0; y--)
	{
		n = 0;
		for (x = 0; x < WIDTH; x++)
		{
			if (FixBlock[x][y] == 1)
				n++;
		}

		if (n == WIDTH)
		{
			m++;
			remove = remove | (1 << (NowBlock.y - y));
			setfillcolor(RGB(181, 196, 177));
			fillrectangle(0, (HEIGHT - y - 1) * UNIT,
				WIDTH * UNIT - 1, (HEIGHT - y) * UNIT - 2);
		}
	}

	if (m != 0)
	{
		//更新分数并显示;
		if (m == 1)
			score = score + 10;
		else if (m == 2)
			score = score + 30;
		else if (m == 3)
			score = score + 50;
		else
			score = score + 100;

		char s[10];
		sprintf_s(s, "%d", score);
		setbkmode(TRANSPARENT);
		setfillcolor(RGB(181, 196, 177));

		setlinecolor(RGB(181, 196, 177));
		fillrectangle((WIDTH + 1) * UNIT, 5 * UNIT, (WIDTH + 10) * UNIT, 6 * UNIT - 1);
		settextstyle(30, 0, _T("黑体"));
		outtextxy((WIDTH + 2.5) * UNIT + 15, 5 * UNIT, _T(s));
	}

	if (remove)
	{
		Sleep(100);
		XhMusic();
		// 擦掉刚才标记的行
		IMAGE img;
		for (i = 0; i < 4; i++, remove >>= 1)
		{
			if (remove & 1)
			{
				for (y = NowBlock.y - i + 1; y < HEIGHT; y++)
					for (x = 0; x < WIDTH; x++)
					{
						FixBlock[x][y - 1] = FixBlock[x][y];
						FixBlock[x][y] = 0;
					}
				//调用getimage()保存屏幕图像
				getimage(&img, 0, 0, WIDTH * UNIT, (HEIGHT - (NowBlock.y - i + 1)) * UNIT);
				//用函数putimage()输出getimage()保存的屏幕图像。
				putimage(0, UNIT, &img);
			}
		}
	}
	CreatBlock();
}

void XhMusic()
{
	mciSendString("open  ./Music/消行.mp3  alias bgmclear", NULL, 0, 0);
	mciSendString("play bgmclear from 9", NULL, 0, 0);
}

void Go_Rotate()
{
	// 获取可以旋转的 x 偏移量
	int dx;
	BlockID temp = NowBlock;
	temp.dir++;
	if (CheckBlock(temp))
	{
		dx = 0;
		goto rotate;
	}
	temp.x = NowBlock.x - 1;
	if (CheckBlock(temp))
	{
		dx = -1;
		goto rotate;
	}
	temp.x = NowBlock.x + 1;
	if (CheckBlock(temp))
	{
		dx = 1;
		goto rotate;
	}
	temp.x = NowBlock.x - 2;
	if (CheckBlock(temp))
	{
		dx = -2;
		goto rotate;
	}
	temp.x = NowBlock.x + 2;
	if (CheckBlock(temp))
	{
		dx = 2;
		goto rotate;
	}
	return;

rotate:
	// 旋转
	ClearBlock(NowBlock);
	NowBlock.dir++;
	NowBlock.x += dx;
	DrawBlock(NowBlock);
}


bool CheckBlock(BlockID temp)
{
	int b = blocks[temp.id].dir[temp.dir];
	int x, y, i;

	for (i = 0; i < 16; i++, b = b << 1)
	{
		if (b & 0x8000)
		{
			x = temp.x + i % 4;
			y = temp.y - i / 4;
			if ((x < 0) || (x >= WIDTH) || (y < 0))
				return false;
			if ((y < HEIGHT) && (FixBlock[x][y]))
				return false;
		}
	}
	return true;
}
void ReadGrade()
{
	FILE* pf = NULL;
	errno_t err = fopen_s(&pf, "最高分记录.txt", "r");
	if (pf == NULL)
	{
		fopen_s(&pf, "最高分记录.txt", "w");
		fwrite(&score, sizeof(int), 1, pf);
	}
	fseek(pf, 0, SEEK_SET); //使文件指针pf指向文件开头
	fread(&max, sizeof(int), 1, pf); //读取文件中的最高历史得分到max当中
	fclose(pf); //关闭文件
	pf = NULL; //文件指针及时置空
}
void WriteGrade()
{
	FILE* pf = NULL;
	errno_t err = fopen_s(&pf, "最高分记录.txt", "w");
	if (pf == NULL) //打开文件失败
	{
		//printf("保存最高得分记录失败\n");
		exit(0);
	}
	fwrite(&score, sizeof(int), 1, pf); //将本局游戏得分写入文件当中（更新最高历史得分）
	fclose(pf); //关闭文件
	pf = NULL; //文件指针及时置空
}
// 结束游戏
void GameOver()
{
	HWND wnd = GetHWnd(); // 获得窗口句柄

	if (MessageBox(wnd, _T("游戏结束。\n您想重新来一局吗？"),
		_T("游戏结束"), MB_YESNO | MB_ICONQUESTION) == IDYES)
	{
		Sleep(100);
		NewGame();
	}
	else
		Quit();
}
void Quit()
{
	closegraph();
	exit(0);
}
int main()
{
	ReadGrade();
	UI_WelcomeView();
	Init();
	//NewGame();
	CMD cmd;
	while (true)
	{
		cmd = GetCommand();
		SendCommand(cmd);
	}
	_getch();
	closegraph();
	return 0;
}