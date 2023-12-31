
// MFCOCCTDoc.cpp: CMFCOCCTDoc 类的实现
//

#include "pch.h"
#include "framework.h"

#ifndef SHARED_HANDLERS
#include "MFCOCCT.h"
#endif

#include "MFCOCCTDoc.h"
#include "MFCOCCTView.h"

#include <propkey.h>
#include "OCCDataExchange.h"
#include "OccFunctions.h"
#include "AISCurve.h"
#include "AISSurface.h"

// CMFCOCCTDoc

IMPLEMENT_DYNCREATE(CMFCOCCTDoc, CDocument)

BEGIN_MESSAGE_MAP(CMFCOCCTDoc, CDocument)
	ON_COMMAND(ID_POPU_ADDTEXTURE, &CMFCOCCTDoc::OnPopuAddtexture)
END_MESSAGE_MAP()

// CMFCOCCTDoc 构造/析构

void CMFCOCCTDoc::DrawScaleColor(int aMax, int aMin)
{
	// //云图制作集成在这个方法中
	Handle(AIS_ColorScale) aColorScale = new AIS_ColorScale();
	//获取界面的长和宽
	CRect cr;
	int cx = GetSystemMetrics(SM_CXFULLSCREEN);
	int cy = GetSystemMetrics(SM_CYFULLSCREEN);
	//设置云图参数,具体参数的含义可以参看OCC自带的用户手册
	aColorScale->SetBreadth((int)(cx * 0.06));
	aColorScale->SetHeight((int)(cy * 0.45));
	aColorScale->SetRange(aMin, aMax);
	aColorScale->SetNumberOfIntervals(10);
	aColorScale->SetSmoothTransition(Standard_True);
	aColorScale->SetLabelPosition(Aspect_TOCSP_RIGHT);
	//这一步是将云图从X-Y空间 进行转换，很重要
	aColorScale->SetZLayer(Graphic3d_ZLayerId_TopOSD);
	double z = 0.1 * cy;
	gp_Pnt thePoint(0.0, 0.0, z);
	Graphic3d_Vec2i anoffset(0, Standard_Integer(thePoint.Z()));
	myAISContext->SetTransformPersistence(aColorScale, new Graphic3d_TransformPers(Graphic3d_TMF_2d, Aspect_TOTP_LEFT_LOWER, anoffset));
	//云图显示
	myAISContext->SetDisplayMode(aColorScale, 1, Standard_False);
	myAISContext->Display(aColorScale, Standard_False);
}

void CMFCOCCTDoc::BoundBox()
{
	gp_Pnt point1(-1, -1, -1);  // 包围盒最小顶点
	gp_Pnt point2(1, 1, 1);    // 包围盒最大顶点

	BRepPrimAPI_MakeBox makeBox(point1, point2);  // 创建包围盒实体
	TopoDS_Shape boundingBox = makeBox.Shape();      // 获取包围盒形状
	Handle(AIS_Shape) shape = new AIS_Shape(boundingBox);
	myAISContext->Display(shape, Standard_True);
}

CMFCOCCTDoc::CMFCOCCTDoc() noexcept
{
	//获取了一个名为theGraphicDriver的Graphic3d_GraphicDriver类型的图形驱动对象。
	//这个图形驱动对象是用于与底层图形设备进行交互，例如创建窗口、显示图形等。
	Handle(Graphic3d_GraphicDriver) theGraphicDriver = ((CMFCOCCTApp*)AfxGetApp())->GetGraphicDriver();
	//创建了一个V3d_Viewer对象，并将之前获取的图形驱动对象传递给它。
	//V3d_Viewer是OCCT中用于显示和管理视图（View）的类。
	//SetDefaultLights()用于设置默认光源，SetLightOn()用于开启光照。
	myViewer = new V3d_Viewer(theGraphicDriver);
	myViewer->SetDefaultLights();
	myViewer->SetLightOn();

	//创建了一个AIS_InteractiveContext对象，并将之前创建的V3d_Viewer对象传递给它。
	//AIS_InteractiveContext用于处理交互操作，例如选择、高亮、变换等。
	//SetDisplayMode(AIS_Shaded, true)设置显示模式为平滑着色模式，
	//SetAutomaticHilight(Standard_False)关闭自动高亮功能。
	myAISContext = new AIS_InteractiveContext(myViewer);
	//BoundBox();
	myAISContext->SetDisplayMode(AIS_Shaded, true);
	myAISContext->SetAutomaticHilight(Standard_True);
}

CMFCOCCTDoc::~CMFCOCCTDoc()
{
}

BOOL CMFCOCCTDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: 在此添加重新初始化代码
	// (SDI 文档将重用该文档)

	return TRUE;
}

double Angle2radian(double angle)
{
	double ag = (angle * M_PI) / 180;
	return ag;
}

