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
#include <toadlet/tadpole/bsp/BSPSceneNode.h>
#include <toadlet/tadpole/node/MeshNode.h>

using namespace toadlet::egg;
using namespace toadlet::egg::image;
using namespace toadlet::peeper;
using namespace toadlet::tadpole::mesh;
using namespace toadlet::tadpole::node;

namespace toadlet{
namespace tadpole{
namespace bsp{

TOADLET_NODE_IMPLEMENT(BSPSceneNode,"toadlet::tadpole::bsp::BSPSceneNode");

BSPSceneNode::BSPSceneNode():node::Scene(),
	mEpsilon(0)
{
	mEpsilon=0.03125f;
}

BSPSceneNode::~BSPSceneNode(){}

node::Node *BSPSceneNode::create(Engine *engine){
	super::create(engine);

	return this;
}

void BSPSceneNode::destroy(){
	super::destroy();
}

void BSPSceneNode::setBSPMap(BSPMap::ptr map){
	int i,j;

	// TODO: Clear out old contents of map

	mBSPMap=map;

	int vertexCount=0;
	for(i=0;i<mBSPMap->faces.size();i++){
		vertexCount+=mBSPMap->faces[i].edgeCount;
	}

	VertexFormat::ptr vertexFormat(new VertexFormat(4));
	vertexFormat->addVertexElement(VertexElement(VertexElement::Type_POSITION,VertexElement::Format_BIT_FLOAT_32|VertexElement::Format_BIT_COUNT_3));
	vertexFormat->addVertexElement(VertexElement(VertexElement::Type_NORMAL,VertexElement::Format_BIT_FLOAT_32|VertexElement::Format_BIT_COUNT_3));
	vertexFormat->addVertexElement(VertexElement(VertexElement::Type_TEX_COORD,VertexElement::Format_BIT_FLOAT_32|VertexElement::Format_BIT_COUNT_2));
	vertexFormat->addVertexElement(VertexElement(VertexElement::Type_TEX_COORD_2,VertexElement::Format_BIT_FLOAT_32|VertexElement::Format_BIT_COUNT_2));
	VertexBuffer::ptr vertexBuffer=mEngine->getBufferManager()->createVertexBuffer(Buffer::UsageFlags_STATIC,Buffer::AccessType_WRITE_ONLY,vertexFormat,vertexCount);
	mVertexData=VertexData::ptr(new VertexData(vertexBuffer));

	mRenderFaces.resize(mBSPMap->faces.size());

	vertexCount=0;
	vba.lock(vertexBuffer);
	for(i=0;i<mBSPMap->faces.size();i++){
		const Face &face=mBSPMap->faces[i];

		mRenderFaces[i].indexData=IndexData::ptr(new IndexData(IndexData::Primitive_TRIFAN,NULL,vertexCount,face.edgeCount));

		for(j=0;j<face.edgeCount;j++){
			int t=mBSPMap->surfedges[face.edgeStart+j];

			int vert=0;
			if(t<0){
				vert=mBSPMap->edges[-t].v[1];
			}
			else{
				vert=mBSPMap->edges[t].v[0];
			}

			vba.set3(vertexCount+j,0,mBSPMap->vertexes[vert]);

			const Plane &plane=mBSPMap->planes[face.plane];
			if(face.side){
				vba.set3(vertexCount+j,1,-plane.normal.x,-plane.normal.y,-plane.normal.z);
			}
			else{
				vba.set3(vertexCount+j,1,plane.normal.x,plane.normal.y,plane.normal.z);
			}
		}

		// START: Needs cleanup, conversion to our datastructures in BSPMap
		const BSPMap::texinfo &texinfo=mBSPMap->texinfos[face.texinfo];
		int width=0,height=0;
		BSPMap::miptexlump *lump=(BSPMap::miptexlump*)&mBSPMap->textures[0];
		int miptexofs=lump->dataofs[texinfo.miptex];
		if(miptexofs!=-1){
			BSPMap::miptex *tex=(BSPMap::miptex*)(&mBSPMap->textures[miptexofs]);
			width=tex->width;
			height=tex->height;
		}
		Vector2 mins,maxs;
		calculateSurfaceExtents(face,mins,maxs);

		Image::ptr lightmap;
		unsigned int lmwidth=0,lmheight=0;

		if((texinfo.flags&TEX_SPECIAL)==0){
			lightmap=computeLightmap(face,mins,maxs);
			lmwidth=lightmap->getWidth();
			lmheight=lightmap->getHeight();

			unsigned int newlmwidth=lmwidth,newlmheight=lmheight;
			if(Math::isPowerOf2(newlmwidth)==false){
				newlmwidth=Math::nextPowerOf2(newlmwidth);
			}
			if(Math::isPowerOf2(newlmheight)==false){
				newlmheight=Math::nextPowerOf2(newlmheight);
			}

//			if(lmwidth!=newlmwidth || lmheight!=newlmheight){
//				Image::ptr newLightmap(new Image(lightmap->getDimension(),lightmap->getFormat(),newlmwidth,newlmheight));
//				ImageUtilities::scaleImage(lightmap,newLightmap,false);
//				lightmap=newLightmap;
//			}

			mRenderFaces[i].lightmap=mEngine->getTextureManager()->createTexture(lightmap);
			mRenderFaces[i].lightmap->retain();
		}

		if(width>0 && height>0){
			float is=1.0f/(float)width;
			float it=1.0f/(float)height;

			for(j=0;j<face.edgeCount;j++){
				Vector3 p;
				vba.get3(vertexCount+j,0,p);
				float s=Math::dot(p,*(Vector3*)texinfo.vecs[0]) + texinfo.vecs[0][3];
				float t=Math::dot(p,*(Vector3*)texinfo.vecs[1]) + texinfo.vecs[1][3];
 
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

		vertexCount+=face.edgeCount;
	}
	vba.unlock();

	mRendererData.markedFaces.resize((mBSPMap->faces.size()+7)/8);

	decompressVIS();

	// START: Needs cleanup
	BSPMap::miptexlump *lump=(BSPMap::miptexlump*)&mBSPMap->textures[0];
	#ifdef TOADLET_BIG_ENDIAN
		littleInt32(lump->nummiptex);
	#endif

	for(i=0;i<lump->nummiptex;i++){
		#ifdef TOADLET_BIG_ENDIAN
			littleInt32(lump->dataofs[i]);
		#endif
		if(lump->dataofs[i]==-1){
			// Unused mip lump.
			textures.add(NULL);
			continue;
		}

		BSPMap::miptex *tex=(BSPMap::miptex*)(&mBSPMap->textures[lump->dataofs[i]]);
		#ifdef TOADLET_BIG_ENDIAN
			littleUInt32(tex->width);
			littleUInt32(tex->height);
		#endif

		if(tex->width<1 || tex->height<1){
			// Invalid texture
			// Do we want a dummy texture here?
			textures.add(NULL);
			continue;
		}

		int size=tex->width*tex->height;

		if(tex->offsets[0] != 0){
			int datasize = size + (size/4) + (size/16) + (size/64);

			unsigned char *indices=(unsigned char *)tex + tex->offsets[0];
			unsigned char *palette = indices + datasize + 2;

			// TODO: Load the texture here from the BSP, using 'palette'
			// Look at wad loader for howto
			Logger::alert("embedded tex:"+String(tex->name));
			textures.add(NULL);
			continue;
		}
		else{
			Texture::ptr texture=mEngine->getTextureManager()->findTexture(tex->name);
			if(texture!=NULL){
				texture->retain();
			}
			textures.add(texture);
			continue;
		}

		// Does this mip contain alpha transparency?
		if(tex->name[0]=='{'){
			// TODO: Convert any (0,0,1) to (0,0,0,0);
			// This will involve re-allocating the texture as RGBA
		}

		// TODO: Look for textures beginning with "+0" and have these be AnimatedTextures.
	}

	getRenderLayer(0)->forceRender=true;
	// END: Needs cleanup

	// START: Needs to be removed, or a map/member setting, so skyboxes could be added outside of this class
	Texture::ptr bottom=mEngine->getTextureManager()->findTexture("nightsky/nightsky_down.png");
	Texture::ptr top=mEngine->getTextureManager()->findTexture("nightsky/nightsky_up.png");
	Texture::ptr left=mEngine->getTextureManager()->findTexture("nightsky/nightsky_west.png");
	Texture::ptr right=mEngine->getTextureManager()->findTexture("nightsky/nightsky_east.png");
	Texture::ptr back=mEngine->getTextureManager()->findTexture("nightsky/nightsky_south.png");
	Texture::ptr front=mEngine->getTextureManager()->findTexture("nightsky/nightsky_north.png");

	Mesh::ptr mesh=mEngine->getMeshManager()->createSkyBox(1024,false,bottom,top,left,right,back,front);
	for(i=0;i<mesh->subMeshes.size();++i){
		mesh->subMeshes[i]->material->setLayer(-1);
	}
	setLayerClearing(0,false);

	node::MeshNode::ptr node=mEngine->createNodeType(node::MeshNode::type());
	node->setMesh(mesh);
	getBackground()->attach(node);
	// END: Needs to be removed
}

bool BSPSceneNode::preLayerRender(Renderer *renderer,int layer){
	if(layer!=0){
		return false;
	}

	processVisibleFaces(mCamera);
	renderVisibleFaces(renderer);

	return true;
}

int BSPSceneNode::findLeaf(const Vector3 &point) const{
	int nodeIndex=mBSPMap->trees[0].nodeStart;

	while(nodeIndex>=0){
		const bsp::Node &node=mBSPMap->nodes[nodeIndex];
		const Plane &plane=mBSPMap->planes[node.plane];
		if(Math::length(plane,point)>=0){ // in front or on the plane
			nodeIndex=node.children[0];
		}
		else{  // behind the plane
			nodeIndex=node.children[1];
		}
	}

	return -(nodeIndex+1);
}

scalar BSPSceneNode::traceSegment(Vector3 &normal,const Segment &segment){
	scalar result=Math::ONE;
	Vector3 end;
	segment.getEndPoint(end);
	traceNode(result,normal,0,segment.origin,end,0,Math::ONE,NULL,NULL);
	return result;
}

scalar BSPSceneNode::traceSphere(Vector3 &normal,const Segment &segment,const Sphere &sphere){
	scalar result=Math::ONE;
	Vector3 end;
	segment.getEndPoint(end);
	traceNode(result,normal,0,segment.origin,end,0,Math::ONE,&sphere,NULL);
	return result;
}

scalar BSPSceneNode::traceAABox(Vector3 &normal,const Segment &segment,const AABox &box){
	scalar result=Math::ONE;
	Vector3 end;
	segment.getEndPoint(end);	
	traceNode(result,normal,0,segment.origin,end,0,Math::ONE,NULL,&box);
	return result;
}

// TODO: Rework this so we dont need to pass, start,end AND startFraction,endFraction.  We could probably get away with only fractions?
// TODO: Change the BSP naming stuff to use nodei & such to indicate an index into an array perhaps?
void BSPSceneNode::traceNode(scalar &result,Vector3 &normal,int nodeIndex,const Vector3 &start,const Vector3 &end,scalar startFraction,scalar endFraction,const Sphere *sphere,const AABox *box){
	int i;

	if(nodeIndex<0){
		const Leaf &leaf=mBSPMap->leaves[-nodeIndex-1];
		for(i=0;i<leaf.brushCount;++i){
			const Brush &brush=mBSPMap->brushes[leaf.brushStart+i];
			if(brush.contents==-2){ // TODO: -2=CONTENTS_SOLID, This is in BSP30Handler, and shouldnt be tested this way
				traceBrush(result,normal,brush,start,end,sphere,box);
			}
		}
		return;
	}

	const bsp::Node &node=mBSPMap->nodes[nodeIndex];
	const Plane &plane=mBSPMap->planes[node.plane];

	scalar startDistance=Math::length(plane,start);
	scalar endDistance=Math::length(plane,end);
	scalar offset=0;
	if(sphere!=NULL){
		offset=sphere->radius;
	}
	else if(box!=NULL){
		offset=	Math::abs(Math::mul(Math::maxVal(-box->mins.x,box->maxs.x),plane.normal.x)) +
				Math::abs(Math::mul(Math::maxVal(-box->mins.y,box->maxs.y),plane.normal.y)) +
				Math::abs(Math::mul(Math::maxVal(-box->mins.z,box->maxs.z),plane.normal.z));
	}

	if(startDistance>=offset && endDistance>=offset){
		// Segment is all in front
		traceNode(result,normal,node.children[0],start,end,startFraction,endFraction,sphere,box);
	}
	else if(startDistance<-offset && endDistance<-offset){
		// Segment is all behind
		traceNode(result,normal,node.children[1],start,end,startFraction,endFraction,sphere,box);
	}
	else{
		// Split segment
		int side;
		scalar fraction1, fraction2, middleFraction;
		Vector3 middle;

		// split the segment into two
		if (startDistance < endDistance)
		{
			side = 1; // back
			scalar inverseDistance = Math::div(Math::ONE,startDistance - endDistance);
			fraction1 = (startDistance - offset + mEpsilon) * inverseDistance;
			fraction2 = (startDistance + offset + mEpsilon) * inverseDistance;
		}
		else if (endDistance < startDistance)
		{
			side = 0; // front
			scalar inverseDistance = Math::div(Math::ONE,startDistance - endDistance);
			fraction1 = (startDistance + offset + mEpsilon) * inverseDistance;
			fraction2 = (startDistance - offset - mEpsilon) * inverseDistance;
		}
		else
		{
			side = 0; // front
			fraction1 = Math::ONE;
			fraction2 = 0;
		}

		// make sure the numbers are valid
		fraction1=Math::clamp(0,Math::ONE,fraction1);
		fraction2=Math::clamp(0,Math::ONE,fraction2);

		// calculate the middle point for the first side
		middleFraction = startFraction + Math::mul(endFraction - startFraction, fraction1);
		Math::lerp(middle,start,end,fraction1);

		// check the first side
		traceNode(result,normal,node.children[side],start,end,startFraction,middleFraction,sphere,box);

		// calculate the middle point for the second side
		middleFraction = startFraction + Math::mul(endFraction - startFraction, fraction2);
		Math::lerp(middle,start,end,fraction2);

		// check the second side
		traceNode(result,normal,node.children[!side],start,end,startFraction,middleFraction,sphere,box);
	}
}

void BSPSceneNode::traceBrush(scalar &result,Vector3 &normal,const Brush &brush,const Vector3 &start,const Vector3 &end,const Sphere *sphere,const AABox *box){
	scalar startFraction=-Math::ONE,endFraction=Math::ONE;
	Vector3 startNormal;
	bool startsOut=false;
	bool endsOut=false;
	int i,j;

	for(i=0;i<brush.planeCount;i++){
		const Plane &plane=mBSPMap->planes[brush.planeStart+i];

		scalar startDistance, endDistance;
		if(sphere==NULL && box==NULL){
			startDistance=Math::length(plane,start) + mEpsilon;
			endDistance=Math::length(plane,end) - mEpsilon;
		}
		else if(sphere!=NULL){
			startDistance=Math::length(plane,start) - sphere->radius + mEpsilon;
			endDistance=Math::length(plane,end)- sphere->radius - mEpsilon;
		}
		else if(box!=NULL){
			Vector3 offset;
			for(j=0;j<3;++j){
				if(plane.normal[j]<0){
					offset[j]=box->maxs[j];
				}
				else{
					offset[j]=box->mins[j];
				}
			}

			Vector3 temp;
			Math::add(temp,start,offset);
			startDistance=Math::length(plane,temp) + mEpsilon;
			Math::add(temp,end,offset);
			endDistance=Math::length(plane,temp) - mEpsilon;
		}

		if(startDistance>0){
			startsOut=true;
		}
		if(endDistance>0){
			endsOut=true;
		}

		// make sure the trace isn't completely on one side of the brush
		if(startDistance>0 && endDistance>0){
			// both are in front of the plane, its outside of this brush
			return;
		}
		if(startDistance<=0 && endDistance<=0){
			// both are behind this plane, it will get clipped by another one
			continue;
		}

		if(startDistance>endDistance){
			// line is entering into the brush
			scalar fraction=Math::div(startDistance-mEpsilon, startDistance-endDistance);
			if(fraction>startFraction){
				startFraction=fraction;
				startNormal=plane.normal;
			}
		}
		else{
			// line is leaving the brush
			scalar fraction=Math::div(startDistance+mEpsilon, startDistance-endDistance);
			if(fraction<endFraction){
				endFraction=fraction;
			}
		}
	}

	if(startsOut==false){
//		outputStartsOut=false;
//		if(endsOut==false){
//			outputAllSolid=true;
//		}
		return;
	}

	if(startFraction<endFraction){
		if(startFraction>-Math::ONE && startFraction<result){
			if(startFraction<0){
				startFraction=0;
			}
			result=startFraction;
			normal=startNormal;
		}
	}
}

void BSPSceneNode::decompressVIS(){
	int count;
	int i,c,index;
	unsigned char bit;

	if(mBSPMap->visibility.size()==0){
		return; // No vis data to decompress
	}

	leafVisibility.resize(mBSPMap->leaves.size());

	for(i=0;i<mBSPMap->leaves.size();i++){
		int v=mBSPMap->leaves[i].visibilityStart;
		if(v>=0){
			count=0;

			// first count the number of visible leafs...
			for(c=1;c<mBSPMap->trees[0].visleafs;v++){
				if(mBSPMap->visibility[v]==0){
					v++;
					c+=(mBSPMap->visibility[v]<<3);
				}
				else{
					for (bit = 1; bit; bit<<=1,c++){
						if(mBSPMap->visibility[v]&bit){
							count++;
						}
					}
				}
			}

			// allocate space to store the uncompressed VIS bit set...
			leafVisibility[i].resize(count);
		}
	}

	// now go through the VIS bit set again and store the VIS leafs...
	for(i=0;i<mBSPMap->leaves.size();i++){
		int v=mBSPMap->leaves[i].visibilityStart;
		if(v>=0){
			index=0;

			for(c=1;c<mBSPMap->trees[0].visleafs;v++){
				if(mBSPMap->visibility[v]==0){
					v++;
					c+=(mBSPMap->visibility[v]<<3);
				}
				else{
					for(bit=1;bit;bit<<=1,c++){
						if(mBSPMap->visibility[v]&bit){
							leafVisibility[i][index]=c;
							index++;
						}
					}
				}
			}
		}
	}
}

void BSPSceneNode::processVisibleFaces(CameraNode *camera){
	int i;
	RendererData &data=(RendererData&)mRendererData;

	// clear stuff
	memset(&data.markedFaces[0],0,data.markedFaces.size()*sizeof(unsigned char));
	data.textureVisibleFaces.resize(textures.size());

	// find leaf we're in
	int idx=findLeaf(camera->getRenderWorldTranslate());

	// If no visibility information just test all leaves
	if(idx==0){ // TODO: Replace with an mHasVisData, and do this too if no visdata
		for(i=0;i<mBSPMap->leaves.size();i++){
			addLeafToVisible(mBSPMap->leaves[i],data,camera);
		}
	}
	else{
		// go thru leaf visibility list
		for(i=0;i<leafVisibility[idx].size();i++){
			addLeafToVisible(mBSPMap->leaves[leafVisibility[idx][i]],data,camera);
		}
	}
}

void BSPSceneNode::renderVisibleFaces(Renderer *renderer){
	int i,j;
	RendererData &data=(RendererData&)mRendererData;
	TextureStage::ptr textureStage(new TextureStage());
	textureStage->setMinFilter(TextureStage::Filter_LINEAR);
	textureStage->setMipFilter(TextureStage::Filter_LINEAR);
	textureStage->setMagFilter(TextureStage::Filter_LINEAR);

	renderer->setLighting(false);
	renderer->setFaceCulling(Renderer::FaceCulling_FRONT);
	renderer->setDepthWrite(true);
	renderer->setModelMatrix(Math::IDENTITY_MATRIX4X4);

	TextureStage::ptr lightmapStage(new TextureStage());
	lightmapStage->setTexCoordIndex(1);
	lightmapStage->setBlend(TextureBlend(TextureBlend::Operation_MODULATE,TextureBlend::Source_PREVIOUS,TextureBlend::Source_TEXTURE));
	lightmapStage->setMinFilter(TextureStage::Filter_LINEAR);
	lightmapStage->setMipFilter(TextureStage::Filter_LINEAR);
	lightmapStage->setMagFilter(TextureStage::Filter_LINEAR);
	lightmapStage->setUAddressMode(TextureStage::AddressMode_CLAMP_TO_EDGE);
	lightmapStage->setVAddressMode(TextureStage::AddressMode_CLAMP_TO_EDGE);

	for(i=0;i<data.textureVisibleFaces.size();i++){
		Collection<int> &visibleFaces=data.textureVisibleFaces[i];

		if(visibleFaces.size()>0){
if(textures.size()>i) textureStage->setTexture(textures[i]);
			renderer->setTextureStage(0,textureStage);

			for(j=0;j<visibleFaces.size();++j){
				int vf=visibleFaces[j];

//				lightmapStage->setTexture(mRenderFaces[vf].lightmap);
//				renderer->setTextureStage(1,lightmapStage);

				renderer->renderPrimitive(mVertexData,mRenderFaces[vf].indexData);
			}
		}

		visibleFaces.clear();
	}
}

void BSPSceneNode::addLeafToVisible(const Leaf &leaf,RendererData &data,CameraNode *camera) const{
	if(camera->culled(leaf.bound)==false){
		const unsigned short* p=&mBSPMap->marksurfaces[leaf.marksurfaceStart];

		for(int x=0;x<leaf.marksurfaceCount;x++){
			// don't render those already rendered
			int face_idx=*p++;

			if(!(data.markedFaces[face_idx>>3] & (1<<(face_idx & 7)))){
				// back face culling
				const Face &f=mBSPMap->faces[face_idx];

				float d=Math::length(mBSPMap->planes[f.plane],camera->getRenderWorldTranslate());
				if(f.side){
					if(d>0) continue;
				}
				else{
					if(d<0) continue;
				}

				// mark face as visible
				data.markedFaces[face_idx>>3] |= (1<<(face_idx & 7));

				const BSPMap::texinfo &texinfo=mBSPMap->texinfos[f.texinfo];

				if((texinfo.flags&TEX_SPECIAL)==0){
if(data.textureVisibleFaces.size()<=texinfo.miptex){
	data.textureVisibleFaces.resize(texinfo.miptex+1);
}
					data.textureVisibleFaces[texinfo.miptex].push_back(face_idx);
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

void BSPSceneNode::calculateSurfaceExtents(const Face &face,Vector2 &mins,Vector2 &maxs){
	float val;
	int i,j,e;

	Vector3 v;
	mins[0]=mins[1]=999999;
	maxs[0]=maxs[1]=-999999;

	const BSPMap::texinfo &tex=mBSPMap->texinfos[face.texinfo];

	for(i=0;i<face.edgeCount;i++){
		e=mBSPMap->surfedges[face.edgeStart + i];
		if(e>=0){
			v=mBSPMap->vertexes[mBSPMap->edges[e].v[0]];
		}
		else{
			v=mBSPMap->vertexes[mBSPMap->edges[-e].v[1]];
		}

		for(j=0;j<2;j++){
			val=v.x * tex.vecs[j][0] +
				v.y * tex.vecs[j][1] +
				v.z * tex.vecs[j][2] +
				tex.vecs[j][3];
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

Image::ptr BSPSceneNode::computeLightmap(const Face &face,const Vector2 &mins,const Vector2 &maxs){
	if(mBSPMap->lighting.size()==0){return NULL;}

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

	memcpy(image->getData(),&mBSPMap->lighting[face.lightinfo],lsz);

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

