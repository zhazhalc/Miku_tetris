#include<graphics.h>//����ͼ�ο�
#include<conio.h>//���ڼ��̵Ĳ���
#include<time.h>
#include<stdio.h>
#include <Windows.h>
#include<mmsystem.h>//BGM
#pragma comment(lib,"winmm.lib")
//////////////////////////////////////////////////
//���峣����ȫ�ֱ������ṹ�塢ö��
//////////////////////////////////////////////////
#define WIDTH 10 //��Ϊ10����Ԫ
#define HEIGHT 20 //�� 20
#define UNIT 30 //�����С
struct block {
	int dir[4];//������ĸ���ת״̬
	COLORREF color;//��ɫ
}blocks[7] = {
	{0x0F00, 0x4444, 0x0F00, 0x4444,RGB(222,206,206)},		// I
	{0x0660, 0x0660, 0x0660, 0x0660, RGB(219,226,36)},		// O
	{0x4460, 0x02E0, 0x0622, 0x0740, RGB(201, 191, 211)},   //L
	{0x2260, 0x0E20, 0x0644, 0x0470, RGB(248, 235, 216)},	// J
	{0x4E00, 0x4C40, 0x0E40, 0x4640, RGB(191, 191, 191)},		// T
	{0x0360, 0x4620, 0x0360, 0x4620,RGB(150, 164, 138)},	// S
	{0x0C60, 0x2640, 0x0C60, 0x2640, RGB(150, 83, 84)}	// Z
};//��16����ת���ɶ����ƿ�����������ĸ���״̬
struct BlockID
{
	int id;//����id
	char x, y;//��������Ϸ�е�����
	unsigned int dir : 2;//����	dir:2 ��ʾdir�����Աֻ��2��bit.
}NowBlock, NextBlock;
enum CMD
{
	ROTATE, LEFT, RIGHT, DOWN, SINK, QUIT
};//���̲���

long int old_time, new_time;//ʱ�����
char FixBlock[WIDTH][HEIGHT] = { 0 };//�̶�������
int score, max, level, speed = 70;//����,�ȼ�,�ٶ�

/*��������*/
void Init();
void UI_WelcomeView();//��ӭ����
void ChooseLevel(int x);//�ı��ٶ�
void NewGame();//��ʼ����Ϸ
void XhMusic();//��������
void CreatBlock();//�����·���
void DrawBlock(BlockID block);//������
void DrawUnit(int x, int y, COLORREF color);//�ƶ������еĸ���
void ClearBlock(BlockID block); //�ƶ������еĸ���
void ClearUnit(int x, int y, COLORREF color = BLACK);//�ƶ������еĸ���
CMD GetCommand();//���̲���
void SendCommand(CMD cmd);//�������̲���
void Go_Down();//��
void Go_Left();//��
void Go_Right();//��
void Go_Sink();//����+�÷ָ���+����
void Go_Rotate();//�������ת
bool CheckBlock(BlockID temp);//�߽�
void ReadGrade();//������߷�
void WriteGrade();//������߷�
void GameOver();
void Quit();

