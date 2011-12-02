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

#ifndef TOADLET_TADPOLE_STDUIO_STUDIOTYPES_H
#define TOADLET_TADPOLE_STDUIO_STUDIOTYPES_H

#include <toadlet/tadpole/Types.h>

namespace toadlet{
namespace tadpole{
namespace studio{

typedef float vec3_t[3];
typedef void *cache_user_t;

static const int IDST=1414743113; // "IDST"
static const int IDSQ=1364411465; // "IDSQ"

static const int MAXSTUDIOTRIANGLES=20000;
static const int MAXSTUDIOVERTS=2048;
static const int MAXSTUDIOSEQUENCES=256;
static const int MAXSTUDIOSKINS=100;
static const int MAXSTUDIOSRCBONES=512;
static const int MAXSTUDIOBONES=128;
static const int MAXSTUDIOMODELS=32;
static const int MAXSTUDIOBODYPARTS=32;
static const int MAXSTUDIOGROUPS=4;
static const int MAXSTUDIOANIMATIONS=512;
static const int MAXSTUDIOMESHES=256;
static const int MAXSTUDIOEVENTS=1024;
static const int MAXSTUDIOPIVOTS=256;
static const int MAXSTUDIOCONTROLLERS=8;

struct studiohdr{
	int					id;
	int					version;

	char				name[64];
	int					length;

	vec3_t				eyeposition;	// ideal eye position
	vec3_t				min;			// ideal movement hull size
	vec3_t				max;			

	vec3_t				bbmin;			// clipping bounding box
	vec3_t				bbmax;		

	int					flags;

	int					numbones;			// bones
	int					boneindex;

	int					numbonecontrollers;		// bone controllers
	int					bonecontrollerindex;

	int					numhitboxes;			// complex bounding boxes
	int					hitboxindex;			
	
	int					numseq;				// animation sequences
	int					seqindex;

	int					numseqgroups;		// demand loaded sequences
	int					seqgroupindex;

	int					numtextures;		// raw textures
	int					textureindex;
	int					texturedataindex;

	int					numskinref;			// replaceable textures
	int					numskinfamilies;
	int					skinindex;

	int					numbodyparts;		
	int					bodypartindex;

	int					numattachments;		// queryable attachable points
	int					attachmentindex;

	int					soundtable;
	int					soundindex;
	int					soundgroups;
	int					soundgroupindex;

	int					numtransitions;		// animation node to animation node transition graph
	int					transitionindex;
};

// header for demand loaded sequence group data
struct studioseqhdr{
	int					id;
	int					version;

	char				name[64];
	int					length;
};

// bones
struct studiobone{
	char				name[32];	// bone name for symbolic links
	int		 			parent;		// parent bone
	int					flags;		// ??
	int					bonecontroller[6];	// bone controller index, -1 == none
	float				value[6];	// default DoF values
	float				scale[6];   // scale for delta DoF values
};


// bone controllers
struct studiobonecontroller{
	int					bone;	// -1 == 0
	int					type;	// X, Y, Z, XR, YR, ZR, M
	float				start;
	float				end;
	int					rest;	// byte index value at rest
	int					index;	// 0-3 user set controller, 4 mouth
};

// intersection boxes
struct studiobbox{
	int					bone;
	int					group;			// intersection group
	vec3_t				bbmin;		// bounding box
	vec3_t				bbmax;		
};

// demand loaded sequence groups
struct studioseqgroup{
	char				label[32];	// textual name
	char				name[64];	// file name
	cache_user_t		cache;		// cache index pointer
	int					data;		// hack for group 0
};

// sequence descriptions
struct studioseqdesc{
	char				label[32];	// sequence label

	float				fps;		// frames per second	
	int					flags;		// looping/non-looping flags

	int					activity;
	int					actweight;

	int					numevents;
	int					eventindex;

	int					numframes;	// number of frames per sequence

	int					numpivots;	// number of foot pivots
	int					pivotindex;

