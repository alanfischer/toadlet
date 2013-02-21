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

#ifndef TOADLET_TADPOLE_STDUIO_SPRITETYPES_H
#define TOADLET_TADPOLE_STDUIO_SPRITETYPES_H

#include <toadlet/tadpole/Types.h>

namespace toadlet{
namespace tadpole{
namespace studio{

static const int IDSP=1347634249; // "IDSP"
const static int SPRITE_VERSION=2;

enum synctype{
	ST_SYNC=0,
	ST_RAND
};

struct spritehdr{
	int ident;
	int version;
	int type;
	int texFormat;
	float boundingradius;
	int width;
	int height;
	int numframes;
	float beamlength;
	synctype sync;
};

static const int SPRITE_VP_PARALLEL_UPRIGHT=	0;
static const int SPRITE_FACING_UPRIGHT=			1;
static const int SPRITE_VP_PARALLEL=			2;
static const int SPRITE_ORIENTED=				3;
static const int SPRITE_VP_PARALLEL_ORIENTED=	4;

static const int SPRITE_NORMAL=					0;
static const int SPRITE_ADDITIVE=				1;
static const int SPRITE_INDEXALPHA=				2;
static const int SPRITE_ALPHATEST=				3;

struct spriteframe{
	int origin[2];
	int width;
	int height;
};

struct spritegroup{
	int numframes;
};

struct spriteinterval{
	float interval;
};

enum spriteframetype{
	SFT_SINGLE=0,
	SFT_GROUP
};

}
}
}

#endif
