
// MapEditorView.cpp: CMapEditorView 类的实现
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "MapEditor.h"
#endif

#include "MapEditorDoc.h"
#include "MapEditorView.h"

#include "CCreateFileDlg.h"
#include "MyDataType.h"
#include "paint.h"
#include "WriteOrRead.h"
#include "Calculate.h"
#include "PointParameterDlg.h"
#include "LineParameterDlg.h"
#include "RegionParameterDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

///————————点数据相关的全局控制变量————————///
bool GPntFCreated = false;  //临时文件是否创建
CString GPntFName;  //永久文件名（含路径）
CString GPntTmpFName = CString("tempPntF.dat");  //临时文件名（含路径）
bool GPntChanged = false;  //是否更改
int GPntNum =0;  //物理数
int GPntLNum = 0;  //逻辑数
CFile* GPntTmpF = new CFile();  //读取临时文件的指针对象

///————————线数据相关的全局控制变量————————///
bool GLinFCreated = false;  //临时文件是否创建
CString GLinFName;  //永久文件名（含路径）
CString GLinTmpNdxFName = CString("tempLinF.ndx");  //临时索引文件名（含路径）
CString GLinTmpDatFName = CString("tempLinF.dat");  //临时数据文件名（含路径）
bool GLinChanged = false;  //是否更改
int GLinNum = 0;  //物理数
int GLinLNum = 0;  //逻辑数
CFile* GLinTmpDatF = new CFile();  //读取临时数据文件的指针对象
CFile* GLinTmpNdxF = new CFile();  //读取临时索引文件的指针对象

///————————区数据相关的全局控制变量————————///
bool GRegFCreated = false;  //临时文件是否创建
CString GRegFName;  //永久文件名（含路径）
CString GRegTmpNdxFName = CString("tempRegF.ndx");  //临时索引文件名（含路径）
CString GRegTmpDatFName = CString("tempRegF.dat");  //临时数据文件名（含路径）
bool GRegChanged = false;  //是否更改
int GRegNum = 0;  //物理数
int GRegLNum = 0;  //逻辑数
CFile* GRegTmpDatF = new CFile();  //读取临时数据文件的指针对象
CFile* GRegTmpNdxF = new CFile();  //读取临时索引文件的指针对象

VERSION GPntVer = {
	GPntVer.flag[0] = 'P',
	GPntVer.flag[1] = 'N',
	GPntVer.flag[2] = 'T',
	GPntVer.version=10  //默认版本号
};

///————————与操作相关————————///
enum Action{
	Noaction,
	OPERSTATE_INPUT_PNT,
	OPERSTATE_DELETE_PNT,
	OPERSTATE_MOVE_PNT,
	OPERSTATE_INPUT_LIN,
	OPERSTATE_DELETE_LIN,
	OPERSTATE_MOVE_LIN,
	OPERSTATE_INPUT_REG,
	OPERSTATE_DELETE_REG,
	OPERSTATE_MOVE_REG,
	OPERSTATE_ZOOM_IN,
	OPERSTATE_ZOOM_OUT,
	OPERSTATE_WINDOW_MOVE,
	OPERSTATE_LIN_DELETE_PNT,
	OPERSTATE_LIN_ADD_PNT,
	OPERSTATE_LINK_LIN,
	OPERSTATE_MODIFY_POINT_PARAMETER,
	OPERSTATE_MODIFY_LINE_PARAMETER,
	OPERSTATE_MODIFY_REGION_PARAMETER,
	OPERSTATE_UNDELETE_PNT,
	OPERSTATE_UNDELETE_LIN,
	OPERSTATE_UNDELETE_REG
};//枚举操作状态
Action GCurOperState; // 操作参数

///————————默认点结构与临时点结构————————///
PNT_STRU GPnt = { GPnt.isDel=0,GPnt.color = RGB(0,0,0),GPnt.pattern = 0 };  //默认点参数

int GPntNdx = -1; //找到的点位于文件中的位置

PNT_STRU GTPnt;//临时点，存储找到的点数据

///-----------------默认线索引结构、临时线索引结构及其相关----------------///
LIN_NDX_STRU GLin={GLin.isDel = 0,GLin.color = RGB(0,0,0),GLin.pattern = 0,
GLin.dotNum = 0,GLin.datOff = 0};   // 默认线参数
LIN_NDX_STRU GTLin;  //线
POINT GLPnt={GLPnt.x=-1,GLPnt.y=-1}; //记录线段的起点
CPoint GMPnt(-1, -1);   //记录鼠标上一状态的点

int GLinNdx = -1;//找到线位于文件中的位置

VERSION GLinVer = {
	GLinVer.flag[0] = 'L',
	GLinVer.flag[1] = 'I',
	GLinVer.flag[2] = 'N',
	GLinVer.version = 10 //默认版本号
};

CPoint GLinLBDPnt(-1,-1);//记录鼠标左键按下的位置，用来计算偏移量
CPoint GLinMMPnt(-1, -1);//记录鼠标移动时的上一状态，用来擦除移动时的前一条线
long GLinMMOffsetX = 0; //记录鼠标移动时候的x轴的偏移量；
long GLinMMOffsetY = 0; // 记录鼠标移动时候的Y轴的偏移量
LIN_NDX_STRU GLinMMTmpNdx; //记录鼠标选中的线的索引

CPoint GZoomLBDPnt(-1, -1);// 放大时鼠标左键抬起的点
CPoint GZoomMMPnt(-1, -1); //放大时鼠标移动前一状态

double GZoomOffset_x = 0;//偏移向量
double GZoomOffset_y = 0;
double GZoom = 1.0;// 缩放系数
int GZoomStyle = 0;//放大方式

///——————与连接线相关——————///
LIN_NDX_STRU GStartLin = GLin;  //选中的第一条线
int GnStart = -1;
LIN_NDX_STRU GEndLin = GLin;  //选中的第二条线
int GnEnd = -1;
int GnLine = 0;

///--------------------造区过程相关的点数据--------------------///
CPoint GRegCreateMMPnt(-1,-1); // 鼠标移动前一状态点
CPoint GRegCreateStartPnt(-1, -1); // 造区的起点
///--------------默认区索引结构、临时索引结构及其相关--------------///
REG_NDX_STRU GReg = {GReg.isDel = 0,GReg.color = RGB(0,0,0),
GReg.pattern = 0, GReg.dotNum = 0,GReg.datOff = 0 };
REG_NDX_STRU GTReg;

VERSION GRegVer = {
	GRegVer.flag[0] = 'R',
	GRegVer.flag[1] = 'E',
	GRegVer.flag[2] = 'G',
	GRegVer.version = 10 //默认版本号
};

int GRegNdx = -1;//找到的区位于文件中的位置

REG_NDX_STRU GRegMMTmpNdx; //记录鼠标选中的区的索引
CPoint GRegLBDPnt(-1,-1);//记录鼠标左键按下的位置，用来计算偏移量
CPoint GRegMMPnt(-1,-1);//记录鼠标移动时上一状态，擦除移动时前一个区
long GRegMMOffsetX = 0;//记录鼠标移动时候的X轴的偏移量
long GRegMMOffsetY = 0;//记录鼠标移动时候的Y轴的偏移量

///-----------------移动窗口-------------------
CPoint GWinMoveLBDPnt(-1, -1);//移动窗口时左键按下点
CPoint GWinMoveMMPnt(-1,- 1);//移动窗口时鼠标移动前状态点位置

// 外包矩形的顶点坐标
double GMaxX = 0;
double GMaxY = 0;
double GMinX = 0;
double GMinY = 0;

enum State{SHOWSTATE_UNDEL,SHOWSTATE_DEL};//枚举显示类型
State GCurShowState=SHOWSTATE_UNDEL;//显示状态，默认为显示非删除状态
bool GShowPnt = true;//当前显示的结构是否为点
bool GShowLin = true;//当前显示的结构是否为线
bool GShowReg = true;//当前显示的结构是否为区

// CMapEditorView

IMPLEMENT_DYNCREATE(CMapEditorView, CView)

BEGIN_MESSAGE_MAP(CMapEditorView, CView)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_COMMAND(ID_WINDOW_ZOOM_IN, &CMapEditorView::OnWindowZoomIn)
	ON_COMMAND(ID_FILE_NEW, &CMapEditorView::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN_POINT, &CMapEditorView::OnFileOpenPoint)
	ON_COMMAND(ID_FILE_OPEN_LINE, &CMapEditorView::OnFileOpenLine)
	ON_COMMAND(ID_FILE_OPEN_REGION, &CMapEditorView::OnFileOpenRegion)
	ON_COMMAND(ID_FILE_SAVE_POINT, &CMapEditorView::OnFileSavePoint)
	ON_COMMAND(ID_FILE_SAVE_LINE, &CMapEditorView::OnFileSaveLine)
	ON_COMMAND(ID_FILE_SAVE_REGION, &CMapEditorView::OnFileSaveRegion)
	ON_COMMAND(ID_FILE_SAVE_AS_POINT, &CMapEditorView::OnFileSaveAsPoint)
	ON_COMMAND(ID_FILE_SAVE_AS_LINE, &CMapEditorView::OnFileSaveAsLine)
	ON_COMMAND(ID_FILE_SAVE_AS_REGION, &CMapEditorView::OnFileSaveAsRegion)
	ON_COMMAND(ID_APP_EXIT, &CMapEditorView::OnAppExit)
	ON_COMMAND(ID_WINDOW_ZOOM_OUT, &CMapEditorView::OnWindowZoomOut)
	ON_COMMAND(ID_WINDOW_MOVE, &CMapEditorView::OnWindowMove)
	ON_COMMAND(ID_WINDOW_RESET, &CMapEditorView::OnWindowReset)
	ON_COMMAND(ID_WINDOW_SHOW_POINT, &CMapEditorView::OnWindowShowPoint)
	ON_COMMAND(ID_WINDOW_SHOW_LINE, &CMapEditorView::OnWindowShowLine)
	ON_COMMAND(ID_WINDOW_SHOW_REGION, &CMapEditorView::OnWindowShowRegion)
	ON_COMMAND(ID_POINT_CREATE, &CMapEditorView::OnPointCreate)
	ON_COMMAND(ID_POINT_MOVE, &CMapEditorView::OnPointMove)
	ON_COMMAND(ID_POINT_DELETE, &CMapEditorView::OnPointDelete)
	ON_COMMAND(ID_POINT_UNDELETE, &CMapEditorView::OnPointUndelete)
	ON_COMMAND(ID_POINT_SHOW_DELETED, &CMapEditorView::OnPointShowDeleted)
	ON_COMMAND(ID_POINT_MODIFY_PARAMETER, &CMapEditorView::OnPointModifyParameter)
	ON_COMMAND(ID_POINT_SET_DEFPARAMETER, &CMapEditorView::OnPointSetDefparameter)
	ON_COMMAND(ID_LINE_CREATE, &CMapEditorView::OnLineCreate)
	ON_COMMAND(ID_LINE_MOVE, &CMapEditorView::OnLineMove)
	ON_COMMAND(ID_LINE_DELETE, &CMapEditorView::OnLineDelete)
	ON_COMMAND(ID_LINE_SHOW_DELETED, &CMapEditorView::OnLineShowDeleted)
	ON_COMMAND(ID_LINE_UNDELETE, &CMapEditorView::OnLineUndelete)
	ON_COMMAND(ID_LINE_DELETE_DOT, &CMapEditorView::OnLineDeleteDot)
	ON_COMMAND(ID_LINE_ADD_DOT, &CMapEditorView::OnLineAddDot)
	ON_COMMAND(ID_LINE_LINK, &CMapEditorView::OnLineLink)
	ON_COMMAND(ID_LINE_MODIFY_PARAMETER, &CMapEditorView::OnLineModifyParameter)
	ON_COMMAND(ID_LINE_SET_DEFPARAMETER, &CMapEditorView::OnLineSetDefparameter)
	ON_COMMAND(ID_REGION_CREATE, &CMapEditorView::OnRegionCreate)
	ON_COMMAND(ID_REGION_MOVE, &CMapEditorView::OnRegionMove)
	ON_COMMAND(ID_REGION_DELETE, &CMapEditorView::OnRegionDelete)
	ON_COMMAND(ID_REGION_SHOW_DELETE, &CMapEditorView::OnRegionShowDelete)
	ON_COMMAND(ID_REGION_UNDELETE, &CMapEditorView::OnRegionUndelete)
	ON_COMMAND(ID_REGION_MODIFY_PARAMETER, &CMapEditorView::OnRegionModifyParameter)
	ON_COMMAND(ID_REGION_SET_DEFPARAMETER, &CMapEditorView::OnRegionSetDefparameter)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_DESTROY()
	ON_WM_MOUSEMOVE()
	ON_UPDATE_COMMAND_UI(ID_WINDOW_SHOW_POINT, &CMapEditorView::OnUpdateWindowShowPoint)
	ON_UPDATE_COMMAND_UI(ID_WINDOW_SHOW_LINE, &CMapEditorView::OnUpdateWindowShowLine)
	ON_UPDATE_COMMAND_UI(ID_WINDOW_SHOW_REGION, &CMapEditorView::OnUpdateWindowShowRegion)
	ON_UPDATE_COMMAND_UI(ID_POINT_SHOW_DELETED, &CMapEditorView::OnUpdatePointShowDeleted)
	ON_UPDATE_COMMAND_UI(ID_LINE_SHOW_DELETED, &CMapEditorView::OnUpdateLineShowDeleted)
	ON_UPDATE_COMMAND_UI(ID_REGION_SHOW_DELETE, &CMapEditorView::OnUpdateRegionShowDelete)
