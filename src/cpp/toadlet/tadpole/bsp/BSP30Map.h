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
#include <toadlet/egg/Logger.h>
#include <toadlet/tadpole/bsp/BSP30Types.h>
#include <toadlet/tadpole/Collision.h>
#include <toadlet/tadpole/Material.h>
#include <toadlet/tadpole/TextureManager.h>
#include <stdlib.h>

namespace toadlet{
namespace tadpole{
namespace bsp{

class TOADLET_API BSP30Map:public egg::BaseResource{
public:
	TOADLET_BASERESOURCE_PASSTHROUGH(BaseResource);
	TOADLET_SHARED_POINTERS(BSP30Map);

	const static int LIGHTMAP_SIZE=256;

	BSP30Map();	
	void destroy();

	int modelCollisionTrace(Collision &result,int model,const Vector3 &size,const Vector3 &start,const Vector3 &end);
	bool modelLightTrace(peeper::Color &result,int model,const Vector3 &start,const Vector3 &end);
	void findSurfaceExtents(bface *face,int *mins,int *maxs);

	void initLightmap();
	int allocLightmap(int st[],int width,int height);
	void uploadLightmap(TextureManager *manager);

	static int findPointLeaf(bplane *planes,void *hull,int hullStride,int index,const Vector3 &point);
	static bool hullTrace(Collision &result,bplane *planes,bleaf *leafs,void *hull,int hullStride,int index,scalar p1t,scalar p2t,const Vector3 &p1,const Vector3 &p2,scalar epsilon,int stopContentsBits,int *lastIndex);
	static void findBoundLeafs(egg::Collection<int> &leafs,bnode *hull,int index,const AABox &box);

	static bool testIntersection(bnode *node,const AABox &box){
		return	!(Math::fromInt(node->mins[0])>box.maxs.x || Math::fromInt(node->mins[1])>box.maxs.y || Math::fromInt(node->mins[2])>box.maxs.z ||
				  box.mins.x>Math::fromInt(node->maxs[0]) || box.mins.y>Math::fromInt(node->maxs[1]) || box.mins.z>Math::fromInt(node->maxs[2]));
	}

	struct facedata{
		facedata():index(0),lightmapIndex(0),visible(true),next(NULL){}

		int index;
		peeper::IndexData::ptr indexData;
		int lightmapIndex;
		bool visible;
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
	egg::image::Image::ptr lightmapImage;
	egg::Collection<peeper::Texture::ptr> lightmapTextures;
	int lightmapAllocated[LIGHTMAP_SIZE];
};

}
}
}

#endif
