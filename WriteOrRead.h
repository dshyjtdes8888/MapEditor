#ifndef WRITEORREAD_H
#define WRITEORREAD_H
#include "MyDataType.h"

void WritePntToFile(CFile* PntTmpF, int i, PNT_STRU point);//��������д����ʱ�ļ��ĺ�������
void ReadTempFileToPnt(CFile* PntTmpF, int i, PNT_STRU& point);//����ʱ���ļ���ȡ�����ݵĺ�������
void ReadPntPermanentFileToTemp(CFile* pntF, CFile* pntTmpF, int& nPnt, int& nLpnt);//�������ļ�������ʱ�ļ��ĺ�����������
void UpdatePnt(CFile* pntTmpF, int i, PNT_STRU pnt);//�޸���ʱ�ļ��е����ݵĺ�������

void WriteLinNdxToFile(CFile* linTmpNdxF, int i, LIN_NDX_STRU line);//��������д����ʱ�����ļ��ĺ�������
void WriteLinDatToFile(CFile* linTmpDatF, long datOff,int i, D_DOT point);//���߽ڵ�����д������ʱ�����ļ��ĺ�������
//���ߵ������͵�����д�������ļ��ĺ�������
void WriteTempToLinePermanentFile(CFile* LinF, CFile* LinTmpDatF, CFile* LinTmpNdxF,
	VERSION LinVer, int nLin, int nLLin);

void ReadTempFileToLinDat(CFile* LinTmpDatF, long datOff, int i, D_DOT& pnt);//����ʱ�������ļ��ж�ȡ�ߵĵ����ݵĺ�������
void ReadTempFileToLinNdx(CFile* LinTmpDNdxF, int i, LIN_NDX_STRU&LinNdx);//������ʱ�����ļ��ж�ȡ�������ĺ�������
//�������ļ���ȡ�����ݵ���ʱ�ļ��ĺ�������
void ReadLinPermanentFileToTemp(CFile* LinF, CFile* LinTmpDatF,
	CFile* LinTmpNdxF, VERSION& LinVer, int&nLin, int&nLLin, long&TempFLinDatOffset);

//�����ߵĵ����ݵ���ʱ�ļ�
void UpdateLin(CFile*LinTmpNdxF,CFile*LinTmpDatF,int LinNdx,double offset_x,double offse_y);

//�޸ĵ�-���������ĺ�������
void AlterStartLin(CFile* LinTnpNdxF, long subdatOff, int nLine, int subNum);
//�޸ĵڶ����������ĺ�������
void AlterEndLin(CFile* LinTmpNdxF, int nLine);

void WriteRegNdxToFile(CFile* RegTmpNdxF, int i, REG_NDX_STRU Region); 
void WriteRegDatToFile(CFile* RegTmpDatF, long datOff, int i, D_DOT point);
void ReadTempFileToRegNdx(CFile* RegTmpNdxF,int i, REG_NDX_STRU &RegNdx); //������ʱ�����ļ��ж�ȡ�������ĺ�������
void ReadTempFileToRegDat(CFile* RegTmpDatF, long dat0ff, int i, D_DOT& Pnt);//����ʱ�������ļ��ж�ȡ���ĵ����ݵĺ�������

// ��ʱ�����ļ����������ݵĺ�������
void UpdateReg(CFile * RegTmpNdxF, int nReg,REG_NDX_STRU Region);

//�����������͵�����д�������ļ��ĺ�������
void WriteTempToRegPermanentFile(CFile* RegF, CFile* RegTmpDatF, CFile* RegTmpNdxF,
	VERSION RegVer, int nReg, int nRReg);

//�������ļ���ȡ�����ݵ���ʱ�ļ��ĺ�������
void ReadRegPermanentFileToTemp(CFile* RegF, CFile* RegTmpDatF,
	CFile* RegTmpNdxF, VERSION& RegVer, int& nReg, int& nRReg, long& TempFRegDatOffset);

// �������ĺ�������
void UpdateReg(CFile * RegTmpNdxF, CFile * RegTmpDatF,int RegNdx,double offset_x, double offset_y);

#endif


