#ifndef MESHEXPORT_H
#define MESHEXPORT_H

#include <toadlet/egg/Collection.h>
#include <toadlet/egg/io/FileInputStream.h>
#include <toadlet/egg/io/FileOutputStream.h>
#include <toadlet/egg/math/MathTextSTDStream.h>
#include <toadlet/peeper/Light.h>
#include <toadlet/peeper/Texture.h>
#include <toadlet/tadpole/Engine.h>
#include <toadlet/tadpole/sg/SceneManager.h>
#include <toadlet/tadpole/mesh/MeshData.h>
#include <toadlet/tadpole/material/StandardMaterial.h>
#include <toadlet/tadpole/skeleton/SkeletonData.h>

#include "Max.h"
#include "stdmat.h"
#include "resource.h"
#include "istdplug.h"
#include "iparamb2.h"
#include "iparamm2.h"
#include "phyexp.h"

#include "ExportOptions.h"

#include <vector>
#include <string>
#include <sstream>

typedef toadlet::egg::Map<Mtl*,toadlet::egg::Collection<int> > MtlIndexBufferMap;
typedef toadlet::egg::Map<INode*,int> NodeIndexMap;

// Handles the conversion of a Max model into a Toadlet MeshData
class MeshExport{
	public:
		ExportOptions *mOptions;
		int mIndexCounter;
		int mNumNodes;
		toadlet::egg::Collection<toadlet::egg::math::Vector3> mNormals;
		toadlet::egg::Collection<toadlet::egg::math::Vector3> mVertexes;
		toadlet::egg::Collection<toadlet::egg::math::Vector2> mTextureVertexes;
		toadlet::egg::Collection<toadlet::egg::Collection<int> > mBoneIndexes;
		toadlet::egg::Collection<toadlet::egg::Collection<float> > mBoneWeights;
		toadlet::egg::Collection<toadlet::tadpole::material::Material::Ptr> mMaterials;
		toadlet::egg::Collection<toadlet::peeper::Light> mLights;
		toadlet::tadpole::skeleton::SkeletonData::Ptr mSkeleton;
		toadlet::tadpole::skeleton::AnimationData::Ptr mAnimation;
		MtlIndexBufferMap mMtlIndexBufferMap;
		NodeIndexMap mNodeIndexMap;
		toadlet::tadpole::mesh::MeshData::Ptr mMesh;
		Interface *mInterface;
		toadlet::tadpole::Engine *mEngine;
		
		MeshExport();
		virtual ~MeshExport();
		int doExport(const TCHAR *name, ExpInterface *ei, Interface *i, BOOL suppressPrompts, DWORD options);
		bool skipNode(INode *node);
		void setNodeIndex(INode *node, int index);
		int getNodeIndex(INode *node);
		bool constructMeshObject();
};

#endif