#pragma once


// PointParameterDlg 对话框

class PointParameterDlg : public CDialogEx
{
	DECLARE_DYNAMIC(PointParameterDlg)

public:
	PointParameterDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~PointParameterDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_POINT_PARAMETER };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CComboBox m_ComboBox;
	CMFCColorButton m_ColorButton;
	int m_Pattern;//点型参数
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangePointPattern();
};
