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
	case 0:  //ʵ��
		pen.CreatePen(PS_SOLID, 1, line.color);  //����һ��ʵ�ߵĻ���
		break;
	case 1:  //����
		pen.CreatePen(PS_DASH, 1, line.color);  // ����һ�����ߵĻ���
		break;
	case 2:  //����
		pen.CreatePen(PS_DOT, 1, line.color);  //����һ�����ߵĻ���
		break;
	default:
		break;
	}
	CPen* oldPen = dc->SelectObject(&pen);
	dc->MoveTo(point1.x, point1.y); // ��ʼ����, ������Ƶ�һ����ʼλ��
	dc->LineTo(point2.x, point2.y); // ������:�ӳ�ʼ�㵽�ƶ���
	dc->SelectObject(oldPen);
}
