#ifndef TREETRAVERSALCLASSES_H
#define TREETRAVERSALCLASSES_H

#include "MeshExport.h"


// Index all non-ignored max nodes in the scene 
class IndexNodes : public ITreeEnumProc{
	public:
		MeshExport *mMeshExport;
		int mNodeIndex;

		virtual int callback(INode *node);
};


// Tree Enumeration Callback - Grab the skeleton from the nodes
class GetSkeleton : public ITreeEnumProc{
	public:
		MeshExport *mMeshExport;
		TimeValue mDumpTime;

		virtual int callback(INode *node);
};


// Tree Enumeration Callback - Grab the animations from the nodes
class GetAnimation : public ITreeEnumProc{
	public:
		MeshExport *mMeshExport;
		TimeValue mDumpTime;
		bool mFirstLoop;

		virtual int callback(INode *node);
};


// Tree Enumeration Callback - Get the triangle meshes from the nodes
class GetGeometry : public ITreeEnumProc{
	public:
		MeshExport *mMeshExport;
		TimeValue mDumpTime;	// dump snapshot at this frame time

		virtual int callback(INode *node);
	
	private:
		bool mUsePhysique;
		IPhyContextExport *mPhyContextExport;
		IPhysiqueExport *mPhyExport;
		Modifier *mPhyMod;

		bool findPhysiqueModifier(INode *node);
		Point3 getVertexNormal(RVertex *rv, DWORD groupFace);
		int getNumberOfBonesControllingVertex(int i);
		int getBoneControllerOfVertex(int boneIndex,int vertexIndex);
		float getBoneControllerWeightOfVertex(int boneIndex,int vertexIndex);
};


// Collect all lights in the scene 
class GetLights : public ITreeEnumProc{
	public:
		MeshExport *mMeshExport;
		TimeValue mDumpTime;

		virtual int callback(INode *node);
};


#endif