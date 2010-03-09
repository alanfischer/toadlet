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

#define CLIP

// TODO:
// - make AABox & Sphere adaption routines, and then move HopEntities conversion stuff over to that
// - clean up the aabox querying so its a lot easier to use
// - split up queueRenderables into a findVisualNodes, so we have a findVisualNodes and findNodesInAABozx or something
// - merge external trace function into the class (maybe the Map class?)
// - make the traceSegment function take a radius perhaps, and then a radius==hull[x] expanded size would be a trace through that hull
// - modify the rendering to be material based, with objects that it would push into the pipeline, so we don't need to do the preRender crap
// - make members protected again
// - MAYBE: eliminate the need to create a worldspawn model, and that would instead all be handled by the Scene's rendering + Scene's tracing (however... that now needs the hopScene to be aware of us, not just our solids, but that wouldn't be impossible with the Traceable interface)
// - move lightmaps to use the packer, and have just a small set of actual lightmap textures so we can eliminate texture switching

#include <toadlet/egg/EndianConversion.h>
#include <toadlet/peeper/VertexFormat.h>
#include <toadlet/tadpole/Engine.h>
#include <toadlet/tadpole/bsp/BSP30SceneNode.h>
#include <toadlet/tadpole/node/MeshNode.h>
#include <toadlet/tadpole/handler/WADArchive.h>
#include <string.h> // memset

using namespace toadlet::egg;
using namespace toadlet::egg::image;
using namespace toadlet::peeper;
using namespace toadlet::tadpole::mesh;
using namespace toadlet::tadpole::node;
using namespace toadlet::tadpole::handler;

float trace(toadlet::tadpole::bsp::BSP30Map *map,int startnode,Vector3 start,Vector3 end,Vector3 &normal);