	int					motiontype;	
	int					motionbone;
	vec3_t				linearmovement;
	int					automoveposindex;
	int					automoveangleindex;

	vec3_t				bbmin;		// per sequence bounding box
	vec3_t				bbmax;		

	int					numblends;
	int					animindex;		// mstudioanim_t pointer relative to start of sequence group data
										// [blend][bone][X, Y, Z, XR, YR, ZR]

	int					blendtype[2];	// X, Y, Z, XR, YR, ZR
	float				blendstart[2];	// starting value
	float				blendend[2];	// ending value
	int					blendparent;

	int					seqgroup;		// sequence group for demand loading

	int					entrynode;		// transition node at entry
	int					exitnode;		// transition node at exit
	int					nodeflags;		// transition rules
	
	int					nextseq;		// auto advancing sequences
};

// events
struct studioevent{
	int 				frame;
	int					event;
	int					type;
	char				options[64];
};

// pivots
struct studiopivot{
	vec3_t				org;	// pivot point
	int					start;
	int					end;
};

// attachment
struct studioattachment{
	char				name[32];
	int					type;
	int					bone;
	vec3_t				org;	// attachment point
	vec3_t				vectors[3];
};

struct studioanim{
	unsigned short	offset[6];
};

// animation frames
union studioanimvalue{
	struct {
		tbyte	valid;
		tbyte	total;
	} num;
	short		value;
};

// body part index
struct studiobodyparts{
	char				name[64];
	int					nummodels;
	int					base;
	int					modelindex; // index into models array
};

// skin info
struct studiotexture{
	char					name[64];
	int						flags;
	int						width;
	int						height;
	int						index;
};

// skin families
// short	index[skinfamilies][skinref]

// studio models
struct studiomodel{
	char				name[64];

	int					type;

	float				boundingradius;

	int					nummesh;
	int					meshindex;

	int					numverts;		// number of unique vertices
	int					vertinfoindex;	// vertex bone info
	int					vertindex;		// vertex vec3_t
	int					numnorms;		// number of unique surface normals
	int					norminfoindex;	// normal bone info
	int					normindex;		// normal vec3_t

	int					numgroups;		// deformation groups
	int					groupindex;
};

// vec3_t	boundingbox[model][bone][2];	// complex intersection info

// meshes
struct studiomesh{
	int					numtris;
	int					triindex;
	int					skinref;
	int					numnorms;		// per mesh normals
	int					normindex;		// normal vec3_t
};

// lighting options
static const int STUDIO_NF_FLATSHADE=	0x0001;
static const int STUDIO_NF_CHROME=		0x0002;
static const int STUDIO_NF_FULLBRIGHT=	0x0004;

// motion flags
static const int STUDIO_X=		0x0001;
static const int STUDIO_Y=		0x0002;
static const int STUDIO_Z=		0x0004;
static const int STUDIO_XR=		0x0008;
static const int STUDIO_YR=		0x0010;
static const int STUDIO_ZR=		0x0020;
static const int STUDIO_LX=		0x0040;
static const int STUDIO_LY=		0x0080;
static const int STUDIO_LZ=		0x0100;
static const int STUDIO_AX=		0x0200;
static const int STUDIO_AY=		0x0400;
static const int STUDIO_AZ=		0x0800;
static const int STUDIO_AXR=	0x1000;
static const int STUDIO_AYR=	0x2000;
static const int STUDIO_AZR=	0x4000;
static const int STUDIO_TYPES=	0x7FFF;
static const int STUDIO_RLOOP=	0x8000;	// controller that wraps shortest distance

// sequence flags
static const int STUDIO_LOOPING=0x0001;

// bone flags
static const int STUDIO_HAS_NORMALS=	0x0001;
static const int STUDIO_HAS_VERTICES=	0x0002;
static const int STUDIO_HAS_BBOX=		0x0004;
static const int STUDIO_HAS_CHROME=		0x0008;	// if any of the textures have chrome on them

}
}
}

#endif
