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

#ifndef TOADLET_TADPOLE_TERRAIN_TEXTUREDATASOURCE_H
#define TOADLET_TADPOLE_TERRAIN_TEXTUREDATASOURCE_H

#include <toadlet/tadpole/Types.h>
#include <toadlet/tadpole/Engine.h>
#include <toadlet/tadpole/terrain/TerrainNodeDataSource.h>

namespace toadlet{
namespace tadpole{
namespace terrain{

class TOADLET_API TextureDataSource:public Object,public TerrainNodeDataSource{
public:
	TOADLET_OBJECT(TextureDataSource);

	TextureDataSource(Engine *engine,const Vector3 &scale,const String &name);
	TextureDataSource(Engine *engine,const Vector3 &scale,Texture *texture=NULL);
	virtual ~TextureDataSource();
	
	bool setTexture(const String &name,int px,int py);
	bool setTexture(Texture *texture,int px,int py);

	int getPatchSize(){return mPatchSize;}
	const Vector3 &getPatchScale(){return mPatchScale;}
	bool getPatchHeightData(scalar *data,int px,int py);
	bool getPatchLayerData(tbyte *data,int px,int py);
	
protected:
	Engine *mEngine;
	int mPatchSize;
	Vector3 mPatchScale;
	TextureFormat::ptr mTextureFormat,mFormat;
	tbyte *mTextureData,*mData;
	Map<int,Map<int,Texture::ptr> > mTexture;
};

}
}
}

#endif
