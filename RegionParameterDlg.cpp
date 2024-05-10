// RegionParameterDlg.cpp: 实现文件
//

#include "pch.h"
#include "MapEditor.h"
#include "RegionParameterDlg.h"
#include "afxdialogex.h"


// RegionParameterDlg 对话框

IMPLEMENT_DYNAMIC(RegionParameterDlg, CDialogEx)

RegionParameterDlg::RegionParameterDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_REGION_PARAMETER, pParent)
{
	m_Pattern = 0; //设置区型参数初始值
}

RegionParameterDlg::~RegionParameterDlg()
{
}

void RegionParameterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_REGION_PATTERN, m_ComboBox);
	DDX_Control(pDX, IDC_REGION_COLOR, m_ColorButton);
}


BEGIN_MESSAGE_MAP(RegionParameterDlg, CDialogEx)
	ON_CBN_SELCHANGE(IDC_REGION_PATTERN, &RegionParameterDlg::OnSelchangeRegionPattern)
END_MESSAGE_MAP()


// RegionParameterDlg 消息处理程序


BOOL RegionParameterDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	m_ComboBox.SetCurSel(m_Pattern); //设置m_ComboBox的初始值

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void RegionParameterDlg::OnSelchangeRegionPattern()
{
	// TODO: 在此添加控件通知处理程序代码
	m_Pattern = m_ComboBox.GetCurSel(); //设置当前选择的区型
}
