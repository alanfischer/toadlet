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

#ifndef TOADLET_TADPOLE_TERRAIN_DIFFUSETERRAINMATERIALSOURCE_H
#define TOADLET_TADPOLE_TERRAIN_DIFFUSETERRAINMATERIALSOURCE_H

#include <toadlet/tadpole/Engine.h>
#include <toadlet/tadpole/terrain/TerrainNodeMaterialSource.h>

namespace toadlet{
namespace tadpole{
namespace terrain{

class TOADLET_API DiffuseTerrainMaterialSource:protected Object,public TerrainNodeMaterialSource{
public:
	TOADLET_OBJECT(DiffuseTerrainMaterialSource);
	
	DiffuseTerrainMaterialSource(Engine *engine,const Vector3 &scale,const String &name);
	DiffuseTerrainMaterialSource(Engine *engine,const Vector3 &scale,Texture *texture=NULL);
	virtual ~DiffuseTerrainMaterialSource(){}

	void createShaders();
	void destroy();

	bool setDiffuseTexture(int layer,const String &name);
	bool setDiffuseTexture(int layer,Texture *texture);
	Texture::ptr getDiffuseTexture(int layer){return mDiffuseTextures.size()>layer?mDiffuseTextures[layer]:NULL;}

	Material::ptr getMaterial(TerrainPatchNode *patch);
	
protected:
	Engine *mEngine;
	Shader::ptr mDiffuseVertexShader,mDiffuseBaseFragmentShader,mDiffuseLayerFragmentShader;
	Vector3 mTextureScale;
	Collection<Texture::ptr> mDiffuseTextures;
};

}
}
}

#endif
