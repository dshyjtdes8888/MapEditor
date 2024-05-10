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
	char flag[3];  //��ʶ��PNT LINREG
	int version;  //10�������Ϊ1.0�汾
}VERSION;

typedef struct {
	char isDel;  //�Ƿ�ɾ��
	COLORREF color;  //����ɫ
	int pattern;  //���ͺ�
	long dotNum;   //�߽ڵ���
	long datOff;  //�߽ڵ��������ݴ洢λ��
}LIN_NDX_STRU;

typedef struct {
	double x; //�ڵ�x����
	double y; //�ڵ�y����
}D_DOT;

typedef struct{
	char isDel;//�Ƿ�ɾ��
	COLORREF color; // ����ɫ
	int pattern; // ͼ�����ţ�
	long dotNum;// �߽�ڵ���
	long datOff;// �߽�ڵ����ݴ洢λ��
}REG_NDX_STRU;

#endif


