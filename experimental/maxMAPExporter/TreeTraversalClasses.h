#ifndef TREETRAVERSALCLASSES_H
#define TREETRAVERSALCLASSES_H

#include "MAPExport.h"


// Tree Enumeration Callback - Get the solids from the nodes
class GetGeometry : public ITreeEnumProc{
	public:
		MAPExport *mExport;
		TimeValue mDumpTime;	// dump snapshot at this frame time

		virtual int callback(INode *node);
};


#endif