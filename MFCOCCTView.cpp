
// MFCOCCTView.cpp: CMFCOCCTView 类的实现
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "MFCOCCT.h"
#endif

#include "MFCOCCTDoc.h"
#include "MFCOCCTView.h"
#include "ImportExport.h"

//#ifdef _DEBUG
//#define new DEBUG_NEW
//#endif


// CMFCOCCTView

IMPLEMENT_DYNCREATE(CMFCOCCTView, CView)

BEGIN_MESSAGE_MAP(CMFCOCCTView, CView)
	// 标准打印命令
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CMFCOCCTView::OnFilePrintPreview)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MBUTTONDOWN()
	ON_WM_MBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEWHEEL()
	ON_COMMAND(ID_BTN_IMPORTSTEP, &CMFCOCCTView::OnBtnImportstep)
	ON_COMMAND(ID_BUTTON_DRAWRICLE, &CMFCOCCTView::OnButtonDrawricle)
	ON_COMMAND(ID_BUTTON_TRANGLE, &CMFCOCCTView::OnButtonTrangle)
	ON_COMMAND(ID_BUTTON_DRAWCYLINDER, &CMFCOCCTView::OnButtonDrawcylinder)
	ON_COMMAND(ID_BUTTON_SURFACE, &CMFCOCCTView::OnButtonSurface)
	ON_COMMAND(ID_BUTTON_TROUS, &CMFCOCCTView::OnButtonTrous)
	ON_COMMAND(ID_BUTTON_MAKEBOTLE, &CMFCOCCTView::OnButtonMakebotle)
	ON_COMMAND(ID_POPUP_ERASE, &CMFCOCCTView::OnPopupErase)
	ON_COMMAND(ID_BUTTON_2DCLICE, &CMFCOCCTView::OnButton2dclice)
	ON_COMMAND(ID_32795, &CMFCOCCTView::On32795)
	ON_COMMAND(ID_32803, &CMFCOCCTView::On32803)
	ON_COMMAND(ID_32804, &CMFCOCCTView::On32804)
	ON_COMMAND(ID_32796, &CMFCOCCTView::On32796)
	ON_COMMAND(ID_Menu, &CMFCOCCTView::OnMenu)
	ON_COMMAND(ID_32798, &CMFCOCCTView::On32798)
	ON_COMMAND(ID_32799, &CMFCOCCTView::On32799)
	ON_COMMAND(ID_32800, &CMFCOCCTView::On32800)
	ON_COMMAND(ID_32802, &CMFCOCCTView::On32802)
	ON_COMMAND(ID_32805, &CMFCOCCTView::On32805)
	ON_COMMAND(ID_32806, &CMFCOCCTView::On32806)
	ON_COMMAND(ID_32807, &CMFCOCCTView::On32807)
	ON_COMMAND(ID_IMPORT_OBJ, &CMFCOCCTView::OnImportObj)
	ON_COMMAND(ID_READCLOUDPOINT, &CMFCOCCTView::OnReadcloudpoint)
	ON_COMMAND(ID_CLOUDPOINT_WRIFE, &CMFCOCCTView::OnCloudpointWrife)
	ON_COMMAND(ID_CLOUDPOINT_FACE, &CMFCOCCTView::OnCloudpointFace)
END_MESSAGE_MAP()

// CMFCOCCTView 构造/析构

gp_Pnt CMFCOCCTView::ConvertClickToPoint(Standard_Real theX, Standard_Real theY, Handle(V3d_View) theView)
{
	Standard_Real XEye, YEye, ZEye, XAt, YAt, ZAt;
	theView->Eye(XEye, YEye, ZEye);
	theView->At(XAt, YAt, ZAt);
	gp_Pnt EyePoint(XEye, YEye, ZEye);
	gp_Pnt AtPoint(XAt, YAt, ZAt);

	gp_Vec EyeVector(EyePoint, AtPoint);
	gp_Dir EyeDir(EyeVector);

	gp_Pln PlaneOfTheView = gp_Pln(AtPoint, EyeDir);
	Standard_Real X, Y, Z;
	theView->Convert(int(theX), int(theY), X, Y, Z);
	gp_Pnt ConvertedPoint(X, Y, Z);
	gp_Pnt2d ConvertedPointOnPlane = ProjLib::Project(PlaneOfTheView, ConvertedPoint);

	gp_Pnt ResultPoint = ElSLib::Value(ConvertedPointOnPlane.X(), ConvertedPointOnPlane.Y(),
		PlaneOfTheView);
	return ResultPoint;
}

