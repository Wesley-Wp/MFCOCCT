#include "pch.h"
#include "framework.h"

#include "OCCViewerView.h"

#include "AISCurve.h"
#include "AISSurface.h"

void COCCViewerView::ClearContext(bool All)
{
	AIS_ListOfInteractive aList;
	myAISContext->DisplayedObjects(aList);
	AIS_ListIteratorOfListOfInteractive aListIterator;
	for (aListIterator.Initialize(aList);aListIterator.More();aListIterator.Next()) 
	{
		if (All)
		{			
			myAISContext->Remove(aListIterator.Value(), Standard_True);
		}
		else
		{
			if (aListIterator.Value()->Type() == AIS_KindOfInteractive::AIS_KOI_Shape)
			{
				myAISContext->Remove(aListIterator.Value(), Standard_True);
			}
		}
	}

	// Redisplay AIS trihedron
	if (bShowAISTrihedron)
	{
		ShowAISTrihedron(bShowAISTrihedron);
	}

	// Redisplay
	if (bShowCube)
	{
		ShowCube(bShowCube);
	}
}

AIS_ListOfInteractive COCCViewerView::GetListofBodies()
{
	// Initialize list
	AIS_ListOfInteractive m_ShapeList;	// List of only shapes
	AIS_ListOfInteractive aList;		// List of all interactive objects
	m_ShapeList.Clear();

	// Get objects from context
	myAISContext->ObjectsInside(aList);

	// Iterate on the list to get shape objects
	if (aList.Size() > 0)
	{
		for (AIS_ListIteratorOfListOfInteractive aLI(aList); aLI.More(); aLI.Next())
		{
			if (aLI.Value()->Type() == AIS_KindOfInteractive::AIS_KOI_Shape)
			{
				m_ShapeList.Append(aLI.Value());
			}
		}
	}

	return m_ShapeList;
}

void COCCViewerView::OnShaded()
{
	// Set shaded display mode
	myAISContext->SetDisplayMode(AIS_Shaded, Standard_True);

	// Turn off edges
	Handle(Prs3d_Drawer) defdrawer = myAISContext->DefaultDrawer();
	defdrawer->SetFaceBoundaryDraw(false);

	// Redisplay objects
	AIS_ListOfInteractive aList;
	myAISContext->ObjectsInside(aList);
	for (AIS_ListIteratorOfListOfInteractive aLI(aList); aLI.More(); aLI.Next())
	{
		myAISContext->ClearPrs(aLI.Value(), 1, Standard_False);
		myAISContext->RecomputePrsOnly(aLI.Value(), Standard_False);
		myAISContext->SetDisplayMode(aLI.Value(), 1, Standard_False);
		myAISContext->Redisplay(aLI.Value(), 1, true);
	}

	myAISContext->UpdateCurrentViewer();
}

void COCCViewerView::OnShadedEdges()
{
	// Set shaded display mode
	myAISContext->SetDisplayMode(AIS_Shaded, Standard_True);

	// Turn on edges
	Handle(Prs3d_Drawer) defdrawer = myAISContext->DefaultDrawer();
	defdrawer->SetFaceBoundaryDraw(true);
	defdrawer->SetFaceBoundaryAspect(aEdgeAspt);

	// Redisplay objects
	AIS_ListOfInteractive aList;
	myAISContext->ObjectsInside(aList);
	for (AIS_ListIteratorOfListOfInteractive aLI(aList); aLI.More(); aLI.Next())
	{
		aLI.Value()->Attributes()->SetFaceBoundaryAspect(aEdgeAspt);
		myAISContext->ClearPrs(aLI.Value(), 1, Standard_False);
		myAISContext->RecomputePrsOnly(aLI.Value(), Standard_False);
		myAISContext->SetDisplayMode(aLI.Value(), 1, Standard_False);
		myAISContext->Redisplay(aLI.Value(), 1, true);
	}

	myAISContext->UpdateCurrentViewer();
}

