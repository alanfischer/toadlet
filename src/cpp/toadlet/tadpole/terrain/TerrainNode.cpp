/********** Copyright header - do not remove **********
 *
 * The Toadlet Engine
 *
 * Copyright 2009, Lightning Toads Productions, LLC
 *
 * Author(s): Alan Fischer, Andrew Fischer
 *
 * This file is part of The Toadlet Engine.
 *
 * The Toadlet Engine is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * The Toadlet Engine is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.

 * You should have received a copy of the GNU Lesser General Public License
 * along with The Toadlet Engine.  If not, see <http://www.gnu.org/licenses/>.
 *
 ********** Copyright header - do not remove **********/

#include <toadlet/egg/image/BMPHandler.h>
#include <toadlet/egg/io/FileStream.h>
#include <toadlet/tadpole/Engine.h>
#include <toadlet/tadpole/Scene.h>
#include <toadlet/tadpole/Noise.h>
#include <toadlet/tadpole/terrain/TerrainNode.h>
#include <toadlet/tadpole/terrain/TerrainPatchNode.h>

#include <string.h> //temp

using namespace toadlet::egg;
using namespace toadlet::egg::io;
using namespace toadlet::egg::image;
using namespace toadlet::peeper;
using namespace toadlet::tadpole::mesh;
using namespace toadlet::tadpole::node;
using namespace toadlet::tadpole::handler;

namespace toadlet{
namespace tadpole{
namespace terrain{

TOADLET_NODE_IMPLEMENT(TerrainNode,Categories::TOADLET_TADPOLE_TERRAIN+".TerrainNode");

TerrainNode::TerrainNode():super(){}

TerrainNode::~TerrainNode(){}

Node *TerrainNode::create(Scene *scene){
	super::create(scene);

	return this;
}

void TerrainNode::setDataSource(TerrainDataSource *dataSource,const Vector3 &scale,Material::ptr material){
	mDataSource=dataSource;

	mPatchSize=64;//mDataSource->getSize();
	mPatchScale.set(scale);
	setMaterial(material);

	mSize=3;
	mTerrainPatches.resize(mSize*mSize);

	int i,j;
	for(j=0;j<mSize;++j){
		for(i=0;i<mSize;++i){
			TerrainPatchNode::ptr patch=mEngine->createNodeType(TerrainPatchNode::type(),mScene);
			patch->setScale(mPatchScale);
			patch->setTranslate(i*mPatchSize*mPatchScale.x - mPatchSize*mPatchScale.x*(float)mSize/3.0,j*mPatchSize*mPatchScale.y - mPatchSize*mPatchScale.y*(float)mSize/3.0,0);
			attach(patch);
			mTerrainPatches[j*mSize+i]=patch;
		}
	}

	Noise noise(4,4,1,1,256);
	scalar data[64*64];
	memset(data,0,mPatchSize*mPatchSize*sizeof(float));
	for(j=0;j<mSize;j++){
		for(i=0;i<mSize;i++){
			for(int px=0;px<64;++px){
				for(int py=0;py<64;++py){
					int x=i*64+px;
					int width=3*64;
					int y=j*64+py;
					int height=3*64;
					float n=(noise.perlin2((float)x/(float)width,(float)y/(float)height)*0.5 + 0.5);
					data[py*64+px]=-n*20;
				}
			}

			TerrainPatchNode::ptr patch=mTerrainPatches[j*mSize+i];
			patch->setData(data,mPatchSize,mPatchSize,mPatchSize);
			patch->setMaterial(material);

			if(i>0){
				mTerrainPatches[j*3+(i-1)]->stitchToRight(mTerrainPatches[j*3+i]);
			}
			if(j>0){
				mTerrainPatches[(j-1)*3+i]->stitchToBottom(mTerrainPatches[j*3+i]);
			}
		}
	}

	updateLocalBound();
}

void TerrainNode::destroy(){
	if(mPatchMaterial!=NULL){
		mPatchMaterial->release();
		mPatchMaterial=NULL;
	}

	super::destroy();
}

void TerrainNode::setMaterial(Material::ptr material){
	if(mPatchMaterial!=material){
		if(mPatchMaterial!=NULL){
			mPatchMaterial->release();
		}

		mPatchMaterial=material;

		if(mPatchMaterial!=NULL){
			mPatchMaterial->retain();
		}
	}

	int i;
	for(i=0;i<mTerrainPatches.size();++i){
		mTerrainPatches[i]->setMaterial(mPatchMaterial);
	}
}

void TerrainNode::queueRenderables(CameraNode *camera,RenderQueue *queue){
	int i;
	for(i=0;i<mTerrainPatches.size();++i){
		mTerrainPatches[i]->updateBlocks(camera);
	}

	for(i=0;i<mTerrainPatches.size();++i){
		mTerrainPatches[i]->updateVertexes();
	}

	super::queueRenderables(camera,queue);
}

void TerrainNode::traceSegment(Collision &result,const Vector3 &position,const Segment &segment,const Vector3 &size){
	result.time=Math::ONE;

	Collision r;
	int i;
	for(i=0;i<mTerrainPatches.size();++i){
		mTerrainPatches[i]->traceSegment(r,mTerrainPatches[i]->getWorldTranslate(),segment,size);
		if(r.time<result.time){
			result.set(r);
		}
	}
}

void TerrainNode::updateLocalBound(){
	Sphere bound;
	int i;
	for(i=0;i<mTerrainPatches.size();++i){
		bound.set(mTerrainPatches[i]->getLocalBound());
		Math::add(bound,mTerrainPatches[i]->getTranslate());
		if(i==0) mLocalBound.set(bound);
		else Node::merge(mLocalBound,bound);
	}
}

}
}
}
