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

#include <toadlet/tadpole/bsp/BSP30Map.h>
#include <string.h> // memset
#include <stdlib.h> // free

using namespace toadlet::egg;
using namespace toadlet::egg::image;
using namespace toadlet::peeper;

namespace toadlet{
namespace tadpole{
namespace bsp{

BSP30Map::BSP30Map(Engine *engine):
	models(NULL),		nmodels(0),
	vertexes(NULL),		nvertexes(0),
	planes(NULL),		nplanes(0),
	leafs(NULL),		nleafs(0),
	nodes(NULL),		nnodes(0),
	texinfos(NULL),		ntexinfos(0),
	clipnodes(NULL),	nclipnodes(0),
	faces(NULL),		nfaces(0),
	marksurfaces(NULL),	nmarksurfaces(0),
	surfedges(NULL),	nsurfedges(0),
	edges(NULL),		nedges(0),
	visibility(NULL),	nvisibility(0),
	textures(NULL),		ntextures(0),
	lighting(NULL),		nlighting(0),
	entities(NULL),		nentities(0)
{
	header.version=0;

	this->engine=engine;

	lightmapImage=Image::ptr(Image::createAndReallocate(Image::Dimension_D2,Image::Format_RGB_8,LIGHTMAP_SIZE,LIGHTMAP_SIZE));

	lightmapStage=engine->getMaterialManager()->createTextureStage(NULL);
	lightmapStage->setTexCoordIndex(1);
	lightmapStage->setBlend(TextureBlend(TextureBlend::Operation_MODULATE,TextureBlend::Source_PREVIOUS,TextureBlend::Source_TEXTURE));
};

void BSP30Map::destroy(){
	if(models!=NULL){		free(models);		nmodels=0;}
	if(vertexes!=NULL){		free(vertexes);		nvertexes=0;}
	if(planes!=NULL){		free(planes);		nplanes=0;}
	if(leafs!=NULL){		free(leafs);		nleafs=0;}
	if(nodes!=NULL){		free(nodes);		nnodes=0;}
	if(texinfos!=NULL){		free(texinfos);		ntexinfos=0;}
	if(clipnodes!=NULL){	free(clipnodes);	nclipnodes=0;}
	if(faces!=NULL){		free(faces);		nfaces=0;}
	if(marksurfaces!=NULL){	free(marksurfaces);	nmarksurfaces=0;}
	if(surfedges!=NULL){	free(surfedges);	nsurfedges=0;}
	if(edges!=NULL){		free(edges);		nedges=0;}
	if(visibility!=NULL){	free(visibility);	nvisibility=0;}
	if(textures!=NULL){		free(textures);		ntextures=0;}
	if(lighting!=NULL){		free(lighting);		nlighting=0;}
	if(entities!=NULL){		free(entities);		nentities=0;}

	int i;
	for(i=0;i<parsedTextures.size();++i){
		parsedTextures[i]->release();
	}
	parsedTextures.clear();

	for(i=0;i<materials.size();++i){
		materials[i]->release();
	}
	materials.clear();

	for(i=0;i<lightmapTextures.size();++i){
		lightmapTextures[i]->release();
	}
	lightmapTextures.clear();

	for(i=0;i<modelResources.size();++i){
		modelResources[i]->release();
	}
	modelResources.clear();
}

int BSP30Map::modelCollisionTrace(Collision &result,int model,const Vector3 &size,const Vector3 &start,const Vector3 &end){
	int hullIndex=0;
	if(header.version==Q1BSPVERSION){
		if(size[0]<3){
			hullIndex=0;
		}
		else if(size[0]<=32){
			hullIndex=1;
		}
		else{
			hullIndex=2;
		}
	}
	else{
		if(size[0]<3){
			hullIndex=0; // 0x0x0
		}
		else if(size[0]<=32){
			if(size[2]<54){ // Nearest of 36 or 72
				hullIndex=3; // 32x32x36
			}
			else{
				hullIndex=1; // 32x32x72
			}
		}
		else{
			hullIndex=2; // 64x64x64
		}
	}

	int headNode=models[model].headnode[0];
	int headClipNode=models[model].headnode[hullIndex];
	if(headNode<0 || headNode>=nnodes || headClipNode<0 || headClipNode>=nclipnodes){
		return 0;
	}

	int contents=0;
	if(hullIndex==0){
		hullTrace(result,planes,leafs,nodes,sizeof(bnode),headNode,0,Math::ONE,start,end,0.03125,(-1-CONTENTS_SOLID)<<1,NULL);
		contents=leafs[findPointLeaf(planes,nodes,sizeof(bnode),headNode,start)].contents;
	}
	else{
		hullTrace(result,planes,NULL,clipnodes,sizeof(bclipnode),headClipNode,0,Math::ONE,start,end,0.03125,(-1-CONTENTS_SOLID)<<1,NULL);
		contents=-1-findPointLeaf(planes,clipnodes,sizeof(bclipnode),headClipNode,start);
	}

	return contents;
}

bool BSP30Map::modelLightTrace(Vector4 &result,int model,const Vector3 &start,const Vector3 &end){
	int headNode=models[model].headnode[0];
	if(headNode<0 || headNode>=nnodes){
		return false;
	}

	Collision collision;
	int lastNode=-1;
	hullTrace(collision,planes,leafs,nodes,sizeof(bnode),headNode,0,Math::ONE,start,end,0.03125,(-1-CONTENTS_SOLID)<<1,&lastNode);
	if(lastNode>=0){
		int mins[2],maxs[2];
		bnode *node=nodes+lastNode;
		bface *face=faces+node->firstface;
		int i;
		for(i=0;i<node->numfaces;++i,++face){
			btexinfo *texinfo=texinfos+face->texinfo;
			if((texinfo->flags&TEX_SPECIAL)!=0) continue;
			if(face->lightofs==0) continue;
			
			findSurfaceExtents(face,mins,maxs);

			int s=Math::dot(collision.point,(Vector3&)texinfo->vecs[0]) + texinfo->vecs[0][3];
			int t=Math::dot(collision.point,(Vector3&)texinfo->vecs[1]) + texinfo->vecs[1][3];

			if(s<mins[0] || t<mins[1]) continue;
			if(s>maxs[0] || t>maxs[1]) continue;

			int lmwidth=((maxs[0]-mins[0])>>4)+1;
			tbyte *lightdata=((tbyte*)lighting) + face->lightofs + (lmwidth*((t-mins[1])>>4) + ((s-mins[0])>>4))*3;
			result.set(lightdata[0]/255.0,lightdata[1]/255.0,lightdata[2]/255.0,0);

			return true;
		}
	}
	return false;
}

void BSP30Map::findSurfaceExtents(bface *face,int *smins,int *smaxs){
	float val;
	int i,j,surfedge;
	bvertex *v;
	float mins[2],maxs[2];
	float bmin,bmax;

	mins[0]=mins[1]=999999;
	maxs[0]=maxs[1]=-999999;

	btexinfo *texinfo=texinfos+face->texinfo;
	for(i=0;i<face->numedges;i++){
		surfedge=surfedges[face->firstedge+i];
		v=&vertexes[surfedge<0?edges[-surfedge].v[1]:edges[surfedge].v[0]];

		for(j=0;j<2;j++){
			val=Math::dot((Vector3&)v->point,texinfo->vecs[j]) + texinfo->vecs[j][3];

			if(val<mins[j])	mins[j]=val;
			if(val>maxs[j])	maxs[j]=val;
		}
	}

	for(i=0;i<2;i++){
		bmin=Math::floor(mins[i]/16);
		bmax=Math::ceil(maxs[i]/16);

		smins[i]=bmin*16;
		smaxs[i]=bmax*16;
		if(smaxs[i]-smins[i]<16){
			smaxs[i]=smins[i]+16;
		}
	}
}

void BSP30Map::initLightmap(){
	memset(lightmapImage->getData(),0,lightmapImage->getSlicePitch());
	memset(lightmapAllocated,0,sizeof(lightmapAllocated));
}

void BSP30Map::uploadLightmap(){
	Texture::ptr texture=engine->getTextureManager()->createTexture(lightmapImage);
	texture->retain();
	lightmapTextures.add(texture);
}

void BSP30Map::renderFaces(Renderer *renderer,facedata *face){
	while(face!=NULL){
		if(face->visible){
			lightmapStage->setTexture(lightmapTextures[face->lightmapIndex]); /// @todo: Check to see if our lightmapIndex is the same as previous, and then not change
			renderer->setTextureStage(1,lightmapStage);
			renderer->renderPrimitive(vertexData,face->indexData);
		}
		face=face->next;
	}
}

int BSP30Map::allocLightmap(int st[],int width,int height){
	int i,j;
	int best=LIGHTMAP_SIZE,best2=0;

	for(i=0;i<LIGHTMAP_SIZE-width;++i){
		best2=0;
		for(j=0;j<width && lightmapAllocated[i+j]<best;++j){
			if(lightmapAllocated[i+j]>best2){
				best2=lightmapAllocated[i+j];
			}
		}
		if(j==width){
			st[0]=i;
			st[1]=best=best2;
		}
	}

	if(best+height>LIGHTMAP_SIZE){
		return -1;
	}

	for(i=0;i<width;i++){
		lightmapAllocated[st[0]+i]=best+height;
	}

	return lightmapTextures.size();
}

int BSP30Map::findPointLeaf(bplane *planes,void *hull,int hullStride,int index,const Vector3 &point){
	scalar d=0;
	bclipnode *node=NULL;
	bplane *plane=NULL;

	while(index>=0){
		node=(bclipnode*)(((tbyte*)hull)+hullStride*index);
		plane=planes+node->planenum;

		if(plane->type<3){
			d=point[plane->type] - plane->dist;
		}
		else{
			d=Math::dot(plane->normal,point) - plane->dist;
		}

		if(d<0){
			index=node->children[1];
		}
		else{
			index=node->children[0];
		}
	}

	return -1-index;
}

bool BSP30Map::hullTrace(Collision &result,bplane *planes,bleaf *leafs,void *hull,int hullStride,int index,scalar p1t,scalar p2t,const Vector3 &p1,const Vector3 &p2,scalar epsilon,int stopContentsBits,int *lastIndex){
	bclipnode *node=NULL;
	bplane *plane=NULL;
	scalar t1=0,t2=0;
	scalar time=0;
	Vector3 mid;
	scalar midt=0;
	int side=0;

	if(index<0){
		int contents=(leafs!=NULL)?(leafs[-1-index].contents):(index);
		int contentsBits=(-1-contents)<<1;
		result.scope=contentsBits;
		if((contentsBits&stopContentsBits)>0){
			result.time=0;
			result.point.set(p1);
		}
		return true;
	}

	node=(bclipnode*)(((tbyte*)hull)+hullStride*index);
	plane=planes+node->planenum;

	if(plane->type<3){
		t1=p1[plane->type]-plane->dist;
		t2=p2[plane->type]-plane->dist;
	}
	else{
		t1=Math::dot(plane->normal,p1)-plane->dist;
		t2=Math::dot(plane->normal,p2)-plane->dist;
	}

#if 1
	if(t1>=0 && t2>=0){
		return hullTrace(result,planes,leafs,hull,hullStride,node->children[0],p1t,p2t,p1,p2,epsilon,stopContentsBits,lastIndex);
	}
	if(t1<0 && t2<0){
		return hullTrace(result,planes,leafs,hull,hullStride,node->children[1],p1t,p2t,p1,p2,epsilon,stopContentsBits,lastIndex);
	}
#else
	if(t1>=epsilon && t2>=epsilon){
		return hullTrace(result,planes,leafs,hull,hullStride,node->children[0],p1t,p2t,p1,p2,epsilon,stopContentsBits,lastIndex);
	}
	if(t1<epsilon && t2<epsilon){
		return hullTrace(result,planes,leafs,hull,hullStride,node->children[1],p1t,p2t,p1,p2,epsilon,stopContentsBits,lastIndex);
	}
#endif

	if(t1<0){
		time=Math::div(t1+epsilon,t1-t2);
	}
	else{
		time=Math::div(t1-epsilon,t1-t2);
	}
	time=Math::clamp(0,Math::ONE,time);

	midt=p1t+Math::mul(p2t-p1t,time);
	mid.x=p1.x+Math::mul(p2.x-p1.x,time);
	mid.y=p1.y+Math::mul(p2.y-p1.y,time);
	mid.z=p1.z+Math::mul(p2.z-p1.z,time);

	side=(t1<0);

	if(!hullTrace(result,planes,leafs,hull,hullStride,node->children[side],p1t,midt,p1,mid,epsilon,stopContentsBits,lastIndex)){
		return false;
	}

	int leaf=findPointLeaf(planes,hull,hullStride,node->children[!side],mid);
	int contents=((leafs!=NULL)?(leafs[leaf].contents):(-1-leaf));
	int contentsBits=(-1-contents)<<1;
	if((contentsBits&CONTENTS_SOLID)==0){
		return hullTrace(result,planes,leafs,hull,hullStride,node->children[!side],midt,p2t,mid,p2,epsilon,stopContentsBits,lastIndex);
	}

	if(result.time==0){
		return false;
	}

	// Other side is solid, we have impact point
	if(!side){
		result.normal.set(plane->normal);
	}
	else{
		Math::neg(result.normal,plane->normal);
	}

	if(lastIndex!=NULL){
		*lastIndex=index;
	}

	// Sometimes we have to walk backwards
	while(true){
		leaf=findPointLeaf(planes,hull,hullStride,0,mid);
		contents=((leafs!=NULL)?(leafs[leaf].contents):(-1-leaf));
		contentsBits=(-1-contents)<<1;
		if((contentsBits&CONTENTS_SOLID)==0) break;

		time-=epsilon*4;
		if(time<0){
			result.time=midt;
			result.point.set(mid);
			return false;
		}

		midt=p1t+Math::mul(p2t-p1t,time);
		mid.x=p1.x+Math::mul(p2.x-p1.x,time);
		mid.y=p1.y+Math::mul(p2.y-p1.y,time);
		mid.z=p1.z+Math::mul(p2.z-p1.z,time);
	}

	result.time=midt;
	result.point.set(mid);

	return false;
}

void BSP30Map::findBoundLeafs(Collection<int> &leafs,bnode *hull,int index,const AABox &box){
	if(index<0){
		// Ignore solid leaf
		if((-1-index)>0){
			leafs.add(-1-index);
		}
		return;
	}

	bnode *node=hull+index;
	if(testIntersection(node,box)){
		findBoundLeafs(leafs,hull,node->children[0],box);
		findBoundLeafs(leafs,hull,node->children[1],box);
	}
}

}
}
}
