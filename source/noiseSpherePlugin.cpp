#include <maya/MFnPlugin.h>
#include <maya/MPxCommand.h> 
#include <maya/MGlobal.h>	 
#include <maya/MObject.h>
#include <maya/MSyntax.h>

#include <maya/MItMeshVertex.h>
#include <maya/MSelectionList.h>
#include <maya/MDagPath.h>

class myTestCmd : public MPxCommand
{
public:
	virtual MStatus doIt(const MArgList &argList){
		MGlobal::executeCommand("polySphere -sx 150 -sy 150");
		MSelectionList selectionList;
		MGlobal::getActiveSelectionList(selectionList);
		MDagPath sphereDagPath;
		selectionList.getDagPath(0, sphereDagPath);

		MItMeshVertex itr(sphereDagPath);

		for (; !itr.isDone(); itr.next())
		{
			MPoint vertexPosition = itr.position();
			double rand = (std::rand() / double(RAND_MAX) - 0.5) / 20;
			itr.setPosition(vertexPosition + MVector(rand, rand, rand));
		}

		return MS::kSuccess;
	}
	static void *creator() { return new myTestCmd; }
};


MStatus initializePlugin(MObject obj)
{
	MStatus status;

	MFnPlugin fnPlugin(obj, "Any", "1.0", "Any");

	status = fnPlugin.registerCommand("createNoiseSphere", myTestCmd::creator);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	return MS::kSuccess;
}

MStatus uninitializePlugin(MObject obj)
{
	MStatus status;

	MFnPlugin fnPlugin(obj);

	status = fnPlugin.deregisterCommand("createNoiseSphere");
	CHECK_MSTATUS_AND_RETURN_IT(status);

	return MS::kSuccess;
}