//��ʼ����
void UI_WelcomeView()
{
	mciSendString("open ./Music/ǧ��ӣ.mp3 alias bgm_time", NULL, 0, 0);
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
	settextstyle(20, 0, _T("��������"));
	setcolor(BLACK);
	outtextxy(200, 500, "������ּ�1/2/3ѡ����Ϸ�Ѷȣ�");
	outtextxy(200, 525, "�ٸ���ģʽ������ģʽ�۵���ģʽ");
	settextstyle(18, 0, _T("���Ĳ���"));
	setcolor(BLACK);
	outtextxy(150, 550, "�Ұ��������Ļ�����Ĭ��Ϊ������ģʽ��Ŷ��");
	settextstyle(18, 0, _T("���Ĳ���"));
	setcolor(BLACK);
	outtextxy(100, 300, "����˹������һ�������Ƴ���ʷ����Ϸ...");
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
//������Ϸ����(������Ϸ���ڡ�
void Init()
{
	mciSendString("open  ./Music/BKmusic.mp3  alias gamerun", NULL, 0, 0);
	mciSendString("play gamerun repeat", NULL, 0, 0);
	initgraph(500, 645);
	IMAGE img;
	loadimage(&img, "test.jpg", 500, 645);
	putimage(0, 0, &img);
	setbkcolor(WHITE);
	setbkmode(TRANSPARENT);//ȥ�����ֺڵף������
	setorigin(20, 20);	// ��������ԭ��
	// ������Ϸ���߽�
	rectangle(-1, -1, WIDTH * UNIT, HEIGHT * UNIT);		//�����
	line((WIDTH + 1) * UNIT - 1, -1, (WIDTH + 1) * UNIT - 1, HEIGHT * UNIT);	//��ֱ��
	line((WIDTH + 1) * UNIT - 1, -1, 550, -1);	//����1
	line((WIDTH + 1) * UNIT - 1, 4 * UNIT, 550, 4 * UNIT);//����2
	line((WIDTH + 1) * UNIT - 1, 9 * UNIT, 550, 9 * UNIT);//����3
	line((WIDTH + 1) * UNIT - 1, HEIGHT * UNIT, 500, HEIGHT * UNIT);//����4
	settextstyle(18, 0, _T("����"));
	outtextxy((WIDTH + 2) * UNIT, 10 * UNIT, _T("����˵��"));
	outtextxy((WIDTH + 2) * UNIT, 11 * UNIT, _T("������ת"));
	outtextxy((WIDTH + 2) * UNIT, 12.5 * UNIT, _T("��������"));
	outtextxy((WIDTH + 2) * UNIT, 14 * UNIT, _T("��������"));
	outtextxy((WIDTH + 2) * UNIT, 15.5 * UNIT, _T("��������"));
	outtextxy((WIDTH + 2) * UNIT - 8, 17 * UNIT, _T("�ո񣺳���"));
	outtextxy((WIDTH + 2) * UNIT - 4, 18.5 * UNIT, _T("ESC���˳�"));
	settextstyle(30, 0, _T("�����п�"));
	outtextxy((WIDTH + 2.5) * UNIT, 4 * UNIT, _T("�÷�"));
	outtextxy((WIDTH + 1.5) * UNIT, 6 * UNIT, _T("��߼�¼"));
	NewGame();
}
//�ٶ�
void ChooseLevel(int x)
{
	level = x;
	speed = 2 * level * 100;
}
//��ʼ����Ϸ
void NewGame()
{
	setfillcolor(RGB(181, 196, 177));
	solidrectangle(0, 0, WIDTH * UNIT - 1, HEIGHT * UNIT - 1);//��һ��ʵ�ľ���
	ZeroMemory(FixBlock, WIDTH * HEIGHT);//FixBlock[WID][HIG] = 0;ZeroMemory������0�������һ���ڴ�����
	score = 0;//��ʼ������
	char s[10], m[10];
	setbkcolor(TRANSPARENT);
	setbkmode(TRANSPARENT);
	sprintf_s(s, "%d", score);
	sprintf_s(m, "%d", max);
	settextstyle(30, 0, _T("����"));
	outtextxy((WIDTH + 2.5) * UNIT + 25, 5 * UNIT, s);
	outtextxy((WIDTH + 2.5) * UNIT + 25, 7 * UNIT, m);
	//��ʼ����һ������
	srand((unsigned int)time(NULL));
	NextBlock.id = rand() % 7;
	NextBlock.x = WIDTH + 1;
	NextBlock.y = HEIGHT - 1;
	NextBlock.dir = rand() % 4;
	CreatBlock();
}
//�����µķ���
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
	old_time = GetTickCount();// ���ü�ʱ���������ж��Զ�����
}
//������
void DrawBlock(BlockID block)
{
	int b = blocks[block.id].dir[block.dir];
	int x, y;
	int i;

	for (i = 0; i < 16; i++, b <<= 1)
	{
		if (b & 0x8000)		//��0x8000����������Ϊ�˲������λ�Ƿ�Ϊ1.
		{
			x = block.x + i % 4;
			y = block.y - i / 4;
			if (y < HEIGHT)
				DrawUnit(x, y, blocks[block.id].color);//�ҵ�1������
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
		if (b & 0x8000)		//��0x8000����������Ϊ�˲������λ�Ƿ�Ϊ1.
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
	// ���㵥Ԫ�����Ӧ����Ļ����
	int left = x * UNIT;
	int top = (HEIGHT - y - 1) * UNIT;
	int right = (x + 1) * UNIT - 1;
	int bottom = (HEIGHT - y) * UNIT - 1;

	// ����Ԫ����
	setfillcolor(color);
	setlinecolor(WHITE);
	fillrectangle(left + 2, top + 2, right - 2, bottom - 2);
}
void ClearUnit(int x, int y, COLORREF color)
{
	// ���㵥Ԫ�����Ӧ����Ļ����
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
		new_time = GetTickCount();// �����ƣ�Ĭ���Զ�����һ��		
		if (new_time - old_time > speed)//���������ٶȣ�ѡ���Ѷ�
		{
			old_time = new_time;
			return DOWN;
		}
		// ����а��������ذ�����Ӧ�Ĺ���
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
			case 27:	//ESC��
				cmd = QUIT;
				return cmd;
			case ' ':
				cmd = SINK;
				return cmd;
			}
		}
	}
}
//���Ʋ���
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
		// ���˳�ʱ����ʾ�Ի�����ѯ�û��Ƿ��˳�
		HWND wnd = GetHWnd();
		if (MessageBox(wnd, _T("��ȷ��Ҫ�˳���Ϸ��"), _T("����"), MB_OKCANCEL | MB_ICONQUESTION) == IDOK)
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
		Go_Sink();	//��������ʱ��ִ�г������
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

