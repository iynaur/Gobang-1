//MainWindow.c
//五子棋游戏主窗口
#include <WindowsX.h>
#include <windows.h>
#include <stdlib.h>
#include "Gobang.h"
//定义五子棋谱的线数
#ifndef LINECOUNT 
#define LINECOUNT 15
#endif
//定义五子棋棋子半径
#ifndef RADIUS
#define RADIUS 15
#endif
//#pragma comment(lib, "gdi32.lib")
//#pragma comment(lib, "User32.lib")
//声明窗口过程函数
LRESULT CALLBACK WinProc(
						 HWND hwnd,
						 UINT uMsg,
						 WPARAM wParam,
						 LPARAM lParam
						 );

//定义主函数
int WINAPI WinMain(
				   HINSTANCE hInstance,		//当前的进程实例
				   HINSTANCE hPrevInstance,	//前一进程实例,默认为NULL
				   LPSTR lpCmdLine,			//命令行参数
				   int nCmdShow				//窗口显示状态
				   )
{
	HBRUSH hBrush;
	WNDCLASS wndcls;
	HWND hwnd = NULL;
	MSG msg;

	hBrush=CreateSolidBrush(RGB(255,250,20));

	//设计窗口类
	wndcls.cbClsExtra=0;
	wndcls.cbWndExtra=0;
	//	wndcls.hbrBackground=(HBRUSH)GetStockObject(WHITE_BRUSH);
	wndcls.hbrBackground=hBrush;
	wndcls.hCursor=LoadCursor(NULL,IDC_ARROW);
	//	wndcls.hIcon=LoadIcon(NULL,IDI_WINLOGO);
	wndcls.hIcon=LoadIcon(hInstance,MAKEINTRESOURCE( 100 ));
	wndcls.hInstance=hInstance;
	wndcls.lpfnWndProc=WinProc;
	wndcls.lpszClassName=TEXT("Gobang");
	wndcls.lpszMenuName=NULL;
	wndcls.style=CS_HREDRAW | CS_VREDRAW;
	//注册窗口类
	RegisterClass(&wndcls);

	//创建窗口

	hwnd=CreateWindow(TEXT("Gobang"),TEXT("五子棋游戏"),WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,CW_USEDEFAULT,550,600,NULL,NULL,hInstance,NULL);
	//显示并更新窗口
	ShowWindow(hwnd,SW_SHOWNORMAL);
	UpdateWindow(hwnd);


	//进入消息循环
	while(GetMessage(&msg,NULL,0,0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.wParam;
	//	UNREFERENCED_PARAMETER(lpCmdLine); 
}

//定义窗口过程函数
LRESULT CALLBACK WinProc(
						 HWND hwnd,
						 UINT uMsg,
						 WPARAM wParam,
						 LPARAM lParam
						 )
{
	HDC hdc;
	//	HPEN hPen;
	//	HPEN hPenOld;
	//客户区坐标系中的坐标
	RECT rectClient;
	//屏幕坐标系中的坐标
	RECT rectWin;
	PAINTSTRUCT ps;


	//高度的偏移量
	static unsigned  offset=40;
	//高度与宽度的差
	static unsigned heightSubWid=50;
	//初始宽度
	static unsigned initWidth=550;
	int xPos;
	int yPos;
	int screenWidth;
	int screenHeight;

	static int preWinx;
	static unsigned count=0;			//已下棋子的个数
	POINT pt;							//单击选中的交叉点客户区坐标
	POINT ptIndex;						//交叉点客户区坐标对应的坐标索引

	static POINT existedPoint[225];			//存储已下棋子的坐标索引	
	static int blackIndex[15][15];
	static int whiteIndex[15][15];

	RECT rt;
	unsigned width = 0;
	unsigned height = 0;
	unsigned titleHeight = 0;

	switch(uMsg)
	{
		//键入字符消息
	case WM_CHAR:
		MessageBox(hwnd,TEXT("char"),TEXT("char"),0);
		break;
		//	case WM_CREATE:
		//	GetWindowRect(hwnd, &r);
		//	hdc=GetDC(hwnd);
		//	hPen = CreatePen( PS_SOLID , 3 , RGB( 0 , 0 , 0 ));
		//	hPenOld = ( HPEN )SelectObject ( hdc , hPen );
		//	MoveToEx(hdc, 100, 0, NULL);
		//	LineTo(hdc, 100, 100);
		//	SelectObject(hdc, hPenOld);
		//	DeleteObject(hPen);
		//	break;
		//		case WM_SIZING:
		//			GetWindowRect(hwnd, &r);
		//	break;
		//窗口大小改变消息
	case WM_SIZE:
		//获得窗口在屏幕坐标系下的坐标
		GetWindowRect(hwnd, &rectWin);
		//获得窗口在客户区坐标系下的坐标
		GetClientRect(hwnd, &rectClient);

		SystemParametersInfo(SPI_GETWORKAREA, 0, &rt, SPIF_SENDCHANGE);	
		//计算窗口的宽度和高度，高度为实际减去偏移值
		width = rectWin.right - rectWin.left;
		height = rectWin.bottom - rectWin.top - heightSubWid;
		titleHeight = rectWin.bottom - rectWin.top -
			(rectClient.bottom - rectClient.top);
		//	screenWidth = GetSystemMetrics(SM_CXFRAME);
		//	screenHeight = GetSystemMetrics(SM_CYFRAME);
		screenWidth = rt.right - rt.left;
		screenHeight = rt.bottom - rt.top;


		/*
		char str[25];
		itoa(rt.bottom-rt.top, str, 10);
		MessageBox(hwnd, str, "显示", MB_OKCANCEL);
		//	itoa(rectWin.left, str, 10);
		//	MessageBox(hwnd, str, "显示", MB_OKCANCEL);						
		*/
		//若小于初始宽度或高度，恢复为初始值

		if (wParam == SIZE_MAXIMIZED)
		{
			//	MessageBox(hwnd, "最大化", "显示", MB_OKCANCEL);
			MoveWindow(hwnd, preWinx, 0, 
				screenHeight - offset, screenHeight, TRUE);
			UpdateWindow(hwnd);
		}
		else if(width < initWidth || height < initWidth )
			MoveWindow(hwnd, rectWin.left, rectWin.top, 
			initWidth, initWidth+heightSubWid, TRUE);
		//大于初始高度或宽度，按比例缩放
		else
		{
			if(width<height)
				MoveWindow(hwnd, rectWin.left, rectWin.top, 
				height, height+offset, TRUE);
			else if(width>height)
				MoveWindow(hwnd, rectWin.left, rectWin.top, 
				width, width+offset, TRUE);

		}
		preWinx=rectWin.left;
		break;

	case WM_PAINT:
		//		hdc=GetDC(hwnd);
		hdc=BeginPaint(hwnd, &ps);
		GetClientRect(hwnd, &rectClient);
		GetWindowRect(hwnd, &rectWin);

		/*
		char str[25];
		//	itoa(rectWin.right - rectWin.left , str, 10);
		//	MessageBox(hwnd, str, "显示", MB_OKCANCEL);
		itoa(rectClient.bottom-rectClient.top , str, 10);
		MessageBox(hwnd, str, "显示", MB_OKCANCEL);
		*/

		DisplayInitBackground(&hdc, rectClient, offset);
		DisplayChess(&hdc, existedPoint, count, rectClient, offset, RADIUS);
		ShowGameStatus(&hdc, count, rectClient, offset);
		EndPaint(hwnd, &ps);
		//			ReleaseDC(hwnd, hdc);
		break;

	case WM_LBUTTONDOWN:
		//获得单击的坐标
		xPos = GET_X_LPARAM(lParam); 
		yPos = GET_Y_LPARAM(lParam);
		GetClientRect(hwnd, &rectClient);
		hdc=GetDC(hwnd);



		/*			char str[25];
		itoa(screenWidth, str, 10);
		MessageBox(hwnd, str, "显示", MB_OKCANCEL);
		itoa(screenHeight, str, 10);
		MessageBox(hwnd, str, "显示", MB_OKCANCEL);
		*/


		//判断单击的是否为棋盘的交叉点
		if(IsIntersection(xPos, yPos, rectClient, offset, 5))
		{
			//找到单击的点
			pt = FindPosition(xPos, yPos, rectClient, offset, 5);
			//找到交叉点对应的坐标索引
			ptIndex = FindIndex(pt, rectClient, offset);
			//该点不存在棋子，绘制棋子
			if (!IsChessExisted(existedPoint, count, ptIndex))
			{

				//加入已下棋子数组中，并更新计数变量
				existedPoint[count++] = ptIndex;

				//显示棋子，并更新对应的索引数组
				//奇数，黑棋
				if (count%2)
				{
					DisplayBlackChess(&hdc, pt, RADIUS);
					blackIndex[ptIndex.x][ptIndex.y]=1;
					if(IsWin(ptIndex, blackIndex))
						MessageBox(hwnd,TEXT("黑棋胜"),TEXT("比赛结果")
						,MB_OKCANCEL | MB_ICONWARNING);
				}
				else
				{
					DisplayWhiteChess(&hdc, pt, RADIUS);
					whiteIndex[ptIndex.x][ptIndex.y]=1;
					if(IsWin(ptIndex, whiteIndex))
						MessageBox(hwnd,TEXT("白棋胜"),TEXT("比赛结果"),MB_OKCANCEL);
				}
				ShowGameStatus(&hdc, count, rectClient, offset);

			}


		}

		break;
	case WM_CLOSE:
		DestroyWindow(hwnd);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hwnd,uMsg,wParam,lParam);
	}

	return 0;
}