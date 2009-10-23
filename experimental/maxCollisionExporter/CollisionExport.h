#ifndef COLLISIONEXPORT_H
#define COLLISIONEXPORT_H

#include <toadlet/egg/Collection.h>
#include <toadlet/egg/Map.h>

#include "../XCOLHandler/XCOLHandler.h"

#include "Max.h"
#include "stdmat.h"
#include "resource.h"
#include "istdplug.h"
#include "iparamb2.h"
#include "iparamm2.h"

#include "ExportOptions.h"

typedef toadlet::egg::Map<INode*,int> NodeIndexMap;

// Handles the conversion of a Max model into an Opal Blueprint
class CollisionExport{
	public:
		ExportOptions *mOptions;
		int mIndexCounter;
		int mNumNodes;
		toadlet::egg::Collection<toadlet::egg::math::Vector3> mVertexes;
		NodeIndexMap mNodeIndexMap;
		ShapeDataCollection mShapeCollection;
		Interface *mInterface;
		
		CollisionExport();
		virtual ~CollisionExport();
		int doExport(const TCHAR *name, ExpInterface *ei, Interface *i, BOOL suppressPrompts, DWORD options);
		bool skipNode(INode *node);
		void setNodeIndex(INode *node, int index);
		int getNodeIndex(INode *node);
};

#endif