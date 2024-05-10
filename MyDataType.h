#ifndef MYDATATYPE_H
#define MYDATATYPE_H

typedef struct{
	double x;
	double y;
    COLORREF color;
	int pattern;
	char isDel;
}PNT_STRU;

typedef struct {
	char flag[3];  //标识符PNT LINREG
	int version;  //10，可理解为1.0版本
}VERSION;

typedef struct {
	char isDel;  //是否被删除
	COLORREF color;  //线颜色
	int pattern;  //线型号
	long dotNum;   //线节点数
	long datOff;  //线节点坐标数据存储位置
}LIN_NDX_STRU;

typedef struct {
	double x; //节点x坐标
	double y; //节点y坐标
}D_DOT;

typedef struct{
	char isDel;//是否被删除
	COLORREF color; // 区颜色
	int pattern; // 图案（号）
	long dotNum;// 边界节点数
	long datOff;// 边界节点数据存储位置
}REG_NDX_STRU;

#endif


