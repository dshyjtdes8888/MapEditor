#ifndef CALCULATE_H
#define CALCULATE_H
#include "MyDataType.h"

double Distance(double x1, double y1, double x2, double y2);//�����������ĺ�������
PNT_STRU FindPnt(CPoint mousePoint, int pntNum,CFile* pntTmpF, int& nPnt);//���������ĺ�������

void PntToDot(D_DOT* dot, POINT* pnt, int num);
void PntToDot(D_DOT& dot, POINT pnt);
void DotToPnt(POINT* pnt, D_DOT* dot, int num); 
void DotToPnt(POINT& pnt, D_DOT dot);

double DisPntToSeg(D_DOT pt1, D_DOT pt2, D_DOT pt);//������굥��λ�õ��ߵľ���ĺ�������
//���������������ߵĺ�������
LIN_NDX_STRU FindLin(CFile* LinTmpNdxF, CFile* LinTmpDatF, CPoint mousePoint, int LinNum, int& nLinNdx);
//���������ݵĺ�������
void UpdateLin(CFile* LinTmpNdxF, int nLin, LIN_NDX_STRU line);

/*��������ϵת������������ϵ*/
void PntDPtoVP(D_DOT& pt, double zoom, double offset_x, double offset_y);

//����������ĵĺ�������
D_DOT GetCenter(RECT rect);

// ��������Ŵ�ʱ�Ŵ����ĺ�������
void modulusZoom(RECT client, RECT rect, double& zoom);

//��������ϵת������������ϵ�ĺ�������
void PntVPtoDP(D_DOT& pt, double zoom, double offset_x, double offset_y);

/*�ı��ߵĵ�����*/
void AlterLindot(CFile* LinTmpDatF, LIN_NDX_STRU startLine,
	LIN_NDX_STRU endLine, int start, int end, long allDataOff);

// �жϵ���λ���Ƿ������ڵĺ�������
BOOL PtInPolygon(CPoint p,D_DOT * ptPolygon,int nCount);

//����������������ĺ�������
REG_NDX_STRU FindReg(CFile * RegTmpNdxF, CFile * RegTmpDatF, CPoint mousePoint, int RegNum, int& nRegNdx);

//�������ɾ����ĺ�������
PNT_STRU FindDeletePnt(CPoint mousePoint, int PntNum, CFile* PntImpF, int& nPnt);

//�������ɾ���ߵĺ�������
LIN_NDX_STRU FindDeleteLin(CFile* LinTmpNdxF, CFile* LinTmpDatF, CPoint mousePoint, int LinNum, int& nLinNdx);

//�������ɾ�����ĺ�������
REG_NDX_STRU FindDeleteReg(CFile* RegTmpNdxF, CFile* RegTmpDatF, CPoint mousePoint, int RegNum, int& nRegNdx);

#endif
