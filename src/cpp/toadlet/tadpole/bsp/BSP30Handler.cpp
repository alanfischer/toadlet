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

	BSP30Map::ptr map(new BSP30Map());

	readLump(stream,LUMP_MODELS,		(void**)&map->models,sizeof(bmodel),			&map->nmodels);
	readLump(stream,LUMP_VERTEXES,		(void**)&map->vertexes,sizeof(bvertex),			&map->nvertexes);
	readLump(stream,LUMP_PLANES,		(void**)&map->planes,sizeof(bplane),			&map->nplanes);
	readLump(stream,LUMP_LEAFS,			(void**)&map->leafs,sizeof(bleaf),				&map->nleafs);
	readLump(stream,LUMP_NODES,			(void**)&map->nodes,sizeof(bnode),				&map->nnodes);
	readLump(stream,LUMP_TEXINFO,		(void**)&map->texinfos,sizeof(btexinfo),		&map->ntexinfos);
	readLump(stream,LUMP_CLIPNODES,		(void**)&map->clipnodes,sizeof(bclipnode),		&map->nclipnodes);
	readLump(stream,LUMP_FACES,			(void**)&map->faces,sizeof(bface),				&map->nfaces);
	readLump(stream,LUMP_MARKSURFACES,	(void**)&map->marksurfaces,sizeof(bmarksurface),&map->nmarksurfaces);
	readLump(stream,LUMP_SURFEDGES,		(void**)&map->surfedges,sizeof(bsurfedge),		&map->nsurfedges);
	readLump(stream,LUMP_EDGES,			(void**)&map->edges,sizeof(bedge),				&map->nedges);
	readLump(stream,LUMP_VISIBILITY,	(void**)&map->visibility,1,						&map->nvisibility);
	readLump(stream,LUMP_TEXTURES,		(void**)&map->textures,1,						&map->ntextures);
	readLump(stream,LUMP_LIGHTING,		(void**)&map->lighting,1,						&map->nlighting);
	readLump(stream,LUMP_ENTITIES,		(void**)&map->entitydata,1,						&map->nentitydata);

	// Parse Entity data
	char *data=map->entitydata;
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