END_MESSAGE_MAP()

// CMapEditorView 构造/析构

CMapEditorView::CMapEditorView() noexcept
{
	// TODO: 在此处添加构造代码

}

CMapEditorView::~CMapEditorView()
{
}

BOOL CMapEditorView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	return CView::PreCreateWindow(cs);
}

// CMapEditorView 绘图

void CMapEditorView::OnDraw(CDC* /*pDC*/)
{
	// TODO: 在此处为本机数据添加绘制代码
	CMapEditorDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: 在此处为本机数据添加绘制代码
	CRect mrect;
	GetClientRect(&mrect); //获取窗口客户区的坐标
	CClientDC dc(this);
	dc.FillSolidRect(0, 0, mrect.Width(), mrect.Height(), dc.GetBkColor()); //用—单色填充—个矩形
	dc.SetROP2(R2_NOTXORPEN); //设置绘图模式
	//ShowAllPnt(&dc, GPntTmpF, GPntNum); //绘制显示所有点
	//ShowAllLin(&dc, GLinTmpNdxF, GLinTmpDatF, GLinNum); //绘制显示所有线
	//绘制显示所有点
	if (GShowPnt)
	{
		ShowAllPnt(&dc, GPntTmpF, GPntNum, GZoomOffset_x, GZoomOffset_y, GZoom, GCurShowState);
	}
	//绘制显示所有线
	if (GShowLin)
	{
		ShowAllLin(&dc, GLinTmpNdxF, GLinTmpDatF, GLinNum, GZoomOffset_x, GZoomOffset_y, GZoom, GCurShowState);
	}
	//绘制显示所有区
	if (GShowReg)
	{
		ShowAllReg(&dc, GRegTmpNdxF, GRegTmpDatF, GRegNum, GZoomOffset_x, GZoomOffset_y, GZoom, GCurShowState);
	}
	ReleaseDC(&dc); //释放dc
}

void CMapEditorView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	CClientDC dc(this);
	dc.SetROP2(R2_NOTXORPEN);
	if (GLinFCreated)
	{
		switch (GCurOperState)
		{
		case OPERSTATE_INPUT_LIN:
			if (GTLin.dotNum > 1)
			{
				WriteLinNdxToFile(GLinTmpNdxF, GLinNum, GTLin);//将线索引写入线临时索引文件中
				GLinNum++;
				GLinLNum++;
				DrawSeg(&dc, GTLin, GLPnt, point);
				GLin.datOff += (GTLin.dotNum * sizeof(D_DOT));
				memset(&GTLin, 0, sizeof(LIN_NDX_STRU));
				GMPnt.SetPoint(-1, -1);
				GLPnt.x = -1;
				GLPnt.y = -1;
			}
			else if (GTLin.dotNum == 1)
			{
				DrawSeg(&dc, GTLin, GLPnt, point);
				memset(&GTLin, 0, sizeof(LIN_NDX_STRU));
				GMPnt.SetPoint(-1, -1);
				GLPnt.x = -1;
				GLPnt.y = -1;
			}
			break;
		}
	}

	if (GRegFCreated)
		switch (GCurOperState)
		{
		case OPERSTATE_INPUT_REG: // 当前为造区操作状态
			if (GTReg.dotNum > 2)
			{
				WriteRegNdxToFile(GRegTmpNdxF, GRegNum, GTReg);
				++GRegNum;
				++GRegLNum;
				POINT* pt = new POINT[3];
				D_DOT dot;
				ReadTempFileToRegDat(GRegTmpDatF, GTReg.datOff, 0, dot);
				PntDPtoVP(dot, GZoom, GZoomOffset_x, GZoomOffset_y);
				DotToPnt(pt[0], dot);
				ReadTempFileToRegDat(GRegTmpDatF, GTReg.datOff, GTReg.dotNum - 1, dot);
				PntDPtoVP(dot, GZoom, GZoomOffset_x, GZoomOffset_y);
				DotToPnt(pt[1], dot);
				pt[2] = point;
				DrawReg(&dc, GTReg, pt, 3);
				delete[]pt;
				GReg.datOff += (GTReg.dotNum * sizeof(D_DOT));
				memset(&GTReg, 0, sizeof(REG_NDX_STRU));
				GRegCreateMMPnt = CPoint(-1, -1);
				GRegCreateStartPnt = CPoint(-1, -1);
			}
			else if (GTReg.dotNum == 2)
			{
				POINT* pt = new POINT[3];
				D_DOT dot;
				ReadTempFileToRegDat(GRegTmpDatF, GTReg.datOff, 0, dot);
				PntDPtoVP(dot, GZoom, GZoomOffset_x, GZoomOffset_y);
				DotToPnt(pt[0], dot);
				ReadTempFileToRegDat(GRegTmpDatF, GTReg.datOff, GTReg.dotNum - 1, dot);
				PntDPtoVP(dot, GZoom, GZoomOffset_x, GZoomOffset_y);
				DotToPnt(pt[1], dot);
				pt[2] = point;
				DrawReg(&dc, GTReg, pt, 3);
				delete[]pt;
				memset(&GTReg, 0, sizeof(REG_NDX_STRU));
				GRegCreateMMPnt = CPoint(-1, -1);
				GRegCreateStartPnt = CPoint(-1, -1);
			}
			else if (GTReg.dotNum == 1)
			{
				LIN_NDX_STRU tln = { tln.pattern = GTReg.pattern, tln.color = GTReg.color };
				DrawSeg(&dc, tln, GRegCreateStartPnt, GRegCreateMMPnt);
				memset(&GTReg, 0,sizeof(REG_NDX_STRU));
				GRegCreateMMPnt = CPoint(-1, -1);
				GRegCreateStartPnt = CPoint(-1, -1);
			}
			break;
		default:
			break;
		}
}


void CMapEditorView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CMapEditorView 诊断

#ifdef _DEBUG
void CMapEditorView::AssertValid() const
{
	CView::AssertValid();
}

void CMapEditorView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CMapEditorDoc* CMapEditorView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMapEditorDoc)));
	return (CMapEditorDoc*)m_pDocument;
}
#endif //_DEBUG


// CMapEditorView 消息处理程序


void CMapEditorView::OnWindowZoomIn()
{
	// TODO: 在此添加命令处理程序代码
	if (GPntFCreated||GLinFCreated||GRegFCreated)
	{
		GCurOperState=OPERSTATE_ZOOM_IN;//当前为放大操作状态
	}
	else
	{
		MessageBox(L"TempFile have not been created.", L"Message", MB_OK);
	}
}


void CMapEditorView::OnFileNew()
{
	if (GPntFCreated && GLinFCreated && GRegFCreated)
	{
		MessageBox(L"File have been created.", L"Message", MB_OK);
		return;
	}

	CCreateFileDlg dlg;  //创建“新建临时文件”对话框对象
	if (dlg.DoModal() != IDOK)  //判断当前操作是否为“确定”按钮，不是则返回
		return;
	CString str;  //创建输出信息对象

	if (!GPntFCreated)  //判断点临时文件是否存在，不存在则重建
	{
		GPntTmpFName = dlg.m_add + CString("\\") + GPntTmpFName;  //临时点数据文件名
		if (GPntTmpF->Open(GPntTmpFName, CFile::modeCreate | CFile::modeReadWrite | CFile::typeBinary))
		{
			GPntFCreated = true;  //设置点临时文件新建成功标志值
			str += "tempPntF.dat\n";
		}
		else
		{
			GPntTmpFName = CString("tempPntF.dat");
			TRACE(_T("File could not be opened \n"));
		}
	}

	if (!GLinFCreated)  //判断线临时文件是否存在，不存在则重建
	{
		GLinTmpNdxFName = dlg.m_add + CString("\\") + GLinTmpNdxFName;  //临时线索引文件名
		GLinTmpDatFName = dlg.m_add + CString("\\") + GLinTmpDatFName;  //临时线数据文件名
		if (GLinTmpNdxF->Open(GLinTmpNdxFName, CFile::modeCreate | CFile::modeReadWrite | CFile::typeBinary)&&
			GLinTmpDatF->Open(GLinTmpDatFName, CFile::modeCreate | CFile::modeReadWrite | CFile::typeBinary))
		{
			GLinFCreated = true;  //设置线临时文件新建成功标志值
			str += "tempLinF.dat tempLinF.ndx\n";
		}
		else
		{
			GLinTmpDatFName = CString("tempLinF.dat");
			GLinTmpNdxFName = CString("tempLinF.ndx");
			TRACE(_T("File could not be opened \n"));
		}
	}
	
	if (!GRegFCreated)  //判断区临时文件是否存在，不存在则重建
	{
		GRegTmpNdxFName = dlg.m_add + CString("\\") + GRegTmpNdxFName;  //临时区索引文件名
		GRegTmpDatFName = dlg.m_add + CString("\\") + GRegTmpDatFName;  //临时区数据文件名
		if (GRegTmpNdxF->Open(GRegTmpNdxFName, CFile::modeCreate | CFile::modeReadWrite | CFile::typeBinary) &&
			GRegTmpDatF->Open(GRegTmpDatFName, CFile::modeCreate | CFile::modeReadWrite | CFile::typeBinary))
		{
			GRegFCreated = true;  //设置区临时文件新建成功标志值
			str += "tempRegF.ndx tempRegF.dat\n";
		}
		else
		{
			GRegTmpDatFName = CString("tempRegF.dat");
			GRegTmpNdxFName = CString("tempRegF.ndx");
			TRACE(_T("File could not be opened \n"));
		}
	}


	if (GPntFCreated && GLinFCreated && GRegFCreated)
	{
		str += "creat successful!";
		MessageBox(str, L"message", MB_OK);  //临时文件创建成功弹出提示框
	}


}


void CMapEditorView::OnFileOpenPoint()
{
	// TODO: 在此添加命令处理程序代码
	GCurOperState = Noaction;
	CFileDialog dlg(true);
	dlg.m_ofn.lpstrFilter = L"pnt\0*.pnt";
	//如果临时文件中的数据已改变，则提示是否保存，保存则调用OnFileSavePoint  
	if (GPntChanged == true)
	{
		if (IDYES == AfxMessageBox(L"File has not been saved. Does save File?", MB_YESNO, MB_ICONQUESTION))
			OnFileSavePoint();
	}
	if (dlg.DoModal()==IDOK) // 弹出打开文件对话框让用户指定要打开的文件
	{
		GPntFName = dlg.m_ofn.lpstrFile; // M永久文件(含路径)
		CFile *pntF =new CFile();
		if (!pntF->Open(GPntFName,CFile::modeRead | CFile::typeBinary))
		{
			TRACE(_T("File could not be opened\n"));
			return;
		}
		int nPos = GPntFName.ReverseFind(_T('\\'));
		CString floderAdd = GPntFName.Left(nPos);
		if (GPntTmpFName != "tempPntF. dat") //如果临时点文件已经创建则定位到文件开头
		{
			GPntTmpF->SeekToBegin();
		}
		else
		{// 否则创建临时文件
			GPntTmpFName = floderAdd + CString("\\") + GPntTmpFName;
			if (!GPntTmpF->Open(GPntTmpFName, CFile::modeCreate | CFile::modeReadWrite | CFile::typeBinary))
			{
				GPntTmpFName = CString("tempPntF. dat");
				TRACE(_T("File could not be opened\n"));
			}
			else
			{
				GPntFCreated = true;
			}
		}
		ReadPntPermanentFileToTemp(pntF, GPntTmpF,GPntNum,GPntLNum); // 读点永久文件到临时文件
		pntF->Close();
		delete pntF;
		CString windowText = dlg.GetFileName() + "-MapEditor";
		GetParent()->SetWindowTextW(windowText);
		this->InvalidateRect(NULL); // 让视窗口无效，触发MFC调用OnDraw函数重绘窗口
	}
}