void COCCViewerView::OnWireFrame()
{
	// Set wireframe display mode
	myAISContext->SetDisplayMode(AIS_WireFrame, Standard_True);
	myView->SetComputedMode(Standard_False);

	// Redisplay objects
	AIS_ListOfInteractive aList;
	myAISContext->ObjectsInside(aList);
	for (AIS_ListIteratorOfListOfInteractive aLI(aList); aLI.More(); aLI.Next())
	{
		myAISContext->ClearPrs(aLI.Value(), 1, Standard_False);
		myAISContext->RecomputePrsOnly(aLI.Value(), Standard_False);
		myAISContext->SetDisplayMode(aLI.Value(), 0, Standard_False);
		myAISContext->Redisplay(aLI.Value(), 1, true);
	}

	myAISContext->UpdateCurrentViewer();
}

void COCCViewerView::OnTransparency()
{
	if (bSetTransparent)
	{
		for (myAISContext->InitSelected();
			myAISContext->MoreSelected();
			myAISContext->NextSelected())
		{
			myAISContext->SetTransparency(myAISContext->SelectedInteractive(), 0.5, Standard_False);
		}
	}
	else
	{
		for (myAISContext->InitSelected();
			myAISContext->MoreSelected();
			myAISContext->NextSelected())
		{
			myAISContext->SetTransparency(myAISContext->SelectedInteractive(), 0, Standard_False);
		}
	}
}

void COCCViewerView::Display(const Handle(AIS_InteractiveObject)& theObj, 
	                         Standard_Boolean theToUpdateViewer)
{
	myAISContext->Display(theObj, theToUpdateViewer);
	myView->FitAll();
}

void COCCViewerView::DisplayShape(TopoDS_Shape& aShape)
{
	// Initialize shape object
	Handle(AIS_Shape) aisShape = new AIS_Shape(aShape);

	// Set Display mode
	aisShape->SetDisplayMode((int)AIS_DisplayMode::AIS_Shaded);

	// Set Edge Color
	aisShape->Attributes()->SetFaceBoundaryAspect(aEdgeAspt);

	Display(aisShape, true);
}

void COCCViewerView::DisplayShape(TopoDS_Shape& aShape, Quantity_Color theColor)
{
	// Initialize shape object
	Handle(AIS_Shape) aisShape = new AIS_Shape(aShape);

	// Set Color
	aisShape->SetColor(theColor);

	// Set Display mode
	aisShape->SetDisplayMode((int)AIS_DisplayMode::AIS_Shaded);

	// Set Edge Color
	aisShape->Attributes()->SetFaceBoundaryAspect(aEdgeAspt);

	Display(aisShape, true);
}

void COCCViewerView::DisplayText(gp_Pnt2d& thePoint, const char* theText, Standard_Real theXoffset, Standard_Real theYoffset, Standard_Real theTextScale)
{
	Handle(AIS_TextLabel) aLabel = new AIS_TextLabel();
	aLabel->SetText(theText);
	aLabel->SetPosition(gp_Pnt(thePoint.X() + theXoffset, thePoint.Y() + theYoffset, 0.0));
	aLabel->SetHeight(theTextScale);
	aLabel->SetColor(Quantity_NOC_BLACK);
	//(void)theTextScale;
	Display(aLabel, true);
}

void COCCViewerView::DisplayText(gp_Pnt& thePoint, const char* theText,
	Standard_Real theXoffset, Standard_Real theYoffset, Standard_Real theZoffset, Standard_Real theTextScale)
{
	Handle(AIS_TextLabel) aLabel = new AIS_TextLabel();
	aLabel->SetText(theText);
	aLabel->SetPosition(gp_Pnt(thePoint.X() + theXoffset, thePoint.Y() + theYoffset, thePoint.Z() + theZoffset));
	aLabel->SetHeight(theTextScale);
	aLabel->SetColor(Quantity_NOC_BLACK);
	Display(aLabel, true);
}

