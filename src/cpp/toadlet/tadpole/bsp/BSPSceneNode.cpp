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

using namespace toadlet::egg;
using namespace toadlet::peeper;

namespace toadlet{
namespace tadpole{
namespace bsp{

BSPSceneNode::BSPSceneNode():ParentNode(){}

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

	VertexFormat::ptr vertexFormat(new VertexFormat(4));
	vertexFormat->addVertexElement(VertexElement(VertexElement::Type_POSITION,VertexElement::Format_BIT_FLOAT_32|VertexElement::Format_BIT_COUNT_3));
	vertexFormat->addVertexElement(VertexElement(VertexElement::Type_NORMAL,VertexElement::Format_BIT_FLOAT_32|VertexElement::Format_BIT_COUNT_3));
	vertexFormat->addVertexElement(VertexElement(VertexElement::Type_TEX_COORD,VertexElement::Format_BIT_FLOAT_32|VertexElement::Format_BIT_COUNT_2));
	vertexFormat->addVertexElement(VertexElement(VertexElement::Type_TEX_COORD_2,VertexElement::Format_BIT_FLOAT_32|VertexElement::Format_BIT_COUNT_2));
	VertexBuffer::ptr vertexBuffer=mEngine->getBufferManager()->createVertexBuffer(Buffer::UsageFlags_STATIC,Buffer::AccessType_WRITE_ONLY,vertexFormat,mBSPMap->vertexes.size());
	mVertexData=VertexData::ptr(new VertexData(vertexBuffer));

	mRenderFaces.resize(mBSPMap->faces.size());

	int vertexCount=0;
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
/*
		const texinfo_t &texinfo=texInfos[face.texinfo];
		int width=0,height=0;
		dmiptexlump_t *mipTexLump=(dmiptexlump_t*)&texData[0];
		int miptexofs=mipTexLump->dataofs[texinfo.miptex];
		if(miptexofs!=-1){
			miptex_t *mipTex=(miptex_t*)(&texData[miptexofs]);
			width=mipTex->width;
			height=mipTex->height;
		}
		Vector2 mins,maxs;
		calculateSurfaceExtents(&face,mins,maxs);

		Image::ptr lightmap;
		unsigned int lmwidth=0,lmheight=0;

		if((texinfo.flags&TEX_SPECIAL)==0){
			lightmap=computeLightmap(&face,mins,maxs);
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

			mRenderFaces[i].lightmap=engine->getTextureManager()->createTexture(lightmap);
			mRenderFaces[i].lightmap->retain();
		}

		if(width>0 && height>0){
			float is=1.0f/(float)width;
			float it=1.0f/(float)height;

			for(j=0;j<face.numedges;j++){
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
*/
		vertexCount+=face.edgeCount;
	}
	vba.unlock();

	mRendererData.markedFaces.resize((mBSPMap->faces.size()+7)/8);
}

int BSPSceneNode::findLeaf(const Vector3 &point) const{
int BSPSceneNode::findLeaf(const Vector3 &coords) const{
	int i=models[0].headnode[0];

	while(i>=0){
		const dnode_t *n=&nodes[i];
		const dplane_t *p=&planes[n->planenum];

		float d;

		// f(x,y,z) = Ax+Ay+Az-D
		d=p->normal[0]*coords[0]
			+p->normal[1]*coords[1]
			+p->normal[2]*coords[2]
			-p->dist;

		if(d>=0){  // in front or on the plane
			i=n->children[0];
		}
		else{  // behind the plane
			i=n->children[1];
		}
	}

	return -(i+1);
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
	if(idx==0 || visData.size()==0){ // TODO: Replace with an mHasVisData
		for(i=0;i<leafs.size();i++){
			addLeafToVisible(leafs[i],data,camera);
		}
	}
	else{
		const vis_leaf_t &vis_leaf=leafVisibility[idx];
		// go thru leaf visibility list
		for(i=0;i<vis_leaf.num_leafs;i++){
			addLeafToVisible(leafs[vis_leaf.leafs[i]],data,camera);
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
			textureStage->setTexture(textures[i]);
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


void BSPSceneNode::addLeafToVisible(const dleaf_t &leaf,RendererData &data,CameraNode *camera) const{
	if(camera->culled(leaf.bound)==false){
		const unsigned short* p=&markSurfaces[leaf.firstmarksurface];

		int x;		for(x=0;x<leaf.nummarksurfaces;x++){
			// don't render those already rendered
			int face_idx=*p++;

			if(!(data.markedFaces[face_idx>>3] & (1<<(face_idx & 7)))){
				// back face culling
				const dface_t &f=faces[face_idx];

				// The planes should be converted to our type
				float d=Math::dot(camera->getRenderWorldTranslate(),*(Vector3*)planes[f.planenum].normal) - planes[f.planenum].dist;

				if(f.side){
					if(d>0) continue;
				}
				else{
					if(d<0) continue;
				}

				// mark face as visible
				data.markedFaces[face_idx>>3] |= (1<<(face_idx & 7));

				const texinfo_t &texinfo=texInfos[f.texinfo];

				if((texinfo.flags&TEX_SPECIAL)==0){
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
 


}
}
}

