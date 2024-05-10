#include "pch.h"
#include "Calculate.h"
#include "WriteOrRead.h"
#include <math.h>

//计算两点之间距离的函数
double Distance(double x1, double y1, double x2, double y2)
{
	return(sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2)));
}
//判断浮点数大小
bool isSmall(double x1, double x2)
{
	if (x1 < x2)
		return false;
	else
		return true;
}

PNT_STRU FindPnt(CPoint mousePoint, int pntNum, CFile* pntTmpF, int& nPnt)
{
	PNT_STRU point,tPnt = { 0,0,RGB(0,0,0),0,0 };
	double min = 100;//在100个像素范围内寻找
	double dst=0;
	for (int i = 0; i < pntNum; i++)
	{
		ReadTempFileToPnt(pntTmpF,i,point);
		if (point.isDel)
			continue;
			dst = Distance(mousePoint.x, mousePoint.y, point.x, point.y); 
		if (dst<=min)
		{
			min = dst;
		    tPnt = point;
			nPnt = i;
		}
	}
	return(tPnt);
}


/*POINT转D_DoT的函数(线、区)*/
void PntToDot(D_DOT* dot, POINT* pnt, int num)
{
	for (int i = 0; i < num; ++i)
	{
		dot[i].x = pnt[i].x;
		dot[i].y = pnt[i].y;
	}
}

/*POINT转D_DOT的函数(点)*/
void PntToDot(D_DOT&dot, POINT pnt)
{
	dot.x = pnt.x;
	dot.y = pnt.y;
}
	
/*D_DoT转POINT的函数(线、区)*/
void DotToPnt(POINT* pnt, D_DOT* dot, int num)
{
	for (int i = 0; i < num; ++i)
	{
		pnt[i].x = (long)dot[i].x; 
		pnt[i].y = (long)dot[i].y;
	}
}

/*D_DoT转POINT的函数(点)*/
void DotToPnt(POINT & pnt, D_DOT dot)
{
	pnt.x = (long)dot.x;
	pnt.y = (long)dot.y;
}

/*计算鼠标单击位置到线之间的距离*/
double DisPntToSeg(D_DOT pt1, D_DOT pt2, D_DOT pt)
{
	// 求点到线段间最短距离函数
	// 定义向量积a，指示夹角(pt 2，pt 1，pt)
	double a = (pt2.x - pt1.x) * (pt.x - pt1.x) + (pt2.y - pt1.y) * (pt.y - pt1.y);
	// 定义向量积b，指示夹角(pt 1，pt 2，pt)
	double b = (pt1.x - pt2.x) * (pt.x - pt2.x) + (pt1.y - pt2.y) * (pt.y - pt2.y);
	if (a * b > 1e-10)
	{
		//如果a* b > 0，则两夹角均为锐角，最短距离为pt到线段的高
		double area; // 定义三角形面积
		double hight;//点到线段的高
		double s = (Distance(pt1.x, pt1.y, pt2.x, pt2.y)
			+ Distance(pt.x, pt.y, pt1.x, pt1.y)
			+ Distance(pt.x, pt.y, pt2.x, pt2.y)) / 2;//海伦公式的中间变量S 
		area = sqrt(s * (s - Distance(pt.x, pt.y, pt1.x, pt1.y))
			* (s - Distance(pt.x, pt.y, pt2.x, pt2.y))
			* (s - Distance(pt1.x, pt1.y, pt2.x, pt2.y)));
		hight = 2 * area / Distance(pt1.x, pt1.y, pt2.x, pt2.y); return hight;
	}
	else
	{
		return(Distance(pt1.x, pt1.y, pt.x, pt.y) > Distance(pt.x, pt.y, pt2.x, pt2.y))
			? Distance(pt.x, pt.y, pt2.x, pt2.y) : Distance(pt.x, pt.y, pt1.x, pt1.y);
	}
}