void CMapEditorView::OnFileOpenLine()
{
	// TODO: 在此添加命令处理程序代码
	CFileDialog dlg(true);
	dlg.m_ofn.lpstrFilter = L"lin\0*.lin";
	if (GLinChanged == true)
	{
		if (IDYES == AfxMessageBox(L"File have not been saved. Dose save File?", MB_YESNO, MB_ICONQUESTION))
		{
			OnFileSaveLine();//保存线（从临时文件写入永久文件中）
		}
	}
	if (dlg.DoModal() == IDOK)
	{
		GLinFCreated = false;
		int IsCreate = 0;
		GLinFName = dlg.m_ofn.lpstrFile;//永久文件（含路径）
		CFile* LinF = new CFile();
		if (!LinF->Open(GLinFName, CFile::modeRead | CFile::typeBinary))
		{
			TRACE(_T("File could not be opened\n"));
			return;
		}
		int nPos = GLinFName.ReverseFind(_T('\\'));
		CString floderAdd = GLinFName.Left(nPos);
		if (GLinTmpDatFName != "tempLinF.dat")
		{
			GLinTmpDatF->SeekToBegin();
			++IsCreate;
		}
		else
		{
			GLinTmpDatFName = floderAdd + CString("\\") + GLinTmpDatFName;
			if (!GLinTmpDatF->Open(GLinTmpDatFName, CFile::modeCreate | CFile::modeReadWrite | CFile::typeBinary))
			{
				GLinTmpDatFName = CString("tempLinF.dat");
				TRACE(_T("File could not be opened\n"));
			}
			else
				++IsCreate;
		}
		if (GLinTmpNdxFName != "tempLinF.ndx")
		{
			GLinTmpNdxF->SeekToBegin();
			++IsCreate;
		}
		else
		{
			GLinTmpNdxFName = floderAdd + CString("\\") + GLinTmpNdxFName;
			if (!GLinTmpNdxF->Open(GLinTmpNdxFName, CFile::modeCreate | CFile::modeReadWrite | CFile::typeBinary))
			{
				GLinTmpNdxFName = CString("tempLinF.ndx");
				TRACE(_T("File could not be opened\n"));
			}
			else
				++IsCreate;
		}
		if (2 == IsCreate)
			GLinFCreated = true;
		ReadLinPermanentFileToTemp(LinF, GLinTmpDatF, GLinTmpNdxF, GLinVer,
			GLinNum, GLinLNum, GLin.datOff);//从永久文件读取线数据到临时文件
		LinF->Close();//关闭线永久文件
		delete LinF;
		CString windowText = dlg.GetFileName() + "-MapEditor";
		GetParent()->SetWindowTextW(windowText);
		this->InvalidateRect(NULL);//重绘窗口显示
		GCurOperState = Noaction;
	}
}


void CMapEditorView::OnFileOpenRegion()
{
	// TODO: 在此添加命令处理程序代码
	CFileDialog dlg(true);
	dlg.m_ofn.lpstrFilter = L"reg\0*.reg";
	if (GRegChanged == true)
	{
		if (IDYES == AfxMessageBox(L"File have not been saved. Dose save File?", MB_YESNO, MB_ICONQUESTION))
		{
			OnFileSaveRegion();//保存区（从临时文件写入永久文件中）
		}
	}
	if (dlg.DoModal() == IDOK)
	{
		GRegFCreated = false;
		int IsCreate = 0;
		GRegFName = dlg.m_ofn.lpstrFile;//永久文件（含路径）
		CFile* RegF = new CFile();
		if (!RegF->Open(GRegFName, CFile::modeRead | CFile::typeBinary))
		{
			TRACE(_T("File could not be opened\n"));
			return;
		}
		int nPos = GRegFName.ReverseFind(_T('\\'));
		CString floderAdd = GRegFName.Left(nPos);
		if (GRegTmpDatFName != "tempRegF.dat")
		{
			GRegTmpDatF->SeekToBegin();
			++IsCreate;
		}
		else
		{
			GRegTmpDatFName = floderAdd + CString("\\") + GRegTmpDatFName;
			if (!GRegTmpDatF->Open(GRegTmpDatFName, CFile::modeCreate | CFile::modeReadWrite | CFile::typeBinary))
			{
				GRegTmpDatFName = CString("tempRegF.dat");
				TRACE(_T("File could not be opened\n"));
			}
			else
				++IsCreate;
		}
		if (GRegTmpNdxFName != "tempRegF.ndx")
		{
			GRegTmpNdxF->SeekToBegin();
			++IsCreate;
		}
		else
		{
			GRegTmpNdxFName = floderAdd + CString("\\") + GRegTmpNdxFName;
			if (!GRegTmpNdxF->Open(GRegTmpNdxFName, CFile::modeCreate | CFile::modeReadWrite | CFile::typeBinary))
			{
				GRegTmpNdxFName = CString("tempRegF.ndx");
				TRACE(_T("File could not be opened\n"));
			}
			else
				++IsCreate;
		}
		if (2 == IsCreate)
			GRegFCreated = true;
		ReadLinPermanentFileToTemp(RegF, GRegTmpDatF, GRegTmpNdxF, GRegVer,
			GRegNum, GRegLNum, GReg.datOff);//从永久文件读取线数据到临时文件
		RegF->Close();//关闭线永久文件
		delete RegF;
		CString windowText = dlg.GetFileName() + "-MapEditor";
		GetParent()->SetWindowTextW(windowText);
		this->InvalidateRect(NULL);//重绘窗口显示
		GCurOperState = Noaction;
	}
}


void CMapEditorView::OnFileSavePoint()
{
	// TODO: 在此添加命令处理程序代码
	GCurOperState=Noaction;
    //1.如果还没有新建或打开点文件，则提示文件还没有打开，然后返回。
		if (GPntFCreated == false)
		{
			MessageBox(L"File have not been created.", L"Message", MB_OK);
			return;
		}
	CFile* pntF = new CFile();
     //2.如果点文件名不为空，则删除原来的文件，否则调用 CFileDialog 类让用户输入文件名
	if(GPntFName.IsEmpty() == false)
	{
		CFile::Remove(GPntFName);
	}
		else
		{
			LPCTSTR lpszFilters;
			lpszFilters=_T("点(*. pnt)|*. pnt||");
			CFileDialog dlg(false,_T("pnt"),NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,lpszFilters);
			int nPos = GPntTmpFName.ReverseFind(_T('\\'));
				CString folderAdd= GPntTmpFName.Left(nPos);
				dlg.m_ofn.lpstrInitialDir= folderAdd;
			if (dlg.DoModal()==IDOK)
				GPntFName =dlg.GetPathName();
			else
				return;
		}
	PNT_STRU tempPnt;
	//`3.重新创建点永久文件，并写入版本信息，点物理数和逻辑数
		if (pntF->Open(GPntFName, CFile::modeCreate|CFile::modeWrite|CFile::typeBinary))
		{
		pntF->Write(&GPntVer,sizeof(VERSION));
		pntF->Write(&GPntNum,sizeof(int));
		pntF->Write(&GPntLNum,sizeof(int));
		}
		else
		{
			TRACE(_T("File could not be opened\n"));
		}
	//4.将点临时文件中的信息逐条写到永久文件中
	for (int i = 0; i < GPntNum; i++)
	{
		ReadTempFileToPnt(GPntTmpF,i,tempPnt);
		pntF->Write(&tempPnt,sizeof(PNT_STRU));
	}
	pntF->Close();
	delete pntF;
	//5.修改数据变化标志，改变主窗口标题名称
		GPntChanged= false;
	int nPos = GPntFName.ReverseFind(_T('\\'));
	CString windowText= GPntFName.Right(GPntFName.GetLength() - nPos - 1) + "-MapEditor";
	GetParent()->SetWindowTextW(windowText);
}


void CMapEditorView::OnFileSaveLine()
{
	// TODO: 在此添加命令处理程序代码
	if (GLinFCreated)//已经存在临时文件
	{
		CFile* LinF = new CFile();
		if (GLinFName.IsEmpty())
		{
			LPCTSTR lpszFilters;
			lpszFilters = _T("线(*.lin)|*.lin||");
			CFileDialog dlg(false, _T("lin"), NULL, OFN_HIDEREADONLY
				| OFN_OVERWRITEPROMPT, lpszFilters); //保存线的对话框
			int nPos = GLinTmpDatFName.ReverseFind(_T('\\'));
			CString folderAdd = GLinTmpDatFName.Left(nPos);
			dlg.m_ofn.lpstrInitialDir = folderAdd;
			if (dlg.DoModal() == IDOK)
			{
				GLinFName = dlg.GetPathName();//线文件的名称
			}
			else
			{
				return;
			}
		}
		else
		{
			LinF->Remove(GLinFName);
		}
		if (!LinF->Open(GLinFName, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary))
		{
			TRACE(_T("File could not be opened \n"));
			return;
		}
		WriteTempToLinePermanentFile(LinF, GLinTmpDatF, GLinTmpNdxF, GLinVer, GLinNum, GLinLNum);//将线的索引和点数据写入永久文件
		LinF->Close();
		delete LinF;
		GLinChanged = false;//线数据无变更
		int nPos = GLinFName.ReverseFind(_T('\\'));
		CString windowText = GLinFName.Right(GLinFName.GetLength() - nPos - 1) + "-MapEditor";
		GetParent()->SetWindowTextW(windowText);
	}
	else
	{
		MessageBox(L"File have not been created.", L"Message", MB_OK);
	}
	GCurOperState = Noaction;
}


void CMapEditorView::OnFileSaveRegion()
{
	// TODO: 在此添加命令处理程序代码
	if (GRegFCreated)//已经存在临时文件
	{
		CFile* RegF = new CFile();
		if (GRegFName.IsEmpty())
		{
			LPCTSTR lpszFilters;
			lpszFilters = _T("区(*.reg)|*.reg||");
			CFileDialog dlg(false, _T("reg"), NULL, OFN_HIDEREADONLY
				| OFN_OVERWRITEPROMPT, lpszFilters); //保存区的对话框
			int nPos = GRegTmpDatFName.ReverseFind(_T('\\'));
			CString folderAdd = GRegTmpDatFName.Left(nPos);
			dlg.m_ofn.lpstrInitialDir = folderAdd;
			if (dlg.DoModal() == IDOK)
			{
				GRegFName = dlg.GetPathName();//线文件的名称
			}
			else
			{
				return;
			}
		}
		else
		{
			RegF->Remove(GRegFName);
		}
		if (!RegF->Open(GRegFName, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary))
		{
			TRACE(_T("File could not be opened \n"));
			return;
		}
		WriteTempToRegPermanentFile(RegF, GRegTmpDatF, GRegTmpNdxF, GRegVer, GRegNum, GRegLNum);//将区的索引和点数据写入永久文件
		RegF->Close();
		delete RegF;
		GRegChanged = false;//线数据无变更
		int nPos = GRegFName.ReverseFind(_T('\\'));
		CString windowText = GRegFName.Right(GRegFName.GetLength() - nPos - 1) + "-MapEditor";
		GetParent()->SetWindowTextW(windowText);
	}
	else
	{
		MessageBox(L"File have not been created.", L"Message", MB_OK);
	}
	GCurOperState = Noaction;
}


void CMapEditorView::OnFileSaveAsPoint()
{
	// TODO: 在此添加命令处理程序代码
	CString tempFName = GPntFName;  //保留原点永久文件
	GPntFName = CString("");  //将点永久文件名设置为空
	OnFileSavePoint();  //调用OnFileSavePoint函数，永久文件名为空时自动弹出文件名对话框
	if (GPntFName == "")  //若另存失败，则还原原永久文件名
	{
		GPntFName = tempFName;
	}
}


void CMapEditorView::OnFileSaveAsLine()
{
	// TODO: 在此添加命令处理程序代码
	CString tempFName = GLinFName;  //保留原线文件名
	GLinFName = CString("");
	OnFileSaveLine(); //调用OnFileSaveLine函数，永久文件名为空时自动弹出文件名对话框
	if (GLinFName == "") //若另存失败，则还原原永久文件名
	{
		GLinFName = tempFName;
	}
}


void CMapEditorView::OnFileSaveAsRegion()
{
	// TODO: 在此添加命令处理程序代码
	CString tempFName = GRegFName;  //保留原区文件名
	GRegFName = CString("");
	OnFileSaveRegion(); //调用OnFileSaveRegion函数，永久文件名为空时自动弹出文件名对话框
	if (GRegFName == "") //若另存失败，则还原原永久文件名
	{
		GRegFName = tempFName;
	}
}


void CMapEditorView::OnAppExit()
{
	// TODO: 在此添加命令处理程序代码
	//1.如果点数据已改变，则保存
	if (GPntChanged == true)
	{
		if (IDYES == AfxMessageBox(L"File has not been saved File?", MB_YESNO, MB_ICONQUESTION))
		{
			OnFileSavePoint();  //保存点
		}
	}
	//2.调用父窗口CMainFrame的销毁窗口函数DestroyWindow（）
	GetParent()->DestroyWindow();
}