CMFCOCCTView::CMFCOCCTView() noexcept
{
	// TODO: 在此处添加构造代码
	mouseDownPT.x = 0;
	mouseDownPT.y = 0;

	m_Pt.x = 0;
	m_Pt.y = 0;
	m_scale = 1.0;

}

CMFCOCCTView::~CMFCOCCTView()
{
}

BOOL CMFCOCCTView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	return CView::PreCreateWindow(cs);
}

void CMFCOCCTView::OnInitialUpdate()
{
	CView::OnInitialUpdate();

	myView = GetDocument()->GetViewer()->CreateView();
	myView->SetShadingModel(V3d_GOURAUD);
	Handle(Graphic3d_GraphicDriver) theGraphicDriver = ((CMFCOCCTApp*)AfxGetApp())->GetGraphicDriver();
	Aspect_Handle aWindowHandle = (Aspect_Handle)GetSafeHwnd();
	Handle(WNT_Window) aWntWindow = new WNT_Window(GetSafeHwnd());
	myView->SetWindow(aWntWindow);
	if (!aWntWindow->IsMapped()) 
	{
		aWntWindow->Map();
	}
	Standard_Integer w = 100;
	Standard_Integer h = 100;
	aWntWindow->Size(w, h);
	::PostMessage(GetSafeHwnd(), WM_SIZE, SIZE_RESTORED, w + h * 65536);
	myView->FitAll();

	myView->ZBufferTriedronSetup(Quantity_NOC_RED, Quantity_NOC_GREEN, Quantity_NOC_BLUE1, 0.8, 0.05, 12);
	myView->TriedronDisplay(Aspect_TOTP_LEFT_LOWER, Quantity_NOC_WHITE, 0.2, V3d_ZBUFFER);
}

// CMFCOCCTView 绘图

void CMFCOCCTView::OnDraw(CDC* /*pDC*/)
{
	pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	//设置坐标轴相关
	// Handle(AIS_InteractiveContext) context = new AIS_InteractiveContext();

// create box shape as an OpenCASCADE entity
	//BRepPrimAPI_MakeBox boxMaker(gp_Pnt(-5,-5,-5), gp_Pnt(5,5,5));
	//TopoDS_Shape boxShape = boxMaker.Shape();
	//Handle(AIS_Shape) boxEntity = new AIS_Shape(boxShape);
	//pDoc->myAISContext->Display(boxEntity, true);

	// set the drawing style of the box entity
	//Handle(Prs3d_Drawer) boxStyle = boxEntity->Attributes()->FacePresentation();
	//boxStyle->ChangeTransparency() = 0.6;
	//boxStyle->SetShadingAspect(new Graphic3d_MaterialAspect(
		//BOUNDING_BOX_COLOR,
		//Graphic3d_NOM_SILVER,
		//1.0));

	//boxEntity->Attributes()->SetFaceBoundaryDraw(true);  // Show boundary edges
	//boxEntity->Attributes()->SetFaceBoundaryAspect(new Prs3d_LineAspect(Quantity_NOC_BLACK, Aspect_TOL_SOLID, 1.0));
	//myView->TriedronDisplay(Aspect_TOTP_LEFT_LOWER, Quantity_NOC_GOLD, 0.08, V3d_ZBUFFER);
	myView->MustBeResized();
	myView->Update();
}


// CMFCOCCTView 打印


