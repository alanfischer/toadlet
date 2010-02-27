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

#include <toadlet/egg/Error.h>
#include <toadlet/tadpole/bsp/BSP30Handler.h>
#include <toadlet/tadpole/bsp/BSPMap.h>
#include <stdlib.h>
#include <string.h> // memset

using namespace toadlet::egg;
using namespace toadlet::egg::io;

namespace toadlet{
namespace tadpole{
namespace bsp{

BSP30Handler::BSP30Handler(Engine *engine):ResourceHandler(),
	mEngine(NULL)
{
	mEngine=engine;
	memset(&header,0,sizeof(header));
}

BSP30Handler::~BSP30Handler(){}

Resource::ptr BSP30Handler::load(Stream::ptr stream,const ResourceHandlerData *handlerData){
	DataStream::ptr dataStream(new DataStream(stream));
	header.version=dataStream->readLittleInt32();
	dataStream->reset();

	if(header.version!=BSPVERSION){
		Error::unknown(String("incorrect bsp version:")+header.version);
		return NULL;
	}

	Logger::debug(Categories::TOADLET_TADPOLE,"Reading map");

	stream->read((byte*)&header,sizeof(header));

	bmodel *models;		int nmodels;
	bvertex *vertexes;	int nvertexes;
	bplane *planes;		int nplanes;
	bleaf *leafs;		int nleafs;
	bnode *nodes;		int nnodes;
	btexinfo *texinfos;	int ntexinfos;
	bclipnode *clipnodes;int nclipnodes;
	bface *faces;		int nfaces;
	bmarksurface *marksurfaces;int nmarksurfaces;
	bsurfedge *surfedges;int nsurfedges;
	bedge *edges;		int nedges;
	void *visibility;	int nvisibility;
	void *textures;		int ntextures;
	void *lighting;		int nlighting;
	char *entities;		int nentities;

	readLump(stream,LUMP_MODELS,(void**)&models,sizeof(bmodel),&nmodels);
	readLump(stream,LUMP_VERTEXES,(void**)&vertexes,sizeof(bvertex),&nvertexes);
	readLump(stream,LUMP_PLANES,(void**)&planes,sizeof(bplane),&nplanes);
	readLump(stream,LUMP_LEAFS,(void**)&leafs,sizeof(bleaf),&nleafs);
	readLump(stream,LUMP_NODES,(void**)&nodes,sizeof(bnode),&nnodes);
	readLump(stream,LUMP_TEXINFO,(void**)&texinfos,sizeof(btexinfo),&ntexinfos);
	readLump(stream,LUMP_CLIPNODES,(void**)&clipnodes,sizeof(bclipnode),&nclipnodes);
	readLump(stream,LUMP_FACES,(void**)&faces,sizeof(bface),&nfaces);
	readLump(stream,LUMP_MARKSURFACES,(void**)&marksurfaces,sizeof(bmarksurface),&nmarksurfaces);
	readLump(stream,LUMP_SURFEDGES,(void**)&surfedges,sizeof(bsurfedge),&nsurfedges);
	readLump(stream,LUMP_EDGES,(void**)&edges,sizeof(bedge),&nedges);
	readLump(stream,LUMP_VISIBILITY,(void**)&visibility,1,&nvisibility);
	readLump(stream,LUMP_TEXTURES,(void**)&textures,1,&ntextures);
	readLump(stream,LUMP_LIGHTING,(void**)&lighting,1,&nlighting);
	readLump(stream,LUMP_ENTITIES,(void**)&entities,1,&nentities);

	#ifdef TOADLET_BIG_ENDIAN
		int i,j;
		for(i=0;i<nmodels;++i){
			bmodel &model=models[i];
			littleFloat(model.mins[0]);
			littleFloat(model.mins[1]);
			littleFloat(model.mins[2]);

			littleFloat(model.maxs[0]);
			littleFloat(model.maxs[1]);
			littleFloat(model.maxs[2]);

			littleFloat(model.origin[0]);
			littleFloat(model.origin[1]);
			littleFloat(model.origin[2]);

			for(j=0;j<MAX_MAP_HULLS;++j){
				littleInt32(model.headnode[j]);
			}

			littleInt32(model.visleafs);
			littleInt32(model.firstface);
			littleInt32(model.numfaces);
		}

		for(i=0;i<nvertexes;++i){
			bvertex &vertex=vertexes[i];
			littleFloat(vertex.point.x);
			littleFloat(vertex.point.y);
			littleFloat(vertex.point.z);
		}

		for(i=0;i<nplanes;++i){
			bplane &plane=planes[i];
			littleFloat(plane.dist);
			littleFloat(plane.normal.x);
			littleFloat(plane.normal.y);
			littleFloat(plane.normal.z);
			littleInt32(plane.type);
		}

		for(i=0;i<nleafs;++i){
			bleaf &leaf=leafs[i];
			littleInt32(leaf.contents);
			littleInt32(leaf.visofs)
			littleInt16(leaf.mins[0]);
			littleInt16(leaf.mins[1]);
			littleInt16(leaf.mins[2]);
			littleInt16(leaf.maxs[0]);
			littleInt16(leaf.maxs[1]);
			littleInt16(leaf.maxs[2]);
			littleUInt32(leaf.firstmarksurface);
			littleUInt32(leaf.nummarksurfaces);
		}

		for(i=0;i<nnodes;++i){
			bnode &node=nodes[i];
			littleInt32(node.planenum);
			littleInt16(node.children[0]);
			littleInt16(node.children[1]);
			littleInt16(node.mins[0]);
			littleInt16(node.mins[1]);
			littleInt16(node.mins[2]);
			littleInt16(node.maxs[0]);
			littleInt16(node.maxs[1]);
			littleInt16(node.maxs[2]);
			littleInt16(node.firstface);
			littleInt16(node.numfaces);
		}

		#error Need to convert rest of this
	#endif

	int i;
	BSPMap::ptr map(new BSPMap());

	map->vertexes.resize(nvertexes);
	memcpy(&map->vertexes[0],vertexes,nvertexes*sizeof(bvertex));

	map->planes.resize(nplanes);
	for(i=0;i<nplanes;++i){
		Plane &p=map->planes[i];
		bplane &pn=planes[i];

		p.normal.x=pn.normal[0];
		p.normal.y=pn.normal[1];
		p.normal.z=pn.normal[2];
		p.distance=pn.dist;
	}

	map->nodes.resize(nnodes);
	for(i=0;i<nnodes;++i){
		Node &n=map->nodes[i];
		bnode &bn=nodes[i];

		n.plane=bn.planenum;
		n.children[0]=bn.children[0];
		n.children[1]=bn.children[1];
		n.bound.mins[0]=bn.mins[0];
		n.bound.mins[1]=bn.mins[1];
		n.bound.mins[2]=bn.mins[2];
		n.bound.maxs[0]=bn.maxs[0];
		n.bound.maxs[1]=bn.maxs[1];
		n.bound.maxs[2]=bn.maxs[2];
	}

	map->leaves.resize(nleafs);
	for(i=0;i<nleafs;++i){
		Leaf &l=map->leaves[i];
		bleaf &bl=leafs[i];

		l.contents=bl.contents;
		l.visibilityStart=bl.visofs;
		l.bound.mins[0]=bl.mins[0];
		l.bound.mins[1]=bl.mins[1];
		l.bound.mins[2]=bl.mins[2];
		l.bound.maxs[0]=bl.maxs[0];
		l.bound.maxs[1]=bl.maxs[1];
		l.bound.maxs[2]=bl.maxs[2];
		l.marksurfaceStart=bl.firstmarksurface;
		l.marksurfaceCount=bl.nummarksurfaces;
	}

	map->faces.resize(nfaces);
	for(i=0;i<nfaces;++i){
		Face &f=map->faces[i];
		bface &bf=faces[i];

		f.plane=bf.planenum;
		f.side=bf.side>0;
		f.edgeStart=bf.firstedge;
		f.edgeCount=bf.numedges;
		f.texinfo=bf.texinfo;
		f.lightinfo=bf.lightofs;
	}

	map->trees.resize(nmodels);
	for(i=0;i<nmodels;++i){
		Tree &t=map->trees[i];
		bmodel &bm=models[i];

		t.bound.mins[0]=bm.mins[0];
		t.bound.mins[1]=bm.mins[1];
		t.bound.mins[2]=bm.mins[2];
		t.bound.mins[0]=bm.mins[0];
		t.bound.mins[1]=bm.mins[1];
		t.bound.mins[2]=bm.mins[2];
		t.nodeStart=bm.headnode[0];
		t.origin[0]=bm.origin[0];
		t.origin[1]=bm.origin[1];
		t.origin[2]=bm.origin[2];
		t.visleafs=bm.visleafs;
	}

	// Parse Entity data
	char *data=entities;
	Map<String,String> keyValues;
	while(*data!=0){
		if(*data=='{'){
			keyValues.clear();
		}
		else if(*data=='}'){
			map->entities.add(keyValues);
		}
		else if(*data=='\"'){
			char *key=++data;
			while(*data!='\"') data++;
			*data=0;

			while(*data!='\"') data++;

			char *value=++data;
			while(*data!='\"') data++;
			*data=0;

			keyValues[key]=value;
		}
		data++;
	}

	// Hacky ones
	map->surfedges.resize(nsurfedges);
	memcpy(&map->surfedges[0],surfedges,nsurfedges*sizeof(int));

	map->edges.resize(nedges);
	memcpy(&map->edges[0],edges,nedges*sizeof(bedge));

	map->visibility.resize(nvisibility);
	memcpy(&map->visibility[0],visibility,nvisibility);

	map->marksurfaces.resize(nmarksurfaces);
	memcpy(&map->marksurfaces[0],marksurfaces,nmarksurfaces*sizeof(bmarksurface));

	map->texinfos.resize(ntexinfos);
	memcpy(&map->texinfos[0],texinfos,ntexinfos*sizeof(btexinfo));

	map->textures.resize(ntextures);
	memcpy(&map->textures[0],textures,ntextures);

	map->lighting.resize(nlighting);
	memcpy(&map->lighting[0],lighting,nlighting);

	// Add clipnodes as a hack till we sort out our collision
	map->clipnodes.resize(nclipnodes);
	memcpy(&map->clipnodes[0],clipnodes,nclipnodes*sizeof(bclipnode));

	Logger::debug(Categories::TOADLET_TADPOLE,"Reading map finished ");

	return map;
}

void BSP30Handler::readLump(Stream *stream,int lump,void **data,int size,int *count){
	int length=header.lumps[lump].filelen;
	int ofs=header.lumps[lump].fileofs;

	stream->seek(ofs);
	*data=malloc(length);
	stream->read((byte*)*data,length);

	if(count!=NULL){
		*count=length/size;
	}
}

}
}
}