void CMapEditorView::OnWindowZoomOut()
{
	// TODO: 在此添加命令处理程序代码
	if (GPntFCreated||GLinFCreated||GRegFCreated)
	{
		GCurOperState = OPERSTATE_ZOOM_OUT; // 设置为缩小操作状态
	}
	else
	{
		MessageBox(L"TempFile have not been created.", L"Message", MB_OK);
	}

}


void CMapEditorView::OnWindowMove()
{
	// TODO: 在此添加命令处理程序代码
	if (GPntFCreated||GLinFCreated||GRegFCreated)
	{
		GCurOperState = OPERSTATE_WINDOW_MOVE;//设置窗口移动状态
	}
	else
	{
		MessageBox(L"TempFile have not been created.", L"Message", MB_OK);
	}
}


void CMapEditorView::OnWindowReset()
{
	// TODO: 在此添加命令处理程序代码
	GCurShowState = SHOWSTATE_UNDEL; //设置当前为显示未删除状态
	GShowPnt = true;
	GShowLin = true;
	GShowReg = true;
	//重置偏移量和放大倍数
	GZoomOffset_x = 0;//偏移向量x
	GZoomOffset_y = 0; //偏移向量y
	GZoom = 1.0; // 放大系数
	//遍历点、线、区的点数据，计算外包络矩形。
	D_DOT tempPt;
	PNT_STRU tempPnt;
	LIN_NDX_STRU tempLin;
	REG_NDX_STRU tempReg;
	bool isInit = false;
	// 没有图形
	if (GPntLNum == 0 && GLinLNum == 0 && GRegLNum == 0)
		return;
	//初始化外包矩形
	if (isInit == false && GPntLNum > 0)//初始化点的外包矩形
	{
		for (int i = 0; i < GPntNum; ++i)
		{
			ReadTempFileToPnt(GPntTmpF, i, tempPnt); // 从临时文件中读取点
			if (tempPnt.isDel)
				continue;
			else
			{
				GMaxX = tempPnt.x;
				GMinX = tempPnt.x;
				GMaxY = tempPnt.y;
				GMinY = tempPnt.y;
				isInit = true;
				break;
			}
		}
	}
	if (isInit == false && GLinLNum > 0)//初始化线的外包络矩形
	{
		for (int i = 0; i < GLinNum; ++i)
		{
			ReadTempFileToLinNdx(GLinTmpNdxF, i, tempLin);//从临时文件读取线索引
			if (tempLin.isDel)
				continue;
			else
			{
				for (int j = 0; j < tempLin.dotNum; ++j)
				{
					ReadTempFileToLinDat(GLinTmpDatF, tempLin.datOff, j, tempPt);
					GMaxX = tempPt.x;
					GMinX = tempPt.x;
					GMaxY = tempPt.y;
					GMinY = tempPt.y;
					isInit = true;
					break;
				}
			}
		}
	}
	if (isInit == false && GRegLNum > 0)//初始化区的外包络矩形
	{
		for (int i = 0; i < GRegNum; ++i)
		{
			ReadTempFileToRegNdx(GRegTmpNdxF, i, tempReg);//从临时文件读区索引
			if (tempReg.isDel)
				continue;
			else
			{
				for (int j = 0; j < tempReg.dotNum; ++j)
				{
					ReadTempFileToRegDat(GRegTmpDatF, tempReg.datOff, j, tempPt);
					GMaxX = tempPt.x;
					GMinX = tempPt.x;
					GMaxY = tempPt.y;
					GMinY = tempPt.y;
					isInit = true;
					break;
				}
			}
		}
	}
	//未能初始化
	if (isInit == false)
	{
		this->Invalidate();
		return;
	}
	// 遍历所有点
	if (GPntFCreated)
	{
		for (int i = 0; i < GPntNum; ++i)
		{
			ReadTempFileToPnt(GPntTmpF, i, tempPnt);
			if (tempPnt.isDel)
				continue;
			else
			{
				if (tempPnt.x > GMaxX)GMaxX = tempPnt.x;
				if (tempPnt.y > GMaxY)
					GMaxY = tempPnt.y;
				if (tempPnt.x < GMinX)
					GMinX = tempPnt.x;
				if (tempPnt.y < GMinY)
					GMinY = tempPnt.y;
			}
		}
	}
	// 遍历所有线
	if (GLinFCreated)
	{
		for (int i = 0; i < GLinNum; ++i)
		{
			ReadTempFileToLinNdx(GLinTmpNdxF, i, tempLin);
			if (tempLin.isDel)
				continue;
			else
			{
				for (int j = 0; j < tempLin.dotNum; ++j)
				{
					ReadTempFileToLinDat(GLinTmpDatF, tempLin.datOff, j, tempPt);
					if (tempPt.x > GMaxX)
						GMaxX = tempPt.x;
					if (tempPt.y > GMaxY)
						GMaxY = tempPt.y;
					if (tempPt.x < GMinX)
						GMinX = tempPt.x;
					if (tempPt.y < GMinY)
						GMinY = tempPt.y;
				}
			}
		}
	}
	// 遍历所有区
	if (GRegFCreated)
	{
		for (int i = 0; i < GRegLNum; ++i)
		{
			ReadTempFileToRegNdx(GRegTmpNdxF, i, tempReg);
			if (tempReg.isDel)
				continue;
			else
			{
				for (int j = 0; j < tempReg.dotNum; ++j)
				{
					ReadTempFileToRegDat(GRegTmpDatF, tempReg.datOff, j, tempPt);
					if (tempPt.x > GMaxX)
						GMaxX = tempPt.x;
					if (tempPt.y > GMaxY)
						GMaxY = tempPt.y;
					if (tempPt.x < GMinX)
						GMinX = tempPt.x;
					if (tempPt.y < GMinY)
						GMinY = tempPt.y;
				}
			}
		}
	}
	GMaxX += 20;
	GMinX -= 20;
	GMaxY += 20;
	GMinY -= 20;
	RECT rect, client;
	double zoom;
	GetClientRect(&client);
	rect.right = (long)GMaxX;
	rect.left = (long)GMinX;
	rect.bottom = (long)GMaxY;
	rect.top = (long)GMinY;
	modulusZoom(client, rect, zoom);
	GMaxX += 20 / zoom;
	GMinX -= 20 / zoom;
	GMaxY += 20 / zoom;
	GMinY -= 20 / zoom;
	rect.right = (long)GMaxX;
	rect.left = (long)GMinX;
	rect.bottom = (long)GMaxY;
	rect.top = (long)GMinY;
	// 根据外包络矩形计算偏移量和放大倍数，并重绘客户区
	modulusZoom(client, rect, zoom);
	double x0 = GetCenter(rect).x - (client.right / 2.0) + (client.right * (zoom - 1) / (2.0 * zoom));
	double y0 = GetCenter(rect).y - (client.bottom / 2.0) + (client.bottom * (zoom - 1) / (2.0 * zoom));
	GZoomOffset_x += (x0 / GZoom);
	GZoomOffset_y += (y0 / GZoom);
	GZoom *= zoom;
	GCurOperState = Noaction;
	this->Invalidate();
}


void CMapEditorView::OnWindowShowPoint()
{
	// TODO: 在此添加命令处理程序代码
	// 若当前显示状态是显示删除状态，则先把所有显示开关关闭
	if(GCurShowState== SHOWSTATE_DEL)
	{
		GShowPnt = false;
		GShowLin = false;
		GShowReg = false;
	}
	GCurShowState = SHOWSTATE_UNDEL; // 将显示状态更改为显示未删除状态
	//若当前已“显示点”，则将关闭开关，不再“显示点”
		if (GShowPnt == true)
			GShowPnt = false;
		else
			GShowPnt = true;
	this->InvalidateRect(NULL); // 刷新窗口
}


void CMapEditorView::OnWindowShowLine()
{
	// TODO: 在此添加命令处理程序代码
		// 若当前显示状态是显示删除状态，则先把所有显示开关关闭
	if (GCurShowState == SHOWSTATE_DEL)
	{
		GShowPnt = false;
		GShowLin = false;
		GShowReg = false;
	}
	GCurShowState = SHOWSTATE_UNDEL; // 将显示状态更改为显示未删除状态
	//若当前已“显示点”，则将关闭开关，不再“显示点”
	if (GShowLin == true)
		GShowLin = false;
	else
		GShowLin = true;
	this->InvalidateRect(NULL); // 刷新窗口
}


void CMapEditorView::OnWindowShowRegion()
{
	// TODO: 在此添加命令处理程序代码
		// 若当前显示状态是显示删除状态，则先把所有显示开关关闭
	if (GCurShowState == SHOWSTATE_DEL)
	{
		GShowPnt = false;
		GShowLin = false;
		GShowReg = false;
	}
	GCurShowState = SHOWSTATE_UNDEL; // 将显示状态更改为显示未删除状态
	//若当前已“显示点”，则将关闭开关，不再“显示点”
	if (GShowReg == true)
		GShowReg = false;
	else
		GShowReg = true;
	this->InvalidateRect(NULL); // 刷新窗口
}


void CMapEditorView::OnPointCreate()
{
	if (GPntFCreated)
	{
		GCurOperState = OPERSTATE_INPUT_PNT;  //设置为“造点”状态
		GCurShowState = SHOWSTATE_UNDEL;  // 设置当前为显示未删除状态
		this->Invalidate();
		GShowPnt = true;
		GShowReg = true;
		GShowLin = true;
	}
	else
	{
		MessageBox(L"File have not been created.", L"Message", MB_OK);
	}
}


void CMapEditorView::OnPointMove()
{
	// TODO: 在此添加命令处理程序代码
	if (GPntFCreated)
	{
		GCurOperState = OPERSTATE_MOVE_PNT;  //设置操作状态(移动点）
		GCurShowState = SHOWSTATE_UNDEL;  // 设置当前为显示未删除状态
		this->Invalidate();
		GShowPnt = true;
		GShowReg = true;
		GShowLin = true;
	}
	else
	{
		MessageBox(L"TempFile have not been created.", L"Message", MB_OK);
	}
}


void CMapEditorView::OnPointDelete()
{
	// TODO: 在此添加命令处理程序代码
	if (GPntFCreated)
	{
		GCurOperState = OPERSTATE_DELETE_PNT;  //设置操作状态（删除点）
		GCurShowState = SHOWSTATE_UNDEL;  // 设置当前为显示未删除状态
		this->Invalidate();
		GShowPnt = true;
		GShowReg = true;
		GShowLin = true;
	}
	else
	{
		MessageBox(L"TempFile has not been created.", L"Message", MB_OK);
	}
}


void CMapEditorView::OnPointShowDeleted()
{
	// TODO: 在此添加命令处理程序代码
	// 若当前显示状态不是显示删除状态，则切换为显示删除状态并显示点
		if (GCurShowState!= SHOWSTATE_DEL)
		{
			GCurShowState = SHOWSTATE_DEL; // 设置为显示删除状态
			GShowPnt = true;
			GShowLin - false;
			GShowReg = false;
		}
	//若当前状态是显示删除状态，但当前显示的不是点，则将显示点的开关打开
		else if(GCurShowState == SHOWSTATE_DEL && GShowPnt != true)
	{
		GShowPnt = true;
		GShowLin = false;
		GShowReg = false;
	}
	// 其他情况下则将显示状态设置为显示未删除的状态，并打开所有显示的开关
		else
		{
			GCurShowState = SHOWSTATE_UNDEL; // 设置为显示未删除状态
			GShowPnt = true;
			GShowLin = true;
			GShowReg = true;
		}
	this->InvalidateRect(NULL); // 刷新窗口
}


void CMapEditorView::OnPointUndelete()
{
	// TODO: 在此添加命令处理程序代码
	if (GPntFCreated)
	{
		GCurOperState = OPERSTATE_UNDELETE_PNT; //当前操作状态(恢复点)
		GCurShowState = SHOWSTATE_DEL; //当前显示状态(删除点)
		this->Invalidate();
		GShowPnt = true; // 打开显示点
		GShowLin = false;//关闭显示线
		GShowReg = false;//关闭显示区
	}
	else
	{
		MessageBox(L"File have not been created.", L"Message", MB_OK);
	}
}

void CMapEditorView::OnPointModifyParameter()
{
	// TODO: 在此添加命令处理程序代码
	if (GPntFCreated)
	{
		GCurOperState = OPERSTATE_MODIFY_POINT_PARAMETER; //修改点参数
		GCurShowState = SHOWSTATE_UNDEL; //显示状态（不删除点）
		GShowPnt = true; //打开显示点
		GShowLin = false; //关闭显示线
		GShowReg = false; //关闭显示区
		this->Invalidate();
	}
	else
	{
		MessageBox(L"File have not been created.", L"Message", MB_OK);
	}
}


