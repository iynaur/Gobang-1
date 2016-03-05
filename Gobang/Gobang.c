//Gobang.c
//五子棋函数的实现文件
#ifndef _WINDOWS_
#include <windows.h>
#endif
#include <math.h>
#ifndef _INC_STDLIB
#include <stdlib.h>
#endif
#include "Gobang.h"
//定义五子棋谱的线数
#ifndef LINECOUNT 
#define LINECOUNT 15
#endif
//显示五子棋游戏背景
//输入参数为设备环境句柄指针pdc，当前窗口大小r以及高度方向上的偏移offset
BOOL DisplayInitBackground(HDC* pdc, RECT r, unsigned offset)
{
	//五子棋盘每行、列宽、高度
	unsigned width = (r.right-r.left)/(LINECOUNT+1);
	//边缘多出的像素值，即上一步求整余下的值
	unsigned edge = r.right-r.left-width*(LINECOUNT+1);
	//计算棋谱位于窗口坐标系中的左上角点坐标
	unsigned left = edge/2+width;
	unsigned top = offset+edge/2+width;
	int i = 0;
	/*
	char str[25];
	itoa(left, str, 10);
	MessageBox(NULL, str, "显示", MB_OKCANCEL);
	itoa(top, str, 10);
	MessageBox(NULL, str, "显示", MB_OKCANCEL);
	*/

	HPEN hPen;			//将要使用的画笔
	HPEN hPenOld;		//当前画笔

	//定义画笔
	hPen = CreatePen(PS_SOLID, 2, RGB(0 , 0 , 0));
	if (hPen==NULL) return FALSE;
	//选中画笔
	hPenOld = (HPEN)SelectObject(*pdc , hPen );
	if (hPenOld==NULL) return FALSE;

	//绘制五子棋谱
	//绘制横线
	for (i = 0; i < LINECOUNT; ++i)
	{
		MoveToEx(*pdc, left, top+i*width, NULL);
		LineTo(*pdc, left+(LINECOUNT-1)*width, top+i*width);
	}
	//绘制纵线
	for (i = 0; i < LINECOUNT; ++i)
	{
		MoveToEx(*pdc, left+i*width, top, NULL);
		LineTo(*pdc, left+i*width, top+(LINECOUNT-1)*width);
	}
	if(SelectObject(*pdc, hPenOld)==NULL)
		return FALSE;
	return TRUE;

}

//判断点击的坐标是否在棋盘线的交点上，是否可以放置棋子
//输入客户去坐标系中的横纵坐标、当前窗口在客户坐标系中的位置、高度偏差以及
//单击的允许偏差
BOOL IsIntersection(int x, int y, RECT r, int offset, int shift)
{
	//五子棋盘每行、列宽、高度
	int width = (r.right-r.left)/(LINECOUNT+1);
	//边缘多出的像素值，即上一步求整余下的值
	int edge = r.right-r.left-width*(LINECOUNT+1);
	//计算棋谱位于窗口坐标系中的左上角点坐标
	int left = edge/2+width;
	int top = offset+edge/2+width;

	/*	
	char str[25];
	itoa(abs((x-left)%width), str, 10);
	MessageBox(NULL, str, "显示", MB_OKCANCEL);
	itoa(abs((y-top)%width), str, 10);
	MessageBox(NULL, str, "显示", MB_OKCANCEL);
	*/
	//在棋盘范围内
	if ((x-left) >= -shift  &&	(x-left) <= ((LINECOUNT-1)*width+shift)
		&& (y-top) >= -shift  && (y-top) <= ((LINECOUNT-1)*width+shift)  
		//在棋盘线交点误差范围内
		&&( (x-left)%width <= shift  || (x-left)%width >= width-shift )
		&&( (y-top)%width <= shift  || (y-top)%width >= width-shift )  )
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}

}

