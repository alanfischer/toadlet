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
#include <stdlib.h>

namespace toadlet{
namespace tadpole{
namespace bsp{

class TOADLET_API BSP30Map:public egg::BaseResource{
	TOADLET_BASERESOURCE_PASSTHROUGH(BaseResource);
public:
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
	{};
	
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

	egg::Collection<egg::Collection<int> > parsedVisibility;
	egg::Collection<egg::Map<egg::String,egg::String>> parsedEntities;
	egg::Collection<peeper::Texture::ptr> parsedTextures;
};

}
}
}

#endif