namespace toadlet{
namespace tadpole{
namespace bsp{

TOADLET_NODE_IMPLEMENT(BSP30ModelNode,Categories::TOADLET_TADPOLE_BSP+".BSP30ModelNode");

BSP30ModelNode::BSP30ModelNode():Node(),
	//mMap,
	mModelIndex(-1),
	mVisible(true),
	mInternalScope(0)
{}

BSP30ModelNode::~BSP30ModelNode(){
}

Node *BSP30ModelNode::create(Engine *engine){
	return super::create(engine);
}

void BSP30ModelNode::destroy(){
	super::destroy();
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

	bmodel *model=&mMap->models[mModelIndex];

	Sphere bound;
	Math::sub(bound.origin,model->maxs,model->mins);
	Math::div(bound.origin,Math::TWO);
	bound.radius=Math::length(bound.origin);
	Math::add(bound.origin,model->mins);
	setLocalBound(bound);
}

void BSP30ModelNode::queueRenderable(SceneNode *queue,CameraNode *camera){
	if(mVisible){
		queue->queueRenderable(this);
	}
}

Material *BSP30ModelNode::getRenderMaterial() const{
	return NULL;
}

const Matrix4x4 &BSP30ModelNode::getRenderTransform() const{
	return getWorldRenderTransform();
}

void BSP30ModelNode::render(peeper::Renderer *renderer) const{
	if(mModelIndex<=0){ // We don't render any invalid (-1), or the world (0) model, since the SceneManager handles that currently
		return;		// TODO: I believe we could just check to see if visdata is available for this model, and if so then render it that way, otherwise without.  Then the world model would be rendered same way.
	}

	BSP30SceneNode *scene=(BSP30SceneNode*)mEngine->getScene()->getRootScene();
	BSP30SceneNode::RendererData &data=(BSP30SceneNode::RendererData&)scene->mRendererData;

	// clear stuff
	memset(&data.markedFaces[0],0,data.markedFaces.size()*sizeof(unsigned char));
	data.textureVisibleFaces.resize(mMap->parsedTextures.size());

	bmodel *model=&mMap->models[mModelIndex];
	for(int i=0;i<model->numfaces;++i){
		bface *face=&mMap->faces[model->firstface+i];
		btexinfo *texinfo=&mMap->texinfos[face->texinfo];
		data.textureVisibleFaces[texinfo->miptex].push_back(model->firstface+i);
	}

	renderer->setAlphaTest(Renderer::AlphaTest_GEQUAL,0.9);

	scene->renderVisibleFaces(renderer);
}

// TODO: Change the tracing so ONE = no collision, and -ONE is either in solid or unused
void BSP30ModelNode::traceSegment(Collision &result,const Segment &segment){
	Segment tseg;
	Vector3 invtrans=-getTranslate();
	Quaternion invrot; Math::invert(invrot,getRotate());
	Math::add(tseg.origin,invtrans,segment.origin);
	Math::mul(tseg.direction,invrot,segment.direction);

	result.time=Math::ONE;
	Vector3 end;
	segment.getEndPoint(end);
	if(mMap!=NULL){
	// TODO: Using headnode[1]!! is that right??
#if !defined(CLIP)
result.time=trace(mMap,mMap->models[mModelIndex].headnode[0],segment.origin,end,result.normal);
#else
result.time=trace(mMap,mMap->models[mModelIndex].headnode[1],segment.origin,end,result.normal);
#endif

int contents=0;
BSP30SceneNode *scene=(BSP30SceneNode*)mEngine->getScene()->getRootScene();
int leaf=mMap->findPointLeaf(mMap->planes,mMap->nodes,sizeof(bnode),0,segment.origin);
if(leaf>=0 && leaf<mMap->nleafs){contents=mMap->leafs[leaf].contents;}

if(result.time==0 && mInternalScope!=0){result.scope|=mInternalScope; result.time=Math::ONE; }

if(contents<=-1){ result.scope|=(-1-contents)<<1; } // Would be nice to merge this into the trace
	}

	if(result.time==Math::ONE){
		Math::add(result.point,segment.direction,segment.origin);
	}
	else{
		Math::madd(result.point,segment.direction,result.time,segment.origin);
	}
}


TOADLET_NODE_IMPLEMENT(BSP30SceneNode,Categories::TOADLET_TADPOLE_NODE+".BSP30SceneNode");

BSP30SceneNode::BSP30SceneNode():super(),
	mCounter(1)
{
}

BSP30SceneNode::~BSP30SceneNode(){}

node::Node *BSP30SceneNode::create(Engine *engine){
	super::create(engine);

	return this;
}

void BSP30SceneNode::destroy(){
	super::destroy();
}

void BSP30SceneNode::setMap(BSP30Map::ptr map){
	int i,j;

	for(i=0;i<mChildren.size();++i){
		Node *node=mChildren[i];
		Collection<int> &indexes=((childdata*)node->getParentData())->leafs;
		removeNodeLeafIndexes(indexes,node);
	}

	mMap=map;

	mLeafData.resize(mMap->nleafs);
	mRenderFaces.resize(mMap->nfaces);

	int vertexCount=0;
	for(i=0;i<mMap->nfaces;i++){
		vertexCount+=mMap->faces[i].numedges;
	}

	VertexFormat::ptr vertexFormat(new VertexFormat(4));
	vertexFormat->addVertexElement(VertexElement(VertexElement::Type_POSITION,VertexElement::Format_BIT_FLOAT_32|VertexElement::Format_BIT_COUNT_3));
	vertexFormat->addVertexElement(VertexElement(VertexElement::Type_NORMAL,VertexElement::Format_BIT_FLOAT_32|VertexElement::Format_BIT_COUNT_3));
	vertexFormat->addVertexElement(VertexElement(VertexElement::Type_TEX_COORD,VertexElement::Format_BIT_FLOAT_32|VertexElement::Format_BIT_COUNT_2));
	vertexFormat->addVertexElement(VertexElement(VertexElement::Type_TEX_COORD_2,VertexElement::Format_BIT_FLOAT_32|VertexElement::Format_BIT_COUNT_2));
	VertexBuffer::ptr vertexBuffer=mEngine->getBufferManager()->createVertexBuffer(Buffer::UsageFlags_STATIC,Buffer::AccessType_WRITE_ONLY,vertexFormat,vertexCount);
	mVertexData=VertexData::ptr(new VertexData(vertexBuffer));

	vertexCount=0;
	vba.lock(vertexBuffer);
	for(i=0;i<mMap->nfaces;i++){
		bface *face=&mMap->faces[i];

		mRenderFaces[i].indexData=IndexData::ptr(new IndexData(IndexData::Primitive_TRIFAN,NULL,vertexCount,face->numedges));

		for(j=0;j<face->numedges;j++){
			int t=mMap->surfedges[face->firstedge+j];

			int vert=0;
			if(t<0){
				vert=mMap->edges[-t].v[1];
			}
			else{
				vert=mMap->edges[t].v[0];
			}

			vba.set3(vertexCount+j,0,(float*)&mMap->vertexes[vert]);

			bplane *plane=&mMap->planes[face->planenum];
			if(face->side){
				vba.set3(vertexCount+j,1,-plane->normal[0],-plane->normal[1],-plane->normal[2]);
			}
			else{
				vba.set3(vertexCount+j,1,plane->normal[0],plane->normal[1],plane->normal[2]);
			}
		}

		// START: Needs cleanup, conversion to our datastructures in Map
		btexinfo *texinfo=&mMap->texinfos[face->texinfo];
		int width=0,height=0;
		bmiptexlump *lump=(bmiptexlump*)mMap->textures;
		int miptexofs=lump->dataofs[texinfo->miptex];
		if(miptexofs!=-1){
			WADArchive::wmiptex *tex=(WADArchive::wmiptex*)(&((byte*)mMap->textures)[miptexofs]);
			width=tex->width;
			height=tex->height;
		}
		Vector2 mins,maxs;
		calculateSurfaceExtents(face,mins,maxs);

		Image::ptr lightmap;
		unsigned int lmwidth=0,lmheight=0;
		if((texinfo->flags&TEX_SPECIAL)==0){
			lightmap=computeLightmap(face,mins,maxs);
			lmwidth=lightmap->getWidth();
			lmheight=lightmap->getHeight();
			mRenderFaces[i].lightmap=mEngine->getTextureManager()->createTexture(lightmap);
			mRenderFaces[i].lightmap->retain();
		}

		if(width>0 && height>0){
			float is=1.0f/(float)width;
			float it=1.0f/(float)height;

			for(j=0;j<face->numedges;j++){
				Vector3 p;
				vba.get3(vertexCount+j,0,p);
				float s=Math::dot(p,texinfo->vecs[0]) + texinfo->vecs[0][3];
				float t=Math::dot(p,texinfo->vecs[1]) + texinfo->vecs[1][3];
 
				vba.set2(vertexCount+j,2,s*is,t*it);

				if(lightmap!=NULL){
					// compute lightmap coords
					float mid_poly_s = (mins[0] + maxs[0])/2.0f;
					float mid_poly_t = (mins[1] + maxs[1])/2.0f;
					float mid_tex_s = (float)lmwidth / 2.0f;
					float mid_tex_t = (float)lmheight / 2.0f;
					float ls = mid_tex_s + (s - mid_poly_s) / 16.0f;
					float lt = mid_tex_t + (t - mid_poly_t) / 16.0f;
					ls /= (float)lmwidth;
					lt /= (float)lmheight;

					vba.set2(vertexCount+j,3,ls,lt);
				}
			}
		}
		// END: Needs cleanup

		vertexCount+=face->numedges;
	}
	vba.unlock();

	mRendererData.markedFaces.resize((mMap->nfaces+7)/8);

	getRenderLayer(0)->forceRender=true;
	// END: Needs cleanup

	for(i=0;i<mChildren.size();++i){
		Node *node=mChildren[i];
		Collection<int> &indexes=((childdata*)node->getParentData())->leafs;
		mMap->findBoundLeafs(indexes,mMap->nodes,0,node->getWorldBound());
		insertNodeLeafIndexes(indexes,node);
	}

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

	node::MeshNode::ptr node=mEngine->createNodeType(node::MeshNode::type());
	node->setMesh(mesh);
	getBackground()->attach(node);
	// END: Needs to be removed
}

bool BSP30SceneNode::performAABoxQuery(SpacialQuery *query,const AABox &box,bool exact){
	if(mMap==NULL){
		return false;
	}

	SpacialQueryResultsListener *listener=query->getSpacialQueryResultsListener();
	mCounter++;
	Collection<int> &newIndexes=mLeafIndexes; 
	newIndexes.clear();
	mMap->findBoundLeafs(newIndexes,mMap->nodes,0,box);
	for(int i=0;i<newIndexes.size();i++){
		const Collection<Node*> &occupants=mLeafData[newIndexes[i]].occupants;
		for(int j=0;j<occupants.size();++j){
			Node *occupant=occupants[j];
			childdata *data=(childdata*)occupant->getParentData();
			if(data->counter!=mCounter && Math::testIntersection(occupant->getWorldBound(),box)){
				data->counter=mCounter;
				listener->resultFound(occupant);
			}
		}
	}
	return true;
}

bool BSP30SceneNode::attach(Node *node){
	bool result=super::attach(node);
	if(result){
		node->setParentData(new childdata());

		if(mMap!=NULL){
			// HACK: Need to make sure the node is updated before we check its bounds.
			//  This requirement will go away once we move the SceneNode's node updating into the nodes themselves
			super::logicUpdate(node,0,-1);

			Collection<int> &newIndexes=((childdata*)node->getParentData())->leafs;
			mMap->findBoundLeafs(newIndexes,mMap->nodes,0,node->getWorldBound());
			insertNodeLeafIndexes(newIndexes,node);
		}
	}
	return result;
}

bool BSP30SceneNode::remove(Node *node){
	int i;
	for(i=mChildren.size()-1;i>=0;--i){
		if(mChildren[i]==node)break;
	}

	bool result=super::remove(node);
	if(result){
		if(mMap!=NULL){
			removeNodeLeafIndexes(((childdata*)node->getParentData())->leafs,node);
		}

		delete node->getParentData();
		node->setParentData(NULL);
	}
	return result;
}

void BSP30SceneNode::insertNodeLeafIndexes(const Collection<int> &indexes,Node *node){
	int i;
	for(i=indexes.size()-1;i>=0;--i){
		mLeafData[indexes[i]].occupants.add(node);
	}
}

void BSP30SceneNode::removeNodeLeafIndexes(const Collection<int> &indexes,Node *node){
	int i;
	for(i=indexes.size()-1;i>=0;--i){
		mLeafData[indexes[i]].occupants.remove(node);
	}
}

void BSP30SceneNode::childTransformUpdated(Node *child){
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

void BSP30SceneNode::queueRenderables(){
	if(mMap==NULL){
		super::queueRenderables();
		return;
	}

	int i,j;
	int leaf=mMap->findPointLeaf(mMap->planes,mMap->nodes,sizeof(bnode),0,mCamera->getWorldTranslate());
	// If no visibility information just test all leaves
	if(leaf==0 || mMap->parsedVisibility.size()==0){
		for(i=0;i<mChildren.size();++i){
			super::queueRenderables(mChildren[i]);
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
				if(data->counter!=mCounter && mCamera->culled(occupant->getWorldBound())==false){
					data->counter=mCounter;
					super::queueRenderables(occupant);
				}
			} 
		}
	}
}

bool BSP30SceneNode::preLayerRender(Renderer *renderer,int layer){
	if(layer!=0 || mMap==NULL){
		return false;
	}

	processVisibleFaces(mCamera);

	renderer->setModelMatrix(Math::IDENTITY_MATRIX4X4);
	renderVisibleFaces(renderer);

	return true;
}

void BSP30SceneNode::processVisibleFaces(CameraNode *camera){
	int i;
	RendererData &data=(RendererData&)mRendererData;

	// clear stuff
	memset(&data.markedFaces[0],0,data.markedFaces.size()*sizeof(unsigned char));
	data.textureVisibleFaces.resize(mMap->parsedTextures.size());

	// find leaf we're in
	int leaf=mMap->findPointLeaf(mMap->planes,mMap->nodes,sizeof(bnode),0,camera->getWorldTranslate());

	// If no visibility information just test all leaves
	if(leaf==0 || mMap->nvisibility==0){
		for(i=0;i<mMap->nleafs;i++){
			addLeafToVisible(&mMap->leafs[i],data,camera);
		}
	}
	else{
		// go thru leaf visibility list
		for(i=0;i<mMap->parsedVisibility[leaf].size();i++){
			addLeafToVisible(&mMap->leafs[mMap->parsedVisibility[leaf][i]],data,camera);
		}
	}
}

void BSP30SceneNode::renderVisibleFaces(Renderer *renderer){
	int i,j;
	RendererData &data=(RendererData&)mRendererData;
	TextureStage::ptr textureStage(new TextureStage());
	textureStage->setMinFilter(TextureStage::Filter_LINEAR);
	textureStage->setMipFilter(TextureStage::Filter_LINEAR);
	textureStage->setMagFilter(TextureStage::Filter_LINEAR);

	renderer->setLighting(false);
	renderer->setFaceCulling(Renderer::FaceCulling_FRONT);
	renderer->setDepthWrite(true);

	TextureStage::ptr lightmapStage(new TextureStage());
	lightmapStage->setTexCoordIndex(1);
	lightmapStage->setBlend(TextureBlend(TextureBlend::Operation_MODULATE_2X,TextureBlend::Source_PREVIOUS,TextureBlend::Source_TEXTURE));
	lightmapStage->setMinFilter(TextureStage::Filter_LINEAR);
	lightmapStage->setMipFilter(TextureStage::Filter_LINEAR);
	lightmapStage->setMagFilter(TextureStage::Filter_LINEAR);
	lightmapStage->setUAddressMode(TextureStage::AddressMode_CLAMP_TO_EDGE);
	lightmapStage->setVAddressMode(TextureStage::AddressMode_CLAMP_TO_EDGE);

	for(i=0;i<data.textureVisibleFaces.size();i++){
		Collection<int> &visibleFaces=data.textureVisibleFaces[i];

		if(visibleFaces.size()>0){
if(mMap->parsedTextures.size()>i) textureStage->setTexture(mMap->parsedTextures[i]);
			renderer->setTextureStage(0,textureStage);

			for(j=0;j<visibleFaces.size();++j){
				int vf=visibleFaces[j];

				lightmapStage->setTexture(mRenderFaces[vf].lightmap);
				renderer->setTextureStage(1,lightmapStage);

				renderer->renderPrimitive(mVertexData,mRenderFaces[vf].indexData);
			}
		}

		visibleFaces.clear();
	}
	
	renderer->setTextureStage(0,NULL);
	renderer->setTextureStage(1,NULL);
}

void BSP30SceneNode::addLeafToVisible(bleaf *leaf,RendererData &data,CameraNode *camera) const{
	AABox bound(leaf->mins[0],leaf->mins[1],leaf->mins[2],leaf->maxs[0],leaf->maxs[1],leaf->maxs[2]);
	if(camera->culled(bound)==false){
		const unsigned short* p=&mMap->marksurfaces[leaf->firstmarksurface];

		for(int x=0;x<leaf->nummarksurfaces;x++){
			// don't render those already rendered
			int face_idx=*p++;

			if(!(data.markedFaces[face_idx>>3] & (1<<(face_idx & 7)))){
				// back face culling
				bface *face=&mMap->faces[face_idx];

				float d=Math::length(*(Plane*)&mMap->planes[face->planenum],camera->getWorldRenderTranslate());
				if(face->side){
					if(d>0) continue;
				}
				else{
					if(d<0) continue;
				}

				// mark face as visible
				data.markedFaces[face_idx>>3] |= (1<<(face_idx & 7));

				btexinfo *texinfo=&mMap->texinfos[face->texinfo];

				if((texinfo->flags&TEX_SPECIAL)==0){
if(data.textureVisibleFaces.size()<=texinfo->miptex){
	data.textureVisibleFaces.resize(texinfo->miptex+1);
}
					data.textureVisibleFaces[texinfo->miptex].push_back(face_idx);
				}
/*
				// chose lightmap style
				f->lightmap = f->lightmaps[0];
				if(current_style != 0) {
					for(int c = 0; c < MAXLIGHTMAPS; c++) {
						if(f->styles[c] == current_style) {
							f->lightmap = f->lightmaps[c];
							break;
						}
					}
				}
*/			}
		}
	}
}

void BSP30SceneNode::calculateSurfaceExtents(bface *face,Vector2 &mins,Vector2 &maxs){
	float val;
	int i,j,e;

	Vector3 v;
	mins[0]=mins[1]=999999;
	maxs[0]=maxs[1]=-999999;

	btexinfo *tex=&mMap->texinfos[face->texinfo];

	for(i=0;i<face->numedges;i++){
		e=mMap->surfedges[face->firstedge + i];
		if(e>=0){
			v=(float*)&mMap->vertexes[mMap->edges[e].v[0]];
		}
		else{
			v=(float*)&mMap->vertexes[mMap->edges[-e].v[1]];
		}

		for(j=0;j<2;j++){
			val=v.x * tex->vecs[j][0] +
				v.y * tex->vecs[j][1] +
				v.z * tex->vecs[j][2] +
				tex->vecs[j][3];
			if(val<mins[j]){
				mins[j]=val;
			}
			if(val>maxs[j]){
				maxs[j]=val;
			}
		}
	}
}

#define TEXTURE_SIZE 16

Image::ptr BSP30SceneNode::computeLightmap(bface *face,const Vector2 &mins,const Vector2 &maxs){
	if(mMap->nlighting==0){return NULL;}

	int c;
	int width, height;

	// compute lightmap size
	int size[2];
	for(c=0;c<2;c++){
		float tmin=(float)floor(mins[c]/TEXTURE_SIZE);
		float tmax=(float)ceil(maxs[c]/TEXTURE_SIZE);
		size[c]=(int)(tmax-tmin);
	}

	width=size[0]+1;
	height=size[1]+1;
	int lsz=width*height*3;  // RGB buffer size

	// generate lightmaps texture
	Image::ptr image(new Image());

//	for(c=0;c<1;c++){
		//if(face->styles[c]==-1)break;
		//face->styles[c]=dface->styles[c];

	image->reallocate(Image::Dimension_D2,Image::Format_RGB_8,width,height);

	memcpy(image->getData(),&((byte*)mMap->lighting)[face->lightofs],lsz);

	// Line below is valid for when there are multiple lightmaps on this face, then c would be that index.
	// Need to check into how there can be multiple lightmaps
	//memcpy(image->data,&mLightData[face->lightofs+(lsz*c)],lsz);
//	}

//    face->lightmap = face->lightmaps[0];
	return image;
}

}
}
}