void CMFCOCCTDoc::Show2DShape(Handle(Geom_TrimmedCurve) theCurve, const Aspect_TypeOfLine aTypeOfline,
	const Aspect_WidthOfLine aWidthOfLine, const Quantity_Color theColor,
	Standard_Boolean theToUpdateViewer)
{
	Handle(AISCurve) aGraphicCurve = new AISCurve(theCurve, aTypeOfline, aWidthOfLine, theColor);
	aGraphicCurve->SetDisplayMode(0);
	ShowAllShapes(aGraphicCurve, 1000, 0);
}

void CMFCOCCTDoc::ShowAllShapes(const Handle(AIS_InteractiveObject)& AShapes, int aMax, int aMin)
{
	myAISContext->EraseAll(Standard_True);
	myAISContext->Display(AShapes, Standard_False);
	DrawScaleColor(aMax, aMin);
	GetView()->FitAll();
}

void CMFCOCCTDoc::DisplayShape(TopoDS_Shape& aShape, Quantity_Color theColor)
{
	Handle(AIS_Shape) aisShape = new AIS_Shape(aShape);
	aisShape->SetColor(theColor);
	aisShape->SetDisplayMode((int)AIS_DisplayMode::AIS_Shaded);
	ShowAllShapes(aisShape, 1000, 0);
}

void CMFCOCCTDoc::DisplayText(gp_Pnt2d& thePoint, const char* theText, Standard_Real theXoffset, Standard_Real theYoffset, Standard_Real theTextScale)
{
	Handle(AIS_TextLabel) aLabel = new AIS_TextLabel();
	aLabel->SetText(theText);
	aLabel->SetPosition(gp_Pnt(thePoint.X() + theXoffset, thePoint.Y() + theYoffset, 0.0));
	aLabel->SetHeight(theTextScale);
	aLabel->SetColor(Quantity_NOC_BLACK);
	ShowAllShapes(aLabel, 1000, 0);
}

void CMFCOCCTDoc::DisplayText(gp_Pnt& thePoint, const char* theText,
	Standard_Real theXoffset, Standard_Real theYoffset, Standard_Real theZoffset, Standard_Real theTextScale)
{
	Handle(AIS_TextLabel) aLabel = new AIS_TextLabel();
	aLabel->SetText(theText);
	aLabel->SetPosition(gp_Pnt(thePoint.X() + theXoffset, thePoint.Y() + theYoffset, thePoint.Z() + theZoffset));
	aLabel->SetHeight(theTextScale);
	aLabel->SetColor(Quantity_NOC_BLACK);
	ShowAllShapes(aLabel, 1000, 0);
}

void CMFCOCCTDoc::DisplayPoint(gp_Pnt2d& thePoint)
{
	gp_Pnt P(thePoint.X(), thePoint.Y(), 0.0);
	DisplayPoint(P);
}

void CMFCOCCTDoc::DisplayPoint(gp_Pnt& thePoint)
{
	Handle(AIS_Point) aGraphicPoint = new AIS_Point(new Geom_CartesianPoint(thePoint));
	aGraphicPoint->SetMarker(Aspect_TypeOfMarker::Aspect_TOM_O);
	aGraphicPoint->SetColor(Quantity_NOC_RED);
	ShowAllShapes(aGraphicPoint, 1000, 0);
}

void CMFCOCCTDoc::DisplayCurve(Handle(Geom_Curve) theCurve,
	Quantity_Color theColor,
	Standard_Boolean theToUpdateViewer)
{
	Handle(AISCurve) aGraphicCurve = new AISCurve(theCurve, Aspect_TOL_SOLID, 1, theColor);
	aGraphicCurve->SetDisplayMode(0);
	ShowAllShapes(aGraphicCurve, 1000, 0);
}

void CMFCOCCTDoc::DisplayCurve(Handle(Geom2d_Curve) theCurve,
	const Aspect_TypeOfLine aTypeOfline,
	const Aspect_WidthOfLine aWidthOfLine,
	const Quantity_Color theColor,
	Standard_Boolean theToUpdateViewer)
{
	Handle(AISCurve) aGraphicCurve = new AISCurve(theCurve, aTypeOfline, aWidthOfLine, theColor);
	aGraphicCurve->SetDisplayMode(0);
	ShowAllShapes(aGraphicCurve, 1000, 0);
}

void CMFCOCCTDoc::DisplaySurface(Handle(Geom_Surface) theSurface,
	Quantity_Color aNameOfColor,
	Standard_Boolean theToUpdateViewer)
{
	Handle(AISSurface) aAISSurface = new AISSurface(theSurface);
	aAISSurface->SetColor(aNameOfColor);
	ShowAllShapes(aAISSurface, 1000, 0);
}

void CMFCOCCTDoc::DragEvent2D(const Standard_Integer x, const Standard_Integer y, const Standard_Integer TheState, const Handle(V3d_View)& aView)
{
}

