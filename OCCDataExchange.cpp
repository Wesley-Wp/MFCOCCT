#include "pch.h"
#include "OCCDataExchange.h"

Handle(TopTools_HSequenceOfShape) OCCDataExchange::ImportStep(Standard_CString& aFileName)
{
	Handle(TopTools_HSequenceOfShape) TopoDsShapes = new TopTools_HSequenceOfShape();

	if (aFileName == "")
		return TopoDsShapes;

	STEPControl_Reader Reader;

	Standard_Integer status = Reader.ReadFile(aFileName);

	if (status != IFSelect_RetDone)
		return TopoDsShapes;

	Reader.WS();
	Reader.PrintCheckTransfer(false, IFSelect_ItemsByEntity);

	Interface_Static::SetIVal("read.step.assembly.level", 1);

	Standard_Integer nbr = Reader.NbRootsForTransfer();

	for (Standard_Integer n = 1; n <= nbr; n++)
	{
		Reader.TransferRoot(n);
	}

	// Collecting resulting entities
	Standard_Integer nbs = Reader.NbShapes();
	if (nbs == 0)
	{
		return TopoDsShapes;
	}

	for (Standard_Integer i = 1; i <= nbs; i++)
	{
		TopoDsShapes->Append(Reader.Shape(i));
	}

	return TopoDsShapes;
}

bool OCCDataExchange::ExportStep(Standard_CString& aFileName, TopoDS_Shape& model)
{
	if (aFileName == "")
		return false;

	if (model.IsNull())
		return false;

	STEPControl_Writer aWriter;
	IFSelect_ReturnStatus status;

	status = aWriter.Transfer(model, STEPControl_StepModelType::STEPControl_AsIs);

	if (status != IFSelect_RetDone)
		return false;

	status = aWriter.Write(aFileName);

	if (status != IFSelect_RetDone)
		return false;

	return true;
}

TopoDS_Shape OCCDataExchange::ImportIges(Standard_CString& aFileName)
{
	if (aFileName == "")
		return TopoDS_Shape();

	IGESControl_Reader Reader;

	Standard_Integer status = Reader.ReadFile(aFileName);

	if (status != IFSelect_RetDone)
		return TopoDS_Shape();

	Reader.TransferRoots();

	TopoDS_Shape model = Reader.OneShape();

	return model;
}

bool OCCDataExchange::ExportIges(Standard_CString& aFileName, TopoDS_Shape& model)
{
	if (aFileName == "")
		return false;

	if (model.IsNull())
		return false;

	IGESControl_Controller::Init();
	IGESControl_Writer ICW(Interface_Static::CVal("XSTEP.iges.unit"),
		Interface_Static::IVal("XSTEP.iges.writebrep.mode"));

	ICW.AddShape(model);

	ICW.ComputeModel();
	Standard_Boolean result = ICW.Write(aFileName);

	return result;
}

Handle(Poly_Triangulation) OCCDataExchange::ImportStl(Standard_CString& aFileName)
{
	if (aFileName == "")
		return NULL;

	Message_ProgressRange arng;
	Handle(Poly_Triangulation) astlModel = RWStl::ReadFile(aFileName, arng);

	return astlModel;
}

bool OCCDataExchange::ExportStl(Standard_CString& aFileName, TopoDS_Shape& model)
{
	if (aFileName == "")
		return false;

	if (model.IsNull())
		return false;

	StlAPI_Writer myStlWriter;
	return myStlWriter.Write(model, aFileName);
}
