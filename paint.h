#ifndef PAINT_H
#define PAINT_H
#include "MyDataType.h"
#include "WriteOrRead.h"

void DrawPnt(CClientDC* dc, PNT_STRU point);
void ShowALLPnt(CClientDC* dc, CFile* pntTmpF, int pntNum);

void DrawSeg(CClientDC* dc, LIN_NDX_STRU line, POINT point1, POINT point2);//�����߶εĺ�������
void ShowAllLin(CClientDC* dc, CFile* LinTmpNdxF, CFile* LinTmpDatF, int LinNum);//��ʾ�����ߵĺ�������

//��ʾ���е�(��)�ĺ�������
void ShowAllPnt(CClientDC * dc, CFile * PntTmpF, int PntNum, double zoomoffset_x,
	double zoom0ffset_y, double zoom, char isDel);
// ��ʾ������(��)�ĺ�������
void ShowAllLin(CClientDC * dc, CFile * LinTmpNdxF, CFile * LinTmpDatF, 
	int LinNum, double zoomoffset_x, double zoomoffset_y, double zoom, char isDel);

void ShowAllReg(CClientDC* dc, CFile* RegTmpNdxF, CFile* RegTmpDatF, int RegNum);//��ʾ�������ĺ�������

//��ʾ�����£��ĺ�������
void ShowAllReg(CClientDC* dc, CFile* RegTmpNdxF, CFile* RegTmpDatF,
	int RegNum, double zoomOffset_x, double zoomOffset_y, double zoom, char isDel);
//���������������ĺ�������
void DrawReg(CClientDC* dc, REG_NDX_STRU Region, POINT* pt, long nPnt);

#endif


