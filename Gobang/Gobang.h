/*-----------------------------------
	Gobang.h
	五子棋函数的定义文件
	开发环境：Visual Studio 2008
------------------------------------*/
#ifndef _GOBANG_H_
#define _GOBANG_H_

#ifndef _WINDOWS_
#include <windows.h>
#endif

/*
	显示五子棋游戏背景
	输入参数为设备环境句柄指针pdc，当前窗口大小r以及高度方向上的偏移offset
*/
BOOL DisplayInitBackground(HDC* pdc, RECT r, unsigned offset);

/*
	判断点击的坐标是否在棋盘线的交点上，是否可以放置棋子
	输入客户去坐标系中的横纵坐标、当前窗口在客户坐标系中的位置、高度偏差以及
	单击的允许偏差
*/
BOOL IsIntersection(int x, int y, RECT r, int offset, int shift);

/*
	找到单击的合法坐标对应的棋盘线交点的位置
	输入单击的横纵坐标，窗口客户区位置、高度偏移和允许的单击偏差
*/
POINT FindPosition(int x, int y, RECT r, int offset, int shift);

/*
	在指定的棋盘交叉点显示黑棋
	输入参数为设备坐标句柄指针,棋盘交叉点位置与棋子半径
*/
BOOL DisplayBlackChess(HDC* pdc, POINT pos, int radius);

/*
	在指定的棋盘交叉点显示白棋
	输入参数为设备坐标句柄指针,棋盘交叉点位置与棋子半径
*/
BOOL DisplayWhiteChess(HDC* pdc, POINT pos, int radius);

/*
	检查单击的棋盘线交叉点是否已有棋子
	输入参数为已下棋子的坐标数组头指针，已下的棋子个数与待判断的棋子坐标
*/
BOOL IsChessExisted(POINT* existedPoint, int count, POINT pt);

/*
	通过棋子的客户区坐标计算棋子对应的坐标索引
	输入棋子的客户区坐标，客户区大小与高度偏移
*/
POINT FindIndex(POINT pt,  RECT r, int offset);

/*
	根据坐标索引（重绘）显示棋子
	输入参数为设备坐标句柄指针,棋盘交叉点位置索引数组头指针，
	已下的棋子个数和客户区大小，高度偏移与棋子半径
*/
BOOL DisplayChess(HDC* pdc, POINT* existedPoint,
				 int count, RECT r,  int offset, int radius);

/*
	检查是否赢得了此局
	输入刚下的棋子坐标索引与已下的棋子坐标索引
*/
BOOL IsWin(POINT ptIndex, int chessIndex[15][15]);

/*
	显示比赛的状态，即当前的黑白棋颜色
	输入设备环境句柄指针，已下点数，窗口大小以及高度偏移量
*/
void ShowGameStatus(HDC* pdc, int count, RECT r, int offset);

#endif