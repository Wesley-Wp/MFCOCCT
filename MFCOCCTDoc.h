
// MFCOCCTDoc.h: CMFCOCCTDoc 类的接口
//


#pragma once
#include "OpenCascadeInclude.h"

struct Point
{
	double x;
	double y;
	double z;
};

class CMFCOCCTView;
class CMFCOCCTDoc : public CDocument
{
private:
	CMFCOCCTView* GetView();

	gp_Pnt myPntStart;
	gp_Pnt myPntEnd;
	Handle(Geom_TrimmedCurve) mySegment1;
	TopoDS_Edge myEdge1;
	std::vector<gp_Pnt> pts;

	void DrawScaleColor(int aMax = 1000, int aMin = 0);
	void BoundBox();

protected: // 仅从序列化创建
	CMFCOCCTDoc() noexcept;
	DECLARE_DYNCREATE(CMFCOCCTDoc)

// 特性
public:

public:
	void Put2DOnTop(bool isMax = true);
	void Put3DOnTop(bool isMax = true);
	void Fit2DViews();
	void Set3DViewsZoom(const Standard_Real& Coef);
	void Fit3DViews(Standard_Real Coef);
	void simplify(const TopoDS_Shape& aShape);
	void Minimize3D();
	void Minimize2D();

	void DrawLineByMouse(gp_Pnt thePnt1, gp_Pnt thePnt2);
// 操作
public:
	Handle(AIS_InteractiveContext) myAISContext;
	Handle(V3d_Viewer) myViewer;
	Handle(V3d_Viewer) GetViewer(void) { return myViewer; }

// 重写
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
#ifdef SHARED_HANDLERS
	virtual void InitializeSearchContent();
	virtual void OnDrawThumbnail(CDC& dc, LPRECT lprcBounds);
#endif // SHARED_HANDLERS

// 实现
public:
	virtual ~CMFCOCCTDoc();
	//清空屏幕
	void EraseScreen();
	//3D画圆
	void DrawSphere(double Radius);
	void DrawTrangle(double x, double y, double z);
	void DrawCyplinder(double x, double y);
	void DrawSurface();
	void DrawTrous();
	void DrawBottle(const Standard_Real myWidth, const Standard_Real myHeight, 
		const Standard_Real myThickness);
	void ImportStepFile();
	void ImportIGESFile();
	void ImportSTLFile();
	void ImportOBJFile();
	void ReadCloudPoints();
	void CloudPoint2WireFrame();
	void CloudPoint2Surface();

	//2D画图
	void Draw2DCircle();

	void Show2DShape(Handle(Geom_TrimmedCurve) theCurve,	const Aspect_TypeOfLine aTypeOfline,
		const Aspect_WidthOfLine aWidthOfLine, const Quantity_Color theColor,
		Standard_Boolean theToUpdateViewer);
	void ShowAllShapes(const Handle(AIS_InteractiveObject)& AShapes, int aMax, int aMin);
public:
	//-------------------- 2D -------------------//

	void DragEvent2D(const Standard_Integer  x, 
		const Standard_Integer  y,
		const Standard_Integer  TheState,
		const Handle(V3d_View)& aView);
	void InputEvent2D(const Standard_Integer  x,
		const Standard_Integer  y,
		const Handle(V3d_View)& aView);
	void MoveEvent2D(const Standard_Integer  x,
		const Standard_Integer  y,
		const Handle(V3d_View)& aView);
	void ShiftMoveEvent2D(const Standard_Integer  x,
		const Standard_Integer  y,
		const Handle(V3d_View)& aView);
	void ShiftDragEvent2D(const Standard_Integer  x,
		const Standard_Integer  y,
		const Standard_Integer  TheState,
		const Handle(V3d_View)& aView);
	void ShiftInputEvent2D(const Standard_Integer  x,
		const Standard_Integer  y,
		const Handle(V3d_View)& aView);
	void Popup2D(const Standard_Integer  x,
		const Standard_Integer  y,
		const Handle(V3d_View)& aView);

public:
	//显示
	void DisplayShape(TopoDS_Shape& aShape, Quantity_Color theColor = Quantity_NOC_YELLOW);

	void DisplayText(gp_Pnt2d& thePoint, const char* theText, Standard_Real theXoffset,
		Standard_Real theYoffset, Standard_Real theTextScale);

	void DisplayText(gp_Pnt& thePoint, const char* theText,	Standard_Real theXoffset, 
		Standard_Real theYoffset, Standard_Real theZoffset, Standard_Real theTextScale);

	void DisplayPoint(gp_Pnt2d& thePoint);

	void DisplayPoint(gp_Pnt& thePoint);

	void DisplayCurve(Handle(Geom_Curve) theCurve, Quantity_Color theColor,
		Standard_Boolean theToUpdateViewer);

	void DisplayCurve(Handle(Geom2d_Curve) theCurve, const Aspect_TypeOfLine aTypeOfline,
		const Aspect_WidthOfLine aWidthOfLine, const Quantity_Color theColor,
		Standard_Boolean theToUpdateViewer);

	void DisplaySurface(Handle(Geom_Surface) theSurface, Quantity_Color aNameOfColor,
		Standard_Boolean theToUpdateViewer);

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 生成的消息映射函数
protected:
	DECLARE_MESSAGE_MAP()

#ifdef SHARED_HANDLERS
	// 用于为搜索处理程序设置搜索内容的 Helper 函数
	void SetSearchContent(const CString& value);
#endif // SHARED_HANDLERS
public:
	afx_msg void OnPopuAddtexture();
};
