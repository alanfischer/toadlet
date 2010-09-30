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

#include <toadlet/tadpole/Engine.h>
#include <toadlet/tadpole/Scene.h>
#include <toadlet/tadpole/terrain/TerrainNode.h>
#include <toadlet/tadpole/terrain/TerrainPatchNode.h>

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

TerrainNode::TerrainNode():super(),
	mListener(NULL),
	//mTarget,
	mDataSource(NULL),
	mSize(0),mHalfSize(0),
	mTerrainX(0),mTerrainY(0),
	//mUnactivePatches,
	//mPatchGrid,
	mPatchSize(0)
	//mPatchMaterial,
	//mPatchScale,
	//mPatchData
{}

TerrainNode::~TerrainNode(){}

Node *TerrainNode::create(Scene *scene){
	super::create(scene);

	mSize=3;
	mHalfSize=mSize/2;
	mPatchGrid.resize(mSize*mSize);

	int i,j;
	for(j=0;j<mSize;++j){
		for(i=0;i<mSize;++i){
			TerrainPatchNode::ptr patch=mEngine->createNodeType(TerrainPatchNode::type(),mScene);
			attach(patch);
			mUnactivePatches.add(patch);
		}
	}

	mTerrainX=0;
	mTerrainY=0;

	return this;
}

void TerrainNode::destroy(){
	if(mPatchMaterial!=NULL){
		mPatchMaterial->release();
		mPatchMaterial=NULL;
	}

	super::destroy();
}

void TerrainNode::setTarget(Node *target){
	mTarget=target;

	const Vector3 &translate=mTarget->getWorldTranslate();
	mTerrainX=fromWorldXi(translate.x);
	mTerrainY=fromWorldYi(translate.y);

	updateTarget();
}

void TerrainNode::setDataSource(TerrainNodeDataSource *dataSource){
	mDataSource=dataSource;

	mPatchSize=mDataSource->getPatchSize();
	mPatchScale.set(mDataSource->getPatchScale());
	mPatchData.resize(mPatchSize*mPatchSize);

	int x,y;
	for(y=mTerrainY-mHalfSize;y<=mTerrainY+mHalfSize;++y){
		for(x=mTerrainX-mHalfSize;x<=mTerrainX+mHalfSize;++x){
			createPatch(x,y);
		}
	}

	if(mListener!=NULL){
		mListener->terrainUpdated(mTerrainX,mTerrainY,mTerrainX,mTerrainY);
	}

	updateLocalBound();
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
	for(i=0;i<mPatchGrid.size();++i){
		if(mPatchGrid[i]!=NULL){
			mPatchGrid[i]->setMaterial(mPatchMaterial);
		}
	}
}

void TerrainNode::queueRenderables(CameraNode *camera,RenderQueue *queue){
	int i;
	for(i=0;i<mPatchGrid.size();++i){
		mPatchGrid[i]->updateBlocks(camera);
	}

	for(i=0;i<mPatchGrid.size();++i){
		mPatchGrid[i]->updateVertexes();
	}

	super::queueRenderables(camera,queue);
}

void TerrainNode::logicUpdate(int dt,int scope){
	super::logicUpdate(dt,scope);

	updateTarget();
}

void TerrainNode::traceSegment(Collision &result,const Vector3 &position,const Segment &segment,const Vector3 &size){
	result.time=Math::ONE;

	Collision r;
	int i;
	for(i=0;i<mPatchGrid.size();++i){
		r.reset();
		if(mPatchGrid[i]!=NULL){
			mPatchGrid[i]->traceSegment(r,mPatchGrid[i]->getWorldTranslate(),segment,size);
		}
		if(r.time<result.time){
			result.set(r);
		}
	}
}