void CMFCOCCTDoc::InputEvent2D(const Standard_Integer x, const Standard_Integer y, const Handle(V3d_View)& aView)
{
	myAISContext->SelectDetected();
	myAISContext->UpdateCurrentViewer();
}

void CMFCOCCTDoc::MoveEvent2D(const Standard_Integer x, const Standard_Integer y, const Handle(V3d_View)& aView)
{
	if (aView->Viewer()->IsGridActive())
	{
		Standard_Real aGridX = 0, aGridY = 0, aGridZ = 0;
		aView->ConvertToGrid(x, y, aGridX, aGridY, aGridZ);
		//View is not updated automatically in ConvertToGrid
		aView->Update();
	}
	this->myAISContext->MoveTo(x, y, aView, Standard_True);
}

void CMFCOCCTDoc::ShiftMoveEvent2D(const Standard_Integer x, const Standard_Integer y, const Handle(V3d_View)& aView)
{
}

void CMFCOCCTDoc::ShiftDragEvent2D(const Standard_Integer x, const Standard_Integer y, const Standard_Integer TheState, const Handle(V3d_View)& aView)
{
}

void CMFCOCCTDoc::ShiftInputEvent2D(const Standard_Integer x, const Standard_Integer y, const Handle(V3d_View)& aView)
{
}

void CMFCOCCTDoc::Popup2D(const Standard_Integer x, const Standard_Integer y, const Handle(V3d_View)& aView)
{
	CMenu menu;
	VERIFY(menu.LoadMenu(IDR_POPUP_EDIT));
	CMenu* pPopup;

	pPopup = menu.GetSubMenu(0);

	ASSERT(pPopup != NULL);

	POINT winCoord = { x , y };
	Handle(WNT_Window) aWNTWindow =
		Handle(WNT_Window)::DownCast(aView->Window());
	ClientToScreen((HWND)(aWNTWindow->HWindow()), &winCoord);
	pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, winCoord.x, winCoord.y,
		AfxGetMainWnd());
}

void CMFCOCCTDoc::EraseScreen()
{
	myAISContext->EraseAll(true);
}

void CMFCOCCTDoc::DrawSphere(double Radius)
{
	BRepPrimAPI_MakeSphere mkSphere(Radius);
	TopoDS_Shape Sphere = mkSphere.Shape();
	DisplayShape(Sphere);
}

void CMFCOCCTDoc::DrawTrangle(double x, double y, double z)
{
	TopoDS_Shape B1 = BRepPrimAPI_MakeBox(x, y, z); 
	TopoDS_Shape B2 = BRepPrimAPI_MakeBox(gp_Ax2(gp_Pnt(-200., -80., -70.), gp_Dir(1., 2., 1.)),
	80., 90., 120.);

	BRepFilletAPI_MakeFillet fillet(B1);
	for (TopExp_Explorer ex(B1, TopAbs_EDGE); ex.More(); ex.Next())
	{
		TopoDS_Edge Edge = TopoDS::Edge(ex.Current());
		fillet.Add(20, Edge);
	}
	TopoDS_Shape blendedBox = fillet.Shape();

	DisplayShape(blendedBox);
	//ShowAllShapes(B2);
}

void CMFCOCCTDoc::DrawCyplinder(double x, double y)
{
	TopoDS_Shape C1 = BRepPrimAPI_MakeCylinder(50., 200.);
	TopoDS_Shape C2 = BRepPrimAPI_MakeCylinder(gp_Ax2(gp_Pnt(200., 200., 0.), gp_Dir(0., 0., 1.)),
		40., 110., Angle2radian(210.));
	DisplayShape(C1);
	DisplayShape(C2);
}

void CMFCOCCTDoc::DrawSurface()
{
	TColgp_Array2OfPnt Poles(1, 2, 1, 4);
	Poles.SetValue(1, 1, gp_Pnt(0, 0, 0));
	Poles.SetValue(1, 2, gp_Pnt(0, 10, 2));
	Poles.SetValue(1, 3, gp_Pnt(0, 20, 10));
	Poles.SetValue(1, 4, gp_Pnt(0, 30, 0));
	Poles.SetValue(2, 1, gp_Pnt(10, 0, 5));
	Poles.SetValue(2, 2, gp_Pnt(10, 10, 3));
	Poles.SetValue(2, 3, gp_Pnt(10, 20, 20));
	Poles.SetValue(2, 4, gp_Pnt(10, 30, 0));

	TColStd_Array1OfReal UKnots(1, 2);
	UKnots.SetValue(1, 0);
	UKnots.SetValue(2, 1);

	TColStd_Array1OfInteger UMults(1, 2);
	UMults.SetValue(1, 2);
	UMults.SetValue(2, 2);

	TColStd_Array1OfReal VKnots(1, 3);
	VKnots.SetValue(1, 0);
	VKnots.SetValue(2, 1);
	VKnots.SetValue(3, 2);

	TColStd_Array1OfInteger VMults(1, 3);
	VMults.SetValue(1, 3);
	VMults.SetValue(2, 1);
	VMults.SetValue(3, 3);

	Standard_Integer UDegree(1);
	Standard_Integer VDegree(2);

	Handle(Geom_BSplineSurface) BSpline = new Geom_BSplineSurface(Poles, UKnots, VKnots, 
		UMults, VMults, UDegree, VDegree);
	TopoDS_Shell aShell = BRepBuilderAPI_MakeShell(BSpline);
	DisplayShape(aShell);
}

