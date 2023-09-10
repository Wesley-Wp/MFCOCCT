#pragma once
#include "OpenCascadeInclude.h"

class OCCDataExchange
{
public:
	// Step 
	static Handle(TopTools_HSequenceOfShape) ImportStep(Standard_CString& aFileName);
	static bool ExportStep(Standard_CString& aFileName, TopoDS_Shape& model);

	// Iges
	static TopoDS_Shape ImportIges(Standard_CString& aFileName);
	static bool ExportIges(Standard_CString& aFileName, TopoDS_Shape& model);

	// Stl
	static Handle(Poly_Triangulation) ImportStl(Standard_CString& aFileName);
	static bool ExportStl(Standard_CString& aFileName, TopoDS_Shape& model);
};

