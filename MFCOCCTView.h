
// MFCOCCTView.h: CMFCOCCTView 类的接口
//

#pragma

class CMFCOCCTView : public CView
{
private:
	CMFCOCCTDoc* pDoc;
	gp_Pnt myPointStart, myPointEnd;

	gp_Pnt ConvertClickToPoint(Standard_Real theX, Standard_Real theY, Handle(V3d_View) theView);
	gp_Pnt m_mousePt;
protected: // 仅从序列化创建
	CMFCOCCTView() noexcept;
	DECLARE_DYNCREATE(CMFCOCCTView)

protected:
	enum CurrentAction3d
	{
		CurAction3d_Nothing,
		CurAction3d_DynamicPanning, //平移
		CurAction3d_DynamicZooming, //缩放
		CurAction3d_DynamicRotation //旋转
	};
	Standard_Integer m_x_max;    //记录鼠标平移坐标X
	Standard_Integer m_y_max;    //记录鼠标平移坐标Y
	float m_scale;    //记录滚轮缩放比例 
	CurrentAction3d m_current_mode; //三维场景转换模式(平移\缩放\旋转)
	bool leftMouseBtn = false; //记录鼠标左键状态
	bool midMouseBtn = false;  //记录鼠标中键状态
	CPoint mouseDownPT;    //控制旋转角度
	CPoint m_Pt;

// 特性
public:
	CMFCOCCTDoc* GetDocument() const;
	Handle(V3d_View) myView;
	Handle(AIS_InteractiveContext) myAISContext;
// 操作
public:

// 重写
public:
	virtual void OnDraw(CDC* pDC);  // 重写以绘制该视图
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnInitialUpdate();
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// 实现
public:
	virtual ~CMFCOCCTView();
	void FitAll() { if (!myView.IsNull()) myView->FitAll();  myView->ZFitAll(); };
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 生成的消息映射函数
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnButtonDrawric();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnBtnImportstep();
	afx_msg void OnButtonDrawricle();
	afx_msg void OnButtonTrangle();
	afx_msg void OnButtonDrawcylinder();
	afx_msg void OnButtonSurface();
	afx_msg void OnButtonTrous();
	afx_msg void OnButtonMakebotle();
	afx_msg void OnPopupErase();
	afx_msg void OnButton2dclice();
	afx_msg void On32795();
	afx_msg void On32803();
	afx_msg void On32804();
	afx_msg void On32796();
	afx_msg void OnMenu();
	afx_msg void On32798();
	afx_msg void On32799();
	afx_msg void On32800();
	afx_msg void On32801();
	afx_msg void On32802();
	afx_msg void On32805();
	afx_msg void On32806();
	afx_msg void On32807();
	afx_msg void OnImportObj();
	afx_msg void OnReadcloudpoint();
	afx_msg void OnCloudpointWrife();
	afx_msg void OnCloudpointFace();
};

#ifndef _DEBUG  // MFCOCCTView.cpp 中的调试版本
inline CMFCOCCTDoc* CMFCOCCTView::GetDocument() const
   { return reinterpret_cast<CMFCOCCTDoc*>(m_pDocument); }
#endif

