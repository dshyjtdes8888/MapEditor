#include "pch.h"
#include "framework.h"
#include "Pain.h"
#include <math.h>
#include "MyDataType.h"

void DrawSeg(CClientDC* dc, LIN_NDX_STRU line, POINT point1, POINT point2)
{

	CPen pen;
	switch (line.pattern)
	{
	case 0:  //实线
		pen.CreatePen(PS_SOLID, 1, line.color);  //创建一个实线的画笔
		break;
	case 1:  //虚线
		pen.CreatePen(PS_DASH, 1, line.color);  // 创建一个虚线的画笔
		break;
	case 2:  //点线
		pen.CreatePen(PS_DOT, 1, line.color);  //创建一个点线的画笔
		break;
	default:
		break;
	}
	CPen* oldPen = dc->SelectObject(&pen);
	dc->MoveTo(point1.x, point1.y); // 开始画线, 将光标移到一个初始位置
	dc->LineTo(point2.x, point2.y); // 绘制线:从初始点到移动点
	dc->SelectObject(oldPen);
}