/*查找离鼠标最近的线*/
LIN_NDX_STRU FindLin(CFile* LinTmpNdxF,CFile* LinTmpDatF,CPoint mousePoint,int LinNum, int& nLinNdx)
{
	double min = 10;
	LIN_NDX_STRU tLine = {
		  tLine.isDel = 0,
	      tLine.color = RGB(0,0,0) ,
	      tLine.pattern = 0,
	      tLine.dotNum = 0,
	      tLine.datOff = 0},
	line;
	D_DOT pt1,pt2,mpt;
	CFile tempLinDatF;
	for (int i = 0; i < LinNum; ++i)
	{
		ReadTempFileToLinNdx(LinTmpNdxF,i,line);
		if (line.isDel == 0)
		{
			for (int j = 0; j < line.dotNum - 1; ++j)
			{
				ReadTempFileToLinDat(LinTmpDatF, line.datOff,j, pt1);
				ReadTempFileToLinDat(LinTmpDatF,line.datOff,j + 1, pt2);
				mpt.x = mousePoint.x;
				mpt.y = mousePoint.y;
				if (isSmall(min,DisPntToSeg(pt1, pt2,mpt)))
				{
					nLinNdx =i;
					min = DisPntToSeg(pt1, pt2, mpt);
					tLine = line;
				}
			}
		}
	}
	return tLine;
}

void UpdateLin(CFile* LinTmpNdxF, int nLin, LIN_NDX_STRU line)
{
	WriteLinNdxToFile(LinTmpNdxF, nLin, line);
}

/*数据坐标系转换到窗口坐标系*/
void PntDPtoVP(D_DOT& pt, double zoom, double offset_x, double offset_y) 
{
	pt.x = pt.x - offset_x;
	pt.y = pt.y - offset_y;
	pt.x = zoom * pt.x;
	pt.y = zoom * pt.y;
}

/*计算矩形中心*/
D_DOT GetCenter(RECT rect)
{
	D_DOT pt;
	pt.y = 0.5 * (rect.bottom + rect.top);
	pt.x = 0.5 * (rect.right + rect.left);
	return pt;
}

/*计算拉框放大时放大的倍数*/
void modulusZoom(RECT client, RECT rect, double& zoom) 
{
	zoom = min(client.right / (double)(rect.right - rect.left),
		client.bottom / (double)(rect.bottom - rect.top));
}

//窗口坐标系转换到数据坐标系
void PntVPtoDP(D_DOT&pt, double zoom, double offset_x, double offset_y) 
{
	pt.x = pt.x / zoom;
	pt.y = pt.y / zoom;
	pt.x = pt.x + offset_x;
	pt.y = pt.y + offset_y;
}

