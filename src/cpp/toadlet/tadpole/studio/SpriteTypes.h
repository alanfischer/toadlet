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
	synctype synctype;
};

static const int SPRITE_VP_PARALLEL_UPRIGHT=	0;
static const int SPRITE_FACING_UPRIGHT=			1;
static const int SPRITE_VP_PARALLEL=			2;
static const int SPRITE_ORIENTED=				3;
static const int SPRITE_VP_PARALLEL_ORIENTED=	4;

static const int SPRITE_NORMAL=					0;
static const int SPRITE_ADDITIVE=				1;
static const int SPRITE_INDEXALPHA=				2;
static const int SPRITE_ALPHTEST=				3;

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