void COCCViewerView::DisplayPoint(gp_Pnt2d& thePoint)
{
	gp_Pnt P(thePoint.X(), thePoint.Y(), 0.0);
	DisplayPoint(P);
}

void COCCViewerView::DisplayPoint(gp_Pnt& thePoint)
{
	Handle(AIS_Point) aGraphicPoint = new AIS_Point(new Geom_CartesianPoint(thePoint));
	aGraphicPoint->SetMarker(Aspect_TypeOfMarker::Aspect_TOM_O);
	aGraphicPoint->SetColor(Quantity_NOC_RED);
	Display(aGraphicPoint, Standard_False);
}

void COCCViewerView::DisplayCurve(Handle(Geom_Curve) theCurve,
	Quantity_Color theColor,
	Standard_Boolean theToUpdateViewer)
{
	Handle(AISCurve) aGraphicCurve = new AISCurve(theCurve, Aspect_TOL_SOLID, 1, theColor);
	aGraphicCurve->SetDisplayMode(0);
	Display(aGraphicCurve, theToUpdateViewer);
}

void COCCViewerView::DisplayCurve(Handle(Geom2d_Curve) theCurve,
	const Aspect_TypeOfLine aTypeOfline,
	const Aspect_WidthOfLine aWidthOfLine,
	const Quantity_Color theColor,
	Standard_Boolean theToUpdateViewer)
{
	Handle(AISCurve) aGraphicCurve = new AISCurve(theCurve, aTypeOfline, aWidthOfLine, theColor);
	aGraphicCurve->SetDisplayMode(0);
	Display(aGraphicCurve, theToUpdateViewer);
}

void COCCViewerView::DisplaySurface(Handle(Geom_Surface) theSurface,
	Quantity_Color aNameOfColor,
	Standard_Boolean theToUpdateViewer)
{
	Handle(AISSurface) aAISSurface = new AISSurface(theSurface);
	aAISSurface->SetColor(aNameOfColor);
	Display(aAISSurface, theToUpdateViewer);
}

