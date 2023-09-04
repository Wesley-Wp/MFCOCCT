#pragma once

#include "OpenCascadeInclude.h"
#include "OccFunctions.h"


/// <summary>
/// Mesh Node
/// </summary>
class MeshNode : public Standard_Transient
{
	DEFINE_STANDARD_RTTIEXT(MeshNode, Standard_Transient)

public:
	MeshNode(int ID, double x, double y, double z);
	~MeshNode() {};

protected:
	int m_ID;
	double m_X;
	double m_Y;
	double m_Z;
	gp_Pnt myVertex;

public:
	int GetID() { return m_ID; };
	void GetData(double& x, double& y, double& z) { x = m_X; y = m_Y; z = m_Z; };
	gp_Pnt& GetVertex();
};

DEFINE_STANDARD_HANDLE(MeshNode, Standard_Transient)

typedef NCollection_List<Handle(MeshNode)> TCol_MeshNode;

/// <summary>
/// Mesh Element
/// </summary>
class MeshElem : public Standard_Transient
{
	DEFINE_STANDARD_RTTIEXT(MeshElem, Standard_Transient)

public:
	MeshElem() {};
	MeshElem(int ID, CString ElemType) { m_ID = ID; myElemType = ElemType; };
	~MeshElem() {};

protected:
	int m_ID;
	CString myElemType;

public:
	int GetElementID() { return m_ID; };
	CString GetElementType() { return myElemType; };

};

DEFINE_STANDARD_HANDLE(MeshElem, Standard_Transient)

typedef NCollection_List<Handle(MeshElem)> TCol_MeshElem;

class TriaElem : public MeshElem
{
	DEFINE_STANDARD_RTTIEXT(TriaElem, MeshElem)

public:
	TriaElem(int ID) { m_ID = ID; myElemType = "TRIA"; };
	~TriaElem() {};

private:
	int N1;
	int N2;
	int N3;

public:
	void AddNodes(int NID1, int NID2, int NID3) { N1 = NID1; N2 = NID2; N3 = NID3; };
	void GetNodes(int& NID1, int& NID2, int& NID3) { NID1 = N1; NID2 = N2; NID3 = N3; };

};

DEFINE_STANDARD_HANDLE(TriaElem, MeshElem)

class QuadElem : public MeshElem
{
	DEFINE_STANDARD_RTTIEXT(QuadElem, MeshElem)

public:
	QuadElem(int ID) { m_ID = ID; myElemType = "QUAD4"; };
	~QuadElem() {};

private:
	int N1;
	int N2;
	int N3;
	int N4;

public:
	void AddNodes(int NID1, int NID2, int NID3, int NID4) { N1 = NID1; N2 = NID2; N3 = NID3; N4 = NID4; };
	void GetNodes(int& NID1, int& NID2, int& NID3, int& NID4) { NID1 = N1; NID2 = N2; NID3 = N3; NID4 = N4;};

};

DEFINE_STANDARD_HANDLE(QuadElem, MeshElem)

/// <summary>
/// Mesh Data Structure
/// </summary>
class MeshDS : public Standard_Transient
{
	DEFINE_STANDARD_RTTIEXT(MeshDS, Standard_Transient)

public:
	MeshDS();
	~MeshDS() {};

private:
	TCol_MeshNode myNodes;
	TCol_MeshElem myElements;

private:
	Quantity_Color myMeshColor;
	Quantity_Color myMeshEdgeColor;
	Quantity_Color myMeshNodeColor;

	int NbNodes = 0;
	int NbTriaElements = 0;
	int NbQuadElements = 0;

	AIS_ListOfInteractive myAISNodes;
	AIS_ListOfInteractive myNodeLabels;

public:

	//========================================================================================
	// Mesh Data Structure
	//
	//========================================================================================

	int GetNbNodes() { return myNodes.Size(); };
	int GetNbElements() { return myElements.Size(); };

	// Add Nodes
	void AddMeshNode(int ID, double x, double y, double z);
	void GetMeshNode(int ID, double& x, double& y, double& z);
	Handle(MeshNode) GetMeshNode(int ID);

	// Add Elements
	Handle(MeshElem) GetElement(int ID);
	void Add3NDTriaElem(int ID, int NID1, int NID2, int NID3);
	void Add4NDQuadElem(int ID, int NID1, int NID2, int NID3, int NID4);

	// For debug purpose
	void CreateTempMesh();

	//========================================================================================
	// Mesh Display
	// Graphics Presentation of Mesh
	//========================================================================================

	bool BuildPrs(Handle(Graphic3d_ArrayOfTriangles)& PAt, Handle(Graphic3d_ArrayOfSegments)& PAs);

	void DisplayNodes(bool bOn, Handle(AIS_InteractiveContext)& theCTX);
	void DisplayNodeLabels(bool bOn, Handle(AIS_InteractiveContext)& theCTX);

};

DEFINE_STANDARD_HANDLE(MeshDS, Standard_Transient)

/// <summary>
/// Mesh Visualization
/// </summary>
class AISMesh : public AIS_InteractiveObject
{
	DEFINE_STANDARD_RTTIEXT(AISMesh, AIS_InteractiveObject)

public:
	AISMesh(const Handle(MeshDS)& theMeshDS);
	virtual ~AISMesh() {};

protected:
	Handle(Graphic3d_AspectFillArea3d) anAspects;

	// Declare Primitive Groups
	Handle(Graphic3d_Group) aMeshGroup;
	Handle(Graphic3d_Group) aEdgeGroup;

	// Declare Primitive Array
	Handle(Graphic3d_ArrayOfTriangles) aMesh;
	Handle(Graphic3d_ArrayOfSegments) aEdges;

private:

	virtual void Compute(const Handle(PrsMgr_PresentationManager)& aPresentationManager,
		const Handle(Prs3d_Presentation)& aPrs, const Standard_Integer aMode = 1);

	virtual void ComputeSelection(const Handle(SelectMgr_Selection)& aSelection,
		const Standard_Integer aMode) {};


private:
	Handle(MeshDS) myMeshDS;
};

DEFINE_STANDARD_HANDLE(AISMesh, AIS_InteractiveObject)