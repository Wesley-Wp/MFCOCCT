#include "pch.h"
#include "AISSurface.h"

//#include <GeomAdaptor_HSurface.hxx>
#include <StdPrs_ShadedSurface.hxx>
#include <StdPrs_WFPoleSurface.hxx>
#include <StdPrs_WFSurface.hxx>

IMPLEMENT_STANDARD_RTTIEXT(AISSurface, AIS_InteractiveObject)

AISSurface::AISSurface(const Handle(Geom_Surface)& theSurface)
: mySurface(theSurface)
{
    aShadeAspect = new Prs3d_ShadingAspect();
    CreateShellFromSurface();
}

void AISSurface::CreateShellFromSurface()
{
	myShell = CreateShell(mySurface);
}

void AISSurface::Compute(const Handle(PrsMgr_PresentationManager)& aPresentationManager,
                         const Handle(Prs3d_Presentation)& aPrs,
                         const Standard_Integer theMode)
{
    GeomAdaptor_Surface anAdaptorSurface(mySurface);
    //Handle(GeomAdaptor_HSurface) anAdaptorHSurface = new GeomAdaptor_HSurface(mySurface);       
    
    if (this->HasColor())
    {
        this->Color(aNameOfColor);
        SetColor(aNameOfColor);
        aShadeAspect->SetColor(aNameOfColor);
        myDrawer->SetShadingAspect(aShadeAspect);
    }
    else
    {
        aNameOfColor = Quantity_NOC_BLACK;
    }

    switch (theMode)
    {
        case AIS_Shaded:
        {
            try
            {
                OCC_CATCH_SIGNALS
                    myDrawer->SetFaceBoundaryDraw(1);

                    StdPrs_ShadedShape::Add(aPrs, myShell, myDrawer,
                                            myDrawer->ShadingAspect()->Aspect()->ToMapTexture()
                                            && !myDrawer->ShadingAspect()->Aspect()->TextureMap().IsNull(),
                                            myUVOrigin, myUVRepeat, myUVScale);
            }
            catch (Standard_Failure const& anException)
            {
                Message::SendFail(TCollection_AsciiString("Error: AISSurface::Compute() shaded presentation builder has failed (")
                    + anException.GetMessageString() + ")");
                StdPrs_WFShape::Add(aPrs, myShell, myDrawer);
            }
            break;
        }
        case AIS_WireFrame:
        {
            anIsoAspect = new Prs3d_IsoAspect(aNameOfColor, Aspect_TOL_SOLID, 1.0, 20);
            myDrawer->SetUIsoAspect(anIsoAspect);
            myDrawer->SetVIsoAspect(anIsoAspect);
            //StdPrs_WFSurface::Add(aPrs, anAdaptorHSurface, myDrawer);
            break;
        }
    }

}

void AISSurface::ComputeSelection(const Handle(SelectMgr_Selection)& aSelection,
                                  const Standard_Integer aMode)
{
    TopAbs_ShapeEnum TypOfSel = AIS_Shape::SelectionType(aMode);
    TopoDS_Shape shape = myShell;

    Standard_Real aDeflection = StdPrs_ToolTriangulatedShape::GetDeflection(shape, myDrawer);
    try
    {
        OCC_CATCH_SIGNALS
            StdSelect_BRepSelectionTool::Load(aSelection,
                this,
                shape,
                TypOfSel,
                aDeflection,
                myDrawer->DeviationAngle(),
                myDrawer->IsAutoTriangulation());
    }
    catch (Standard_Failure const& anException)
    {
        Message::SendFail(TCollection_AsciiString("Error: AIS_Shape::ComputeSelection(") + aMode + ") has failed ("
            + anException.GetMessageString() + ")");
    }

    // insert the drawer in the BrepOwners for hilight...
    StdSelect::SetDrawerForBRepOwner(aSelection, myDrawer);

}