void CMFCOCCTView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL CMFCOCCTView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 默认准备
	return DoPreparePrinting(pInfo);
}

void CMFCOCCTView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加额外的打印前进行的初始化过程
}

void CMFCOCCTView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加打印后进行的清理过程
}

void CMFCOCCTView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CMFCOCCTView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CMFCOCCTView 诊断

#ifdef _DEBUG
void CMFCOCCTView::AssertValid() const
{
	CView::AssertValid();
}

void CMFCOCCTView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CMFCOCCTDoc* CMFCOCCTView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMFCOCCTDoc)));
	return (CMFCOCCTDoc*)m_pDocument;
}
#endif //_DEBUG


// CMFCOCCTView 消息处理程序

void CMFCOCCTView::OnLButtonDown(UINT nFlags, CPoint point)
{
	m_current_mode = CurAction3d_DynamicRotation;
	mouseDownPT = point;
	myView->StartRotation(point.x, point.y);
	leftMouseBtn = true;
	CView::OnLButtonDown(nFlags, point);
}

void CMFCOCCTView::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	m_current_mode = CurAction3d_Nothing;
	if (leftMouseBtn == true)
	{
		leftMouseBtn = false;
	}
	CView::OnLButtonUp(nFlags, point);
}

void CMFCOCCTView::OnMButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	m_current_mode = CurAction3d_DynamicPanning;
	mouseDownPT = point;
	m_x_max = point.x; //记录平移时起始X位置
	m_y_max = point.y; //记录平移时起始Y位置
	midMouseBtn = true;
	CView::OnMButtonDown(nFlags, point);
}

void CMFCOCCTView::OnMButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	m_current_mode = CurAction3d_Nothing;
	if (midMouseBtn == true)
	{
		midMouseBtn = false;
	}
	CView::OnMButtonUp(nFlags, point);
}

void CMFCOCCTView::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	
	/*//在状态栏显示当前鼠标二维坐标
	CString MousePosition; 
	MousePosition.Format(_T("[%4d,%4d]"), point.x, point.y);
	//获得状态栏的指针
	CMFCStatusBar* pStatusBar = (CMFCStatusBar*)GetParentFrame()->GetDescendantWindow(ID_VIEW_STATUS_BAR);
	//在状态栏的第二个窗格中输出当前鼠标位置
	pStatusBar->SetPaneText(1, MousePosition);*/

	// 转换鼠标位置为OpenCASCADE坐标系中的点
	CClientDC  dc(this);
	Standard_Real x, y, z;
	myView->Convert(point.x, point.y, x, y,z);
	m_mousePt.SetCoord(x, y, z);
	CString strCoord;
	strCoord.Format(_T("X: %.3f, Y: %.3f, Z:%.3f"), x, y, z);
	CSize size = dc.GetTextExtent(strCoord);
	CMFCStatusBar* pStatusBar = (CMFCStatusBar*)GetParentFrame()->GetDescendantWindow(ID_VIEW_STATUS_BAR);
	int nIndex = pStatusBar->CommandToIndex(ID_INDICATOR_MOUSE_POS);
	pStatusBar->SetPaneInfo(nIndex, ID_INDICATOR_MOUSE_POS, SBPS_NORMAL, size.cx);
	pStatusBar->SetPaneText(nIndex,strCoord);

	switch (m_current_mode)
	{

	case CMFCOCCTView::CurAction3d_DynamicPanning:
		//执行平移
		myView->Pan(point.x - m_x_max, m_y_max - point.y);
		m_x_max = point.x;
		m_y_max = point.y;
		break;

	case CMFCOCCTView::CurAction3d_DynamicRotation:
		//执行旋转
		myView->Rotation(point.x, point.y);
		break;
	}
	CView::OnMouseMove(nFlags, point);
}

