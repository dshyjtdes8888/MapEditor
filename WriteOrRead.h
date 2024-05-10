#ifndef WRITEORREAD_H
#define WRITEORREAD_H
#include "MyDataType.h"

void WritePntToFile(CFile* PntTmpF, int i, PNT_STRU point);//将点数据写入临时文件的函数声明
void ReadTempFileToPnt(CFile* PntTmpF, int i, PNT_STRU& point);//从临时点文件读取点数据的函数声明
void ReadPntPermanentFileToTemp(CFile* pntF, CFile* pntTmpF, int& nPnt, int& nLpnt);//从永久文件读到临时文件的函数声明，点
void UpdatePnt(CFile* pntTmpF, int i, PNT_STRU pnt);//修改临时文件中点数据的函数声明

void WriteLinNdxToFile(CFile* linTmpNdxF, int i, LIN_NDX_STRU line);//将线数据写入临时索引文件的函数声明
void WriteLinDatToFile(CFile* linTmpDatF, long datOff,int i, D_DOT point);//将线节点数据写入线临时数据文件的函数声明
//将线的索引和点数据写入永久文件的函数声明
void WriteTempToLinePermanentFile(CFile* LinF, CFile* LinTmpDatF, CFile* LinTmpNdxF,
	VERSION LinVer, int nLin, int nLLin);

void ReadTempFileToLinDat(CFile* LinTmpDatF, long datOff, int i, D_DOT& pnt);//从临时线数据文件中读取线的点数据的函数声明
void ReadTempFileToLinNdx(CFile* LinTmpDNdxF, int i, LIN_NDX_STRU&LinNdx);//从线临时索引文件中读取线索引的函数声明
//从永久文件读取线数据到临时文件的函数声明
void ReadLinPermanentFileToTemp(CFile* LinF, CFile* LinTmpDatF,
	CFile* LinTmpNdxF, VERSION& LinVer, int&nLin, int&nLLin, long&TempFLinDatOffset);

//更新线的点数据到临时文件
void UpdateLin(CFile*LinTmpNdxF,CFile*LinTmpDatF,int LinNdx,double offset_x,double offse_y);

//修改第-条线索引的函数声明
void AlterStartLin(CFile* LinTnpNdxF, long subdatOff, int nLine, int subNum);
//修改第二条线索引的函数声明
void AlterEndLin(CFile* LinTmpNdxF, int nLine);

void WriteRegNdxToFile(CFile* RegTmpNdxF, int i, REG_NDX_STRU Region); 
void WriteRegDatToFile(CFile* RegTmpDatF, long datOff, int i, D_DOT point);
void ReadTempFileToRegNdx(CFile* RegTmpNdxF,int i, REG_NDX_STRU &RegNdx); //从区临时索引文件中读取区索引的函数声明
void ReadTempFileToRegDat(CFile* RegTmpDatF, long dat0ff, int i, D_DOT& Pnt);//从临时区数据文件中读取区的点数据的函数声明

// 临时索引文件更新区数据的函数声明
void UpdateReg(CFile * RegTmpNdxF, int nReg,REG_NDX_STRU Region);

//将区的索引和点数据写入永久文件的函数声明
void WriteTempToRegPermanentFile(CFile* RegF, CFile* RegTmpDatF, CFile* RegTmpNdxF,
	VERSION RegVer, int nReg, int nRReg);

//从永久文件读取线数据到临时文件的函数声明
void ReadRegPermanentFileToTemp(CFile* RegF, CFile* RegTmpDatF,
	CFile* RegTmpNdxF, VERSION& RegVer, int& nReg, int& nRReg, long& TempFRegDatOffset);

// 更新区的函数声明
void UpdateReg(CFile * RegTmpNdxF, CFile * RegTmpDatF,int RegNdx,double offset_x, double offset_y);

#endif