//找到单击的合法坐标对应的棋盘线交点的位置
//输入单击的横纵坐标，窗口客户区位置、高度偏移和允许的单击偏差
POINT FindPosition(int x, int y, RECT r, int offset, int shift)
{
	//五子棋盘每行、列宽、高度
	int width = (r.right-r.left)/(LINECOUNT+1);
	//边缘多出的像素值，即上一步求整余下的值
	int edge = r.right-r.left-width*(LINECOUNT+1);
	//计算棋谱位于窗口坐标系中的左上角点坐标
	int left = edge/2+width;
	int top = offset+edge/2+width;
	//棋盘线交叉点
	POINT pt;

	//寻找横坐标
	//超过交点
	if (abs((x-left)%width) <= shift)
	{
		pt.x = x-(x-left)%width;
	}
	//不到交点
	else if ((x-left)%width >= width-shift)
	{
		pt.x = x-(x-left)%width+width;
	}

	//寻找纵坐标
	//超过交点
	if (abs((y-top)%width) <= shift)
	{
		pt.y = y-(y-top)%width;
	}
	//不到交点
	else if ((y-top)%width >= width-shift)
	{
		pt.y = y-(y-top)%width+width;
	}

	return pt;
}

//在指定的棋盘交叉点显示黑棋
//输入参数为设备坐标句柄指针,棋盘交叉点位置与棋子半径
BOOL DisplayBlackChess(HDC* pdc, POINT pos, int radius)
{
	HBRUSH hBrush;		//将要使用的画刷
	HBRUSH hBrushOld;	//当前画刷

	HRGN rgn;			//棋子的区域

	//产生画刷
	hBrush=CreateSolidBrush(RGB(0,0,0));
	//选中画刷
	hBrushOld = (HBRUSH)SelectObject(*pdc , hBrush );

	//产生棋子区域
	rgn=CreateEllipticRgn(pos.x-radius, pos.y-radius, 
		pos.x+radius, pos.y+radius);
	//绘制区域
	PaintRgn(*pdc, rgn);
	//删除区域与对象
	DeleteObject(rgn);
	if(SelectObject(*pdc, hBrushOld)==NULL)
		return FALSE;
	return TRUE;
}

//在指定的棋盘交叉点显示白棋
//输入参数为设备坐标句柄指针,棋盘交叉点位置与棋子半径
BOOL DisplayWhiteChess(HDC* pdc, POINT pos, int radius)
{
	HBRUSH hBrush;		//将要使用的画刷
	HBRUSH hBrushOld;	//当前画刷

	HRGN rgn;			//棋子的区域

	//产生画刷
	hBrush=CreateSolidBrush(RGB(255,255,255));
	//选中画刷
	hBrushOld = (HBRUSH)SelectObject(*pdc , hBrush );

	//产生棋子区域
	rgn = CreateEllipticRgn(pos.x-radius, pos.y-radius, 
		pos.x+radius, pos.y+radius);
	//绘制区域
	PaintRgn(*pdc, rgn);
	//删除区域与对象
	DeleteObject(rgn);
	if(SelectObject(*pdc, hBrushOld)==NULL)
		return FALSE;
	return TRUE;
}

//检查单击的棋盘线交叉点是否已有棋子
//输入参数为已下棋子的坐标数组头指针，已下的棋子个数与待判断的棋子坐标
BOOL IsChessExisted(POINT* existedPoint, int count, POINT pt)
{
	int i = 0;
	for (i = 0; i < count; ++i)
	{
		if(pt.x == existedPoint[i].x && pt.y == existedPoint[i].y)
			return TRUE;
	}

	return FALSE;
}

//通过棋子的客户区坐标计算棋子对应的坐标索引
//输入棋子的客户区坐标，客户区大小与高度偏移
POINT FindIndex(POINT pt,  RECT r, int offset)
{
	//五子棋盘每行、列宽、高度
	int width = (r.right-r.left)/(LINECOUNT+1);
	//边缘多出的像素值，即上一步求整余下的值
	int edge = r.right-r.left-width*(LINECOUNT+1);
	//计算棋谱位于窗口坐标系中的左上角点坐标
	int left = edge/2+width;
	int top = offset+edge/2+width;
	//棋盘线交叉点坐标索引
	POINT ptIndex;

	//将坐标转化为索引
	ptIndex.x = (pt.x-left)/width;
	ptIndex.y = (pt.y-top)/width;

	return ptIndex;
}

