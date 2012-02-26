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

#ifndef TOADLET_TADPOLE_STUDIO_SPRITEMODEL_H
#define TOADLET_TADPOLE_STUDIO_SPRITEMODEL_H

#include <toadlet/egg/BaseResource.h>
#include <toadlet/tadpole/studio/SpriteTypes.h>
#include <toadlet/tadpole/material/Material.h>

namespace toadlet{
namespace tadpole{
namespace studio{

class TOADLET_API SpriteModel:public BaseResource{
public:
	TOADLET_RESOURCE(SpriteModel,SpriteModel);

	SpriteModel():
		data(NULL),
		paletteSize(0),
		palette(NULL)
	{}
	
	void destroy(){
		if(data!=NULL){
			delete[] data;
			data=NULL;
		}
		palette=NULL;

		textures.clear();
		materials.clear();
	}

	spriteframetype *frameType(int i){
		tbyte *p=data+sizeof(spritehdr)+2+paletteSize*3;
		int j;
		for(j=0;j<i;++j){
			spriteframe *f=(spriteframe*)(p+sizeof(spriteframetype));
			p+=sizeof(spriteframetype)+sizeof(spriteframe)+f->width*f->height;
		}
		return (spriteframetype*)p;
	}

	spriteframe *frame(int i){
		tbyte *p=data+sizeof(spritehdr)+2+paletteSize*3+sizeof(spriteframetype);
		int j;
		for(j=0;j<i;++j){
			spriteframe *f=(spriteframe*)p;
			p+=sizeof(spriteframetype)+sizeof(spriteframe)+f->width*f->height;
		}
		return (spriteframe*)p;
	}

	tbyte *data;
	spritehdr *header;
	int paletteSize;
	tbyte *palette;
	Collection<Texture::ptr> textures;
	Collection<Material::ptr> materials;
};

}
}
}

#endif
