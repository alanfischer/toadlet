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
#include <toadlet/peeper/CapabilitySet.h>
#include <toadlet/tadpole/bsp/BSP30Handler.h>
#include <toadlet/tadpole/handler/WADArchive.h>
#include <toadlet/tadpole/PixelPacker.h>
#include <stdlib.h>
#include <string.h> // memset

using namespace toadlet::egg;
using namespace toadlet::egg::io;
using namespace toadlet::egg::image;
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
		Error::unknown(Categories::TOADLET_TADPOLE_BSP,
			String("incorrect bsp version:")+version);
		return NULL;
	}

	Logger::debug(Categories::TOADLET_TADPOLE,"Reading map");

	BSP30Map::ptr map(new BSP30Map());

	stream->read((tbyte*)&map->header,sizeof(map->header));

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

	map->miptexlump=(bmiptexlump*)map->textures;

	parseVisibility(map);
	parseEntities(map);
	parseWADs(map);
	parseTextures(map);
	buildBuffers(map);
	buildMaterials(map);

	Logger::debug(Categories::TOADLET_TADPOLE,"Reading map finished");

	return map;
}

void BSP30Handler::readLump(Stream *stream,blump *lump,void **data,int size,int *count){
	int length=lump->filelen;
	int ofs=lump->fileofs;

	stream->seek(ofs);
	*data=malloc(length);
	stream->read((tbyte*)*data,length);

	if(count!=NULL){
		*count=length/size;
	}
}