/*改变线的点数据*/
void AlterLindot(CFile* LinTmpDatF, LIN_NDX_STRU startLine, LIN_NDX_STRU endLine, int start, int end, long allDataOff)
{
	D_DOT pt1, pt2, pt3, pt4, point;
	int ndot = 0;
	//分别从临时文件中读取两条线的端点数据
	ReadTempFileToLinDat(LinTmpDatF, startLine.datOff, 0, pt1);
	ReadTempFileToLinDat(LinTmpDatF, startLine.datOff, startLine.dotNum - 1, pt2);
	ReadTempFileToLinDat(LinTmpDatF, endLine.datOff, 0, pt3);
	ReadTempFileToLinDat(LinTmpDatF, endLine.datOff, endLine.dotNum - 1, pt4);
	double d1 = min(Distance(pt1.x, pt1.y, pt3.x, pt3.y),
		Distance(pt1.x, pt1.y, pt4.x, pt4.y));//第一条线起点到第二条线端点的最短距离
	double d2 = min(Distance(pt2.x, pt2.y, pt3.x, pt3.y),
		Distance(pt2.x, pt2.y, pt4.x, pt4.y)); //第一条线终点到第二条线端点的最短距离
	if (d1 < d2)//第一条线的起点与第二条线连接
	{
		if (Distance(pt1.x, pt1.y, pt3.x, pt3.y) < Distance(pt1.x, pt1.y, pt4.x, pt4.y))
		{// 第一条线起点与第二条线起点连接
			for (int i = endLine.dotNum - 1; i >= 0; --i)
			{//反向读取第二条线的节点并依次写入文件中
				ReadTempFileToLinDat(LinTmpDatF, endLine.datOff, i, point);
				WriteLinDatToFile(LinTmpDatF, allDataOff, ndot, point);
				ndot++;
			}
			for (int i = 0; i < startLine.dotNum; ++i)
			{// 正向读取第一条线的节点并依次写入文件中)
				ReadTempFileToLinDat(LinTmpDatF, startLine.datOff, i, point);
				WriteLinDatToFile(LinTmpDatF, allDataOff, ndot, point);
				ndot++;
			}
		}
		else
		{//第一条线起点与第二条线终点连接
			for (int i = 0; i < endLine.dotNum; ++i)
			{//正向读取第二条线的节点并依次写入文件中
				ReadTempFileToLinDat(LinTmpDatF, endLine.dotNum, i, point);
				WriteLinDatToFile(LinTmpDatF, allDataOff, ndot, point);
				ndot++;
			}
			for (int i = 0; i < startLine.dotNum; ++i)
			{// 正向读取第一条线的节点并依次写入文件中
				ReadTempFileToLinDat(LinTmpDatF, startLine.datOff, i, point);
				WriteLinDatToFile(LinTmpDatF, allDataOff, ndot, point);
				ndot++;
			}
		}
	}
	else//第一条线的终点与第二条线连接
	{

		if (Distance(pt2.x, pt2.y, pt3.x, pt3.y) < Distance(pt2.x, pt2.y, pt4.x, pt4.y))
		{//第一条线终点与第二条线起点连接
			for (int i = 0; i < startLine.dotNum; ++i)
			{// 正向读取第一条线的节点并写入文件中
				ReadTempFileToLinDat(LinTmpDatF, startLine.datOff, i, point);
				WriteLinDatToFile(LinTmpDatF, allDataOff, ndot, point);
				ndot++;
			}
			for (int i = 0; i < endLine.dotNum; ++i)
			{//正向读取第二条线的节点并写入文件中
				ReadTempFileToLinDat(LinTmpDatF, endLine.datOff, i, point);
				WriteLinDatToFile(LinTmpDatF, allDataOff, ndot, point);
				ndot++;
			}
		}
		else
		{//第一条线终点与第二条线终点连接
			for (int i = 0; i < startLine.dotNum; ++i)
			{//正向读取第一条线的节点并写入文件中
				ReadTempFileToLinDat(LinTmpDatF, startLine.datOff, i, point);
				WriteLinDatToFile(LinTmpDatF, allDataOff, ndot, point);
				ndot++;
			}
			for (int i = endLine.dotNum - 1; i >= 0; --i)
			{//反向读取第二条线的节点并写入文件中
				ReadTempFileToLinDat(LinTmpDatF, endLine.datOff, i, point);
				WriteLinDatToFile(LinTmpDatF, allDataOff, ndot, point);
				ndot++;
			}
		}
	}
	ndot = 0;
}

/*判断单击位置是否在区内*/
BOOL PtInPolygon(CPoint p, D_DOT* ptPolygon, int nCount)
{
	int nCross = 0;
	for (int i = 0; i < nCount; i++)
	{
		D_DOT p1 = ptPolygon[i];
		D_DOT p2 = ptPolygon[(i + 1) % nCount];
		//求解y = p.y与p1 p2的交点.
		if (p1.y == p2.y)//p1p2与y = p0.y平行
			continue;
		if (p.y < min(p1.y, p2.y))//交点在p1p2延长线上
			continue;
		if (p.y >= max(p1.y, p2.y)) // 交点在p1p2延长线上
			continue;
		//求交点的X坐标；
		double x = (double)(p.y - p1.y) * (double)(p2.x - p1.x) / (double)(p2.y - p1.y) + p1.x;
		if (x > p.x)
			nCross++;//只统计单边交点
	}
	// 单边交点为偶数，点在多边形之外
	return(nCross % 2 == 1);
}

