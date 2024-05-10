#ifndef PAINT_H
#define PAINT_H
#include "MyDataType.h"
#include "WriteOrRead.h"

void DrawPnt(CClientDC* dc, PNT_STRU point);
void ShowALLPnt(CClientDC* dc, CFile* pntTmpF, int pntNum);

void DrawSeg(CClientDC* dc, LIN_NDX_STRU line, POINT point1, POINT point2);//构造线段的函数声明
void ShowAllLin(CClientDC* dc, CFile* LinTmpNdxF, CFile* LinTmpDatF, int LinNum);//显示所有线的函数声明

//显示所有点(新)的函数声明
void ShowAllPnt(CClientDC * dc, CFile * PntTmpF, int PntNum, double zoomoffset_x,
	double zoom0ffset_y, double zoom, char isDel);
// 显示所有线(新)的函数声明
void ShowAllLin(CClientDC * dc, CFile * LinTmpNdxF, CFile * LinTmpDatF, 
	int LinNum, double zoomoffset_x, double zoomoffset_y, double zoom, char isDel);

void ShowAllReg(CClientDC* dc, CFile* RegTmpNdxF, CFile* RegTmpDatF, int RegNum);//显示所有区的函数声明

//显示区（新）的函数声明
void ShowAllReg(CClientDC* dc, CFile* RegTmpNdxF, CFile* RegTmpDatF,
	int RegNum, double zoomOffset_x, double zoomOffset_y, double zoom, char isDel);
//绘制区（造区）的函数声明
void DrawReg(CClientDC* dc, REG_NDX_STRU Region, POINT* pt, long nPnt);

#endif