void COCCViewerView::DisplayBoundingBox(Bnd_Box& theBox)
{
	Standard_Real aXmin, aYmin, aZmin, aXmax, aYmax, aZmax;
	theBox.Get(aXmin, aYmin, aZmin, aXmax, aYmax, aZmax);

	DisplayPoint(gp_Pnt(aXmin, aYmax, aZmin));
	DisplayPoint(gp_Pnt(aXmax, aYmax, aZmin));
	DisplayPoint(gp_Pnt(aXmin, aYmin, aZmin));
	DisplayPoint(gp_Pnt(aXmax, aYmin, aZmin));

	DisplayPoint(gp_Pnt(aXmin, aYmax, aZmax));
	DisplayPoint(gp_Pnt(aXmax, aYmax, aZmax));
	DisplayPoint(gp_Pnt(aXmin, aYmin, aZmax));
	DisplayPoint(gp_Pnt(aXmax, aYmin, aZmax));

	DisplayCurve(GC_MakeSegment(gp_Pnt(aXmin, aYmax, aZmin),
		gp_Pnt(aXmax, aYmax, aZmin)).Value(), Quantity_NOC_RED); // X,Ymax,ZMin
	DisplayCurve(GC_MakeSegment(gp_Pnt(aXmin, aYmin, aZmin),
		gp_Pnt(aXmax, aYmin, aZmin)).Value(), Quantity_NOC_RED); // X,Ymin,ZMin
	DisplayCurve(GC_MakeSegment(gp_Pnt(aXmin, aYmin, aZmin),
		gp_Pnt(aXmin, aYmax, aZmin)).Value(), Quantity_NOC_RED); // Xmin,Y,ZMin
	DisplayCurve(GC_MakeSegment(gp_Pnt(aXmax, aYmin, aZmin),
		gp_Pnt(aXmax, aYmax, aZmin)).Value(), Quantity_NOC_RED); // Xmax,Y,ZMin
	DisplayCurve(GC_MakeSegment(gp_Pnt(aXmin, aYmax, aZmax),
		gp_Pnt(aXmax, aYmax, aZmax)).Value(), Quantity_NOC_RED); // X,Ymax,ZMax
	DisplayCurve(GC_MakeSegment(gp_Pnt(aXmin, aYmin, aZmax),
		gp_Pnt(aXmax, aYmin, aZmax)).Value(), Quantity_NOC_RED); // X,Ymin,ZMax
	DisplayCurve(GC_MakeSegment(gp_Pnt(aXmin, aYmin, aZmax),
		gp_Pnt(aXmin, aYmax, aZmax)).Value(), Quantity_NOC_RED); // Xmin,Y,ZMax
	DisplayCurve(GC_MakeSegment(gp_Pnt(aXmax, aYmin, aZmax),
		gp_Pnt(aXmax, aYmax, aZmax)).Value(), Quantity_NOC_RED); // Xmax,Y,ZMax
	DisplayCurve(GC_MakeSegment(gp_Pnt(aXmin, aYmin, aZmin),
		gp_Pnt(aXmin, aYmin, aZmax)).Value(), Quantity_NOC_RED); // Xmin,Ymin,Z
	DisplayCurve(GC_MakeSegment(gp_Pnt(aXmax, aYmin, aZmin),
		gp_Pnt(aXmax, aYmin, aZmax)).Value(), Quantity_NOC_RED); // Xmax,Ymin,Z
	DisplayCurve(GC_MakeSegment(gp_Pnt(aXmin, aYmax, aZmin),
		gp_Pnt(aXmin, aYmax, aZmax)).Value(), Quantity_NOC_RED); // Xmin,Ymax,Z
	DisplayCurve(GC_MakeSegment(gp_Pnt(aXmax, aYmax, aZmin),
		gp_Pnt(aXmax, aYmax, aZmax)).Value(), Quantity_NOC_RED); // Xmax,Ymax,Z

}

void COCCViewerView::DisplayBoundingBox(Bnd_OBB& theBox)
{
	gp_Pnt theP[8];
	theBox.GetVertex(theP);

	DisplayPoint(theP[0]);
	DisplayPoint(theP[1]);
	DisplayPoint(theP[2]);
	DisplayPoint(theP[3]);

	DisplayPoint(theP[4]);
	DisplayPoint(theP[5]);
	DisplayPoint(theP[6]);
	DisplayPoint(theP[7]);

	DisplayCurve(GC_MakeSegment(theP[0], theP[1]).Value(), Quantity_NOC_RED);
	DisplayCurve(GC_MakeSegment(theP[0], theP[2]).Value(), Quantity_NOC_RED);
	DisplayCurve(GC_MakeSegment(theP[2], theP[3]).Value(), Quantity_NOC_RED);
	DisplayCurve(GC_MakeSegment(theP[1], theP[3]).Value(), Quantity_NOC_RED);

	DisplayCurve(GC_MakeSegment(theP[4], theP[5]).Value(), Quantity_NOC_RED);
	DisplayCurve(GC_MakeSegment(theP[5], theP[7]).Value(), Quantity_NOC_RED);
	DisplayCurve(GC_MakeSegment(theP[7], theP[6]).Value(), Quantity_NOC_RED);
	DisplayCurve(GC_MakeSegment(theP[6], theP[4]).Value(), Quantity_NOC_RED);

	DisplayCurve(GC_MakeSegment(theP[3], theP[7]).Value(), Quantity_NOC_RED);
	DisplayCurve(GC_MakeSegment(theP[1], theP[5]).Value(), Quantity_NOC_RED);
	DisplayCurve(GC_MakeSegment(theP[2], theP[6]).Value(), Quantity_NOC_RED);
	DisplayCurve(GC_MakeSegment(theP[0], theP[4]).Value(), Quantity_NOC_RED);

}