void TerrainNode::updateTarget(){
	if(mTarget!=NULL){
		const Vector3 &translate=mTarget->getWorldTranslate();

		scalar centerX=fromWorldXf(toWorldXi(mTerrainX));
		scalar centerY=fromWorldYf(toWorldYi(mTerrainY));
		scalar currentX=fromWorldXf(translate.x);
		scalar currentY=fromWorldYf(translate.y);
		scalar bias=0.15;

		/// @todo: Instead of just shifting 1 tile, we should check so we can shift any amount of tiles.
		///  This would let us follow the player when teleporting cleanly.
		///  However, when we thread this, we will only allow the threaded version to work when moving 1 tile.
		///  Any more than that means a teleport, which would cause us to reload all the terrain, and require a pause.
		///  We could just notify the TerrainListener about the pause being needed, so it could show a dialog to the player.
		int oldTerrainX=mTerrainX,oldTerrainY=mTerrainY;
		int newTerrainX=mTerrainX,newTerrainY=mTerrainY;
		int halfSize=mHalfSize;

		if(currentX-centerX>Math::HALF+bias){
			newTerrainX++;
		}
		else if(currentX-centerX<-Math::HALF-bias){
			newTerrainX--;
		}

		if(currentY-centerY>Math::HALF+bias){
			newTerrainY++;
		}
		else if(currentY-centerY<-Math::HALF-bias){
			newTerrainY--;
		}

		if(newTerrainX!=oldTerrainX || newTerrainY!=oldTerrainY){
			int x,y;
			for(y=oldTerrainY-halfSize;y<=oldTerrainY+halfSize;++y){
				for(x=oldTerrainX-halfSize;x<=oldTerrainX+halfSize;++x){
					bool inOld=Math::abs(x-oldTerrainX)<=halfSize && Math::abs(y-oldTerrainY)<=halfSize;
					bool inNew=Math::abs(x-newTerrainX)<=halfSize && Math::abs(y-newTerrainY)<=halfSize;
					if(inOld && !inNew){
						Logger::debug(Categories::TOADLET_TADPOLE_TERRAIN,String("destroying terrain at:")+x+","+y+" local:"+(x-oldTerrainX)+","+(y-oldTerrainY));
						destroyPatch(x,y);
					}
				}
			}

			Collection<TerrainPatchNode::ptr> newPatchGrid(mPatchGrid.size());
			Collection<TerrainPatchNode::ptr> &oldPatchGrid=mPatchGrid;
			for(y=newTerrainY-halfSize;y<=newTerrainY+halfSize;++y){
				for(x=newTerrainX-halfSize;x<=newTerrainX+halfSize;++x){
					bool inOld=Math::abs(x-oldTerrainX)<=halfSize && Math::abs(y-oldTerrainY)<=halfSize;
					bool inNew=Math::abs(x-newTerrainX)<=halfSize && Math::abs(y-newTerrainY)<=halfSize;
					if(inOld && inNew){
						Logger::debug(Categories::TOADLET_TADPOLE_TERRAIN,String("moving terrain at:")+x+","+y+" from local:"+(x-oldTerrainX)+","+(y-oldTerrainY)+" to local:"+(x-newTerrainX)+","+(y-newTerrainY));
						newPatchGrid[localPatchIndex(x-newTerrainX,y-newTerrainY)]=oldPatchGrid[localPatchIndex(x-oldTerrainX,y-oldTerrainY)];
					}
				}
			}
			mPatchGrid=newPatchGrid;
			mTerrainX=newTerrainX;
			mTerrainY=newTerrainY;

			for(y=newTerrainY-halfSize;y<=newTerrainY+halfSize;++y){
				for(x=newTerrainX-halfSize;x<=newTerrainX+halfSize;++x){
					bool inOld=Math::abs(x-oldTerrainX)<=halfSize && Math::abs(y-oldTerrainY)<=halfSize;
					bool inNew=Math::abs(x-newTerrainX)<=halfSize && Math::abs(y-newTerrainY)<=halfSize;
					if(!inOld && inNew){
						Logger::debug(Categories::TOADLET_TADPOLE_TERRAIN,String("creating terrain at:")+x+","+y+" local:"+(x-newTerrainX)+","+(y-newTerrainY));
						createPatch(x,y);
					}
				}
			}

			if(mListener!=NULL){
				mListener->terrainUpdated(oldTerrainX,oldTerrainY,newTerrainX,newTerrainY);
			}
		}
	}
}

void TerrainNode::createPatch(int x,int y){
	TerrainPatchNode::ptr patch=mUnactivePatches.back();mUnactivePatches.pop_back();
	setPatchAt(x,y,patch);

	patch->setScale(mPatchScale);
	patch->setTranslate(toWorldXi(x)-mPatchSize*mPatchScale.x/2,toWorldYi(y)-mPatchSize*mPatchScale.y/2,0);
	patch->setMaterial(mPatchMaterial);

	mDataSource->getPatchData(&mPatchData[0],x,y);
	patch->setData(&mPatchData[0],mPatchSize,mPatchSize,mPatchSize);

	if(patchAt(x-1,y)!=NULL){
		patchAt(x-1,y)->stitchToRight(patch);
	}
	if(patchAt(x+1,y)!=NULL){
		patch->stitchToRight(patchAt(x+1,y));
	}
	if(patchAt(x,y-1)!=NULL){
		patchAt(x,y-1)->stitchToBottom(patch);
	}
	if(patchAt(x,y+1)!=NULL){
		patch->stitchToBottom(patchAt(x,y+1));
	}

	patch->updateWorldTransform();

	if(mListener!=NULL){
		mListener->terrainPatchCreated(x,y,patch->getWorldBound());
	}
}

void TerrainNode::destroyPatch(int x,int y){
	TerrainPatchNode::ptr patch=patchAt(x,y);

	if(mListener!=NULL){
		mListener->terrainPatchDestroyed(x,y,patch->getWorldBound());
	}

	if(patchAt(x-1,y)!=NULL){
		patchAt(x-1,y)->unstitchFromRight(patch);
	}
	if(patchAt(x+1,y)!=NULL){
		patch->unstitchFromRight(patchAt(x+1,y));
	}
	if(patchAt(x,y-1)!=NULL){
		patchAt(x,y-1)->unstitchFromBottom(patch);
	}
	if(patchAt(x,y+1)!=NULL){
		patch->unstitchFromBottom(patchAt(x,y+1));
	}

	mUnactivePatches.add(patch);
	setPatchAt(x,y,NULL);
}

void TerrainNode::updateLocalBound(){
	int i;
	for(i=0;i<mPatchGrid.size();++i){
		if(i==0) mLocalBound.set(mPatchGrid[i]->getWorldBound());
		else mLocalBound.merge(mPatchGrid[i]->getWorldBound());
	}
}

}
}
}