void CMFCOCCTDoc::DrawTrous()
{
	TopoDS_Shape S1 = BRepPrimAPI_MakeTorus(60., 20.);

	TopoDS_Shape S2 = BRepPrimAPI_MakeTorus(gp_Ax2(gp_Pnt(100., 100., 0.), gp_Dir(1., 1., 1.)),
		50., 20., Angle2radian(210.));
	TopoDS_Shape S3 = BRepPrimAPI_MakeTorus(gp_Ax2(gp_Pnt(-200., -150., -100), gp_Dir(0., 1., 0.)),
		60., 20., Angle2radian(-45.), Angle2radian(45.), Angle2radian(90));
	DisplayShape(S1);
	DisplayShape(S2);
	DisplayShape(S3);
}


TopoDS_Shape MakeBottle(const Standard_Real myWidth, const Standard_Real myHeight, const Standard_Real myThickness)
{
	// 定义瓶子的一些关键坐标点
	gp_Pnt aPnt1(-myWidth / 2., 0, 0);
	gp_Pnt aPnt2(-myWidth / 2., -myThickness / 4., 0);
	gp_Pnt aPnt3(0, -myThickness / 2., 0);
	gp_Pnt aPnt4(myWidth / 2., -myThickness / 4., 0);
	gp_Pnt aPnt5(myWidth / 2., 0, 0);

	// 根据坐标点，画几何曲线
	Handle(Geom_TrimmedCurve) anArcOfCircle = GC_MakeArcOfCircle(aPnt2, aPnt3, aPnt4);
	Handle(Geom_TrimmedCurve) aSegment1 = GC_MakeSegment(aPnt1, aPnt2);
	Handle(Geom_TrimmedCurve) aSegment2 = GC_MakeSegment(aPnt4, aPnt5);

	// 确定网格
	TopoDS_Edge anEdge1 = BRepBuilderAPI_MakeEdge(aSegment1);
	TopoDS_Edge anEdge2 = BRepBuilderAPI_MakeEdge(anArcOfCircle);
	TopoDS_Edge anEdge3 = BRepBuilderAPI_MakeEdge(aSegment2);
	TopoDS_Wire aWire = BRepBuilderAPI_MakeWire(anEdge1, anEdge2, anEdge3);

	// 根据网格拉伸成实体瓶身
	gp_Ax1 xAxis = gp::OX();
	gp_Trsf aTrsf;

	aTrsf.SetMirror(xAxis);
	BRepBuilderAPI_Transform aBRepTrsf(aWire, aTrsf);
	TopoDS_Shape aMirroredShape = aBRepTrsf.Shape();
	TopoDS_Wire aMirroredWire = TopoDS::Wire(aMirroredShape);

	BRepBuilderAPI_MakeWire mkWire;
	mkWire.Add(aWire);
	mkWire.Add(aMirroredWire);
	TopoDS_Wire myWireProfile = mkWire.Wire();

	// 带棱角的瓶身
	TopoDS_Face myFaceProfile = BRepBuilderAPI_MakeFace(myWireProfile);
	gp_Vec aPrismVec(0, 0, myHeight);
	TopoDS_Shape myBody = BRepPrimAPI_MakePrism(myFaceProfile, aPrismVec);

	// 瓶身倒圆角
	BRepFilletAPI_MakeFillet mkFillet(myBody);
	TopExp_Explorer anEdgeExplorer(myBody, TopAbs_EDGE);
	while (anEdgeExplorer.More()) {
		TopoDS_Edge anEdge = TopoDS::Edge(anEdgeExplorer.Current());
		//Add edge to fillet algorithm
		mkFillet.Add(myThickness / 12., anEdge);
		anEdgeExplorer.Next();
	}
	myBody = mkFillet.Shape();

	// 添加瓶口    
	gp_Pnt neckLocation(0, 0, myHeight);
	gp_Dir neckAxis = gp::DZ();
	gp_Ax2 neckAx2(neckLocation, neckAxis);
	Standard_Real myNeckRadius = myThickness / 4.;
	Standard_Real myNeckHeight = myHeight / 5.;
	BRepPrimAPI_MakeCylinder MKCylinder(neckAx2, myNeckRadius, myNeckHeight);
	TopoDS_Shape myNeck = MKCylinder.Shape();
	myBody = BRepAlgoAPI_Fuse(myBody, myNeck);

	// Body : Create a Hollowed Solid
	TopoDS_Face faceToRemove;
	Standard_Real zMax = -1;

	for (TopExp_Explorer aFaceExplorer(myBody, TopAbs_FACE); aFaceExplorer.More(); aFaceExplorer.Next()) 
	{
		TopoDS_Face aFace = TopoDS::Face(aFaceExplorer.Current());
		
		Handle(Geom_Surface) aSurface = BRep_Tool::Surface(aFace);
		if (aSurface->DynamicType() == STANDARD_TYPE(Geom_Plane)) 
		{
			Handle(Geom_Plane) aPlane = Handle(Geom_Plane)::DownCast(aSurface);
			gp_Pnt aPnt = aPlane->Location();
			Standard_Real aZ = aPnt.Z();
			if (aZ > zMax) 
			{
				zMax = aZ;
				faceToRemove = aFace;
			}
		}
	}
	TopTools_ListOfShape facesToRemove;
	facesToRemove.Append(faceToRemove);
	BRepOffsetAPI_MakeThickSolid  BodyMaker;
	BodyMaker.MakeThickSolidByJoin(myBody, facesToRemove, -myThickness / 50, 1.e-3);
	myBody = BodyMaker.Shape();
	// Threading : Create Surfaces
	Handle(Geom_CylindricalSurface) aCyl1 = new Geom_CylindricalSurface(neckAx2, myNeckRadius * 0.99);
	Handle(Geom_CylindricalSurface) aCyl2 = new Geom_CylindricalSurface(neckAx2, myNeckRadius * 1.05);

	// Threading : Define 2D Curves
	gp_Pnt2d aPnt(2. * M_PI, myNeckHeight / 2.);
	gp_Dir2d aDir(2. * M_PI, myNeckHeight / 4.);
	gp_Ax2d anAx2d(aPnt, aDir);

	Standard_Real aMajor = 2. * M_PI;
	Standard_Real aMinor = myNeckHeight / 10;

	Handle(Geom2d_Ellipse) anEllipse1 = new Geom2d_Ellipse(anAx2d, aMajor, aMinor);
	Handle(Geom2d_Ellipse) anEllipse2 = new Geom2d_Ellipse(anAx2d, aMajor, aMinor / 4);
	Handle(Geom2d_TrimmedCurve) anArc1 = new Geom2d_TrimmedCurve(anEllipse1, 0, M_PI);
	Handle(Geom2d_TrimmedCurve) anArc2 = new Geom2d_TrimmedCurve(anEllipse2, 0, M_PI);
	gp_Pnt2d anEllipsePnt1 = anEllipse1->Value(0);
	gp_Pnt2d anEllipsePnt2 = anEllipse1->Value(M_PI);

	Handle(Geom2d_TrimmedCurve) aSegment = GCE2d_MakeSegment(anEllipsePnt1, anEllipsePnt2);
	// Threading : Build Edges and Wires
	TopoDS_Edge anEdge1OnSurf1 = BRepBuilderAPI_MakeEdge(anArc1, aCyl1);
	TopoDS_Edge anEdge2OnSurf1 = BRepBuilderAPI_MakeEdge(aSegment, aCyl1);
	TopoDS_Edge anEdge1OnSurf2 = BRepBuilderAPI_MakeEdge(anArc2, aCyl2);
	TopoDS_Edge anEdge2OnSurf2 = BRepBuilderAPI_MakeEdge(aSegment, aCyl2);
	TopoDS_Wire threadingWire1 = BRepBuilderAPI_MakeWire(anEdge1OnSurf1, anEdge2OnSurf1);
	TopoDS_Wire threadingWire2 = BRepBuilderAPI_MakeWire(anEdge1OnSurf2, anEdge2OnSurf2);
	BRepLib::BuildCurves3d(threadingWire1);
	BRepLib::BuildCurves3d(threadingWire2);

	// Create Threading 
	BRepOffsetAPI_ThruSections aTool(Standard_True);
	aTool.AddWire(threadingWire1);
	aTool.AddWire(threadingWire2);
	aTool.CheckCompatibility(Standard_False);
	TopoDS_Shape myThreading = aTool.Shape();

	// Building the Resulting Compound 
	TopoDS_Compound aRes;
	BRep_Builder aBuilder;
	aBuilder.MakeCompound(aRes);
	aBuilder.Add(aRes, myBody);
	aBuilder.Add(aRes, myThreading);

	return aRes;
}

