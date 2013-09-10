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

#include <toadlet/peeper/VertexFormat.h>
#include <toadlet/tadpole/Engine.h>
#include <toadlet/tadpole/RenderableSet.h>
#include <toadlet/tadpole/Scene.h>
#include <toadlet/tadpole/bsp/BSP30Node.h>
#include <toadlet/tadpole/bsp/BSP30Streamer.h>

namespace toadlet{
namespace tadpole{
namespace bsp{

BSP30Node::BSP30Node(Scene *scene):PartitionNode(scene),
	mCounter(1)
{
	mLocalCamera=new Camera(NULL);
}

void BSP30Node::setMap(const String &name){
	Stream::ptr stream=mEngine->openStream(name);
	if(stream==NULL){
		Error::unknown("Unable to find level");
		return;
	}

	BSP30Streamer::ptr streamer=new BSP30Streamer(mEngine);
	BSP30Map::ptr map=shared_static_cast<BSP30Map>(streamer->load(stream,NULL,NULL));
	map->setName(name);
	setMap(map);
}

void BSP30Node::setMap(BSP30Map *map){
	tforeach(NodeCollection::iterator,node,mNodes){
		Collection<int> &indexes=((childdata*)node->getParentData())->leafs;
		removeNodeLeafIndexes(indexes,node);
	}

	mMap=map;

	mLeafData.resize(mMap->nleafs);
	mMarkedFaces=new uint8[(mMap->nfaces+7)>>3]; // Allocate enough markedfaces for 1 bit for each face
	mVisibleMaterialFaces.resize(map->miptexlump->nummiptex);

	tforeach(NodeCollection::iterator,node,mNodes){
		Collection<int> &indexes=((childdata*)node->getParentData())->leafs;
		findBoundLeafs(indexes,node);
		insertNodeLeafIndexes(indexes,node);
	}

	mBound->set(mMap->models[0].mins,mMap->models[0].maxs);
}

void BSP30Node::setSkyName(const String &skyName){
	mSkyName=skyName;

	if(skyName!=(char*)NULL){
		setSkyTextures(
			"sky/"+skyName+"dn.tga",
			"sky/"+skyName+"up.tga",
			"sky/"+skyName+"ft.tga",
			"sky/"+skyName+"bk.tga",
			"sky/"+skyName+"rt.tga",
			"sky/"+skyName+"lf.tga"
		);
	}
	else{
		if(mSkyMesh!=NULL){
			mSkyMesh->destroy();
			mSkyMesh=NULL;
		}
	}
}

void BSP30Node::setSkyTextures(const String &skyDown,const String &skyUp,const String &skyWest,const String &skyEast,const String &skySouth,const String &skyNorth){
	Log::debug(Categories::TOADLET_TADPOLE,"creating sky box");

	if(mSkyMesh!=NULL){
		mSkyMesh->destroy();
		mSkyMesh=NULL;
	}

	RenderState::ptr renderState=mEngine->getMaterialManager()->createRenderState();
	if(renderState!=NULL){
		renderState->setDepthState(DepthState(DepthState::DepthTest_LEQUAL,false));
		renderState->setMaterialState(MaterialState(false));
		renderState->setFogState(FogState());
	}

	Material::ptr down=mEngine->createSkyBoxMaterial(mEngine->getTextureManager()->findTexture(skyDown),true,renderState);
	Material::ptr up=mEngine->createSkyBoxMaterial(mEngine->getTextureManager()->findTexture(skyUp),true,renderState);
	Material::ptr front=mEngine->createSkyBoxMaterial(mEngine->getTextureManager()->findTexture(skyWest),true,renderState);
	Material::ptr back=mEngine->createSkyBoxMaterial(mEngine->getTextureManager()->findTexture(skyEast),true,renderState);
	Material::ptr right=mEngine->createSkyBoxMaterial(mEngine->getTextureManager()->findTexture(skySouth),true,renderState);
	Material::ptr left=mEngine->createSkyBoxMaterial(mEngine->getTextureManager()->findTexture(skyNorth),true,renderState);

	if(down!=NULL || up!=NULL || front!=NULL || back!=NULL || right!=NULL || left!=NULL){
		Mesh::ptr mesh=mEngine->createSkyBoxMesh(1024,false,false,down,up,front,back,right,left);
		mSkyMesh=new MeshComponent(mEngine);
		mSkyMesh->setMesh(mesh);
		mScene->getBackground()->attach(mSkyMesh);
	}
}

void BSP30Node::nodeAttached(Node *node){
	Node::nodeAttached(node);

	node->setParentData(new childdata());

	if(mMap!=NULL){
		Collection<int> &newIndexes=((childdata*)node->getParentData())->leafs;

		findBoundLeafs(newIndexes,node);
		insertNodeLeafIndexes(newIndexes,node);
	}
}

void BSP30Node::nodeRemoved(Node *node){
	Node::nodeRemoved(node);

	if(mMap!=NULL && node->getParentData()!=NULL){
		removeNodeLeafIndexes(((childdata*)node->getParentData())->leafs,node);
	}

	delete (childdata*)node->getParentData();
	node->setParentData(NULL);
}

void BSP30Node::insertNodeLeafIndexes(const Collection<int> &indexes,Node *node){
	int i;
	for(i=indexes.size()-1;i>=0;--i){
		if(indexes[i]<0){
			mGlobalLeafData.occupants.add(node);
		}
		else{
			mLeafData[indexes[i]].occupants.add(node);
		}
	}
}

void BSP30Node::removeNodeLeafIndexes(const Collection<int> &indexes,Node *node){
	int i;
	for(i=indexes.size()-1;i>=0;--i){
		if(indexes[i]<0){
			mGlobalLeafData.occupants.remove(node);
		}
		else{
			mLeafData[indexes[i]].occupants.remove(node);
		}
	}
}

void BSP30Node::gatherRenderables(Camera *camera,RenderableSet *set){
	TOADLET_PROFILE_AUTOSCOPE();

	int i,j;

	if(mMap==NULL){
		Node::gatherRenderables(camera,set);
		return;
	}

	set->queueRenderable(this);

	Matrix4x4 matrix;
	mWorldTransform->inverseTransform(matrix,camera->getWorldMatrix());
	mLocalCamera->setWorldMatrix(matrix);
	mLocalCamera->setProjectionMatrix(camera->getProjectionMatrix());

	// Queue up the visible faces and nodes
	memset(mMarkedFaces,0,(mMap->nfaces+7)>>3);
	memset(&mVisibleMaterialFaces[0],0,sizeof(BSP30Map::facedata*)*mVisibleMaterialFaces.size());
	int leaf=mMap->findPointLeaf(mMap->planes,mMap->nodes,sizeof(bnode),0,mLocalCamera->getPosition());
	if(leaf==0 || mMap->nvisibility==0){
		bmodel *world=&mMap->models[0];
		// Instead of enumerating all leaves, we just enumerate all faces
		//  Otherwise, the changing of facedata->next that aren't in the world model can mess up other node's rendering
		for(i=0;i<world->numfaces;i++){
			int faceIndex=world->firstface+i;
			bface *face=&mMap->faces[faceIndex];
			BSP30Map::facedata *facedata=&mMap->facedatas[faceIndex];
			btexinfo *texinfo=&mMap->texinfos[face->texinfo];
			facedata->next=mVisibleMaterialFaces[texinfo->miptex];
			mVisibleMaterialFaces[texinfo->miptex]=facedata;
			mMap->updateFaceLights(faceIndex);
		}

		tforeach(NodeCollection::iterator,node,mNodes){
			if((camera->getScope()&node->getScope())!=0 && camera->culled(node->getWorldBound())==false){
				node->gatherRenderables(camera,set);
			}
		}
	}
	else{
		mCounter++;
		const Collection<int> &leafvis=mMap->parsedVisibility[leaf];

		for(i=0;i<leafvis.size();i++){
			bleaf *leaf=mMap->leafs+leafvis[i];
			AABox box(leaf->mins[0],leaf->mins[1],leaf->mins[2],leaf->maxs[0],leaf->maxs[1],leaf->maxs[2]);
			if(mLocalCamera->culled(box)==false){
				addLeafToVisible(leaf,mLocalCamera->getPosition());

				const Collection<Node*> &occupants=mLeafData[leafvis[i]].occupants;
				for(j=0;j<occupants.size();++j){
					Node *node=occupants[j];
					childdata *data=(childdata*)node->getParentData();
					if(data->counter!=mCounter){
						data->counter=mCounter;
						if((camera->getScope()&node->getScope())!=0 && camera->culled(node->getWorldBound())==false){
							node->gatherRenderables(camera,set);
						}
					}
				}
			}
		}

		const Collection<Node*> &occupants=mGlobalLeafData.occupants;
		for(j=0;j<occupants.size();++j){
			Node *node=occupants[j];
			if((camera->getScope()&node->getScope())!=0 && camera->culled(node->getWorldBound())==false){
				node->gatherRenderables(camera,set);
			}
		}
	}

	for(i=0;i<mMap->lightmapTextures.size();++i){
		if(mMap->lightmapDirties[i]){
			Log::excess(Categories::TOADLET_TADPOLE_BSP,String("reloading lightmap:")+i);
			mMap->lightmapDirties[i]=false;
			mEngine->getTextureManager()->textureLoad(mMap->lightmapTextures[i],mMap->lightmapFormat,mMap->lightmapDatas[i]);
		}
	}
}

bool BSP30Node::senseBoundingVolumes(SensorResultsListener *listener,Bound *bound){
	if(mMap==NULL){
		return false;
	}

	bool result=false;
	int i,j;
	mCounter++;
	Collection<int> &newIndexes=mLeafIndexes; 
	newIndexes.clear();
	const AABox &box=bound->getAABox();
	Vector3 origin;
	Math::sub(origin,box.maxs,box.mins);
	Math::mul(origin,Math::HALF);
	Math::add(origin,box.mins);

	mMap->findBoundLeafs(newIndexes,mMap->nodes,0,box);

	for(i=0;i<newIndexes.size();i++){
		int index=newIndexes[i];
		leafdata *data=&mLeafData[index];
		const Collection<Node*> &occupants=data->occupants;
		for(j=0;j<occupants.size();++j){
			Node *occupant=occupants[j];
			childdata *data=(childdata*)occupant->getParentData();
			if(data->counter!=mCounter && occupant->getWorldBound()->testIntersection(bound)){
				data->counter=mCounter;
				result|=true;
				if(listener->resultFound(occupant,Math::lengthSquared(origin,occupant->getWorldTranslate()))==false){
					return true;
				}
			}
		}
	}

	const Collection<Node*> &occupants=mGlobalLeafData.occupants;
	for(j=0;j<occupants.size();++j){
		Node *occupant=occupants[j];
		childdata *data=(childdata*)occupant->getParentData();
		if(data->counter!=mCounter && occupant->getWorldBound()->testIntersection(bound)){
			data->counter=mCounter;
			result|=true;
			if(listener->resultFound(occupant,Math::lengthSquared(origin,occupant->getWorldTranslate()))==false){
				return true;
			}
		}
	}

	return result;
}

bool BSP30Node::sensePotentiallyVisible(SensorResultsListener *listener,const Vector3 &point){
	if(mMap==NULL){
		return false;
	}

	int i,j;
	bool result=false;
	mCounter++;

	// Queue up the visible faces and nodes
	memset(mMarkedFaces,0,(mMap->nfaces+7)>>3);
	memset(&mVisibleMaterialFaces[0],0,sizeof(BSP30Map::facedata*)*mVisibleMaterialFaces.size());
	int leaf=mMap->findPointLeaf(mMap->planes,mMap->nodes,sizeof(bnode),0,point);
	if(leaf==0 || mMap->nvisibility==0){
		tforeach(NodeCollection::iterator,node,mNodes){
			result|=true;
			if(listener->resultFound(node,Math::lengthSquared(point,node->getWorldTranslate()))==false){
				return true;
			}
		}
	}
	else{
		mCounter++;
		const Collection<int> &leafvis=mMap->parsedVisibility[leaf];
		for(i=0;i<leafvis.size();i++){
			const Collection<Node*> &occupants=mLeafData[leafvis[i]].occupants;
			for(j=0;j<occupants.size();++j){
				Node *occupant=occupants[j];
				childdata *data=(childdata*)occupant->getParentData();
				if(data->counter!=mCounter){
					data->counter=mCounter;
					result|=true;
					if(listener->resultFound(occupant,Math::lengthSquared(point,occupant->getWorldTranslate()))==false){
						return true;
					}
				}
			}
		}

		const Collection<Node*> &occupants=mGlobalLeafData.occupants;
		for(j=0;j<occupants.size();++j){
			Node *occupant=occupants[j];
			result|=true;
			if(listener->resultFound(occupant,Math::lengthSquared(point,occupant->getWorldTranslate()))==false){
				return true;
			}
		}
	}

	return result;
}

bool BSP30Node::findAmbientForBound(Vector4 &r,Bound *bound){
	Vector3 start=bound->getSphere().origin;
	Vector3 end=start;
	end.z-=1024;
	scalar threshold=512;
	if(mMap!=NULL && bound->getSphere().radius<threshold){
		return mMap->modelLightTrace(r,0,start,end);
	}
	return false;
}

void BSP30Node::traceSegment(PhysicsCollision &result,const Vector3 &position,const Segment &segment,const Vector3 &size){
	Segment localSegment=segment;
	Transform transform;

	bool transformed=true;//(getWorldTransform()!=Node::identityTransform());
	if(transformed){
		transform.set(position,mWorldTransform->getScale(),mWorldTransform->getRotate());
		transform.inverseTransform(localSegment);
	}

	result.time=Math::ONE;
	localSegment.getEndPoint(result.point);
	if(mMap!=NULL){
		int contents=mMap->modelCollisionTrace(result,0,size,localSegment.origin,result.point);
		if(contents!=CONTENTS_EMPTY){
			result.scope|=(-1-contents)<<1;
		}
	}

	if(transformed && result.time<Math::ONE){
		transform.transform(result.point);
		transform.transformNormal(result.normal);
	}
}

void BSP30Node::render(RenderManager *manager) const{
	int i;
	for(i=0;i<mVisibleMaterialFaces.size();i++){
		if(mVisibleMaterialFaces[i]!=NULL){
			RenderPath *path=mMap->materials[i]->getBestPath();
			tforeach(RenderPath::PassCollection::iterator,pass,path->getPasses()){
				manager->setupPass(pass,manager->getDevice());
				manager->setupPassForRenderable(pass,manager->getDevice(),(Renderable*)this,Math::ZERO_VECTOR4);
				mMap->renderFaces(manager->getDevice(),mVisibleMaterialFaces[i]);
			}
		}
	}
}

void BSP30Node::nodeBoundChanged(Node *child){
	Node::nodeBoundChanged(child);

	if(mMap==NULL || child->getParentData()==NULL){
		return;
	}

	Collection<int> &oldIndexes=((childdata*)child->getParentData())->leafs;
	Collection<int> &newIndexes=mLeafIndexes; 
	newIndexes.clear();
	findBoundLeafs(newIndexes,child);

	if(newIndexes.size()==oldIndexes.size()){
		int i;
		for(i=oldIndexes.size()-1;i>=0;--i){
			if(oldIndexes[i]!=newIndexes[i]) break;
		}
		if(i==-1) return; // No changes necessary
	}

	removeNodeLeafIndexes(oldIndexes,child);
	insertNodeLeafIndexes(newIndexes,child);

	oldIndexes.resize(newIndexes.size());
	memcpy(&oldIndexes[0],&newIndexes[0],newIndexes.size()*sizeof(int));
}

void BSP30Node::addLeafToVisible(bleaf *leaf,const Vector3 &cameraPosition){
	const bmarksurface *p=&mMap->marksurfaces[leaf->firstmarksurface];

	for(int x=0;x<leaf->nummarksurfaces;x++){
		int faceIndex=*p++;

		// Check the proper markedfaces bit
		if(!(mMarkedFaces[faceIndex>>3]&(1<<(faceIndex&7)))){
			bface *face=&mMap->faces[faceIndex];

			float d=Math::length(*(Plane*)&mMap->planes[face->planenum],cameraPosition);
			if(face->side){
				if(d>0) continue;
			}
			else{
				if(d<0) continue;
			}

			mMarkedFaces[faceIndex>>3]|=(1<<(faceIndex&7));

			btexinfo *texinfo=&mMap->texinfos[face->texinfo];

			if((texinfo->flags&TEX_SPECIAL)==0){
				BSP30Map::facedata *facedata=&mMap->facedatas[faceIndex];
				facedata->next=mVisibleMaterialFaces[texinfo->miptex];
				mVisibleMaterialFaces[texinfo->miptex]=facedata;
				mMap->updateFaceLights(faceIndex);
			}
		}
	}
}

void BSP30Node::findBoundLeafs(egg::Collection<int> &leafs,Node *node){
	Bound *bound=node->getWorldBound();

	const AABox &box=bound->getAABox();
	// If the radius is infinite or greater than our threshold, assume its global
	scalar threshold=512;
	if(bound->getType()==Bound::Type_INFINITE || box.maxs.x-box.mins.x>=threshold || box.maxs.y-box.mins.y>=threshold || box.maxs.z-box.mins.z>=threshold){
		leafs.add(-1);
	}
	else{
		mMap->findBoundLeafs(leafs,mMap->nodes,0,box);
	}
}

}
}
}
