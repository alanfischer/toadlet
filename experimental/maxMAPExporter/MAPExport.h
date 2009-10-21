#ifndef COLLISIONEXPORT_H
#define COLLISIONEXPORT_H

#include <toadlet/egg/Collection.h>
#include <toadlet/egg/Map.h>
#include <toadlet/egg/String.h>
#include <toadlet/egg/math/Math.h>
#include "../csg/BrushCollection.h"

#include "Max.h"
#include "stdmat.h"
#include "resource.h"
#include "istdplug.h"
#include "iparamb2.h"
#include "iparamm2.h"

#include "ExportOptions.h"

typedef toadlet::egg::Map<INode*,int> NodeIndexMap;

class MAPExport{
public:	
	MAPExport();
	virtual ~MAPExport();
	int doExport(const TCHAR *name, ExpInterface *ei, Interface *i, BOOL suppressPrompts, DWORD options);
	void setNodeIndex(INode *node, int index);
	int getNodeIndex(INode *node);

	void reportInvalidObject(const toadlet::egg::String &name,csg::Brush::CompileResult result);
	void reportNonBrushObject(const toadlet::egg::String &name);

	void addBrush(csg::Brush *brush);
	int getMaterialIndexFromMtl(Mtl *mtl);

	void addLight(const toadlet::peeper::Light &light);

	toadlet::egg::math::Vector3 snapToGrid(const toadlet::egg::math::Vector3 &point);

	typedef toadlet::egg::Map<csg::Brush::CompileResult,toadlet::egg::Collection<toadlet::egg::String> > InvalidObjectMap;
	typedef toadlet::egg::Map<Mtl*,int> MtlToMaterialIndexMap;

	ExportOptions *mOptions;
	int mIndexCounter;
	int mNumNodes;
	NodeIndexMap mNodeIndexMap;
	Interface *mInterface;
	csg::BrushCollection mBrushCollection;
	MtlToMaterialIndexMap mMtlToMaterialIndexMap;
	int mInvalidObjectCount;
	InvalidObjectMap mInvalidObjectMap;
	toadlet::egg::Collection<toadlet::egg::String> mNonBrushObjects;
	toadlet::egg::Collection<toadlet::peeper::Light> mLights;
	toadlet::egg::Collection<toadlet::egg::String> mMissingMaps;
	toadlet::tadpole::Engine *mEngine;
};

#endif