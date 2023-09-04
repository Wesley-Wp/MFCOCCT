#pragma once

#include "OpenCascadeInclude.h"
#include "OccFunctions.h"
#include <Standard_Macro.hxx>
#include <Standard_DefineHandle.hxx>

class AISSurface : public AIS_InteractiveObject
{
	DEFINE_STANDARD_RTTIEXT(AISSurface, AIS_InteractiveObject)
public:
	AISSurface(const Handle(Geom_Surface)& theSurface);
	virtual ~AISSurface() {}

public:
	TopoDS_Shell& GetSurface() { return myShell; };

private:
	void CreateShellFromSurface();

private:

	virtual void Compute(const Handle(PrsMgr_PresentationManager)& aPresentationManager,
		                 const Handle(Prs3d_Presentation)& aPrs, const Standard_Integer aMode = 1);

	virtual void ComputeSelection(const Handle(SelectMgr_Selection)& aSelection, const Standard_Integer aMode);

private:

	Handle(Geom_Surface)         mySurface;
	Handle(Prs3d_ShadingAspect)  aShadeAspect;
	Handle(Prs3d_IsoAspect)      anIsoAspect;
	Quantity_Color               aNameOfColor;
	TopoDS_Shell                 myShell;
	gp_Pnt2d                     myUVOrigin; //!< UV origin vector for generating texture coordinates
	gp_Pnt2d                     myUVRepeat; //!< UV repeat vector for generating texture coordinates
	gp_Pnt2d                     myUVScale;  //!< UV scale  vector for generating texture coordinates
};