//��������
void Go_Sink()
{
	int x, y, i, n;
	int m = 0;	//����������

	ClearBlock(NowBlock);
	BlockID temp = NowBlock;
	temp.y--;
	while (CheckBlock(temp))
	{
		NowBlock.y--;
		temp.y--;
	}

	DrawBlock(NowBlock);

	//�̶�����
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

	//4λ������Ƿ����漰�� 4 ���Ƿ���������Ϊ
	char remove = 0;		//����Ƿ�Ҫ������
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
		//���·�������ʾ;
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
		settextstyle(30, 0, _T("����"));
		outtextxy((WIDTH + 2.5) * UNIT + 15, 5 * UNIT, _T(s));
	}

	if (remove)
	{
		Sleep(100);
		XhMusic();
		// �����ղű�ǵ���
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
				//����getimage()������Ļͼ��
				getimage(&img, 0, 0, WIDTH * UNIT, (HEIGHT - (NowBlock.y - i + 1)) * UNIT);
				//�ú���putimage()���getimage()�������Ļͼ��
				putimage(0, UNIT, &img);
			}
		}
	}
	CreatBlock();
}

void XhMusic()
{
	mciSendString("open  ./Music/����.mp3  alias bgmclear", NULL, 0, 0);
	mciSendString("play bgmclear from 9", NULL, 0, 0);
}

void Go_Rotate()
{
	// ��ȡ������ת�� x ƫ����
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
	// ��ת
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
	errno_t err = fopen_s(&pf, "��߷ּ�¼.txt", "r");
	if (pf == NULL)
	{
		fopen_s(&pf, "��߷ּ�¼.txt", "w");
		fwrite(&score, sizeof(int), 1, pf);
	}
	fseek(pf, 0, SEEK_SET); //ʹ�ļ�ָ��pfָ���ļ���ͷ
	fread(&max, sizeof(int), 1, pf); //��ȡ�ļ��е������ʷ�÷ֵ�max����
	fclose(pf); //�ر��ļ�
	pf = NULL; //�ļ�ָ�뼰ʱ�ÿ�
}
void WriteGrade()
{
	FILE* pf = NULL;
	errno_t err = fopen_s(&pf, "��߷ּ�¼.txt", "w");
	if (pf == NULL) //���ļ�ʧ��
	{
		//printf("������ߵ÷ּ�¼ʧ��\n");
		exit(0);
	}
	fwrite(&score, sizeof(int), 1, pf); //��������Ϸ�÷�д���ļ����У����������ʷ�÷֣�
	fclose(pf); //�ر��ļ�
	pf = NULL; //�ļ�ָ�뼰ʱ�ÿ�
}
// ������Ϸ
void GameOver()
{
	HWND wnd = GetHWnd(); // ��ô��ھ��

	if (MessageBox(wnd, _T("��Ϸ������\n����������һ����"),
		_T("��Ϸ����"), MB_YESNO | MB_ICONQUESTION) == IDYES)
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