#ifndef TREETRAVERSALCLASSES_H
#define TREETRAVERSALCLASSES_H

#include "CollisionExport.h"


// Index all non-ignored max nodes in the scene 
class IndexNodes : public ITreeEnumProc{
	public:
		CollisionExport *mCollisionExport;
		int mNodeIndex;

		virtual int callback(INode *node);
};


// Tree Enumeration Callback - Get the solids from the nodes
class GetGeometry : public ITreeEnumProc{
	public:
		CollisionExport *mCollisionExport;
		TimeValue mDumpTime;	// dump snapshot at this frame time

		virtual int callback(INode *node);	
};


#endif