// PointParameterDlg.cpp: 实现文件
//

#include "pch.h"
#include "MapEditor.h"
#include "PointParameterDlg.h"
#include "afxdialogex.h"


// PointParameterDlg 对话框

IMPLEMENT_DYNAMIC(PointParameterDlg, CDialogEx)

PointParameterDlg::PointParameterDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_POINT_PARAMETER, pParent)
{
	m_Pattern = 0;//设置点型参数初始值
}

PointParameterDlg::~PointParameterDlg()
{
}

void PointParameterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_POINT_PATTERN, m_ComboBox);
	DDX_Control(pDX, IDC_POINT_COLOR, m_ColorButton);
}


BEGIN_MESSAGE_MAP(PointParameterDlg, CDialogEx)
	ON_CBN_SELCHANGE(IDC_POINT_PATTERN, &PointParameterDlg::OnSelchangePointPattern)
END_MESSAGE_MAP()


// PointParameterDlg 消息处理程序


BOOL PointParameterDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	m_ComboBox.SetCurSel(m_Pattern); //设置m_ComboBox的初始值

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void PointParameterDlg::OnSelchangePointPattern()
{
	// TODO: 在此添加控件通知处理程序代码
	m_Pattern = m_ComboBox.GetCurSel();// 设置当前选择的点型
}
