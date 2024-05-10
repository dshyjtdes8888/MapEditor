// LineParameterDlg.cpp: 实现文件
//

#include "pch.h"
#include "MapEditor.h"
#include "LineParameterDlg.h"
#include "afxdialogex.h"


// LineParameterDlg 对话框

IMPLEMENT_DYNAMIC(LineParameterDlg, CDialogEx)

LineParameterDlg::LineParameterDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_LINE_PARAMETER, pParent)
{
	m_Pattern = 0; //设置线型参数初始值
}

LineParameterDlg::~LineParameterDlg()
{
}

void LineParameterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LINE_PATTERN, m_ComboBox);
	DDX_Control(pDX, IDC_LINE_COLOR, m_ColorButton);
}


BEGIN_MESSAGE_MAP(LineParameterDlg, CDialogEx)
	ON_CBN_SELCHANGE(IDC_LINE_PATTERN, &LineParameterDlg::OnSelchangeLinePattern)
END_MESSAGE_MAP()


// LineParameterDlg 消息处理程序


BOOL LineParameterDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	m_ComboBox.SetCurSel(m_Pattern); //设置m_ComboBox的初始值

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void LineParameterDlg::OnSelchangeLinePattern()
{
	// TODO: 在此添加控件通知处理程序代码
	m_Pattern = m_ComboBox.GetCurSel(); //设置当前选择的线型
}