void CMFCOCCTDoc::DrawBottle(const Standard_Real myWidth, const Standard_Real myHeight,
	const Standard_Real myThickness)
{
	TopoDS_Shape aRes = MakeBottle(myWidth, myHeight, myThickness);
	//ShowAllShapes(aRes);
	DisplayShape(aRes);
}

void CMFCOCCTDoc::ImportStepFile()
{
	CFileDialog dlg(TRUE,
		NULL,
		NULL,
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		L"Step Files (*.step , *.stp)|*.step; *.stp|All Files (*.*)|*.*||",
		NULL, 0, TRUE);

	if (dlg.DoModal() == IDOK)
	{
		Handle(TopTools_HSequenceOfShape) TopoDsShapes;

		CStringA C(dlg.GetPathName());
		LPCSTR aFileName = C;

		TopoDsShapes = OCCDataExchange::ImportStep(aFileName);

		if (!TopoDsShapes.IsNull())
		{
			for (int i = 1; i <= TopoDsShapes->Length(); i++)
			{
				TopoDS_Shape aShape = TopoDsShapes->Value(i);
				DisplayShape(aShape);
			}
		}

	}
}

void CMFCOCCTDoc::ImportIGESFile()
{
	CFileDialog dlg(TRUE,
		NULL,
		NULL,
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		L"Iges Files (*.iges , *.igs)|*.iges; *.igs|All Files (*.*)|*.*||",
		NULL, 0, TRUE);

	if (dlg.DoModal() == IDOK)
	{
		CStringA C(dlg.GetPathName());
		LPCSTR aFileName = C;

		TopoDS_Shape aShape = OCCDataExchange::ImportIges(aFileName);
		IGESControl_Reader reader;

		IFSelect_ReturnStatus status = reader.ReadFile(aFileName);
		if (status == IFSelect_RetDone)
		{
			reader.TransferRoots();
			TopoDS_Shape shape = reader.OneShape();
			Handle(AIS_Shape) aisShape = new AIS_Shape(shape);
			//DisplayShape(aisShape);
			myAISContext->Display(aisShape, true);
			GetView()->FitAll();
		}
	}
}

