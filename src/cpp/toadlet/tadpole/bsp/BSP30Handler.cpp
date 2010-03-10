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
#include <toadlet/tadpole/handler/WADArchive.h>
#include <stdlib.h>
#include <string.h> // memset

using namespace toadlet::egg;
using namespace toadlet::egg::io;
using namespace toadlet::peeper;
using namespace toadlet::tadpole::handler;

namespace toadlet{
namespace tadpole{
namespace bsp{

BSP30Handler::BSP30Handler(Engine *engine):ResourceHandler(),
	mEngine(NULL)
{
	mEngine=engine;
}

BSP30Handler::~BSP30Handler(){}

Resource::ptr BSP30Handler::load(Stream::ptr stream,const ResourceHandlerData *handlerData){
	DataStream::ptr dataStream(new DataStream(stream));
	int version=dataStream->readLittleInt32();
	dataStream->reset();

	if(version!=Q1BSPVERSION && version!=HLBSPVERSION){
		Error::unknown(String("incorrect bsp version:")+version);
		return NULL;
	}

	Logger::debug(Categories::TOADLET_TADPOLE,"Reading map");

	BSP30Map::ptr map(new BSP30Map());

	stream->read((byte*)&map->header,sizeof(map->header));

	readLump(stream,&map->header.lumps[LUMP_MODELS],		(void**)&map->models,sizeof(bmodel),			&map->nmodels);
	readLump(stream,&map->header.lumps[LUMP_VERTEXES],		(void**)&map->vertexes,sizeof(bvertex),			&map->nvertexes);
	readLump(stream,&map->header.lumps[LUMP_PLANES],		(void**)&map->planes,sizeof(bplane),			&map->nplanes);
	readLump(stream,&map->header.lumps[LUMP_LEAFS],			(void**)&map->leafs,sizeof(bleaf),				&map->nleafs);
	readLump(stream,&map->header.lumps[LUMP_NODES],			(void**)&map->nodes,sizeof(bnode),				&map->nnodes);
	readLump(stream,&map->header.lumps[LUMP_TEXINFO],		(void**)&map->texinfos,sizeof(btexinfo),		&map->ntexinfos);
	readLump(stream,&map->header.lumps[LUMP_CLIPNODES],		(void**)&map->clipnodes,sizeof(bclipnode),		&map->nclipnodes);
	readLump(stream,&map->header.lumps[LUMP_FACES],			(void**)&map->faces,sizeof(bface),				&map->nfaces);
	readLump(stream,&map->header.lumps[LUMP_MARKSURFACES],	(void**)&map->marksurfaces,sizeof(bmarksurface),&map->nmarksurfaces);
	readLump(stream,&map->header.lumps[LUMP_SURFEDGES],		(void**)&map->surfedges,sizeof(bsurfedge),		&map->nsurfedges);
	readLump(stream,&map->header.lumps[LUMP_EDGES],			(void**)&map->edges,sizeof(bedge),				&map->nedges);
	readLump(stream,&map->header.lumps[LUMP_VISIBILITY],	(void**)&map->visibility,1,						&map->nvisibility);
	readLump(stream,&map->header.lumps[LUMP_TEXTURES],		(void**)&map->textures,1,						&map->ntextures);
	readLump(stream,&map->header.lumps[LUMP_LIGHTING],		(void**)&map->lighting,1,						&map->nlighting);
	readLump(stream,&map->header.lumps[LUMP_ENTITIES],		(void**)&map->entities,1,						&map->nentities);

	parseVisibility(map);
	parseEntities(map);
	parseWADs(map);
	parseTextures(map);

	return map;
}

void BSP30Handler::readLump(Stream *stream,blump *lump,void **data,int size,int *count){
	int length=lump->filelen;
	int ofs=lump->fileofs;

	stream->seek(ofs);
	*data=malloc(length);
	stream->read((byte*)*data,length);

	if(count!=NULL){
		*count=length/size;
	}
}

void BSP30Handler::parseVisibility(BSP30Map *map){
	int count;
	int i,c,index;
	unsigned char bit;

	if(map->nvisibility==0){
		return; // No vis data to decompress
	}

	map->parsedVisibility.resize(map->nleafs);

	// First allocate space for each leaf
	for(i=0;i<map->nleafs;i++){
		int v=map->leafs[i].visofs;
		if(v>=0){
			count=0;

			// We enumerate through all possible leafs
			for(c=1;c<map->models[0].visleafs;v++){
				// If the whole byte is zero, that means the nextvis*8 leafs are invisible, so skip them
				if(((byte*)map->visibility)[v]==0){
					v++;
					c+=(((byte*)map->visibility)[v]<<3);
				}
				// Otherwise, we need to check each bit to see if that leaf is visible
				else{
					for(bit=1;bit;bit<<=1,c++){
						if(((byte*)map->visibility)[v]&bit){
							count++;
						}
					}
				}
			}

			map->parsedVisibility[i].resize(count);
		}
	}

	// Now actually store the leaf vis info
	for(i=0;i<map->nleafs;i++){
		int v=map->leafs[i].visofs;
		if(v>=0){
			index=0;

			// We enumerate through all possible leafs
			for(c=1;c<map->models[0].visleafs;v++){
				// If the whole byte is zero, that means the nextvis*8 leafs are invisible, so skip them
				if(((byte*)map->visibility)[v]==0){
					v++;
					c+=(((byte*)map->visibility)[v]<<3);
				}
				// Otherwise, we need to check each bit to see if that leaf is visible
				else{
					for(bit=1;bit;bit<<=1,c++){
						if(((byte*)map->visibility)[v]&bit){
							map->parsedVisibility[i][index]=c;
							index++;
						}
					}
				}
			}
		}
	}
}

void BSP30Handler::parseEntities(BSP30Map *map){
	char *data=map->entities;
	Map<String,String> keyValues;
	while(*data!=0){
		if(*data=='{'){
			keyValues.clear();
		}
		else if(*data=='}'){
			map->parsedEntities.add(keyValues);
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
}

void BSP30Handler::parseWADs(BSP30Map *map){
	if(map->parsedEntities.size()>0 && map->parsedEntities[0]["classname"].equals("worldspawn")){
		String wad=map->parsedEntities[0]["wad"];
		int start=0,end=0;
		while((end=wad.find(".wad",end+1))!=-1){
			start=Math::maxVal(wad.rfind('\\',end)+1,wad.rfind(';',end)+1);
			String file=wad.substr(start,(end-start)+4);
			Archive::ptr archive=mEngine->getArchiveManager()->findArchive(file);
			if(archive!=NULL){
				mEngine->getTextureManager()->addResourceArchive(archive);
			}
		}
	}
}

void BSP30Handler::parseTextures(BSP30Map *map){
	bmiptexlump *lump=(bmiptexlump*)map->textures;
	map->parsedTextures.resize(lump->nummiptex);
	int i;
	for(i=0;i<lump->nummiptex;i++){
		Texture::ptr texture;
		if(lump->dataofs[i]!=-1){
			WADArchive::wmiptex *miptex=(WADArchive::wmiptex*)(&((byte*)map->textures)[lump->dataofs[i]]);
			texture=WADArchive::createTexture(mEngine->getTextureManager(),miptex);
			if(texture==NULL){
				texture=mEngine->getTextureManager()->findTexture(miptex->name);
			}
			if(texture!=NULL){
				texture->retain();
			}
		}
		map->parsedTextures[i]=texture;
	}
}

}
}
}

