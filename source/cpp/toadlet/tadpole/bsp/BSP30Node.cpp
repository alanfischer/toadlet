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
#include <toadlet/tadpole/bsp/BSP30Handler.h>

namespace toadlet{
namespace tadpole{
namespace bsp{

BSP30ModelNode::SubModel::SubModel(BSP30ModelNode *modelNode,BSP30Map *map){
	this->modelNode=modelNode;
	this->map=map;
	this->faces=NULL;
}

void BSP30ModelNode::SubModel::render(SceneRenderer *renderer) const{
	map->renderFaces(renderer->getDevice(),faces);
}

TOADLET_NODE_IMPLEMENT(BSP30ModelNode,Categories::TOADLET_TADPOLE_BSP+".BSP30ModelNode");

BSP30ModelNode::BSP30ModelNode():Node(),
	//mMap,
	mModelIndex(0),
	//mSubModels,
	//mOwnedMaterial,
	mRendered(false)
{}

BSP30ModelNode::~BSP30ModelNode(){}

Node *BSP30ModelNode::create(Scene *scene){
	super::create(scene);

	mModelIndex=-1;
	mRendered=true;

	return this;
}

void BSP30ModelNode::destroy(){
	if(mOwnedMaterial!=NULL){
		mOwnedMaterial->release();
		mOwnedMaterial=NULL;
	}

	super::destroy();
}

Node *BSP30ModelNode::set(Node *node){
	super::set(node);

	BSP30ModelNode *modelNode=(BSP30ModelNode*)node;
	setModel(modelNode->getMap(),modelNode->getModel());

	return this;
}

void *BSP30ModelNode::hasInterface(int type){
	switch(type){
		case InterfaceType_TRACEABLE:
			return (Traceable*)this;
		case InterfaceType_VISIBLE:
			return (Visible*)this;
		default:
			return NULL;
	}
}

void BSP30ModelNode::setModel(BSP30Map::ptr map,const String &name){
	// BSP Names are in the form of *X where X is an integer corresponding to the model index
	int index=-1;
	if(name.length()>0 && name[0]=='*'){
		index=name.substr(1,name.length()).toInt32();
	}
	setModel(map,index);
}

void BSP30ModelNode::setModel(BSP30Map::ptr map,int index){
	if(map==NULL || index<0 || index>=map->nmodels){
		Error::invalidParameters(Categories::TOADLET_TADPOLE,
			"invalid Model");
		return;
	}

	if(mOwnedMaterial!=NULL){
		mOwnedMaterial->release();
		mOwnedMaterial=NULL;
	}

	mMap=map;
	mModelIndex=index;
	mSubModels.clear();

	bmodel *model=&mMap->models[mModelIndex];

	mBound.set(model->mins,model->maxs);
	int i,j;
	for(i=0;i<model->numfaces;++i){
		bface *face=&map->faces[model->firstface+i];
		BSP30Map::facedata *facedata=&map->facedatas[model->firstface+i];
		int miptex=map->texinfos[face->texinfo].miptex;
		for(j=mSubModels.size()-1;j>=0;--j){
			if(mSubModels[j]->material==map->materials[miptex]) break;
		}
		if(j<0){
			SubModel::ptr subModel(new SubModel(this,map));
			subModel->material=map->materials[miptex];
			facedata->next=NULL;
			subModel->faces=facedata;
			mSubModels.add(subModel);
		}
		else{
			facedata->next=mSubModels[j]->faces;
			mSubModels[j]->faces=facedata;
		}
	}
}

RenderState::ptr BSP30ModelNode::getSharedRenderState(){
	if(mOwnedMaterial==NULL && mSubModels.size()>0){
		mOwnedMaterial=mSubModels[0]->material=mEngine->getMaterialManager()->createMaterial(mSubModels[0]->material);
		mSubModels[0]->material->setSort(Material::SortType_AUTO);
		int i;
		for(i=1;i<mSubModels.size();++i){
			mSubModels[i]->material=mEngine->getMaterialManager()->cloneMaterial(mSubModels[i]->material);
			mSubModels[i]->material->shareStates(mOwnedMaterial);
			mSubModels[i]->material->setSort(Material::SortType_AUTO);
		}
		mOwnedMaterial->retain();
	}

	return mOwnedMaterial!=NULL?mOwnedMaterial->getPass()->getRenderState():NULL;
}

void BSP30ModelNode::showPlanarFaces(int plane){
	int i;
	for(i=0;i<mSubModels.size();++i){
		BSP30Map::facedata *face=mSubModels[i]->faces;
		while(face!=NULL){
			if(mMap->planes[mMap->faces[face->index].planenum].type!=plane){
				face->visible=false;
			}
			else{
				face->visible=true;
			}
			face=face->next;
		}
	}
}

void BSP30ModelNode::gatherRenderables(CameraNode *camera,RenderableSet *set){
	super::gatherRenderables(camera,set);

	if(mRendered==false){
		return;
	}

	int i;
	bmodel *model=&mMap->models[mModelIndex];
	for(i=0;i<model->numfaces;++i){
		mMap->updateFaceLights(model->firstface+i);
	}
	for(i=0;i<mSubModels.size();++i){
		set->queueRenderable(mSubModels[i]);
	}
}

void BSP30ModelNode::traceSegment(Collision &result,const Vector3 &position,const Segment &segment,const Vector3 &size){
	Segment localSegment;
	Transform transform;

	transform.set(position,mWorldTransform.getScale(),mWorldTransform.getRotate());
	transform.inverseTransform(localSegment,segment);

	result.time=Math::ONE;
	localSegment.getEndPoint(result.point);
	if(mMap!=NULL){
		int contents=mMap->modelCollisionTrace(result,mModelIndex,size,localSegment.origin,result.point);
		if(contents!=CONTENTS_EMPTY){
			result.scope|=(-1-contents)<<1;
		}
	}

	if(result.time<Math::ONE){
		transform.transform(result.point);
		transform.transformNormal(result.normal);
	}
}

TOADLET_NODE_IMPLEMENT(BSP30Node,Categories::TOADLET_TADPOLE_NODE+".BSP30Node");

BSP30Node::BSP30Node():super(),
	mCounter(1)
{
}

BSP30Node::~BSP30Node(){}

Node *BSP30Node::set(Node *node){
	super::set(node);

	BSP30Node *bspNode=(BSP30Node*)this;
	setMap(bspNode->getMap());
	setSkyName(bspNode->getSkyName());

	return this;
}

void BSP30Node::setMap(const String &name){
	Stream::ptr stream=mEngine->openStream(name);
	if(stream==NULL){
		Error::unknown("Unable to find level");
		return;
	}

	BSP30Handler::ptr handler(new BSP30Handler(mEngine));
	BSP30Map::ptr map=shared_static_cast<BSP30Map>(handler->load(stream,NULL,NULL));
	map->setName(name);
	setMap(map);
}

void BSP30Node::setMap(BSP30Map::ptr map){
	Node *node;

	for(node=getFirstChild();node!=NULL;node=node->getNext()){
		Collection<int> &indexes=((childdata*)node->getParentData())->leafs;
		removeNodeLeafIndexes(indexes,node);
	}

	mMap=map;

	mLeafData.resize(mMap->nleafs);
	mMarkedFaces=new uint8[(mMap->nfaces+7)>>3]; // Allocate enough markedfaces for 1 bit for each face
	mVisibleMaterialFaces.resize(map->miptexlump->nummiptex);

	for(node=getFirstChild();node!=NULL;node=node->getNext()){
		Collection<int> &indexes=((childdata*)node->getParentData())->leafs;
		findBoundLeafs(indexes,node);
		insertNodeLeafIndexes(indexes,node);
	}
}

void BSP30Node::setSkyName(const String &skyName){
	mSkyName=skyName;

	setSkyTextures(
		skyName+"dn.tga",
		skyName+"up.tga",
		skyName+"ft.tga",
		skyName+"bk.tga",
		skyName+"rt.tga",
		skyName+"lf.tga"
	);
}

void BSP30Node::setSkyTextures(const String &skyDown,const String &skyUp,const String &skyWest,const String &skyEast,const String &skySouth,const String &skyNorth){
	Logger::debug(Categories::TOADLET_TADPOLE,"Creating sky box");

	if(mSkyNode!=NULL){
		mSkyNode->destroy();
		mSkyNode=NULL;
	}

	Material::ptr down=mEngine->getMaterialManager()->findMaterial(skyDown);
	Material::ptr up=mEngine->getMaterialManager()->findMaterial(skyUp);
	Material::ptr front=mEngine->getMaterialManager()->findMaterial(skyWest);
	Material::ptr back=mEngine->getMaterialManager()->findMaterial(skyEast);
	Material::ptr right=mEngine->getMaterialManager()->findMaterial(skySouth);
	Material::ptr left=mEngine->getMaterialManager()->findMaterial(skyNorth);

	if(down!=NULL || up!=NULL || front!=NULL || back!=NULL || right!=NULL || left!=NULL){
		Mesh::ptr mesh=mEngine->getMeshManager()->createSkyBoxMesh(1024,false,false,down,up,front,back,right,left);
		int i;
		for(i=0;i<mesh->getNumSubMeshes();++i){
			Material *material=mesh->getSubMesh(i)->material;
			if(material!=NULL){
				material->getPass()->setDepthState(DepthState(DepthState::DepthTest_LEQUAL,false));
				material->getPass()->setMaterialState(MaterialState(false,false,MaterialState::ShadeType_FLAT));
				SamplerState samplerState;
				samplerState.uAddress=SamplerState::AddressType_CLAMP_TO_EDGE;
				samplerState.vAddress=SamplerState::AddressType_CLAMP_TO_EDGE;
				material->getPass()->setSamplerState(0,samplerState);
			}
		}

		mSkyNode=mEngine->createNodeType(node::MeshNode::type(),getScene());
		mSkyNode->setMesh(mesh);
		mScene->getBackground()->attach(mSkyNode);
	}
}

void BSP30Node::nodeAttached(Node *node){
	super::nodeAttached(node);

	node->parentDataChanged(new childdata());

	if(mMap!=NULL){
		Collection<int> &newIndexes=((childdata*)node->getParentData())->leafs;
		/// @todo: Is this initial placement necessary, or will this get taken care of by the mergeWorldBounds at the end of the node attachment?
		findBoundLeafs(newIndexes,node);
		insertNodeLeafIndexes(newIndexes,node);
	}
}

void BSP30Node::nodeRemoved(Node *node){
	super::nodeRemoved(node);

	if(mMap!=NULL){
		removeNodeLeafIndexes(((childdata*)node->getParentData())->leafs,node);
	}

	delete (childdata*)node->getParentData();
	node->parentDataChanged(NULL);
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

void BSP30Node::mergeWorldBound(Node *child,bool justAttached){
	super::mergeWorldBound(child,justAttached);
	if(justAttached || child->getTransformUpdated()){
		childTransformUpdated(child);
	}
}

void BSP30Node::gatherRenderables(CameraNode *camera,RenderableSet *set){
	int i,j;

	if(mMap==NULL){
		super::gatherRenderables(camera,set);
		return;
	}

	set->queueRenderable(this);

	// Queue up the visible faces and nodes
	memset(mMarkedFaces,0,(mMap->nfaces+7)>>3);
	memset(&mVisibleMaterialFaces[0],0,sizeof(BSP30Map::facedata*)*mVisibleMaterialFaces.size());
	int leaf=mMap->findPointLeaf(mMap->planes,mMap->nodes,sizeof(bnode),0,camera->getWorldTranslate());
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

		Node *node;
		for(node=getFirstChild();node!=NULL;node=node->getNext()){
			if(camera->culled(node)==false){
				node->gatherRenderables(camera,set);
			}
		}
	}
	else{
		for(i=0;i<mMap->parsedVisibility[leaf].size();i++){
			addLeafToVisible(&mMap->leafs[mMap->parsedVisibility[leaf][i]],camera);
		}

		mCounter++;
		const Collection<int> &leafvis=mMap->parsedVisibility[leaf];
		for(i=0;i<leafvis.size();i++){
			bleaf *leaf=mMap->leafs+leafvis[i];
			AABox box(leaf->mins[0],leaf->mins[1],leaf->mins[2],leaf->maxs[0],leaf->maxs[1],leaf->maxs[2]);
			if(camera->culled(box)==false){
				const Collection<Node*> &occupants=mLeafData[leafvis[i]].occupants;
				for(j=0;j<occupants.size();++j){
					Node *occupant=occupants[j];
					childdata *data=(childdata*)occupant->getParentData();
					if(data->counter!=mCounter){
						data->counter=mCounter;
						if(camera->culled(occupant)==false){
							occupant->gatherRenderables(camera,set);
						}
					}
				}
			}
		}

		const Collection<Node*> &occupants=mGlobalLeafData.occupants;
		for(j=0;j<occupants.size();++j){
			Node *occupant=occupants[j];
			if(camera->culled(occupant)==false){
				occupant->gatherRenderables(camera,set);
			}
		}
	}

	for(i=0;i<mMap->lightmapTextures.size();++i){
		if(mMap->lightmapDirties[i]){
			Logger::excess(Categories::TOADLET_TADPOLE_BSP,String("reloading lightmap:")+i);
			mMap->lightmapDirties[i]=false;
			mEngine->getTextureManager()->textureLoad(mMap->lightmapTextures[i],mMap->lightmapFormat,mMap->lightmapDatas[i]);
		}
	}
}

bool BSP30Node::senseBoundingVolumes(SensorResultsListener *listener,const Bound &bound){
	if(mMap==NULL){
		return false;
	}

	bool result=false;
	int i,j;
	mCounter++;
	Collection<int> &newIndexes=mLeafIndexes; 
	newIndexes.clear();
	const AABox &box=bound.getAABox();
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
			if(data->counter!=mCounter && occupant->getWorldBound().testIntersection(bound)){
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
		if(data->counter!=mCounter && occupant->getWorldBound().testIntersection(bound)){
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

	bool result=false;
	Node *node;
	int i,j;
	mCounter++;

	// Queue up the visible faces and nodes
	memset(mMarkedFaces,0,(mMap->nfaces+7)>>3);
	memset(&mVisibleMaterialFaces[0],0,sizeof(BSP30Map::facedata*)*mVisibleMaterialFaces.size());
	int leaf=mMap->findPointLeaf(mMap->planes,mMap->nodes,sizeof(bnode),0,point);
	if(leaf==0 || mMap->nvisibility==0){
		for(node=getFirstChild();node!=NULL;node=node->getNext()){
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

	return true;
}

bool BSP30Node::findAmbientForPoint(Vector4 &r,const Vector3 &point){
	Vector3 end;
	end.x=point.x;
	end.y=point.y;
	end.z=point.z-1024;
	if(mMap!=NULL){
		return mMap->modelLightTrace(r,0,point,end);
	}
	return false;
}

void BSP30Node::traceSegment(Collision &result,const Vector3 &position,const Segment &segment,const Vector3 &size){
	result.time=Math::ONE;
	segment.getEndPoint(result.point);
	if(mMap!=NULL){
		int contents=mMap->modelCollisionTrace(result,0,size,segment.origin,result.point);
		if(contents!=CONTENTS_EMPTY) result.scope|=(-1-contents)<<1;
	}
}

void BSP30Node::render(SceneRenderer *renderer) const{
	int i,j;
	for(i=0;i<mVisibleMaterialFaces.size();i++){
		RenderPath *path=mMap->materials[i]->getBestPath();
		for(j=0;j<path->getNumPasses();++j){
			renderer->setupPass(path->getPass(j));
			mMap->renderFaces(renderer->getDevice(),mVisibleMaterialFaces[i]);
		}
	}
}

void BSP30Node::childTransformUpdated(Node *child){
	if(mMap==NULL){
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

void BSP30Node::addLeafToVisible(bleaf *leaf,CameraNode *camera){
	AABox bound(leaf->mins[0],leaf->mins[1],leaf->mins[2],leaf->maxs[0],leaf->maxs[1],leaf->maxs[2]);
	if(camera->culled(bound)==false){
		const bmarksurface *p=&mMap->marksurfaces[leaf->firstmarksurface];

		for(int x=0;x<leaf->nummarksurfaces;x++){
			int faceIndex=*p++;

			// Check the proper markedfaces bit
			if(!(mMarkedFaces[faceIndex>>3]&(1<<(faceIndex&7)))){
				bface *face=&mMap->faces[faceIndex];

				float d=Math::length(*(Plane*)&mMap->planes[face->planenum],camera->getWorldTranslate());
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
}

void BSP30Node::findBoundLeafs(egg::Collection<int> &leafs,Node *node){
	const AABox &box=node->getWorldBound().getAABox();
	// If the radius is infinite or greater than our threshold, assume its global
	scalar threshold=512;
	if(box.maxs.x-box.mins.x>=threshold || box.maxs.y-box.mins.y>=threshold || box.maxs.z-box.mins.z>=threshold){
		leafs.add(-1);
	}
	else{
		mMap->findBoundLeafs(leafs,mMap->nodes,0,box);
	}
}

}
}
}