void CMFCOCCTDoc::ImportSTLFile()
{
	myAISContext->EraseAll(true);
	CFileDialog dlg(TRUE,
		NULL,
		NULL,
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		L"Stl Files (*.stl)|*.stl|All Files (*.*)|*.*||",
		NULL, 0, TRUE);

	if (dlg.DoModal() == IDOK)
	{
		CStringA C(dlg.GetPathName());
		LPCSTR aFileName = C;

		Handle(Poly_Triangulation) aShape = OCCDataExchange::ImportStl(aFileName);

		Handle(AIS_Triangulation) aStlShape = new AIS_Triangulation(aShape);
		aStlShape->SetDisplayMode(0);
		myAISContext->Display(aStlShape, true);
		GetView()->FitAll();
	}
}

void CMFCOCCTDoc::ImportOBJFile()
{
	CFileDialog dlg(TRUE, 
		_T("obj"), 
		NULL, 
		OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY,
		_T("OBJ Files (*.obj)|*.obj||"), 
		NULL);

	if (dlg.DoModal() == IDOK)
	{
		CString filePath = dlg.GetPathName();
		const char* aFilePath = (char*) filePath.GetBuffer(256);

		// 创建一个TopoDS_Compound对象来存储OBJ文件的几何体
		TopoDS_Compound compound;
		BRep_Builder builder;
		builder.MakeCompound(compound);

		// 读取OBJ文件的内容
		BRepTools::Read(compound, aFilePath, builder);

		// 将OBJ文件的几何体转换为OpenCASCADE显示对象
		Handle(AIS_Shape) aShape = new AIS_Shape(compound);
		myAISContext->Display(aShape, true);
	}
}

void CMFCOCCTDoc::ReadCloudPoints()
{
	CFileDialog dlg(TRUE,
		NULL,
		NULL,
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		L"All Files (*.*)|*.*||",
		NULL, 0, TRUE);
	if (dlg.DoModal() == IDOK)
	{
		CStringA CP(dlg.GetPathName());
		LPCSTR aFileName = CP;

		std::ifstream file(aFileName);
		if (!file)
			return;
		
		// 逐行读取文件内容
		double minVal = INT_MAX;
		double maxVal = INT_MIN;
		std::string line;
		std::vector<gp_Pnt> pointsVec;
		while (std::getline(file, line))
		{
			std::istringstream iss(line);
			double x, y, z;
			if (!(iss >> x >> y >> z))
				break;
			pointsVec.push_back(gp_Pnt(x, y, z));
			if (z < minVal)
			{
				minVal = z;
			}
			if (z > maxVal)
			{
				maxVal = z;
			}
		}
		file.close();
		TColgp_Array1OfPnt points(1, pointsVec.size());
		for (int i = 1; i <= pointsVec.size(); ++i) {
			points.SetValue(i, pointsVec[i - 1]);
		}

		Handle(Graphic3d_ArrayOfPoints) pointsArray = new Graphic3d_ArrayOfPoints(pointsVec.size());
		for (int i = 1; i <= pointsVec.size(); ++i) {
			pointsArray->AddVertex(points.Value(i));
		}
		

		Handle(AIS_PointCloud) point_cloud = new AIS_PointCloud();
		point_cloud->SetPoints(pointsArray);
		point_cloud->SetColor(Quantity_NOC_RED);
		ShowAllShapes(point_cloud, maxVal, minVal);
	}
}

