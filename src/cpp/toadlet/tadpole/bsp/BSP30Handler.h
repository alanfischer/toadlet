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

#ifndef TOADLET_TADPOLE_BSP_BSP30HANDLER_H
#define TOADLET_TADPOLE_BSP_BSP30HANDLER_H

#include <toadlet/tadpole/Engine.h>
#include <toadlet/tadpole/ResourceHandler.h>

namespace toadlet{
namespace tadpole{
namespace bsp{

class BSP30Handler:public ResourceHandler{
public:
	TOADLET_SHARED_POINTERS(BSP30Handler);

	BSP30Handler(Engine *engine);
	virtual ~BSP30Handler();

	virtual egg::Resource::ptr load(egg::io::InputStream::ptr in,const ResourceHandlerData *handlerData);

protected:
	static const int BSPVERSION=30;
	static const int MAX_MAP_HULLS=4;

	enum{
		LUMP_ENTITIES,
		LUMP_PLANES,
		LUMP_TEXTURES,
		LUMP_VERTEXES,
		LUMP_VISIBILITY,
		LUMP_NODES,
		LUMP_TEXINFO,
		LUMP_FACES,
		LUMP_LIGHTING,
		LUMP_CLIPNODES,
		LUMP_LEAFS,
		LUMP_MARKSURFACES,
		LUMP_EDGES,
		LUMP_SURFEDGES,
		LUMP_MODELS,
		MAX_LUMPS
	};

	struct blump{
		int fileofs,filelen;
	};

	struct bheader{
		int version;
		blump lumps[MAX_LUMPS];
	};

	struct bmodel{
		float mins[3], maxs[3];
		float origin[3];
		int headnode[MAX_MAP_HULLS];
		int visleafs;   // not including the solid leaf 0
		int firstface, numfaces;
	};

	struct bvertex{
		float point[3];
	};

	enum{
		PLANE_X,
		PLANE_Y,
		PLANE_Z,
		PLANE_ANYX,
		PLANE_ANYY,
		PLANE_ANYZ,
	};

	struct bplane{
		float normal[3];
		float dist;
		int type; // PLANE_X - PLANE_ANYZ
	};

	enum{
		CONTENTS_EMPTY=	-1,
		CONTENTS_SOLID=	-2,
		CONTENTS_WATER=	-3,
		CONTENTS_SLIME=	-4,
		CONTENTS_LAVAL=	-5,
		CONTENTS_SKY=	-6,
		CONTENTS_ORIGIN=-7,
		CONTENTS_CLIP=	-8,
	};

	enum{
		AMBIENT_WATER,
		AMBIENT_SKY,
		AMBIENT_SLIME,
		AMBIENT_LAVA,
		MAX_AMBIENTS
	};

	struct bleaf{
		int contents;
		int visofs;	// -1 = no vis

		short mins[3];
		short maxs[3];

		unsigned short firstmarksurface;
		unsigned short nummarksurfaces;

		uint8 ambient_level[MAX_AMBIENTS];
	};

	struct bnode{
		int planenum;
		short children[2]; // <0 = (-leafs+1)
		short mins[3];
		short maxs[3];
		unsigned short firstface;
		unsigned short numfaces; // counting both sides
	};

	struct bclipnode{
		int planenum;
		short children[2]; // <0 = contents
	};

	struct btexinfo{
		float vecs[2][4]; // [s/t][xyz offset]
		int miptex;
		int flags;
	};

	static const int MAX_LIGHTMAPS=4;

	struct bface{
		short planenum;
		short side;

		int firstedge;
		short numedges;
		short texinfo;

		uint8 styles[MAX_LIGHTMAPS];
		int lightofs;
	};

	typedef unsigned short bmarksurface;

	typedef int bsurfedge;

	struct bedge{
		unsigned short v[2];
	};

	void readLump(egg::io::InputStream *in,int lump,void **data,int size,int *count);

	Engine *mEngine;

	bheader header;
};

}
}
}

#endif