/*查找离鼠标点最近的区*/
REG_NDX_STRU FindReg(CFile* RegTmpNdxF, CFile* RegTmpDatF, CPoint mousePoint, int RegNum, int& nRegNdx)
{
	REG_NDX_STRU RegNdx,
		tRegNdx = { tRegNdx.isDel = 0,
				    tRegNdx.color = RGB(0,0,0),
				    tRegNdx.pattern = 0,
				    tRegNdx.dotNum = 0,
				    tRegNdx.datOff = 0 };
	for (int i = 0; i < RegNum; i++)
	{
		ReadTempFileToRegNdx(RegTmpNdxF, i, RegNdx); //从临时文件读区索引
		if (RegNdx.isDel == 0)
		{
			D_DOT* pt = new D_DOT[RegNdx.dotNum];
			for (int j = 0; j < RegNdx.dotNum; j++)
			{
				// 从临时文件中读取区的点数据
				ReadTempFileToRegDat(RegTmpDatF, RegNdx.datOff, j, pt[j]);
			}
			if (PtInPolygon(mousePoint, pt, RegNdx.dotNum))//点在区内
			{
				tRegNdx = RegNdx;
				nRegNdx = i;
				delete[]pt;
				break;
			}
			else
				delete[]pt;
		}
	}
	return tRegNdx;
}

PNT_STRU FindDeletePnt(CPoint mousePoint, int PntNum, CFile* PntTmpF, int& nPnt)
{
	PNT_STRU point;
	PNT_STRU tPnt = { tPnt.x = 0, tPnt.y = 0,
		tPnt.color = RGB(0, 0, 0),
		tPnt.pattern = 0, 
		tPnt.isDel = 0
	};
	double min = 10;//在10个像素范围内寻找
	for (int i = 0; i < PntNum; ++i)
	{
		ReadTempFileToPnt(PntTmpF, i, point); //从临时文件中读取点
		if (!point.isDel)
			continue;// 标记为删除点进行查找
		double dist = Distance(mousePoint.x, mousePoint.y, point.x, point.y);
		if (isSmall(min, dist))
		{
			min = dist;
			tPnt = point;
			nPnt = i;
		}
	}
	return tPnt;
}

//查找最近删除线的函数
LIN_NDX_STRU FindDeleteLin(CFile* LinTmpNdxF, CFile* LinTmpDatF, CPoint mousePoint, int LinNum, int& nLinNdx)
{
	double min = 10;
	LIN_NDX_STRU tLine = { tLine.isDel = 0,tLine.color = RGB(0, 0, 0), tLine.pattern = 0, tLine.dotNum = 0, tLine.datOff = 0 }, line;
	D_DOT pt1, pt2, mpt;
	CFile tempLinDatF;
	for (int i = 0; i < LinNum; ++i)
	{
		ReadTempFileToLinNdx(LinTmpNdxF, i, line);
		if (line.isDel != 0)
		{
			for (int j = 0; j < line.dotNum - 1; ++j)
			{
				ReadTempFileToLinDat(LinTmpDatF, line.datOff, j, pt1);
				ReadTempFileToLinDat(LinTmpDatF, line.datOff, j + 1, pt2);
				mpt.x = mousePoint.x;
				mpt.y = mousePoint.y;
				if (isSmall(min, DisPntToSeg(pt1, pt2, mpt)))
				{
					nLinNdx = i;
					min = DisPntToSeg(pt1, pt2, mpt);
					tLine = line;
				}
			}
		}
	}
	return tLine;
}

//查找最近删除区的函数
REG_NDX_STRU FindDeleteReg(CFile* RegTmpNdxF, CFile* RegTmpDatF, CPoint mousePoint, int RegNum, int& nRegNdx)
{
	REG_NDX_STRU RegNdx, tRegNdx = { tRegNdx.isDel = 0,tRegNdx.color = RGB(0, 0, 0), tRegNdx.pattern = 0, tRegNdx.dotNum = 0, tRegNdx.datOff = 0 };
	for (int i = 0; i < RegNum; i++)
	{
		ReadTempFileToRegNdx(RegTmpNdxF, i, RegNdx); //从临时文件读区索引
		if (RegNdx.isDel != 0)
		{
			D_DOT* pt = new D_DOT[RegNdx.dotNum];
			for (int j = 0; j < RegNdx.dotNum; j++)
			{
				//从临时文件中读取区的点数据
				ReadTempFileToRegDat(RegTmpDatF, RegNdx.datOff, j, pt[j]);
			}
			if (PtInPolygon(mousePoint, pt, RegNdx.dotNum)) //点在区内
			{
				tRegNdx = RegNdx;
				nRegNdx = i;
				delete[] pt;
				break;
			}
			else
			{
				delete[] pt;
			}
		}
	}
	return tRegNdx;
}