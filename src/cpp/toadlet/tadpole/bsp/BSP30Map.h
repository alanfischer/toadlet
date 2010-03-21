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

#ifndef TOADLET_TADPOLE_BSP_BSP30MAP_H
#define TOADLET_TADPOLE_BSP_BSP30MAP_H

#include <toadlet/egg/BaseResource.h>
#include <toadlet/tadpole/bsp/BSP30Types.h>
#include <toadlet/tadpole/Collision.h>
#include <stdlib.h>

namespace toadlet{
namespace tadpole{
namespace bsp{

class TOADLET_API BSP30Map:public egg::BaseResource{
public:
	TOADLET_BASERESOURCE_PASSTHROUGH(BaseResource);
	TOADLET_SHARED_POINTERS(BSP30Map);

	BSP30Map():
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
		entities(NULL),	nentities(0)
	{
		header.version=0;
	};
	
	void destroy(){
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
	}

	int modelTrace(Collision &result,int model,const Vector3 &size,const Vector3 &start,const Vector3 &end){
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
			hullTrace(result,planes,leafs,nodes,sizeof(bnode),headNode,0,Math::ONE,start,end,0.03125);
			contents=leafs[findPointLeaf(planes,nodes,sizeof(bnode),headNode,start)].contents;
		}
		else{
			hullTrace(result,planes,NULL,clipnodes,sizeof(bclipnode),headClipNode,0,Math::ONE,start,end,0.03125);
			contents=-1-findPointLeaf(planes,clipnodes,sizeof(bclipnode),headClipNode,start);
		}

		return contents;
	}

	static int findPointLeaf(bplane *planes,void *hull,int hullStride,int index,const Vector3 &point){
		scalar d=0;
		bclipnode *node=NULL;
		bplane *plane=NULL;

		while(index>=0){
			node=(bclipnode*)(((byte*)hull)+hullStride*index);
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

	static bool hullTrace(Collision &result,bplane *planes,bleaf *leafs,void *hull,int hullStride,int index,scalar p1t,scalar p2t,const Vector3 &p1,const Vector3 &p2,scalar epsilon){
		bclipnode *node=NULL;
		bplane *plane=NULL;
		scalar t1=0,t2=0;
		scalar time=0;
		Vector3 mid;
		scalar midt=0;
		int side=0;

		if(index<0){
			int contents=(leafs!=NULL)?(leafs[-1-index].contents):(index);
			result.scope=(-1-contents)<<1;
			if(contents==CONTENTS_SOLID){
				result.time=0;
				result.point.set(p1);
			}
			return true;
		}

		node=(bclipnode*)(((byte*)hull)+hullStride*index);
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
			return hullTrace(result,planes,leafs,hull,hullStride,node->children[0],p1t,p2t,p1,p2,epsilon);
		}
		if(t1<0 && t2<0){
			return hullTrace(result,planes,leafs,hull,hullStride,node->children[1],p1t,p2t,p1,p2,epsilon);
		}
#else
		if(t1>=epsilon && t2>=epsilon){
			return hullTrace(result,planes,leafs,hull,hullStride,node->children[0],p1t,p2t,p1,p2,epsilon);
		}
		if(t1<epsilon && t2<epsilon){
			return hullTrace(result,planes,leafs,hull,hullStride,node->children[1],p1t,p2t,p1,p2,epsilon);
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

		if(!hullTrace(result,planes,leafs,hull,hullStride,node->children[side],p1t,midt,p1,mid,epsilon)){
			return false;
		}

		int leaf=findPointLeaf(planes,hull,hullStride,node->children[side^1],mid);
		if(((leafs!=NULL)?(leafs[leaf].contents):(-1-leaf))!=CONTENTS_SOLID){
			return hullTrace(result,planes,leafs,hull,hullStride,node->children[side^1],midt,p2t,mid,p2,epsilon);
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

		// Sometimes we have to walk backwards
		while(true){
			int leaf=findPointLeaf(planes,hull,hullStride,0,mid);
			if(((leafs!=NULL)?(leafs[leaf].contents):(-1-leaf))!=CONTENTS_SOLID) break;

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

	template<typename Bound>
	static void findBoundLeafs(egg::Collection<int> &leafs,bnode *hull,int index,const Bound &bound){
		if(index<0){
			// Ignore solid leaf
			if((-1-index)>0){
				leafs.add(-1-index);
			}
			return;
		}

		bnode *node=hull+index;
		if(testIntersection(node,bound)){
			findBoundLeafs(leafs,hull,node->children[0],bound);
			findBoundLeafs(leafs,hull,node->children[1],bound);
		}
	}

	static bool testIntersection(bnode *node,const AABox &box){
		return	!(Math::fromInt(node->mins[0])>box.maxs.x || Math::fromInt(node->mins[1])>box.maxs.y || Math::fromInt(node->mins[2])>box.maxs.z ||
				  box.mins.x>Math::fromInt(node->maxs[0]) || box.mins.y>Math::fromInt(node->maxs[1]) || box.mins.z>Math::fromInt(node->maxs[2]));
	}

	static bool testIntersection(bnode *node,const Sphere &sphere){
		scalar s=0;
		scalar d=0;

		if(sphere.origin.x<Math::fromInt(node->mins[0])){
			s=sphere.origin.x-(node->mins[0]);
			d+=Math::mul(s,s);
		}
		else if(sphere.origin.x>Math::fromInt(node->maxs[0])){
			s=sphere.origin.x-Math::fromInt(node->maxs[0]);
			d+=Math::mul(s,s);
		}

		if(sphere.origin.y<Math::fromInt(node->mins[1])){
			s=sphere.origin.y-Math::fromInt(node->mins[1]);
			d+=Math::mul(s,s);
		}
		else if(sphere.origin.y>Math::fromInt(node->maxs[1])){
			s=sphere.origin.y-Math::fromInt(node->maxs[1]);
			d+=Math::mul(s,s);
		}

		if(sphere.origin.z<Math::fromInt(node->mins[2])){
			s=sphere.origin.z-Math::fromInt(node->mins[2]);
			d+=Math::mul(s,s);
		}
		else if(sphere.origin.z>Math::fromInt(node->maxs[2])){
			s=sphere.origin.z-Math::fromInt(node->maxs[2]);
			d+=Math::mul(s,s);
		}

		return d<=Math::mul(sphere.radius,sphere.radius);
	}

	struct facedata{
		peeper::IndexData::ptr indexData;
		Matrix4x4 lightmapTransform;
		facedata *next;
	};

	bheader header;
	bmodel *models;				int nmodels;
	bvertex *vertexes;			int nvertexes;
	bplane *planes;				int nplanes;
	bleaf *leafs;				int nleafs;
	bnode *nodes;				int nnodes;
	btexinfo *texinfos;			int ntexinfos;
	bclipnode *clipnodes;		int nclipnodes;
	bface *faces;				int nfaces;
	bmarksurface *marksurfaces;	int nmarksurfaces;
	bsurfedge *surfedges;		int nsurfedges;
	bedge *edges;				int nedges;
	void *visibility;			int nvisibility;
	void *textures;				int ntextures;
	void *lighting;				int nlighting;
	char *entities;				int nentities;

	bmiptexlump *miptexlump;
	egg::Collection<egg::Collection<int> > parsedVisibility;
	egg::Collection<egg::Map<egg::String,egg::String> > parsedEntities;
	egg::Collection<peeper::Texture::ptr> parsedTextures;
	egg::Collection<Material::ptr> materials;
	peeper::VertexData::ptr vertexData;
	egg::Collection<facedata> facedatas;
	peeper::Texture::ptr lightmap;
};

}
}
}

#endif