void CMapEditorView::OnPointSetDefparameter()
{
	// TODO: 在此添加命令处理程序代码
	PointParameterDlg dlg; //点参数设置的对话框
	dlg.m_Pattern = GPnt.pattern; //点型
	dlg.m_ColorButton.SetColor(GPnt.color); //颜色
	if (IDOK == dlg.DoModal())
	{
		GPnt.pattern = dlg.m_Pattern;
		COLORREF tempColor = dlg.m_ColorButton.GetColor();
		memcpy_s(&GPnt.color, sizeof(COLORREF), &tempColor, sizeof(COLORREF));
	}
}


void CMapEditorView::OnLineCreate()
{
	// TODO: 在此添加命令处理程序代码
	if (GLinFCreated)
	{
		GCurOperState = OPERSTATE_INPUT_LIN;  //当前为造线状态
		GCurShowState = SHOWSTATE_UNDEL;  // 设置当前为显示未删除状态
		this->Invalidate();
		GShowPnt = true;
		GShowReg = true;
		GShowLin = true;
	}
	else
	{
		MessageBox(L"TempFile have not been created.", L"Message", MB_OK);
	}
}


void CMapEditorView::OnLineMove()
{
	// TODO: 在此添加命令处理程序代码
	if (GLinFCreated)
	{
		GCurOperState = OPERSTATE_MOVE_LIN;//当前为移动线操作状态
		GCurShowState = SHOWSTATE_UNDEL;  // 设置当前为显示未删除状态
		this->Invalidate();
		GShowPnt = true;
		GShowReg = true;
		GShowLin = true;
	}
	else
	{
		MessageBox(L"TempFile have not been created.", L"Message", MB_OK);
	}

}


void CMapEditorView::OnLineDelete()
{
	// TODO: 在此添加命令处理程序代码
	if (GLinFCreated)
	{
		GCurOperState = OPERSTATE_DELETE_LIN;//当前状态为删除线
		GCurShowState = SHOWSTATE_UNDEL;  // 设置当前为显示未删除状态
		this->Invalidate();
		GShowPnt = true;
		GShowReg = true;
		GShowLin = true;
	}
	else
	{
		MessageBox(L"TempFile have not been created.", L"Message", MB_OK);
	}
}


void CMapEditorView::OnLineShowDeleted()
{
	// TODO: 在此添加命令处理程序代码
	//若当前显示状态不是显示删除状态，则切换为显示删除状态并显示点 
	if (GCurShowState != SHOWSTATE_DEL)
	{
		GCurShowState = SHOWSTATE_DEL; //设置为显示删除状态
		GShowPnt = false;
		GShowLin = true;
		GShowReg = false;
	}
	//若当前状态是显示删除状态，但当前显示的不是点，则将显示点的开关打开
	else if (GCurShowState == SHOWSTATE_DEL && GShowLin != true)
	{
		GShowPnt = false;
		GShowLin = true;
		GShowReg = false;
	}
	//其他情况下则将显示状态设置为显示未删除的状态，并打开所有显示的开关
	else
	{
		GCurShowState = SHOWSTATE_UNDEL; //设置为显示未删除状态
		GShowPnt = true;
		GShowLin = true;
		GShowReg = true;
	}
	this->InvalidateRect(NULL); //刷新窗口
}


void CMapEditorView::OnLineUndelete()
{
	// TODO: 在此添加命令处理程序代码
	if (GLinFCreated)
	{
		GCurOperState = OPERSTATE_UNDELETE_LIN; //当前操作状态（恢复线）
		GCurShowState = SHOWSTATE_DEL; //当前显示状态（删除线）
		this->Invalidate();
		GShowPnt = false; //关闭显示点
		GShowLin = true; //打开显示线
		GShowReg = false; //关闭显示区
	}
	else
	{
		MessageBox(L"File have not been created.", L"Message", MB_OK);
	}
}


void CMapEditorView::OnLineDeleteDot()
{
	// TODO: 在此添加命令处理程序代码
}


void CMapEditorView::OnLineAddDot()
{
	// TODO: 在此添加命令处理程序代码
}


void CMapEditorView::OnLineLink()
{
	// TODO: 在此添加命令处理程序代码
	if (GLinFCreated)
	{
		GCurOperState = OPERSTATE_LINK_LIN;//设置为连接线操作状态
		GCurShowState = SHOWSTATE_UNDEL;  // 设置当前为显示未删除状态
		this->Invalidate();
		GShowPnt = true;
		GShowReg = true;
		GShowLin = true;
	}
	else
	{
		MessageBox(L"TempFile have not been created.",L"Message", MB_OK);
	}
}


void CMapEditorView::OnLineModifyParameter()
{
	// TODO: 在此添加命令处理程序代码
	if (GLinFCreated)
	{
		GCurOperState = OPERSTATE_MODIFY_LINE_PARAMETER; //修改点参数
		GCurShowState = SHOWSTATE_UNDEL; //显示状态（不删除点）
		GShowPnt = false; //关闭显示点
		GShowLin = true; //打开显示线
		GShowReg = false; //关闭显示区
		this->Invalidate();
	}
	else
	{
		MessageBox(L"File have not been created.", L"Message", MB_OK);
	}
}


void CMapEditorView::OnLineSetDefparameter()
{
	// TODO: 在此添加命令处理程序代码
	LineParameterDlg dlg; //线参数设置的对话框
	dlg.m_Pattern = GLin.pattern; //线型
	dlg.m_ColorButton.SetColor(GLin.color); //颜色
	if (IDOK == dlg.DoModal())
	{
		GLin.pattern = dlg.m_Pattern;
		COLORREF tempColor = dlg.m_ColorButton.GetColor();
		memcpy_s(&GLin.color, sizeof(COLORREF), &tempColor, sizeof(COLORREF));
	}
}


void CMapEditorView::OnRegionCreate()
{
	// TODO: 在此添加命令处理程序代码
	if (GRegFCreated)
	{
		GCurOperState = OPERSTATE_INPUT_REG; //当前设置为造区操作状态
		
		GCurShowState = SHOWSTATE_UNDEL;  // 设置当前为显示未删除状态
		this->Invalidate();
		GShowPnt = true;
		GShowReg = true;
		GShowLin = true;
	}
	else
	{
		MessageBox(L"TempFile have not been created.",L"Message",MB_OK);
	}
}


void CMapEditorView::OnRegionMove()
{
	// TODO: 在此添加命令处理程序代码
	if (GRegFCreated)
	{
		GCurOperState = OPERSTATE_MOVE_REG; //设笠当前为移动区操作状态
		GCurShowState = SHOWSTATE_UNDEL; //设置当前为显示未删除状态
		this->Invalidate();
		GShowPnt = true;
		GShowReg = true;
		GShowLin = true;
	}
	else
	{
		MessageBox(L"TernpFile have not beencreated. ", L"Message", MB_OK);
	}
}


void CMapEditorView::OnRegionDelete()
{
	// TODO: 在此添加命令处理程序代码
	if (GRegFCreated)
	{
		GCurOperState = OPERSTATE_DELETE_REG;//设置当前为删除区操作状态
		
		GCurShowState = SHOWSTATE_UNDEL;  // 设置当前为显示未删除状态
		this->Invalidate();
		GShowPnt = true;
		GShowReg = true;
		GShowLin = true;
	}
	else
	{
		MessageBox(L"TempFile have not been created.",L"Message",MB_OK);
	}
}


void CMapEditorView::OnRegionShowDelete()
{
	// TODO: 在此添加命令处理程序代码
		//若当前显示状态不是显示删除状态，则切换为显示删除状态并显示区 
	if (GCurShowState != SHOWSTATE_DEL)
	{
		GCurShowState = SHOWSTATE_DEL; //设置为显示删除状态
		GShowPnt = false;
		GShowLin = false;
		GShowReg = true;
	}
	//若当前状态是显示删除状态，但当前显示的不是区，则将显示区的开关打开
	else if (GCurShowState == SHOWSTATE_DEL && GShowReg != true)
	{
		GShowPnt = false;
		GShowLin = false;
		GShowReg = true;
	}
	//其他情况下则将显示状态设置为显示未删除的状态，并打开所有显示的开关
	else
	{
		GCurShowState = SHOWSTATE_UNDEL; //设置为显示未删除状态
		GShowPnt = true;
		GShowLin = true;
		GShowReg = true;
	}
	this->InvalidateRect(NULL); //刷新窗口
}


void CMapEditorView::OnRegionUndelete()
{
	// TODO: 在此添加命令处理程序代码
	if (GRegFCreated)
	{
		GCurOperState = OPERSTATE_UNDELETE_REG; //当前操作状态（恢复线）
		GCurShowState = SHOWSTATE_DEL; //当前显示状态（删除线）
		this->Invalidate();
		GShowPnt = false; //关闭显示点
		GShowLin = false; //关闭显示线
		GShowReg = true; //打开显示区
	}
	else
	{
		MessageBox(L"File have not been created.", L"Message", MB_OK);
	}
}


void CMapEditorView::OnRegionModifyParameter()
{
	// TODO: 在此添加命令处理程序代码
	if (GRegFCreated)
	{
		GCurOperState = OPERSTATE_MODIFY_REGION_PARAMETER; //修改区参数
		GCurShowState = SHOWSTATE_UNDEL; //显示状态（不删除点）
		GShowPnt = false; //关闭显示点
		GShowLin = false; //关闭显示线
		GShowReg = true; //打开显示区
		this->Invalidate();
	}
	else
	{
		MessageBox(L"File have not been created.", L"Message", MB_OK);
	}
}

void CMapEditorView::OnRegionSetDefparameter()
{
	// TODO: 在此添加命令处理程序代码
	RegionParameterDlg dlg; //区参数设置的对话框
	dlg.m_Pattern = GReg.pattern; //区型
	dlg.m_ColorButton.SetColor(GReg.color); //颜色
	if (IDOK == dlg.DoModal())
	{
		GReg.pattern = dlg.m_Pattern;
		COLORREF tempColor = dlg.m_ColorButton.GetColor();
		memcpy_s(&GReg.color, sizeof(COLORREF), &tempColor, sizeof(COLORREF));
	}
}


void CMapEditorView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CView::OnLButtonDown(nFlags, point);
	if (GPntFCreated)
	{
		switch (GCurOperState)
		{
			case OPERSTATE_MOVE_PNT ://当前为移动点操作状态
				D_DOT dot;
				PntToDot(dot, point);
				PntVPtoDP(dot, GZoom, GZoomOffset_x, GZoomOffset_y);//坐标系转换
				DotToPnt(point, dot);
				GTPnt = FindPnt(point, GPntNum, GPntTmpF, GPntNdx); // 查找最近点
				dot.x = GTPnt.x;
				dot.y = GTPnt.y;
				PntDPtoVP(dot, GZoom, GZoomOffset_x, GZoomOffset_y);//坐标系转换
				GTPnt.x = dot.x;
				GTPnt.y = dot.y;
				break;
		default:
			break;
		}
	}

	if (GLinFCreated)
	{
		switch (GCurOperState)
		{
			case OPERSTATE_MOVE_LIN ://当前为移动线操作状态
				D_DOT dot;
			    PntToDot(dot,point);
				PntVPtoDP(dot, GZoom,GZoomOffset_x, GZoomOffset_y); // 坐标系转换
				DotToPnt(point, dot);
				// 查找最近线
				GLinMMTmpNdx = FindLin(GLinTmpNdxF,GLinTmpDatF,point,GLinNum,GLinNdx);
				GLinMMOffsetX= 0;
				GLinMMOffsetY= 0;
				PntDPtoVP(dot, GZoom,GZoomOffset_x, GZoomOffset_y);//坐标系转换
				DotToPnt(point, dot);
				GLinLBDPnt = point;
				GLinMMPnt = point;
				break;
		}
	}


	if (GPntFCreated||GLinFCreated||GRegFCreated)
	{
		switch (GCurOperState)
		{
			case OPERSTATE_ZOOM_IN ://当前为放大操作状态
				GZoomLBDPnt= point;
				GZoomMMPnt= point;
				break;
				case OPERSTATE_WINDOW_MOVE ://当前为移动操作状态
					GWinMoveLBDPnt = point;
					GWinMoveMMPnt = point;
					break;
			default:
					break;
		}
	}


	if (GRegFCreated)
	{
		switch(GCurOperState)
		{
			case OPERSTATE_MOVE_REG: // 当前为移动区操作状态
				GRegLBDPnt = point;
				GRegMMPnt = point;
				D_DOT dot;
				PntToDot(dot,point);
				PntVPtoDP(dot,GZoom,GZoomOffset_x,GZoomOffset_y); //窗口转数据
				DotToPnt(point,dot);
				GRegMMTmpNdx = FindReg(GRegTmpNdxF,GRegTmpDatF,point,GRegNum,GRegNdx);//查找最近区，即点选中的区
				GRegMMOffsetX = 0;
				GRegMMOffsetY = 0;
				break;
		}
	}
}


