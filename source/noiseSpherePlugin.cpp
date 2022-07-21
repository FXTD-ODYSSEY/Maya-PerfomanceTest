#include <stdio.h>
#include <maya/MFnPlugin.h>
#include <maya/MPxCommand.h>
#include <maya/MGlobal.h>
#include <maya/MObject.h>
#include <maya/MSyntax.h>

#include <maya/MItMeshVertex.h>
#include <maya/MSelectionList.h>
#include <maya/MDagPath.h>
#include <maya/MString.h>
#include <maya/MArgDatabase.h>

const char *kSubXShort = "-sx";
const char *kSubXLong = "-subdivisionsX";
const char *kSubYShort = "-sy";
const char *kSubYLong = "-subdivisionsY";

class myTestCmd : public MPxCommand
{
public:
	static void *creator() { return new myTestCmd; }
	static MSyntax newSyntax()
	{
		MSyntax syntax;
		syntax.addFlag(kSubXShort, kSubXLong, MSyntax::kUnsigned);
		syntax.addFlag(kSubYShort, kSubYLong, MSyntax::kUnsigned);
		return syntax;
	}
	virtual bool hasSyntax() { return true; }
	virtual MStatus doIt(const MArgList &argList)
	{
		MStatus status;
		int sx = 150;
		int sy = 150;
		// MString tmp;

		MArgDatabase argData(syntax(), argList, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		if (argData.isFlagSet(kSubXShort))
		{
			sx = argData.flagArgumentInt(kSubXShort, 0, &status);
			CHECK_MSTATUS_AND_RETURN_IT(status);
		}
		if (argData.isFlagSet(kSubYShort))
		{
			sy = argData.flagArgumentInt(kSubYShort, 0, &status);
			CHECK_MSTATUS_AND_RETURN_IT(status);
		}

		char command[40];
		sprintf(command, "polySphere -sx %d -sy %d", sx, sy);
		MGlobal::executeCommand(command);
		MSelectionList selectionList;
		MGlobal::getActiveSelectionList(selectionList);
		MDagPath sphereDagPath;
		selectionList.getDagPath(0, sphereDagPath);

		for (MItMeshVertex itr(sphereDagPath); !itr.isDone(); itr.next())
		{
			MPoint vertexPosition = itr.position();
			double rand = (std::rand() / double(RAND_MAX) - 0.5) / 20;
			itr.setPosition(vertexPosition + MVector(rand, rand, rand));
		}

		return MS::kSuccess;
	}
};

MStatus initializePlugin(MObject obj)
{
	MStatus status;

	MFnPlugin fnPlugin(obj, "TimmyLiang", "1.0", "Any");

	status = fnPlugin.registerCommand("createNoiseSphere", myTestCmd::creator, myTestCmd::newSyntax);
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
