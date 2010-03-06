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
		entitydata(NULL),	nentitydata(0)
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
		if(entitydata!=NULL){	free(entitydata);	nentitydata=0;}
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
	char *entitydata;			int nentitydata;

	egg::Collection<egg::Map<egg::String,egg::String>> entities;
};

}
}
}

#endif