void CMFCOCCTDoc::CloudPoint2WireFrame()
{
	CFileDialog dlg(TRUE,
		NULL,
		NULL,
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		L"All Files (*.*)|*.*||",
		NULL, 0, TRUE);
	if (dlg.DoModal() == IDOK)
	{
		CStringA CP(dlg.GetPathName());
		LPCSTR aFileName = CP;

		std::ifstream file(aFileName);
		if (!file)
		{
			AfxMessageBox(_T("无法打开文件"));
		}

		// 读取文件内容并创建点云
		std::vector<gp_Pnt> pts;
		std::string line;
		while (std::getline(file, line))
		{
			std::istringstream iss(line);
			double x, y, z;
			if (!(iss >> x >> y >> z))
			{
				
			}
			pts.push_back(gp_Pnt(x, y, z));
		}
		file.close();
		BRep_Builder brepBuilder;
		TopoDS_Builder topoDSBuilder;
		TopoDS_Shell shell;
		//TopoDS_Solid shape;
		TopoDS_Shape shape;
		brepBuilder.MakeShell(shell);
		for (int i = 0; i < pts.size(); i++)
		{
			brepBuilder.Add(shell, BRepBuilderAPI_MakeVertex(pts[i]));
		}

		// 创建面片并添加到网格形状
		for (int i = 0; i < pts.size(); i++)
		{
			BRepBuilderAPI_MakePolygon face;
			face = BRepBuilderAPI_MakePolygon(pts[i], pts[i + 1], pts[i + 2], Standard_True);
			brepBuilder.Add(shell, face);
		}

		//topoDSBuilder.MakeSolid(shape);
		topoDSBuilder.Add(shape, shell);
		Handle(AIS_Shape) aisShape = new AIS_Shape(shape);
		myAISContext->Display(aisShape, true);

		//// 创建顶点和线
		//TopoDS_Compound shape;
		//BRep_Builder builder;
		//builder.MakeCompound(shape);

		//for (const auto& point : pts)
		//{
		//	TopoDS_Vertex vertex = BRepBuilderAPI_MakeVertex(point);
		//	builder.Add(shape, vertex);
		//}
		//// 创建面片
		//Handle(Poly_Triangulation) triangulation = new Poly_Triangulation;
		//triangulation->SetNode(1, gp_Pnt());

		//int index = 1;
		//for (const auto& point : pts)
		//{
		//	triangulation->ChangeNode(index).SetCoord(point.X(), point.Y(), point.Z());
		//	index++;
		//}

		//for (int i = 0; i < pts.size(); i += 3)
		//{
		//	Standard_Integer n1 = i + 1;
		//	Standard_Integer n2 = i + 2;
		//	Standard_Integer n3 = i + 3;
		//	triangulation->AddTriangle(n1, n2, n3);
		//}

		//TopoDS_Face face;
		//BRepBuilderAPI_MakePolygon polygon;
		//ShapeFix_Wire wireFixer;
		//wireFixer.SetFaceMode(Standard_True);
		//for (int i = 0; i < triangulation->NbTriangles(); i++)
		//{
		//	Standard_Integer n1, n2, n3;
		//	triangulation->Triangle(i + 1).Get(n1, n2, n3);

		//	polygon.Add(gp_Pnt(triangulation->Node(n1).X(), triangulation->Node(n1).Y(), triangulation->Node(n1).Z()));
		//	polygon.Add(gp_Pnt(triangulation->Node(n2).X(), triangulation->Node(n2).Y(), triangulation->Node(n2).Z()));
		//	polygon.Add(gp_Pnt(triangulation->Node(n3).X(), triangulation->Node(n3).Y(), triangulation->Node(n3).Z()));

		//	if (polygon.IsDone())
		//	{
		//		TopoDS_Shape wireShape = polygon.Wire();
		//		wireFixer.Load(wireShape);
		//		wireFixer.Perform();
		//		wireShape = wireFixer.Wire();
		//		if (wireShape.IsNull() || !wireShape.Closed())
		//		{
		//			polygon.Clear();
		//			continue;
		//		}
		//		face = BRepBuilderAPI_MakeFace(wireFixer.Wire());

		//		builder.Add(shape, face);

		//		polygon.Clear();
		//	}
		//}


	}
	
	// 创建显示对象
	//Handle(AIS_Shape) aisShape = new AIS_Shape(shape);
	//aisShape->SetColor(Quantity_NOC_RED);

	// 将显示对象添加到交互上下文中
	//m_context->Display(aisShape, true);
}

void CMFCOCCTDoc::CloudPoint2Surface()
{
	
}

CMFCOCCTView* CMFCOCCTDoc::GetView()
{
	POSITION pos = GetFirstViewPosition();
	return (CMFCOCCTView*)GetNextView(pos);
}

