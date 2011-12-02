/********** Copyright header - do not remove **********
 *
 * The Toadlet Engine 
 *
 * Copyright (C) 2005, Lightning Toads Productions, LLC
 *
 * Author(s): Alan Fischer
 * 
 * All source code for the Toadlet Engine, including 
 * this file, is the sole property of Lightning Toads 
 * Productions, LLC. It has been developed on our own 
 * time, with our own tools, on our own hardware. None
 * of this code may be observed, used or changed 
 * without our explicit permission. 
 *
 *  - Lightning Toads Productions, LLC
 *
 ********** Copyright header - do not remove **********/

#ifndef TOADLET_PEEPER_NDSTEXTUREPEER_H
#define TOADLET_PEEPER_NDSTEXTUREPEER_H

#include "../../Texture.h"
#include "../../CapabilitySet.h"
#include "NDSIncludes.h"

namespace toadlet{
namespace peeper{

class NDSTexturePeer:public TexturePeer{
public:
	NDSTexturePeer(Renderer *renderer,Texture *texture);

	virtual ~NDSTexturePeer();

	static void getTextureParameters(const Texture *texture,const CapabilitySet &capabilitySet,unsigned int &width,unsigned int &height,unsigned int &depth,int &dimension,int &internalFormat,int &format,int &type);

	int textureHandle;
	int textureDimension;
};

}
}

#endif
