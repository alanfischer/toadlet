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

#include <toadlet/egg/EndianConversion.h>
#include <toadlet/peeper/VertexFormat.h>
#include <toadlet/tadpole/BoundingVolumeSensor.h>
#include <toadlet/tadpole/Engine.h>
#include <toadlet/tadpole/PixelPacker.h>
#include <toadlet/tadpole/RenderQueue.h>
#include <toadlet/tadpole/Scene.h>
#include <toadlet/tadpole/bsp/BSP30Node.h>
#include <toadlet/tadpole/bsp/BSP30Handler.h>
#include <toadlet/tadpole/node/MeshNode.h>
#include <toadlet/tadpole/handler/WADArchive.h>
#include <string.h> // memset

using namespace toadlet::egg;
using namespace toadlet::egg::io;
using namespace toadlet::egg::image;
using namespace toadlet::peeper;
using namespace toadlet::tadpole::mesh;
using namespace toadlet::tadpole::node;
using namespace toadlet::tadpole::handler;

namespace toadlet{
namespace tadpole{
namespace bsp{

BSP30ModelNode::SubModel::SubModel(BSP30ModelNode *modelNode,BSP30Map *map){
	this->modelNode=modelNode;
	this->map=map;
	this->faces=NULL;
}

void BSP30ModelNode::SubModel::render(peeper::Renderer *renderer) const{
	BSP30Map::facedata *face=faces;
	while(face!=NULL){
		if(face->visible){
			renderer->renderPrimitive(map->vertexData,face->indexData);
		}
		face=face->next;
	}
}

TOADLET_NODE_IMPLEMENT(BSP30ModelNode,Categories::TOADLET_TADPOLE_BSP+".BSP30ModelNode");

BSP30ModelNode::BSP30ModelNode():Node(),
	//mMap,
	mModelIndex(-1),
	mVisible(true)
{}

BSP30ModelNode::~BSP30ModelNode(){
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

	mMap=map;
	mModelIndex=index;
	mSubModels.clear();

	bmodel *model=&mMap->models[mModelIndex];

	set(mLocalBound,AABox(model->mins,model->maxs));

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

void BSP30ModelNode::queueRenderables(CameraNode *camera,RenderQueue *queue){
	super::queueRenderables(camera,queue);

	if(mVisible && queue!=NULL){
		int i;
		for(i=0;i<mSubModels.size();++i){
			queue->queueRenderable(mSubModels[i]);
		}
	}
}

void BSP30ModelNode::traceSegment(Collision &result,const Segment &segment,const Vector3 &size){
	Segment localSegment;
	Quaternion invrot; Math::invert(invrot,getWorldRotate());
	Math::sub(localSegment.origin,segment.origin,getWorldTranslate());
	Math::mul(localSegment.origin,invrot);
	Math::mul(localSegment.direction,invrot,segment.direction);

	result.time=Math::ONE;
	localSegment.getEndPoint(result.point);
	if(mMap!=NULL){
		int contents=mMap->modelTrace(result,mModelIndex,size,localSegment.origin,result.point);
		if(contents!=CONTENTS_EMPTY){
			result.scope|=(-1-contents)<<1;
		}
	}

	Math::mul(result.normal,getWorldRotate());
	Math::mul(result.point,getWorldRotate());
	Math::add(result.point,getWorldTranslate());
}

TOADLET_NODE_IMPLEMENT(BSP30Node,Categories::TOADLET_TADPOLE_NODE+".BSP30Node");

BSP30Node::BSP30Node():super(),
	mCounter(1)
{}

BSP30Node::~BSP30Node(){}

void BSP30Node::setMap(const String &name){
	Stream::ptr stream=mEngine->openStream(name);
	if(stream==NULL){
		Error::unknown("Unable to find level");
		return;
	}

	BSP30Handler::ptr handler(new BSP30Handler(mEngine));
	BSP30Map::ptr map=shared_static_cast<BSP30Map>(handler->load(stream,NULL));
	map->setName(name);
	setMap(map);
}

void BSP30Node::setMap(BSP30Map::ptr map){
	int i;

	for(i=0;i<mChildren.size();++i){
		Node *node=mChildren[i];
		Collection<int> &indexes=((childdata*)node->getParentData())->leafs;
		removeNodeLeafIndexes(indexes,node);
	}

	mMap=map;

	mLeafData.resize(mMap->nleafs);
	mMarkedFaces=new uint8[(mMap->nfaces+7)>>3]; // Allocate enough markedfaces for 1 bit for each face
	mVisibleMaterialFaces.resize(map->miptexlump->nummiptex);

	for(i=0;i<mChildren.size();++i){
		Node *node=mChildren[i];
		Collection<int> &indexes=((childdata*)node->getParentData())->leafs;
		mMap->findBoundLeafs(indexes,mMap->nodes,0,node->getWorldBound());
		insertNodeLeafIndexes(indexes,node);
	}

	Logger::debug(Categories::TOADLET_TADPOLE,"Creating skybox");

	// START: Needs to be removed, or a map/member setting, so skyboxes could be added outside of this class
	Texture::ptr bottom=mEngine->getTextureManager()->findTexture("nightsky/nightsky_down.png");
	Texture::ptr top=mEngine->getTextureManager()->findTexture("nightsky/nightsky_up.png");
	Texture::ptr left=mEngine->getTextureManager()->findTexture("nightsky/nightsky_west.png");
	Texture::ptr right=mEngine->getTextureManager()->findTexture("nightsky/nightsky_east.png");
	Texture::ptr back=mEngine->getTextureManager()->findTexture("nightsky/nightsky_south.png");
	Texture::ptr front=mEngine->getTextureManager()->findTexture("nightsky/nightsky_north.png");

	Mesh::ptr mesh=mEngine->getMeshManager()->createSkyBox(1024,false,false,bottom,top,left,right,back,front);
	for(i=0;i<mesh->subMeshes.size();++i){
		mesh->subMeshes[i]->material->setLayer(-1);
	}

	node::MeshNode::ptr node=mEngine->createNodeType(node::MeshNode::type(),getScene());
	node->setMesh(mesh);
	mScene->getBackground()->attach(node);
}

void BSP30Node::nodeAttached(Node *node){
	super::nodeAttached(node);

	node->parentDataChanged(new childdata());

	if(mMap!=NULL){
		Collection<int> &newIndexes=((childdata*)node->getParentData())->leafs;
		mMap->findBoundLeafs(newIndexes,mMap->nodes,0,node->getWorldBound());
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
		mLeafData[indexes[i]].occupants.add(node);
	}
}

void BSP30Node::removeNodeLeafIndexes(const Collection<int> &indexes,Node *node){
	int i;
	for(i=indexes.size()-1;i>=0;--i){
		mLeafData[indexes[i]].occupants.remove(node);
	}
}

void BSP30Node::mergeWorldBound(Node *child,bool justAttached){
	super::mergeWorldBound(child,justAttached);
	if(justAttached || child->getTransformUpdated()){
		childTransformUpdated(child);
	}
}

void BSP30Node::queueRenderables(CameraNode *camera,RenderQueue *queue){
	int i,j;

	if(mMap==NULL){
		super::queueRenderables(camera,queue);
		return;
	}

	queue->queueRenderable(this);

	// Queue up the visible faces and nodes
	memset(mMarkedFaces,0,(mMap->nfaces+7)>>3);
	memset(&mVisibleMaterialFaces[0],0,sizeof(BSP30Map::facedata*)*mVisibleMaterialFaces.size());
	int leaf=mMap->findPointLeaf(mMap->planes,mMap->nodes,sizeof(bnode),0,camera->getWorldTranslate());
	if(leaf==0 || mMap->nvisibility==0){
		for(i=0;i<mMap->nleafs;i++){
			addLeafToVisible(&mMap->leafs[i],camera);
		}

		for(i=0;i<mChildren.size();++i){
			super::queueRenderables(mChildren[i],camera,queue);
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
						if(culled(occupant,camera)==false){
							super::queueRenderables(occupant,camera,queue);
						}
					}
				}
			}
		}
	}
}

bool BSP30Node::senseBoundingVolumes(SensorResultsListener *listener,const Sphere &volume){
	if(mMap==NULL){
		return false;
	}

	mCounter++;
	Collection<int> &newIndexes=mLeafIndexes; 
	newIndexes.clear();
	mMap->findBoundLeafs(newIndexes,mMap->nodes,0,volume);
	for(int i=0;i<newIndexes.size();i++){
		const Collection<Node*> &occupants=mLeafData[newIndexes[i]].occupants;
		for(int j=0;j<occupants.size();++j){
			Node *occupant=occupants[j];
			childdata *data=(childdata*)occupant->getParentData();
			if(data->counter!=mCounter && Math::testIntersection(occupant->getWorldBound(),volume)){
				data->counter=mCounter;
				listener->resultFound(occupant);
			}
		}
	}
	return true;
}

void BSP30Node::traceSegment(Collision &result,const Segment &segment,const Vector3 &size){
	result.time=Math::ONE;
	segment.getEndPoint(result.point);
	if(mMap!=NULL){
		int contents=mMap->modelTrace(result,0,size,segment.origin,result.point);
		if(contents!=CONTENTS_EMPTY) result.scope|=(-1-contents)<<1;
	}
}

void BSP30Node::render(Renderer *renderer) const{
	int i;

	renderer->setModelMatrix(Math::IDENTITY_MATRIX4X4); // Technically I dont need this anymore, since its a renderable.  But i'll keep it in case it ever gets changed again

	Material *previousMaterial=NULL;
	for(i=0;i<mVisibleMaterialFaces.size();i++){
		mMap->materials[i]->setupRenderer(renderer,previousMaterial);
		BSP30Map::facedata *face=mVisibleMaterialFaces[i];
		while(face!=NULL){
			if(face->visible){
				renderer->renderPrimitive(mMap->vertexData,face->indexData);
			}
			face=face->next;
		}
		previousMaterial=mMap->materials[i];
	}

	renderer->setTextureStage(0,NULL);
	renderer->setTextureStage(1,NULL);
}

void BSP30Node::childTransformUpdated(Node *child){
	if(mMap==NULL){
		return;
	}

	Collection<int> &oldIndexes=((childdata*)child->getParentData())->leafs;
	Collection<int> &newIndexes=mLeafIndexes; 
	newIndexes.clear();
	mMap->findBoundLeafs(newIndexes,mMap->nodes,0,child->getWorldBound());

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
				}
			}
		}
	}
}

}
}
}
