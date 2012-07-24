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
#include <toadlet/tadpole/terrain/TerrainPatchComponent.h>

namespace toadlet{
namespace tadpole{
namespace terrain{

TerrainNode::TerrainNode(Scene *scene):PartitionNode(scene),
	mListener(NULL),
	//mTarget,
	mDataSource(NULL),
	mMaterialSource(NULL),
	mSize(0),mHalfSize(0),
	mTerrainX(0),mTerrainY(0),
	//mUnactivePatches,
	//mPatchGrid,
	mPatchSize(0),
	//mPatchMaterial,
	mPatchCameraUpdateScope(0),
	mPatchTerrainScope(0),
	mPatchWaterScope(0),
	mPatchTolerance(0),
	//mPatchScale,
	//mPatchHeightData,
	//mPatchLayerData,
	mUpdateTargetBias(0)
{
	mSize=3;
	mHalfSize=mSize/2;
	mPatchGrid.resize(mSize*mSize);
	mUpdateTargetBias=Math::fromMilli(250);
	mPatchCameraUpdateScope=-1;
	mPatchTerrainScope=-1;
	mPatchWaterScope=-1;
	mPatchTolerance=0.00001f;

	int i,j;
	for(j=0;j<mSize;++j){
		for(i=0;i<mSize;++i){
			TerrainPatchComponent::ptr patch=new TerrainPatchComponent(mScene);
			mUnactivePatches.add(patch);

			Node::ptr node=new Node(mScene);
			node->attach(patch);
			mPatchNodes.add(node);
		}
	}

	mTerrainX=0;
	mTerrainY=0;
}

TerrainNode::~TerrainNode(){
	destroy();
}

void TerrainNode::destroy(){
	mPatchMaterial=NULL;

	int i;
	for(i=0;i<mUnactivePatches.size();++i){
		mUnactivePatches[i]->getParent()->destroy();
	}
	mUnactivePatches.clear();

	for(i=0;i<mPatchGrid.size();++i){
		mPatchGrid[i]->getParent()->destroy();
	}
	mPatchGrid.clear();

	Node::destroy();
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
	mPatchHeightData.resize(mPatchSize*mPatchSize);
	mPatchLayerData.resize(mPatchSize*mPatchSize);

	int x,y;
	for(y=mTerrainY-mHalfSize;y<=mTerrainY+mHalfSize;++y){
		for(x=mTerrainX-mHalfSize;x<=mTerrainX+mHalfSize;++x){
			createPatch(x,y);
		}
	}

	if(mListener!=NULL){
		mListener->terrainUpdated(mTerrainX,mTerrainY,mTerrainX,mTerrainY);
	}

	updateBound();
}

void TerrainNode::setMaterial(Material::ptr material){
	if(mPatchMaterial==material){
		return;
	}

	setMaterialSource(NULL);

	if(mPatchMaterial!=material){
		mPatchMaterial=material;
	}
	
	int i;
	for(i=0;i<mPatchGrid.size();++i){
		if(mPatchGrid[i]!=NULL){
			mPatchGrid[i]->setMaterial(mPatchMaterial);
		}
	}
}

void TerrainNode::setMaterialSource(TerrainNodeMaterialSource *materialSource){
	if(mMaterialSource==materialSource){
		return;
	}

	setMaterial(NULL);
	
	mMaterialSource=materialSource;
	
	if(mMaterialSource!=NULL){
		int i;
		for(i=0;i<mPatchGrid.size();++i){
			if(mPatchGrid[i]!=NULL){
				mPatchGrid[i]->setMaterial(mMaterialSource->getMaterial(mPatchGrid[i]));
			}
		}
	}
}

void TerrainNode::setWaterMaterial(Material::ptr material){
	mPatchWaterMaterial=material;

	int i;
	for(i=0;i<mPatchGrid.size();++i){
		if(mPatchGrid[i]!=NULL){
			mPatchGrid[i]->setWaterMaterial(mPatchWaterMaterial);
		}
	}
}

void TerrainNode::setCameraUpdateScope(int scope){
	mPatchCameraUpdateScope=scope;

	int i;
	for(i=0;i<mPatchGrid.size();++i){
		if(mPatchGrid[i]!=NULL){
			mPatchGrid[i]->setCameraUpdateScope(scope);
		}
	}
}

void TerrainNode::setTerrainScope(int scope){
	mPatchTerrainScope=scope;
	
	int i;
	for(i=0;i<mPatchGrid.size();++i){
		if(mPatchGrid[i]!=NULL){
			mPatchGrid[i]->setTerrainScope(scope);
		}
	}	
}

void TerrainNode::setWaterScope(int scope){
	mPatchWaterScope=scope;
	
	int i;
	for(i=0;i<mPatchGrid.size();++i){
		if(mPatchGrid[i]!=NULL){
			mPatchGrid[i]->setWaterScope(scope);
		}
	}	
}

void TerrainNode::setTolerance(scalar tolerance){
	mPatchTolerance=tolerance;
	
	int i;
	for(i=0;i<mPatchGrid.size();++i){
		if(mPatchGrid[i]!=NULL){
			mPatchGrid[i]->setTolerance(tolerance);
		}
	}	
}

void TerrainNode::gatherRenderables(Camera *camera,RenderableSet *set){
	if((camera->getScope()&mPatchCameraUpdateScope)!=0){
		updatePatches(camera);
	}

	Node::gatherRenderables(camera,set);
}

void TerrainNode::logicUpdate(int dt,int scope){
	Node::logicUpdate(dt,scope);

	updateTarget();
}

void TerrainNode::traceSegment(Collision &result,const Vector3 &position,const Segment &segment,const Vector3 &size){
	result.time=Math::ONE;

	Collision r;
	int i;
	for(i=0;i<mPatchGrid.size();++i){
		r.reset();
		if(mPatchGrid[i]!=NULL){
			mPatchGrid[i]->traceSegment(r,mPatchGrid[i]->getParent()->getWorldTranslate(),segment,size);
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
		scalar bias=mUpdateTargetBias;

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

			Collection<TerrainPatchComponent::ptr> newPatchGrid(mPatchGrid.size());
			Collection<TerrainPatchComponent::ptr> &oldPatchGrid=mPatchGrid;
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
	TerrainPatchComponent::ptr patch=mUnactivePatches.back();mUnactivePatches.pop_back();
	setPatchAt(x,y,patch);

	patch->getParent()->setScale(mPatchScale);
	patch->getParent()->setTranslate(toWorldXi(x)-mPatchSize*mPatchScale.x/2,toWorldYi(y)-mPatchSize*mPatchScale.y/2,0);
	patch->setTolerance(mPatchTolerance);
	patch->setCameraUpdateScope(mPatchCameraUpdateScope);
	patch->setTerrainScope(mPatchTerrainScope);
	patch->setWaterScope(mPatchWaterScope);

	mDataSource->getPatchHeightData(&mPatchHeightData[0],x,y);
	patch->setHeightData(&mPatchHeightData[0],mPatchSize,mPatchSize,mPatchSize,true);
	mDataSource->getPatchLayerData(&mPatchLayerData[0],x,y);
	patch->setLayerData(&mPatchLayerData[0],mPatchSize,mPatchSize,mPatchSize);

	if(mMaterialSource!=NULL){
		patch->setMaterial(mMaterialSource->getMaterial(patch));
	}
	else{
		patch->setMaterial(mPatchMaterial);
	}
	patch->setWaterMaterial(mPatchWaterMaterial);

	attach(patch->getParent());
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

	patch->getParent()->updateWorldTransform();

	if(mListener!=NULL){
		mListener->terrainPatchCreated(x,y,patch->getParent()->getWorldBound());
	}
}

void TerrainNode::destroyPatch(int x,int y){
	TerrainPatchComponent::ptr patch=patchAt(x,y);

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

	remove(patch->getParent());

	if(mListener!=NULL){
		mListener->terrainPatchDestroyed(x,y,patch->getParent()->getWorldBound());
	}

	mUnactivePatches.add(patch);
	setPatchAt(x,y,NULL);
}

void TerrainNode::updateBound(){
	int i;
	for(i=0;i<mPatchGrid.size();++i){
		if(i==0) mBound->set(mPatchGrid[i]->getParent()->getWorldBound());
		else mBound->merge(mPatchGrid[i]->getParent()->getWorldBound(),mScene->getEpsilon());
	}
}

void TerrainNode::updatePatches(Camera *camera){
	int i;
	for(i=0;i<mPatchGrid.size();++i){
		mPatchGrid[i]->updateBlocks(camera);
	}

	for(i=0;i<mPatchGrid.size();++i){
		mPatchGrid[i]->updateVertexes();
	}
}

}
}
}