//根据坐标索引（重绘）显示棋子
//输入参数为设备坐标句柄指针,棋盘交叉点位置索引数组头指针，
//已下的棋子个数和客户区大小，高度偏移与棋子半径
BOOL DisplayChess(HDC* pdc, POINT* existedPoint,
				  int count, RECT r,  int offset, int radius)
{
	//五子棋盘每行、列宽、高度
	int width = (r.right-r.left)/(LINECOUNT+1);
	//边缘多出的像素值，即上一步求整余下的值
	int edge = r.right-r.left-width*(LINECOUNT+1);
	//计算棋谱位于窗口坐标系中的左上角点坐标
	int left = edge/2+width;
	int top = offset+edge/2+width;
	//棋盘线交叉点客户区坐标
	POINT pt;

	int i = 0;

	//重绘、显示棋子
	for(i = 0; i < count; i++)
	{
		//坐标索引转化为客户区坐标
		pt.x = existedPoint[i].x*width+left;
		pt.y = existedPoint[i].y*width+top;

		//绘制棋子
		//i为奇数，白棋
		if(i%2)
		{
			if(!DisplayWhiteChess(pdc, pt, radius))
				return FALSE;
		}

		//偶数，黑棋
		else
		{
			if(!DisplayBlackChess(pdc, pt, radius))
				return FALSE;
		}


	}
	return TRUE;
}

//检查是否赢得了此局
//输入刚下的棋子坐标索引与已下的棋子坐标索引
BOOL IsWin(POINT ptIndex, int chessIndex[15][15])
{
	int i = 1;
	int count = 1;		//记录连续的棋子个数		
	//检查水平方向
	//向左检查
	while(chessIndex[ptIndex.x-i][ptIndex.y])
	{
		count++;
		i++;
	}
	//向右检查
	i = 1;
	while(chessIndex[ptIndex.x+i][ptIndex.y])
	{
		count++;
		i++;
	}
	//判断
	if(count >= 5)
		return TRUE;

	//检查竖直方向
	//向上检查
	i = 1;
	count = 1;
	while(chessIndex[ptIndex.x][ptIndex.y-i])
	{
		count++;
		i++;
	}
	//向右检查
	i = 1;
	while(chessIndex[ptIndex.x][ptIndex.y+i])
	{
		count++;
		i++;
	}
	//判断
	if(count >= 5)
		return TRUE;

	//检查主对角线方向
	//向左上检查
	i = 1;
	count = 1;
	while(chessIndex[ptIndex.x-i][ptIndex.y-i])
	{
		count++;
		i++;
	}
	//向右下检查
	i = 1;
	while(chessIndex[ptIndex.x+i][ptIndex.y+i])
	{
		count++;
		i++;
	}
	//判断
	if(count >= 5)
		return TRUE;

	//检查副对角线方向
	//向右上检查
	i = 1;
	count = 1;
	while(chessIndex[ptIndex.x+i][ptIndex.y-i])
	{
		count++;
		i++;
	}
	//向左下检查
	i = 1;
	while(chessIndex[ptIndex.x-i][ptIndex.y+i])
	{
		count++;
		i++;
	}
	//判断
	if(count >= 5)
		return TRUE;

	return FALSE;
}

//显示比赛的状态，即当前的黑白棋颜色
//输入设备环境句柄指针，已下点数，窗口大小以及高度偏移量
void ShowGameStatus(HDC* pdc, int count, RECT r, int offset)
{
	HPEN hPen;
	HPEN hOldPen;
	HFONT hFont;
	HFONT hOldFont;

	hPen=CreatePen(PS_SOLID, 3, RGB(0,0,0));
	hOldPen=SelectObject(*pdc, hPen);

	hFont=CreateFont(offset, (int)(offset/1.5),
		0,0,FW_THIN,FALSE,FALSE,FALSE,
		DEFAULT_CHARSET,OUT_CHARACTER_PRECIS,
		CLIP_CHARACTER_PRECIS,DEFAULT_QUALITY,
		FF_MODERN,TEXT("宋体"));
	hOldFont=SelectObject(*pdc, hFont);
	if (count%2)
	{
		TextOut(*pdc, 2, 2, TEXT("现在白棋"),4);
	}
	else
	{
		TextOut(*pdc, 2, 2, TEXT("现在黑棋"),4);
	}


	SelectObject(*pdc, hOldPen);
	DeleteObject(hPen);
	SelectObject(*pdc, hOldFont);
	DeleteObject(hFont);
}