void CMapEditorView::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CView::OnLButtonUp(nFlags, point);

	CClientDC dc(this);//画笔，定义dc时调用构造函数 
	dc.SetROP2(R2_NOTXORPEN);//绘图的模式设置
	if (GPntFCreated) //己创建临时文件
	{
		D_DOT dot;
		switch (GCurOperState)
		{
		
		case OPERSTATE_INPUT_PNT://当前为绘制点状态
			PNT_STRU pnt;//点对象
			memcpy_s(&pnt, sizeof(PNT_STRU), &GPnt, sizeof(PNT_STRU));
			PntToDot(dot, point);
			PntVPtoDP(dot, GZoom, GZoomOffset_x, GZoomOffset_y); //坐标系转换
			pnt.x = dot.x;//设置点坐标的x
			pnt.y = dot.y;//设置点坐标的y
			WritePntToFile(GPntTmpF, GPntNum, pnt);//将点写入临时文件
			PntDPtoVP(dot, GZoom, GZoomOffset_x, GZoomOffset_y); //坐标系转换
			pnt.x = dot.x;//设置点坐标的x
			pnt.y = dot.y;//设置点坐标的y
			DrawPnt(&dc, pnt); //绘制点
			GPntNum++;//点物理数加1
			GPntLNum++;//点逻辑数加1
			GPntChanged = true;//是否更改标志设置为true
			break;
		
		case OPERSTATE_DELETE_PNT: //当前为删除点操作状态
			PntToDot(dot, point);
			PntVPtoDP(dot, GZoom, GZoomOffset_x, GZoomOffset_y); //坐标系转换
			DotToPnt(point, dot);
			FindPnt(point, GPntNum, GPntTmpF, GPntNdx); //查找最近点
			if (GPntNdx != -1) //如果找到
			{
				PNT_STRU pnt;
				ReadTempFileToPnt(GPntTmpF, GPntNdx, pnt); //从临时点文件读点
				pnt.isDel = 1; //删除标记置为1
				UpdatePnt(GPntTmpF, GPntNdx, pnt); //更新该点数据
				dot.x = pnt.x;
				dot.y = pnt.y;
				PntDPtoVP(dot, GZoom, GZoomOffset_x, GZoomOffset_y); //坐标系转换
				pnt.x = dot.x;
				pnt.y = dot.y;
				DrawPnt(&dc, pnt); //异或模式重绘该点以清除屏幕
				GPntNdx = -1;
				GPntChanged = true;//数据发生变更
				GPntLNum--; //删除一个点，逻辑数减1，但物理存储不变
			}
			break;
		
		case OPERSTATE_MOVE_PNT: //当前为移动点操作状态
			if (GPntNdx != -1)
			{
				PNT_STRU pnt;
				PntToDot(dot, point);
				PntVPtoDP(dot, GZoom, GZoomOffset_x, GZoomOffset_y); //坐标系转换
				ReadTempFileToPnt(GPntTmpF, GPntNdx, pnt); //从点临时文件读取点
				pnt.x = dot.x; //移动后的点坐标x
				pnt.y = dot.y; //移动后的点坐标y
				UpdatePnt(GPntTmpF, GPntNdx, pnt); //更新点数据（写入临时文件）
				GPntNdx = -1;
				GPntChanged = true; //数据发生变更
			}
			break;
		
		case OPERSTATE_UNDELETE_PNT: //当前橾作状态（恢复点）
			PntToDot(dot, point);
			PntVPtoDP(dot, GZoom, GZoomOffset_x, GZoomOffset_y); //窗口转数据坐标 
			DotToPnt(point, dot);
			FindDeletePnt(point, GPntNum, GPntTmpF, GPntNdx); //查找最近的删除点
			if (GPntNdx != -1)
			{
				PNT_STRU pnt;
				ReadTempFileToPnt(GPntTmpF, GPntNdx, pnt); //从临时文件中读取点
				pnt.isDel = 0; //设置删除标记为0，即不删除
				UpdatePnt(GPntTmpF, GPntNdx, pnt); //更新点
				dot.x = pnt.x;
				dot.y = pnt.y;
				PntDPtoVP(dot, GZoom, GZoomOffset_x, GZoomOffset_y); //数据转窗口
				pnt.x = dot.x;
				pnt.y = dot.y;
				DrawPnt(&dc, pnt); //在当前视窗中用异或模式擦除恢复的点
				GPntChanged = true;
				GPntNdx = -1;
			}
			break;
		
		case OPERSTATE_MODIFY_POINT_PARAMETER: //修改点参数操作状态
			PntToDot(dot, point);
			PntVPtoDP(dot, GZoom, GZoomOffset_x, GZoomOffset_y); //窗口转数据坐标
			DotToPnt(point, dot);
			PNT_STRU tempPoint;
			memcpy_s(&tempPoint, sizeof(PNT_STRU), &FindPnt(point, GPntNum, GPntTmpF, GPntNdx), sizeof(PNT_STRU)); //查找最近点
			if (GPntNdx != -1)
			{
				PointParameterDlg dlg; //点参数设置对话框
				dlg.m_ColorButton.SetColor(tempPoint.color);
				dlg.m_Pattern = tempPoint.pattern;
				if (IDOK == dlg.DoModal())
				{
					COLORREF tempColor = dlg.m_ColorButton.GetColor();
					memcpy_s(&tempPoint.color, sizeof(COLORREF), &tempColor, sizeof(COLORREF));
					tempPoint.pattern = dlg.m_Pattern;
					GPntTmpF->Seek(GPntNdx * sizeof(PNT_STRU), CFile::begin);
					GPntTmpF->Write(&tempPoint, sizeof(PNT_STRU)); //写入点数据
				}
				this->Invalidate();
				GPntChanged = true;
				GPntNdx = -1;
			}
			break;
		
		default:
			break;
		}
	}


	if (GLinFCreated)
	{
		D_DOT dot;
		switch (GCurOperState)
		{
		case OPERSTATE_INPUT_LIN://当前为绘制线操作状态
			if (GTLin.dotNum == 0)
			{
				memcpy_s(&GTLin, sizeof(LIN_NDX_STRU), &GLin, sizeof(LIN_NDX_STRU));
			}
			PntToDot(dot, point);
			PntVPtoDP(dot, GZoom, GZoomOffset_x, GZoomOffset_y);//坐标系转换
			WriteLinDatToFile(GLinTmpDatF, GLin.datOff, GTLin.dotNum, dot);//写入线
			GTLin.dotNum++;
			PntDPtoVP(dot, GZoom, GZoomOffset_x, GZoomOffset_y); // 坐标系转换
			GLPnt.x = (long)dot.x;//设置线段起点x
			GLPnt.y = (long)dot.y;//设置线段起点Y
			GLinChanged = true; // 数据变更
			break;

		case OPERSTATE_DELETE_LIN: // 当前为删除线操作状态
			PntToDot(dot, point);
			PntVPtoDP(dot, GZoom, GZoomOffset_x, GZoomOffset_y);//坐标系转换
			DotToPnt(point, dot);
			FindLin(GLinTmpNdxF, GLinTmpDatF, point, GLinNum, GLinNdx); // 查找线
			if (GLinNdx != -1)
			{
				GLinLNum--;
				GLinChanged = true;
				LIN_NDX_STRU TmpLinNdx;
				D_DOT dot1, dot2;
				POINT pnt1, pnt2;
				ReadTempFileToLinNdx(GLinTmpNdxF, GLinNdx, TmpLinNdx); // 读线索引
				TmpLinNdx.isDel = 1; // 设置删除标记
				UpdateLin(GLinTmpNdxF, GLinNdx, TmpLinNdx); // 更新线
				for (int i = 0; i < TmpLinNdx.dotNum - 1; ++i)
				{
					//从临时点数据文件中读取点
					ReadTempFileToLinDat(GLinTmpDatF, TmpLinNdx.datOff, i, dot1);
					ReadTempFileToLinDat(GLinTmpDatF, TmpLinNdx.datOff, i + 1, dot2);
					//坐标系转换（数据转窗口坐标系）
					PntDPtoVP(dot1, GZoom, GZoomOffset_x, GZoomOffset_y);
					PntDPtoVP(dot2, GZoom, GZoomOffset_x, GZoomOffset_y);
					DotToPnt(pnt1, dot1);
					DotToPnt(pnt2, dot2);
					DrawSeg(&dc, TmpLinNdx, pnt1, pnt2);//重绘（异或模式擦除）
				}
				GLinNdx = -1;
			}
			break;

		case OPERSTATE_MOVE_LIN://当前为移动线操作状态
			if (GLinNdx != -1)
			{
				if (GLinLBDPnt.x != -1 && GLinLBDPnt.y != -1)
				{
					D_DOT dot1, dot2;
					PntToDot(dot1, point);
					PntVPtoDP(dot1, GZoom, GZoomOffset_x, GZoomOffset_y);//坐标系转换
					PntToDot(dot2, GLinLBDPnt);
					PntVPtoDP(dot2, GZoom, GZoomOffset_x, GZoomOffset_y); //坐标系转换
					double offset_x = dot1.x - dot2.x;
					double offset_y = dot1.y - dot2.y;
					UpdateLin(GLinTmpNdxF, GLinTmpDatF, GLinNdx, offset_x, offset_y);
					GLinNdx = -1;
					GLinMMOffsetX = 0;
					GLinMMOffsetY = 0;
					GLinChanged = true;
				}
			}
			break;

		case OPERSTATE_LINK_LIN://当前为连接线操作状态
			if (GnLine < 2)
			{
				LIN_NDX_STRU line;
				D_DOT dot;
				PntToDot(dot, point);
				PntVPtoDP(dot, GZoom, GZoomOffset_x, GZoomOffset_y); //坐标系转换
				DotToPnt(point, dot);
				// 查找鼠标点位置最近的线
				line = FindLin(GLinTmpNdxF, GLinTmpDatF, point, GLinNum, GLinNdx);
				if (GLinNdx != -1)
				{
					GnLine++;
					if (GnLine == 1) // 选中第一条线
					{
						GStartLin = line;
						GnStart = GLinNdx;
					}
					else if (GnLine == 2) // 选中第二条线
					{
						if (GnStart != GLinNdx)
						{
							GEndLin = line;
							GnEnd = GLinNdx;
						}
						else
							GnLine--;
					}
				}
				if (GnLine != 0)
				{
					D_DOT pt;
					if (GnLine == 1) // 选中第一条线，其端点画圆标记
					{
						//从临时文件中读取线的起点并将其转为窗口坐标，画圆
						ReadTempFileToLinDat(GLinTmpDatF, GStartLin.datOff, 0, pt);
						PntDPtoVP(pt, GZoom, GZoomOffset_x, GZoomOffset_y);
						dc.Ellipse((long)pt.x - 2, (long)pt.y - 2, (long)pt.x + 2, (long)pt.y + 2);
						//从临时文件中读取线的终点并将其转为窗口坐标，画圆
						ReadTempFileToLinDat(GLinTmpDatF, GStartLin.datOff, GStartLin.dotNum - 1, pt);
						PntDPtoVP(pt, GZoom, GZoomOffset_x, GZoomOffset_y);
						dc.Ellipse((long)pt.x - 2, (long)pt.y - 2, (long)pt.x + 2, (long)pt.y + 2);
					}
					else // 选中第二条线，连接线
					{
						AlterLindot(GLinTmpDatF, GStartLin, GEndLin,
							GnStart, GnEnd, GLin.datOff);//改变线的点数据，即将连接线的点写入文件中
						AlterStartLin(GLinTmpNdxF, GLin.datOff, GnStart,
							GStartLin.dotNum + GEndLin.dotNum);//修改第一条线索引 
						AlterEndLin(GLinTmpNdxF, GnEnd); //修改第二条线索引
						GLin.datOff += (GStartLin.dotNum + GEndLin.dotNum) * sizeof(D_DOT);//连接线索引
						GnLine = 0;
						GLinLNum--;
						GLinChanged = true;
						GnStart = -1;
						GnEnd = -1;
						GLinNdx = -1;
						this->Invalidate();
					}
				}
			}
			break;

		case OPERSTATE_UNDELETE_LIN: //当前橾作状态（恢复线）
			PntToDot(dot, point);
			PntVPtoDP(dot, GZoom, GZoomOffset_x, GZoomOffset_y); //窗口转数据坐标 
			DotToPnt(point, dot);
			FindDeleteLin(GLinTmpNdxF, GLinTmpDatF, point, GLinNum, GLinNdx); //查找最近的删除线
			if (GLinNdx != -1)
			{
				LIN_NDX_STRU lin;
				D_DOT dot1, dot2;
				POINT pnt1, pnt2;
				ReadTempFileToLinNdx(GLinTmpNdxF, GLinNdx, lin); //从临时线索引文件中读取线索引
				lin.isDel = 0; //设置删除标记为0，即不删除
				UpdateLin(GLinTmpNdxF, GLinNdx, lin); // 新线数据
				for (int i = 0; i < lin.dotNum - 1; ++i)
				{
					//从临时线的点数据文件中读取点
					ReadTempFileToLinDat(GLinTmpDatF, lin.datOff, i, dot1);
					ReadTempFileToLinDat(GLinTmpDatF, lin.datOff, i + 1, dot2);
					//坐标系转换（数据转窗口坐标系）
					PntDPtoVP(dot1, GZoom, GZoomOffset_x, GZoomOffset_y);
					PntDPtoVP(dot2, GZoom, GZoomOffset_x, GZoomOffset_y);
					DotToPnt(pnt1, dot1);
					DotToPnt(pnt2, dot2);
					DrawSeg(&dc, lin, pnt1, pnt2); //重绘
				}
				GLinChanged = true;
				GLinNdx = -1;
			}
			break;

		case OPERSTATE_MODIFY_LINE_PARAMETER: //修改线参数操作状态
			PntToDot(dot, point);
			PntVPtoDP(dot, GZoom, GZoomOffset_x, GZoomOffset_y); //窗口转数据坐标
			DotToPnt(point, dot);
			LIN_NDX_STRU tlin;
			memcpy_s(&tlin, sizeof(LIN_NDX_STRU), &FindLin(GLinTmpNdxF, GLinTmpDatF, point, GLinNum, GLinNdx), sizeof(LIN_NDX_STRU)); //查找最近线
			if (GLinNdx != -1)
			{
				LineParameterDlg dlg; //线参数设置对话框
				dlg.m_ColorButton.SetColor(tlin.color);
				dlg.m_Pattern = tlin.pattern;
				if (IDOK == dlg.DoModal())
				{
					COLORREF tempColor = dlg.m_ColorButton.GetColor();
					memcpy_s(&tlin.color, sizeof(COLORREF), &tempColor, sizeof(COLORREF));
					tlin.pattern = dlg.m_Pattern;
					WriteLinNdxToFile(GLinTmpNdxF, GLinNdx, tlin);//将线索引写入线临时索引文件中
				}
				this->Invalidate();
				GLinChanged = true;
				GLinNdx = -1;
			}
			break;
		}
	}

	if (GPntFCreated || GLinFCreated || GRegFCreated)
	{
		RECT client, rect;
		double zoom = 1.0;
		switch (GCurOperState)
		{
		case OPERSTATE_ZOOM_IN://当前为放大操作状态
			GetClientRect(&client);
			if (abs(GZoomLBDPnt.x - GZoomMMPnt.x) <= 10 && abs(GZoomLBDPnt.y - GZoomMMPnt.y) <= 10)
			{
				GZoomStyle = 0;//单击放大
			}
			else
			{
				GZoomStyle = 1;//拉框放大
			}
			if (GZoomStyle == 0)
			{// 单击放大
				double x0 = point.x - (client.right / 2.0) + (client.right / 8.0);
				double y0 = point.y - (client.bottom / 2.0) + (client.bottom / 8.0);
				GZoomOffset_x += (x0 / GZoom);// 偏移向量x
				GZoomOffset_y += (y0 / GZoom);//偏移向量y
				GZoom *= 4 / 3.0;//缩放系数为4 / 3
			}
			else
			{
				// 拉框放大
				rect.right = max(point.x, GZoomLBDPnt.x);
				rect.left = min(point.x, GZoomLBDPnt.x);
				rect.bottom = max(point.y, GZoomLBDPnt.y);
				rect.top = min(point.y, GZoomLBDPnt.y);
				modulusZoom(client, rect, zoom);
				double x0 = GetCenter(rect).x - (client.right / 2.0) + (client.right *
					(zoom - 1) / (2.0 * zoom));
				double y0 = GetCenter(rect).y - (client.bottom / 2.0) + (client.
					bottom * (zoom - 1) / (2.0 * zoom));
				GZoomOffset_x += (x0 / GZoom);//偏移向量x
				GZoomOffset_y += (y0 / GZoom);//偏移向量y
				GZoom *= zoom;//缩放系数
				GZoomStyle = 0;
			}
			GZoomLBDPnt = CPoint(-1, -1);
			GZoomMMPnt = CPoint(-1, -1);
			this->Invalidate();
			break;

		case OPERSTATE_ZOOM_OUT://当前为缩小操作状态
			if (true)
			{
				GetClientRect(&client);
				double x0 = point.x - (client.right / 2.0) - (client.right / 8.0);
				double y0 = point.y - (client.bottom / 2.0) - (client.bottom / 8.0);
				GZoomOffset_x += (x0 / GZoom);//偏移向量x
				GZoomOffset_y += (y0 / GZoom);//偏移向量y
				GZoom *= 3 / 4.0;//缩放系数为3/4
				this->Invalidate();
			}
			break;

			case OPERSTATE_WINDOW_MOVE ://当前为移动操作状态
				GWinMoveLBDPnt = CPoint(-1, -1); //复位移动窗口时左键按下点
				GWinMoveMMPnt = CPoint(-1, -1);//复位移动窗口鼠标移动前状态点位置
						break;


		default:
			break;
		}
	}

	if (GRegFCreated)
	{
		D_DOT dot;
		switch (GCurOperState)
		{
		case OPERSTATE_INPUT_REG://当前为造区操作状态
			if (GTReg.dotNum == 0)
				memcpy_s(&GTReg, sizeof(REG_NDX_STRU), &GReg, sizeof(REG_NDX_STRU));
			if (GRegCreateStartPnt.x == -1 && GRegCreateStartPnt.y == -1)
				GRegCreateStartPnt = point;
			if (GRegCreateMMPnt.x == -1 && GRegCreateMMPnt.y == -1)
				GRegCreateMMPnt = point;
			PntToDot(dot, point);
			PntVPtoDP(dot, GZoom, GZoomOffset_x, GZoomOffset_y); //窗口转数据
			WriteRegDatToFile(GRegTmpDatF, GReg.datOff, GTReg.dotNum, dot);
			//将区的点数据写入文件
			++GTReg.dotNum;//区节点数加1
			if (GTReg.dotNum == 2)
				this->Invalidate();//区节点数少于3个则取消造区操作 
			GRegChanged = true;
			break;

		case OPERSTATE_DELETE_REG://当前为删除区操作状态
			PntToDot(dot, point);
			PntVPtoDP(dot, GZoom, GZoomOffset_x, GZoomOffset_y);//窗口转数据坐标
			DotToPnt(point, dot);
			FindReg(GRegTmpNdxF, GRegTmpDatF, point, GRegNum, GRegNdx);//查找区
			if (GRegNdx != -1)
			{
				GRegLNum--;
				GRegChanged = true;
				REG_NDX_STRU TmpRegNdx;
				ReadTempFileToRegNdx(GRegTmpNdxF, GRegNdx, TmpRegNdx); // 从临时文件中读取删除区的索引
				TmpRegNdx.isDel = 1;//设置删除标记
				UpdateReg(GRegTmpNdxF, GRegNdx, TmpRegNdx);// 更新区数据
				D_DOT* dot = new D_DOT[TmpRegNdx.dotNum];
				GRegTmpDatF->Seek(TmpRegNdx.datOff, CFile::begin);
				GRegTmpDatF->Read(dot, TmpRegNdx.dotNum * sizeof(D_DOT));
				for (int i = 0; i < TmpRegNdx.dotNum; ++i)
				{// 将删除区的点数据坐标转为窗口坐标
					PntDPtoVP(dot[i], GZoom, GZoomOffset_x, GZoomOffset_y);
				}
				POINT* pnt = new POINT[TmpRegNdx.dotNum];
				DotToPnt(pnt, dot, TmpRegNdx.dotNum);
				DrawReg(&dc, TmpRegNdx, pnt, TmpRegNdx.dotNum);//重绘（攥除区）delete[]pnt;
				delete[]dot;
				GRegNdx = -1;
			}
			

		case OPERSTATE_MOVE_REG: //当前为移动区操作状态
			if (GRegNdx != -1)
			{
				if (GRegLBDPnt.x != -1 && GRegLBDPnt.y != -1)
				{
					D_DOT dot1, dot2;
					PntToDot(dot1, point);
					PntVPtoDP(dot1, GZoom, GZoomOffset_x, GZoomOffset_y);
					PntToDot(dot2, GRegLBDPnt);
					PntVPtoDP(dot2, GZoom, GZoomOffset_x, GZoomOffset_y);
					double offset_x = dot1.x - dot2.x;
					double offset_y = dot1.y - dot2.y;
					UpdateReg(GRegTmpNdxF, GRegTmpDatF, GRegNdx, offset_x, offset_y); //更新区数据
					GRegNdx = -1;
					GRegMMOffsetX = 0;
					GRegMMOffsetY = 0;
					GRegChanged = true;
				}
			}
			break;

		case OPERSTATE_UNDELETE_REG: //当前橾作状态（恢复区）
			PntToDot(dot, point);
			PntVPtoDP(dot, GZoom, GZoomOffset_x, GZoomOffset_y); //窗口转数据坐标 
			DotToPnt(point, dot);
			FindDeleteReg(GRegTmpNdxF, GRegTmpDatF, point, GRegNum, GRegNdx); //查找区
			if (GRegNdx != -1)
			{
				REG_NDX_STRU reg;
				ReadTempFileToRegNdx(GRegTmpNdxF, GRegNdx, reg); //从临时文件中读取区的索引
				reg.isDel = 0; //设置删除标记为0，即不删除
				UpdateReg(GRegTmpNdxF, GRegNdx, reg); //更新区数据
				D_DOT* dot = new D_DOT[reg.dotNum];
				GRegTmpDatF->Seek(reg.datOff, CFile::begin);
				GRegTmpDatF->Read(dot, reg.dotNum * sizeof(D_DOT));
				for (int i = 0; i < reg.dotNum; ++i)
				{
					//将删除区的点数据坐标转为窗口坐标
					PntDPtoVP(dot[i], GZoom, GZoomOffset_x, GZoomOffset_y);
				}
				POINT* pnt = new POINT[reg.dotNum];
				DotToPnt(pnt, dot, reg.dotNum);
				DrawReg(&dc, reg, pnt, reg.dotNum); //重绘（撩除区）
				GRegChanged = true;
				GRegNdx = -1;
				delete[] pnt;
				delete[] dot;
			}
			break;

		case OPERSTATE_MODIFY_REGION_PARAMETER: //修改区参数操作状态
			PntToDot(dot, point);
			PntVPtoDP(dot, GZoom, GZoomOffset_x, GZoomOffset_y); //窗口转数据坐标
			DotToPnt(point, dot);
			REG_NDX_STRU treg;
			memcpy_s(&treg, sizeof(REG_NDX_STRU), &FindReg(GRegTmpNdxF, GRegTmpDatF, point, GRegNum, GRegNdx), sizeof(REG_NDX_STRU)); //查找最近区
			if (GRegNdx != -1)
			{
				RegionParameterDlg dlg; //区参数设置对话框
				dlg.m_ColorButton.SetColor(treg.color);
				dlg.m_Pattern = treg.pattern;
				if (IDOK == dlg.DoModal())
				{
					COLORREF tempColor = dlg.m_ColorButton.GetColor();
					memcpy_s(&treg.color, sizeof(COLORREF), &tempColor, sizeof(COLORREF));
					treg.pattern = dlg.m_Pattern;
					WriteRegNdxToFile(GRegTmpNdxF, GRegNdx, treg);//将区索引写入线临时索引文件中
				}
				this->Invalidate();
				GRegChanged = true;
				GRegNdx = -1;
			}
			break;

		default:
			break;

		}
	}
}