// Code structure from
// http://svn.jansson.be/foreign/quake/q1/trunk/QW/client/pmovetst.c

using namespace toadlet::egg;
using namespace toadlet::tadpole;
using namespace toadlet::tadpole::bsp;

struct pmtrace_t{
	pmtrace_t(){memset(this,0,sizeof(pmtrace_t));
		fraction = 1;
		allsolid = true;}

	bool allsolid;
	bool inopen;
	bool inwater;
	bool startsolid;
	Plane plane;
	float fraction;
	Vector3 endpos;
};

#if !defined(CLIP)
typedef bnode dclipnode_t;
#else
typedef bclipnode dclipnode_t;
#endif
typedef BSP30Map hull_t;
typedef Vector3 vec3_t;
typedef bplane mplane_t;

#define qfalse false
#define qtrue true
#define DotProduct Math::dot
#define VectorCopy(v1,v2) v2.set(v1)
#define	DIST_EPSILON	(0.03125)
#define vec3_origin Math::ZERO_VECTOR3
#define VectorSubtract(v1,v2,r) Math::sub(r,v1,v2);

bool RecursiveHullCheck (hull_t *hull, int num, float p1f, float p2f, Vector3 p1, Vector3 p2, pmtrace_t *trace)
{
	dclipnode_t	*node;
	bplane		*plane;
	float		t1, t2;
	float		frac;
	int			i;
	Vector3		mid;
	int			side;
	float		midf;

// check for empty
	if (num < 0)
	{
		if (num != CONTENTS_SOLID)
		{
			trace->allsolid = qfalse;
			if (num == CONTENTS_EMPTY)
				trace->inopen = qtrue;
			else
				trace->inwater = qtrue;
		}
		else
			trace->startsolid = qtrue;
		return qtrue;		// empty
	}

//
// find the point distances
//
#if !defined(CLIP)
		node = &hull->nodes[num];
#else
		node = &hull->clipnodes[num];
#endif
	plane = &hull->planes[node->planenum];

	if (plane->type < 3)
	{
		t1 = p1[plane->type] - plane->dist;
		t2 = p2[plane->type] - plane->dist;
	}
	else
	{
		t1 = DotProduct (plane->normal, p1) - plane->dist;
		t2 = DotProduct (plane->normal, p2) - plane->dist;
	}
	
#if 1
	if (t1 >= 0 && t2 >= 0)
		return RecursiveHullCheck (hull, node->children[0], p1f, p2f, p1, p2, trace);
	if (t1 < 0 && t2 < 0)
		return RecursiveHullCheck (hull, node->children[1], p1f, p2f, p1, p2, trace);
#else
	if ( (t1 >= DIST_EPSILON && t2 >= DIST_EPSILON) || (t2 > t1 && t1 >= 0) )
		return RecursiveHullCheck (hull, node->children[0], p1f, p2f, p1, p2, trace);
	if ( (t1 <= -DIST_EPSILON && t2 <= -DIST_EPSILON) || (t2 < t1 && t1 <= 0) )
		return RecursiveHullCheck (hull, node->children[1], p1f, p2f, p1, p2, trace);
#endif

// put the crosspoint DIST_EPSILON pixels on the near side
	if (t1 < 0)
		frac = (t1 + DIST_EPSILON)/(t1-t2);
	else
		frac = (t1 - DIST_EPSILON)/(t1-t2);
	if (frac < 0)
		frac = 0;
	if (frac > 1)
		frac = 1;
		
	midf = p1f + (p2f - p1f)*frac;
	for (i=0 ; i<3 ; i++)
		mid[i] = p1[i] + frac*(p2[i] - p1[i]);

	side = (t1 < 0);

// move up to the node
	if (!RecursiveHullCheck (hull, node->children[side], p1f, midf, p1, mid, trace) )
		return qfalse;
	
#if !defined(CLIP)
	int contents=hull->leafs[hull->findPointLeaf(hull->planes,hull->nodes,sizeof(bnode),node->children[side^1],mid)].contents;
#else
	int contents=-1-hull->findPointLeaf(hull->planes,hull->clipnodes,sizeof(bclipnode),node->children[side^1],mid);
#endif
	if(contents!=CONTENTS_SOLID)
		return RecursiveHullCheck (hull, node->children[side^1], midf, p2f, mid, p2, trace);
	
	if (trace->allsolid)
		return qfalse;		// never got out of the solid area
		
//==================
// the other side of the node is solid, this is the impact point
//==================
	if (!side)
	{
		VectorCopy (plane->normal, trace->plane.normal);
		trace->plane.distance = plane->dist;
	}
	else
	{
		VectorSubtract (vec3_origin, plane->normal, trace->plane.normal);
		trace->plane.distance = -plane->dist;
	}

#if !defined(CLIP)
	while (hull->leafs[hull->findPointLeaf(hull->planes,hull->nodes,sizeof(bnode),0,mid)].contents
#else
	while (-1-hull->findPointLeaf(hull->planes,hull->clipnodes,sizeof(bclipnode),0,mid)
#endif
	== CONTENTS_SOLID)
	{ // shouldn't really happen, but does occasionally
		frac -= 0.1;
		if (frac < 0)
		{
			trace->fraction = midf;
			VectorCopy (mid, trace->endpos);
//			Con_DPrintf ("backup past 0\n");
			return qfalse;
		}
		midf = p1f + (p2f - p1f)*frac;
		for (i=0 ; i<3 ; i++)
			mid[i] = p1[i] + frac*(p2[i] - p1[i]);
	}

	trace->fraction = midf;
	VectorCopy (mid, trace->endpos);

	return qfalse;
}

float trace(BSP30Map *map,int startnode,Vector3 start,Vector3 end,Vector3 &normal){
	pmtrace_t t;
	bool result=RecursiveHullCheck(map,startnode,0,1,start,end,&t);
	normal=t.plane.normal;

	if (t.allsolid)
		t.startsolid = qtrue;
	if (t.startsolid)
		t.fraction = 0;

	if(t.allsolid==false && t.startsolid==true){t.fraction=Math::ONE;}

	return t.fraction;
}