BOOL CMFCOCCTView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	static float upLimit = 10.0f;
	static float downLimit = 0.04f;
	static float fUp = 1.1f;
	static float fDown = 0.9f;
	m_Pt = pt;
	myView->StartZoomAtPoint(pt.x, pt.y);   //在鼠标光标的点处进行放大
	if (zDelta > 0 && m_scale <= upLimit)
	{
		m_scale *= fUp;
		myView->ZoomAtPoint(0, 0, 10, 0);
	}
	else if (zDelta<0 && m_scale>downLimit)
	{
		m_scale *= fDown;
		myView->ZoomAtPoint(0, 0, -10, 0);

	}
	return CView::OnMouseWheel(nFlags, zDelta, pt);
}

void CMFCOCCTView::OnBtnImportstep()
{
	pDoc->ImportStepFile();
}

void CMFCOCCTView::OnButtonDrawricle()
{
	pDoc->DrawSphere(12);
}

void CMFCOCCTView::OnButtonTrangle()
{
	pDoc->DrawTrangle(200., 150., 100.);
}

void CMFCOCCTView::OnButtonDrawcylinder()
{
	pDoc->DrawCyplinder(50, 100);
}

void CMFCOCCTView::OnButtonSurface()
{
	pDoc->DrawSurface();
}


void CMFCOCCTView::OnButtonTrous()
{
	pDoc->DrawTrous();
}


void CMFCOCCTView::OnButtonMakebotle()
{
	pDoc->DrawBottle(70.0, 50.0, 30.0);
}


void CMFCOCCTView::OnPopupErase()
{
	pDoc->EraseScreen();
}


void CMFCOCCTView::OnButton2dclice()
{
	pDoc->Draw2DCircle();
}


void CMFCOCCTView::On32795()
{
	myView->SetProj(V3d_XposYposZpos);
	myView->FitAll();
}


void CMFCOCCTView::On32803()
{
	pDoc->myAISContext->SetDisplayMode(AIS_Shaded, true);  
}


void CMFCOCCTView::On32804()
{
	pDoc->myAISContext->SetDisplayMode(AIS_WireFrame, true); 
}


void CMFCOCCTView::On32796()
{
	//主视图
	myView->SetProj(V3d_Yneg);
	myView->FitAll();
}


void CMFCOCCTView::OnMenu()
{
	//俯视图
	myView->SetProj(V3d_Zpos);
	myView->FitAll();

}


void CMFCOCCTView::On32798()
{
	//左视图
	myView->SetProj(V3d_Xneg);
	myView->FitAll();
}


void CMFCOCCTView::On32799()
{
	//右视图
	myView->SetProj(V3d_Xpos);
	myView->FitAll();
}


void CMFCOCCTView::On32800()
{
	//仰视图
	myView->SetProj(V3d_Zneg);
	myView->FitAll();
}

void CMFCOCCTView::On32802()
{
	//后视图
	myView->SetProj(V3d_Ypos);
	myView->FitAll();
}


void CMFCOCCTView::On32805()
{
	TopoDS_Vertex V1 = BRepBuilderAPI_MakeVertex(gp_Pnt(200, 400, 0));
	TopoDS_Vertex V2 = BRepBuilderAPI_MakeVertex(gp_Pnt(200, 600, 0));
	TopoDS_Shape S1 = BRepBuilderAPI_MakeEdge(V1, V2);
	pDoc->DisplayShape(S1);
	pDoc->myViewer->SetDefaultViewProj(V3d_Zpos);
}


void CMFCOCCTView::On32806()
{
	pDoc->ImportIGESFile();
}


void CMFCOCCTView::On32807()
{
	pDoc->ImportSTLFile();
}


void CMFCOCCTView::OnImportObj()
{
	pDoc->ImportOBJFile();
}


void CMFCOCCTView::OnReadcloudpoint()
{
	pDoc->ReadCloudPoints();
}


void CMFCOCCTView::OnCloudpointWrife()
{
	pDoc->CloudPoint2WireFrame();
}


void CMFCOCCTView::OnCloudpointFace()
{
	pDoc->CloudPoint2Surface();
}