void CMapEditorView::OnDestroy()
{
	CView::OnDestroy();

	// TODO: 在此处添加消息处理程序代码
	if (GPntFCreated) // 如果点临时文件已创建，则关闭并删除
	{
	 if (GPntTmpF->m_hFile != CFile::hFileNull)
	 {
	   GPntTmpF->Close();
	   GPntTmpF->Remove(GPntTmpFName);
	 }
	}
	delete GPntTmpF;//删除点临时文件对象
	// 线、区相关功能尚未实现，但先把以下代码加上
		if (GLinFCreated) //如果线临时文件已创建，则关闭并删除
		{
		if (GLinTmpDatF->m_hFile != CFile::hFileNull)
		{
		GLinTmpDatF->Close();
		GLinTmpDatF->Remove(GLinTmpDatFName);
		}
		if (GLinTmpNdxF->m_hFile != CFile::hFileNull)
		{
		GLinTmpNdxF->Close();
		GLinTmpNdxF->Remove(GLinTmpNdxFName);
		}
		}
	delete GLinTmpDatF; // 删除线临时文件对象
	delete GLinTmpNdxF;
	if (GRegFCreated) // 如果区临时文件已创建，则关闭并删除
	{
		if (GRegTmpDatF->m_hFile != CFile::hFileNull)
		{
			GRegTmpDatF->Close();
			GRegTmpDatF->Remove(GRegTmpDatFName);
		}
	}
	if (GRegTmpNdxF->m_hFile != CFile::hFileNull)
	   {
	   GRegTmpNdxF->Close();
	   GRegTmpNdxF->Remove(GRegTmpNdxFName);
	    }
	delete GRegTmpDatF;//删除区临时文件对象
    delete GRegTmpNdxF;
}


