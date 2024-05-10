#ifndef CALCULATE_H
#define CALCULATE_H
#include "MyDataType.h"

double Distance(double x1, double y1, double x2, double y2);//计算两点距离的函数声明
PNT_STRU FindPnt(CPoint mousePoint, int pntNum,CFile* pntTmpF, int& nPnt);//查找最近点的函数声明

void PntToDot(D_DOT* dot, POINT* pnt, int num);
void PntToDot(D_DOT& dot, POINT pnt);
void DotToPnt(POINT* pnt, D_DOT* dot, int num); 
void DotToPnt(POINT& pnt, D_DOT dot);

double DisPntToSeg(D_DOT pt1, D_DOT pt2, D_DOT pt);//计算鼠标单击位置到线的距离的函数声明
//查找离鼠标最近的线的函数声明
LIN_NDX_STRU FindLin(CFile* LinTmpNdxF, CFile* LinTmpDatF, CPoint mousePoint, int LinNum, int& nLinNdx);
//更新线数据的函数声明
void UpdateLin(CFile* LinTmpNdxF, int nLin, LIN_NDX_STRU line);

/*数据坐标系转换到窗口坐标系*/
void PntDPtoVP(D_DOT& pt, double zoom, double offset_x, double offset_y);

//计算矩形中心的函数声明
D_DOT GetCenter(RECT rect);

// 计算拉框放大时放大倍数的函数声明
void modulusZoom(RECT client, RECT rect, double& zoom);

//窗口坐标系转换到数据坐标系的函数声明
void PntVPtoDP(D_DOT& pt, double zoom, double offset_x, double offset_y);

/*改变线的点数据*/
void AlterLindot(CFile* LinTmpDatF, LIN_NDX_STRU startLine,
	LIN_NDX_STRU endLine, int start, int end, long allDataOff);

// 判断单击位置是否在区内的函数声明
BOOL PtInPolygon(CPoint p,D_DOT * ptPolygon,int nCount);

//查找离鼠标点最近区的函数声明
REG_NDX_STRU FindReg(CFile * RegTmpNdxF, CFile * RegTmpDatF, CPoint mousePoint, int RegNum, int& nRegNdx);

//查找最近删除点的函数声明
PNT_STRU FindDeletePnt(CPoint mousePoint, int PntNum, CFile* PntImpF, int& nPnt);

//查找最近删除线的函数声明
LIN_NDX_STRU FindDeleteLin(CFile* LinTmpNdxF, CFile* LinTmpDatF, CPoint mousePoint, int LinNum, int& nLinNdx);

//查找最近删除区的函数声明
REG_NDX_STRU FindDeleteReg(CFile* RegTmpNdxF, CFile* RegTmpDatF, CPoint mousePoint, int RegNum, int& nRegNdx);

#endif
