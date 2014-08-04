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

#ifndef TOADLET_TADPOLE_POINTTEXTURECREATOR_H
#define TOADLET_TADPOLE_POINTTEXTURECREATOR_H

#include <toadlet/tadpole/Engine.h>
#include <toadlet/tadpole/BaseResourceCreator.h>
#include <toadlet/peeper/Texture.h>

namespace toadlet{
namespace tadpole{

class TOADLET_API PointTextureCreator:public BaseResourceCreator{
public:
	TOADLET_OBJECT(PointTextureCreator);

	PointTextureCreator(Engine *engine){
		mEngine=engine;
	}

	void destroy(){}

	Resource::ptr create(const String &name,ResourceData *data){
		TextureFormat::ptr format=new TextureFormat(TextureFormat::Dimension_D2,TextureFormat::Format_LA_8,128,128,1,0);
		Resource::ptr resource=createPointTexture(format,0,1,0,1,1);
		resource->setName(name);
		return resource;
	}
	
	static bool createPointTexture(TextureFormat *format,tbyte *data,int xoffset,int yoffset,int width,int height,float colorOffset,float colorFactor,float alphaOffset,float alphaFactor,float falloff);
	Texture::ptr createPointTexture(TextureFormat *format,float colorOffset,float colorFactor,float alphaOffset,float alphaFactor,float falloff);

protected:
	Engine *mEngine;
};

}
}

#endif
