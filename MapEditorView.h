
// MapEditorView.h: CMapEditorView 类的接口
//

#pragma once

class CMapEditorView : public CView
{
protected: // 仅从序列化创建
	CMapEditorView() noexcept;
	DECLARE_DYNCREATE(CMapEditorView)

// 特性
public:
	CMapEditorDoc* GetDocument() const;

// 操作
public:

// 重写
public:
	virtual void OnDraw(CDC* pDC);  // 重写以绘制该视图
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:

// 实现
public:
	virtual ~CMapEditorView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 生成的消息映射函数
protected:
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnWindowZoomIn();
	afx_msg void OnFileNew();
	afx_msg void OnFileOpenPoint();
	afx_msg void OnFileOpenLine();
	afx_msg void OnFileOpenRegion();
	afx_msg void OnFileSavePoint();
	afx_msg void OnFileSaveLine();
	afx_msg void OnFileSaveRegion();
	afx_msg void OnFileSaveAsPoint();
	afx_msg void OnFileSaveAsLine();
	afx_msg void OnFileSaveAsRegion();
	afx_msg void OnAppExit();
	afx_msg void OnWindowZoomOut();
	afx_msg void OnWindowMove();
	afx_msg void OnWindowReset();
	afx_msg void OnWindowShowPoint();
	afx_msg void OnWindowShowLine();
	afx_msg void OnWindowShowRegion();
	afx_msg void OnPointCreate();
	afx_msg void OnPointMove();
	afx_msg void OnPointDelete();
	afx_msg void OnPointShowDeleted();
	afx_msg void OnPointModifyParameter();
	afx_msg void OnPointSetDefparameter();
	afx_msg void OnLineCreate();
	afx_msg void OnLineMove();
	afx_msg void OnLineDelete();
	afx_msg void OnLineShowDeleted();
	afx_msg void OnLineUndelete();
	afx_msg void OnLineDeleteDot();
	afx_msg void OnLineAddDot();
	afx_msg void OnLineLink();
	afx_msg void OnLineModifyParameter();
	afx_msg void OnLineSetDefparameter();
	afx_msg void OnRegionCreate();
	afx_msg void OnRegionMove();
	afx_msg void OnRegionDelete();
	afx_msg void OnRegionShowDelete();
	afx_msg void OnRegionUndelete();
	afx_msg void OnRegionModifyParameter();
	afx_msg void OnRegionSetDefparameter();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnDestroy();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnUpdateWindowShowPoint(CCmdUI* pCmdUI);
	afx_msg void OnUpdateWindowShowLine(CCmdUI* pCmdUI);
	afx_msg void OnUpdateWindowShowRegion(CCmdUI* pCmdUI);
	afx_msg void OnUpdatePointShowDeleted(CCmdUI* pCmdUI);
	afx_msg void OnPointUndelete();
	afx_msg void OnUpdateLineShowDeleted(CCmdUI* pCmdUI);
	afx_msg void OnUpdateRegionShowDelete(CCmdUI* pCmdUI);
};

#ifndef _DEBUG  // MapEditorView.cpp 中的调试版本
inline CMapEditorDoc* CMapEditorView::GetDocument() const
   { return reinterpret_cast<CMapEditorDoc*>(m_pDocument); }
#endif