void BSP30Handler::parseVisibility(BSP30Map *map){
	Logger::debug(Categories::TOADLET_TADPOLE,"Parsing visibility");

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
				if(((tbyte*)map->visibility)[v]==0){
					v++;
					c+=(((tbyte*)map->visibility)[v]<<3);
				}
				// Otherwise, we need to check each bit to see if that leaf is visible
				else{
					for(bit=1;bit;bit<<=1,c++){
						if(((tbyte*)map->visibility)[v]&bit){
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
				if(((tbyte*)map->visibility)[v]==0){
					v++;
					c+=(((tbyte*)map->visibility)[v]<<3);
				}
				// Otherwise, we need to check each bit to see if that leaf is visible
				else{
					for(bit=1;bit;bit<<=1,c++){
						if(((tbyte*)map->visibility)[v]&bit){
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
	Logger::debug(Categories::TOADLET_TADPOLE,"Parsing entities");

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
	Logger::debug(Categories::TOADLET_TADPOLE,"Parsing WADs");

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
	Logger::debug(Categories::TOADLET_TADPOLE,"Parsing textures");

	map->parsedTextures.resize(map->miptexlump->nummiptex);
	int i;
	for(i=0;i<map->miptexlump->nummiptex;i++){
		Texture::ptr texture;
		int miptexofs=map->miptexlump->dataofs[i];
		if(miptexofs!=-1){
			WADArchive::wmiptex *miptex=(WADArchive::wmiptex*)(&((tbyte*)map->textures)[miptexofs]);
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

void BSP30Handler::buildBuffers(BSP30Map *map){
	Logger::debug(Categories::TOADLET_TADPOLE,"Building buffers and lightmaps");

	int i,j;

	VertexFormat::ptr vertexFormat=mEngine->getBufferManager()->createVertexFormat();
	vertexFormat->addElement(VertexFormat::Semantic_POSITION,0,VertexFormat::Format_BIT_FLOAT_32|VertexFormat::Format_BIT_COUNT_3);
	vertexFormat->addElement(VertexFormat::Semantic_NORMAL,0,VertexFormat::Format_BIT_FLOAT_32|VertexFormat::Format_BIT_COUNT_3);
	vertexFormat->addElement(VertexFormat::Semantic_TEX_COORD,0,VertexFormat::Format_BIT_FLOAT_32|VertexFormat::Format_BIT_COUNT_2);
	vertexFormat->addElement(VertexFormat::Semantic_TEX_COORD,1,VertexFormat::Format_BIT_FLOAT_32|VertexFormat::Format_BIT_COUNT_2);
	VertexBuffer::ptr vertexBuffer=mEngine->getBufferManager()->createVertexBuffer(Buffer::Usage_BIT_STATIC,Buffer::Access_BIT_WRITE,vertexFormat,map->nsurfedges);

	/// @todo: Figure out maximum required size, or allow multiple images
	Image::ptr lightmapImage(Image::createAndReallocate(Image::Dimension_D2,Image::Format_RGB_8,1024,1024,0));
	if(lightmapImage==NULL){
		Error::insufficientMemory(Categories::TOADLET_TADPOLE_BSP,
			"insufficient memory for lightmapImage");
		return;
	}

	PixelPacker packer(lightmapImage->getData(),lightmapImage->getFormat(),lightmapImage->getWidth(),lightmapImage->getHeight());

	int width=0,height=0;
	float iwidth=0,iheight=0;
	float s=0,t=0;
	Vector2 surfmins,surfmaxs;
	float surfmids=0,surfmidt=0;
	int lmwidth=0,lmheight=0;
	float lmmids=0,lmmidt=0;
	float ilmwidth=0,ilmheight=0;
	float ls=0,lt=0;

	map->facedatas.resize(map->nfaces);
	VertexBufferAccessor vba;
	IndexBufferAccessor iba;
	vba.lock(vertexBuffer,Buffer::Access_BIT_WRITE);
	for(i=0;i<map->nfaces;i++){
		bface *face=&map->faces[i];

		map->facedatas[i].index=i;

		// Find texture width & height for this face.
		// We have to use the information from the actual BSP here, instead of the found texture,
		//  since the found texture may be of different dimensions than what was used when the BSP was compiled.
		btexinfo *texinfo=&map->texinfos[face->texinfo];
		int miptexofs=map->miptexlump->dataofs[texinfo->miptex];
		if(miptexofs!=-1){
			WADArchive::wmiptex *miptex=(WADArchive::wmiptex*)(&((tbyte*)map->textures)[miptexofs]);
			width=miptex->width;
			height=miptex->height;
			if(width>0 && height>0){
				iwidth=1.0f/(float)width;
				iheight=1.0f/(float)height;
			}
		}

		if((texinfo->flags&TEX_SPECIAL)==0){
			findSurfaceExtents(map,face,surfmins,surfmaxs);
			surfmids=(surfmins[0]+surfmaxs[0])/2.0f;
			surfmidt=(surfmins[1]+surfmaxs[1])/2.0f;
			lmwidth=(ceil(surfmaxs[0]/16.0) - floor(surfmins[0]/16.0)) + 1;
			lmheight=(ceil(surfmaxs[1]/16.0) - floor(surfmins[1]/16.0)) + 1;
			lmmids=(float)lmwidth/2.0;
			lmmidt=(float)lmheight/2.0;
			ilmwidth=1.0/(float)lmwidth;
			ilmheight=1.0/(float)lmheight;

			packer.insert(lmwidth,lmheight,((tbyte*)map->lighting)+face->lightofs,map->facedatas[i].lightmapTransform);
		}

		if(mEngine->getRenderer()==NULL || mEngine->getRenderer()->getCapabilitySet().triangleFan){
			map->facedatas[i].indexData=IndexData::ptr(new IndexData(IndexData::Primitive_TRIFAN,NULL,face->firstedge,face->numedges));
		}
		else{
			int indexes=(face->numedges-2)*3;
			IndexBuffer::ptr indexBuffer=mEngine->getBufferManager()->createIndexBuffer(Buffer::Usage_BIT_STATIC,Buffer::Access_BIT_WRITE,IndexBuffer::IndexFormat_UINT16,indexes);
			iba.lock(indexBuffer,Buffer::Access_BIT_WRITE);
			for(j=1;j<face->numedges-1;++j){
				iba.set((j-1)*3+0,face->firstedge);
				iba.set((j-1)*3+1,face->firstedge+j);
				iba.set((j-1)*3+2,face->firstedge+j+1);
			}
			iba.unlock();
			map->facedatas[i].indexData=IndexData::ptr(new IndexData(IndexData::Primitive_TRIS,indexBuffer,0,indexes));
		}

		for(j=0;j<face->numedges;j++){
			int faceedge=face->firstedge+j;
			int surfedge=map->surfedges[faceedge];
			bvertex *v=&map->vertexes[surfedge<0?map->edges[-surfedge].v[1]:map->edges[surfedge].v[0]];
			bplane *p=&map->planes[face->planenum];
			int side=-2*face->side+1;

			vba.set3(faceedge,0,v->point);

			// Flip the face normal around if necessary
			vba.set3(faceedge,1,side*p->normal[0],side*p->normal[1],side*p->normal[2]);

			// Project the texture using the projection vectors to find our texture coordinates
			s=Math::dot((Vector3)v->point,texinfo->vecs[0]) + texinfo->vecs[0][3];
			t=Math::dot((Vector3)v->point,texinfo->vecs[1]) + texinfo->vecs[1][3];
			// The texture projection information is not normalized when stored.  So normalize it here.
			vba.set2(faceedge,2,s*iwidth,t*iheight);

			// Calculate lightmap coordinates
			if((texinfo->flags&TEX_SPECIAL)==0){
				ls=(lmmids + (s-surfmids)/16.0);
				lt=(lmmidt + (t-surfmidt)/16.0);
				Vector3 lc(ls*ilmwidth,lt*ilmheight,Math::ONE);
				Math::mulPoint3Fast(lc,map->facedatas[i].lightmapTransform);
				vba.set2(faceedge,3,lc.x,lc.y);
			}
		}
	}
	vba.unlock();

	map->vertexData=VertexData::ptr(new VertexData(vertexBuffer));

	map->lightmap=mEngine->getTextureManager()->createTexture(lightmapImage);
	map->lightmap->retain();
}

void BSP30Handler::buildMaterials(BSP30Map *map){
	Logger::debug(Categories::TOADLET_TADPOLE,"Building materials");

	map->materials.resize(map->miptexlump->nummiptex);
	int i;
	for(i=0;i<map->miptexlump->nummiptex;i++){
		Material::ptr material=mEngine->getMaterialManager()->createMaterial();
		material->retain();
		material->setLighting(false);
		material->setFaceCulling(Renderer::FaceCulling_FRONT);
		material->setDepthWrite(true);

		TextureStage::ptr primary=mEngine->getMaterialManager()->createTextureStage(map->parsedTextures[i]);
		material->setTextureStage(0,primary);

		TextureStage::ptr secondary=mEngine->getMaterialManager()->createTextureStage(map->lightmap);
		secondary->setTexCoordIndex(1);
		secondary->setBlend(TextureBlend(TextureBlend::Operation_MODULATE,TextureBlend::Source_PREVIOUS,TextureBlend::Source_TEXTURE));
		material->setTextureStage(1,secondary);

		map->materials[i]=material;
	}
}

void BSP30Handler::findSurfaceExtents(BSP30Map *map,bface *face,Vector2 &mins,Vector2 &maxs){
	float val;
	int i,j,surfedge;
	bvertex *v;

	mins[0]=mins[1]=999999;
	maxs[0]=maxs[1]=-999999;

	btexinfo *texinfo=&map->texinfos[face->texinfo];
	for(i=0;i<face->numedges;i++){
		surfedge=map->surfedges[face->firstedge+i];
		v=&map->vertexes[surfedge<0?map->edges[-surfedge].v[1]:map->edges[surfedge].v[0]];

		for(j=0;j<2;j++){
			val=Math::dot((Vector3)v->point,texinfo->vecs[j]) + texinfo->vecs[j][3];

			if(val<mins[j])	mins[j]=val;
			if(val>maxs[j])	maxs[j]=val;
		}
	}
}

}
}
}