void CMapEditorView::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CView::OnMouseMove(nFlags, point);
	if (GPntFCreated)
	{
		switch (GCurOperState)
		{
		case OPERSTATE_MOVE_PNT://移动点操作
			if (GPntNdx != -1)
			{
				CClientDC dc(this);//获得本窗口或当前活动视图
				dc.SetROP2(R2_NOTXORPEN);//设置异或模式画点
				DrawPnt(&dc, GTPnt);//在原位置画，清除原点图形
				GTPnt.x = point.x;//移动点的坐标x
				GTPnt.y = point.y; //移动点的坐标y
				DrawPnt(&dc, GTPnt);//在新位置画
			}
			break;
		}
	}

	if (GLinFCreated)
	{
		switch (GCurOperState)
		{
		case OPERSTATE_INPUT_LIN://当前为绘制线状态
			if (GTLin.dotNum > 0)
			{
				CClientDC dc(this);//获得本窗口或当前活动视图
				dc.SetROP2(R2_NOTXORPEN);//设置异或模式画线
				if (GMPnt.x != -1 && GMPnt.y != -1)
				{
					DrawSeg(&dc, GTLin, GLPnt, GMPnt);//默认样式绘制线段
				}
				GMPnt.x = point.x;//设置鼠标上一状态点(x)
				GMPnt.y = point.y;// 设置鼠标上一状态点(y)
				POINT mpoint = { mpoint.x = point.x, mpoint.y = point.y };
				DrawSeg(&dc, GTLin, GLPnt, mpoint);//默认样式绘制线段
			}
			break;

		case OPERSTATE_MOVE_LIN://当前为移动线操作状态
			if (GLinNdx != -1)
			{
				CClientDC dc(this);
				dc.SetROP2(R2_NOTXORPEN);//设置异或模式
				D_DOT dot1, dot2;
				POINT pnt1, pnt2;
				//擦除原来的线
				for (int i = 0; i < GLinMMTmpNdx.dotNum - 1; i++)
				{
					// 从临时文件中读取线的点
					ReadTempFileToLinDat(GLinTmpDatF, GLinMMTmpNdx.datOff, i, dot1);
					ReadTempFileToLinDat(GLinTmpDatF, GLinMMTmpNdx.datOff, i + 1, dot2);
					//坐标系转换（数据转窗口）
					PntDPtoVP(dot1, GZoom, GZoomOffset_x, GZoomOffset_y);
					PntDPtoVP(dot2, GZoom, GZoomOffset_x, GZoomOffset_y);
					DotToPnt(pnt1, dot1);
					DotToPnt(pnt2, dot2);
					pnt1.x += GLinMMOffsetX;
					pnt1.y += GLinMMOffsetY;
					pnt2.x += GLinMMOffsetX;
					pnt2.y += GLinMMOffsetY;
					DrawSeg(&dc, GLinMMTmpNdx, pnt1, pnt2);//重绘（异或模式擦除）
				}
				// 计算偏移量
				GLinMMOffsetX = GLinMMOffsetX + point.x - GLinMMPnt.x;
				GLinMMOffsetY = GLinMMOffsetY + point.y - GLinMMPnt.y;
				//在新的位置绘制一条新的线段
				for (int i = 0; i < GLinMMTmpNdx.dotNum - 1; i++)
				{
					// 从临时文件中读取线的点
					ReadTempFileToLinDat(GLinTmpDatF, GLinMMTmpNdx.datOff, i, dot1);
					ReadTempFileToLinDat(GLinTmpDatF, GLinMMTmpNdx.datOff, i + 1, dot2);
					//转换坐标系(数据转窗口)
					PntDPtoVP(dot1, GZoom, GZoomOffset_x, GZoomOffset_y);
					PntDPtoVP(dot2, GZoom, GZoomOffset_x, GZoomOffset_y);
					DotToPnt(pnt1, dot1);
					DotToPnt(pnt2, dot2);
					pnt1.x += GLinMMOffsetX;
					pnt1.y += GLinMMOffsetY;
					pnt2.x += GLinMMOffsetX;
					pnt2.y += GLinMMOffsetY;
					DrawSeg(&dc, GLinMMTmpNdx, pnt1, pnt2);//重绘(绘制新线)
				}
			}
			GLinMMPnt = point;
			break;
		}
	}

	if (GPntFCreated || GLinFCreated || GRegFCreated)
	{
		CClientDC dc(this);//获得本窗口或当前活动视图
		CPen pen(PS_DOT, 1, RGB(0, 0, 0));
		CPen* oldPen = dc.SelectObject(&pen);
		switch (GCurOperState)
		{
		case OPERSTATE_ZOOM_IN://当前为放大操作状态
			if (GZoomMMPnt.x != -1 && GZoomMMPnt.y != -1)
			{
				dc.SetROP2(R2_NOTXORPEN);//设置异或模式画线
				dc.Rectangle(GZoomLBDPnt.x, GZoomLBDPnt.y, GZoomMMPnt.x, GZoomMMPnt.y);
				dc.Rectangle(GZoomLBDPnt.x, GZoomLBDPnt.y, point.x, point.y);
				GZoomMMPnt = point;
				dc.SelectObject(oldPen);
			}
			break;
			
		case OPERSTATE_WINDOW_MOVE ://当前为移动操作状态
				if (GWinMoveMMPnt.x != -1 && GWinMoveMMPnt.y != -1)
				{
					CPoint offset(0, 0);// 鼠标移动偏移量
						offset.x = point.x - GWinMoveLBDPnt.x;
					offset.y = point.y - GWinMoveLBDPnt.y;
					GZoomOffset_x -= offset.x / GZoom; //变换放大与缩小时的偏移量
					GZoomOffset_y -= offset.y / GZoom;
					GWinMoveLBDPnt = point;
					this->Invalidate();
				}
				break;
		default:
			break;
		}
	}

	if (GRegFCreated)
	{
		switch (GCurOperState)
		{
		case OPERSTATE_INPUT_REG://当前为造区操作状态
			if (GRegCreateMMPnt.x != -1 && GRegCreateMMPnt.y != -1)
			{
				CClientDC dc(this);
				dc.SetROP2(R2_NOTXORPEN);
				LIN_NDX_STRU tln = { tln.pattern = GTReg.pattern,tln.color = GTReg.color };//设置区参数
				if (GTReg.dotNum == 1)
				{
					DrawSeg(&dc, tln, GRegCreateStartPnt, GRegCreateMMPnt);
					DrawSeg(&dc, tln, GRegCreateStartPnt, point);
				}
				else
				{
					D_DOT* dot = new D_DOT[GTReg.dotNum];
					for (int i = 0; i < GTReg.dotNum; ++i)
					{
						ReadTempFileToRegDat(GRegTmpDatF, GTReg.datOff, i, dot[i]);//从临时文件中读取区的点数据
						PntDPtoVP(dot[i], GZoom, GZoomOffset_x, GZoomOffset_y);//将区的点数据坐标转换为窗口坐标
					}
					POINT* pnt = new POINT[GTReg.dotNum + 1];
					DotToPnt(pnt, dot, GTReg.dotNum);
					pnt[GTReg.dotNum] = GRegCreateMMPnt;
					DrawReg(&dc, GTReg, pnt, GTReg.dotNum + 1);
					pnt[GTReg.dotNum] = point;
					DrawReg(&dc, GTReg, pnt, GTReg.dotNum + 1);
					delete[]dot;
					delete[]pnt;
				}
				GRegCreateMMPnt = point;
				break;
			}

		default:
			break;

		case OPERSTATE_MOVE_REG: //当前为移动区操作状态
			if (GRegNdx != -1)
			{
				CClientDC dc(this);
				dc.SetROP2(R2_NOTXORPEN); //设置异或模式
				D_DOT* dot = new D_DOT[GRegMMTmpNdx.dotNum];
				//擦除原来的区
				for (int i = 0; i < GRegMMTmpNdx.dotNum; i++)
				{
					ReadTempFileToRegDat(GRegTmpDatF, GRegMMTmpNdx.datOff, i, dot[i]);
					PntDPtoVP(dot[i], GZoom, GZoomOffset_x, GZoomOffset_y);
					dot[i].x += GRegMMOffsetX;
					dot[i].y += GRegMMOffsetY;
				}
				POINT* pnt = new POINT[GRegMMTmpNdx.dotNum];
				DotToPnt(pnt, dot, GRegMMTmpNdx.dotNum);
				DrawReg(&dc, GRegMMTmpNdx, pnt, GRegMMTmpNdx.dotNum);
				//计算偏移量
				GRegMMOffsetX = GRegMMOffsetX + point.x - GRegMMPnt.x;
				GRegMMOffsetY = GRegMMOffsetY + point.y - GRegMMPnt.y;
				//在新的位置绘制一个新的区
				for (int i = 0; i < GRegMMTmpNdx.dotNum; i++)
				{
					ReadTempFileToRegDat(GRegTmpDatF, GRegMMTmpNdx.datOff, i, dot[i]);
					PntDPtoVP(dot[i], GZoom, GZoomOffset_x, GZoomOffset_y);
					dot[i].x += GRegMMOffsetX;
					dot[i].y += GRegMMOffsetY;
				}
				DotToPnt(pnt, dot, GRegMMTmpNdx.dotNum);
				DrawReg(&dc, GRegMMTmpNdx, pnt, GRegMMTmpNdx.dotNum); delete[] dot;
				delete[] pnt;
				GRegMMPnt = point;
			}
			break;
		}
	}
}



void CMapEditorView::OnUpdateWindowShowPoint(CCmdUI* pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	// 显示状态是显示未删除状态并且显示点则将菜单标记选中
	if (GCurShowState == SHOWSTATE_UNDEL && GShowPnt == true)
		pCmdUI->SetCheck(1); // 菜单选中标记
		else
		pCmdUI->SetCheck(0);
}


void CMapEditorView::OnUpdateWindowShowLine(CCmdUI* pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
		// 显示状态是显示未删除状态并且显示点则将菜单标记选中
	if (GCurShowState == SHOWSTATE_UNDEL && GShowLin == true)
		pCmdUI->SetCheck(1); // 菜单选中标记
	else
		pCmdUI->SetCheck(0);
}


void CMapEditorView::OnUpdateWindowShowRegion(CCmdUI* pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
		// 显示状态是显示未删除状态并且显示点则将菜单标记选中
	if (GCurShowState == SHOWSTATE_UNDEL && GShowReg == true)
		pCmdUI->SetCheck(1); // 菜单选中标记
	else
		pCmdUI->SetCheck(0);
}


void CMapEditorView::OnUpdatePointShowDeleted(CCmdUI* pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	// 若当前显示状态是显示删除状态且显示点，菜单标记选中；否则取消
		if (GCurShowState == SHOWSTATE_DEL && GShowPnt == true)
		{
			pCmdUI->SetCheck(1);
		}
		else
		{
			pCmdUI->SetCheck(0);
		}
}





void CMapEditorView::OnUpdateLineShowDeleted(CCmdUI* pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
		//若当前显示状态是显示删除状态且显示线，菜单标记选中；否则取消
	if (GCurShowState == SHOWSTATE_DEL && GShowLin == true)
	{
		pCmdUI->SetCheck(1);
	}
	else
	{
		pCmdUI->SetCheck(0);
	}
}


void CMapEditorView::OnUpdateRegionShowDelete(CCmdUI* pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
		//若当前显示状态是显示删除状态且显示区，菜单标记选中；否则取消
	if (GCurShowState == SHOWSTATE_DEL && GShowReg == true)
	{
		pCmdUI->SetCheck(1);
	}
	else
	{
		pCmdUI->SetCheck(0);
	}
}