void CMFCOCCTDoc::Draw2DCircle()
{
	//gp_Ax1 ZAxis = gp::OY();
	//gp_Pnt p1(100, 200, 300);
	//gp_Pnt p2(200, 200, 300);
	//Handle(Geom_TrimmedCurve) aShaper = BuildArc(ZAxis, p1, p2);
	//Show2DShape(wire, Aspect_TOL_SOLID, Aspect_WOL_THICK, Quantity_NOC_LIGHTYELLOW, Standard_False);
	gp_Ax2 axis(gp_Pnt(0,0,0), gp::DZ());
	Handle_Geom_Circle theCircle = new Geom_Circle(axis, 4.0);
	BRepBuilderAPI_MakeEdge makeEdge(theCircle);
	TopoDS_Edge edge = makeEdge.Edge();
	TopoDS_Wire wire = BRepBuilderAPI_MakeWire(edge);

	// 将几何对象转换为OpenCASCADE显示对象
	Handle(AIS_Shape) Circle = new AIS_Shape(wire);
	myAISContext->Display(Circle, true);
	
	myViewer->SetDefaultViewProj(V3d_Zpos);
	GetView()->FitAll();
}



// CMFCOCCTDoc 序列化

void CMFCOCCTDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: 在此添加存储代码
	}
	else
	{
		// TODO: 在此添加加载代码
	}
}

#ifdef SHARED_HANDLERS

// 缩略图的支持
void CMFCOCCTDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// 修改此代码以绘制文档数据
	dc.FillSolidRect(lprcBounds, RGB(255, 255, 255));

	CString strText = _T("TODO: implement thumbnail drawing here");
	LOGFONT lf;

	CFont* pDefaultGUIFont = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
	pDefaultGUIFont->GetLogFont(&lf);
	lf.lfHeight = 36;

	CFont fontDraw;
	fontDraw.CreateFontIndirect(&lf);

	CFont* pOldFont = dc.SelectObject(&fontDraw);
	dc.DrawText(strText, lprcBounds, DT_CENTER | DT_WORDBREAK);
	dc.SelectObject(pOldFont);
}

// 搜索处理程序的支持
void CMFCOCCTDoc::InitializeSearchContent()
{
	CString strSearchContent;
	// 从文档数据设置搜索内容。
	// 内容部分应由“;”分隔

	// 例如:     strSearchContent = _T("point;rectangle;circle;ole object;")；
	SetSearchContent(strSearchContent);
}

void CMFCOCCTDoc::SetSearchContent(const CString& value)
{
	if (value.IsEmpty())
	{
		RemoveChunk(PKEY_Search_Contents.fmtid, PKEY_Search_Contents.pid);
	}
	else
	{
		CMFCFilterChunkValueImpl *pChunk = nullptr;
		ATLTRY(pChunk = new CMFCFilterChunkValueImpl);
		if (pChunk != nullptr)
		{
			pChunk->SetTextValue(PKEY_Search_Contents, value, CHUNK_TEXT);
			SetChunkValue(pChunk);
		}
	}
}

#endif // SHARED_HANDLERS

// CMFCOCCTDoc 诊断

#ifdef _DEBUG
void CMFCOCCTDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CMFCOCCTDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CMFCOCCTDoc 命令


void CMFCOCCTDoc::OnPopuAddtexture()
{
	//Handle_AIS_TexturedShape  texture = new AIS_TexturedShape(B1);
	////贴图相关设置
	//texture->SetTextureFileName("F:\StudyProject\CProject\OCCTProject\Two\OCCMFC2\MFCOCCT\x64\KOBES.jpg");
	//texture->SetTextureMapOn();
	//texture->SetDisplayMode(3);
	//texture->SetTextureRepeat(true, 1, 1);
	////texture->EnableTextureModulate();
	//texture->DisableTextureModulate();
	//myAISContext->SetDisplayMode(texture, 3, false);

	//myAISContext->Display(texture, 3, -1, false);
	//myViewer->Redraw();
}

void CMFCOCCTDoc::DrawLineByMouse(gp_Pnt thePnt1, gp_Pnt thePnt2)
{
	//检查传入参数
	if (thePnt1.IsEqual(thePnt2, 1e-3))
		return;

	//构建拓扑线段
	Handle(Geom_TrimmedCurve) aSegment = GC_MakeSegment(thePnt1, thePnt2);
	TopoDS_Edge aEdge = BRepBuilderAPI_MakeEdge(aSegment);
	DisplayShape(aEdge);

	//将构建的拓扑线段设置至AIS_Shape形状中
	//myAISContext->SetShape(aEdge);

	////移除前面绘画的旧线段, 绘制新线段。
	//myAISContext->Remove(myAISContext, GetView());
	//myAISContext->Display(aEdge, Standard_False);

	//更新View
	//myAISContext->UpdateCurrentViewer();
}
