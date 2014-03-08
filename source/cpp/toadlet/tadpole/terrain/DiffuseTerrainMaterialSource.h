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

class TOADLET_API DiffuseTerrainMaterialSource:public Object,public TerrainNodeMaterialSource{
public:
	TOADLET_IOBJECT(DiffuseTerrainMaterialSource);
	
	DiffuseTerrainMaterialSource(Engine *engine);
	virtual ~DiffuseTerrainMaterialSource(){}

	void createShaders();
	void destroy();

	void setDiffuseScale(const Vector3 &scale){mDiffuseScale=scale;}
	bool setDiffuseTexture(int layer,const String &name);
	bool setDiffuseTexture(int layer,Texture *texture);
	Texture::ptr getDiffuseTexture(int layer){return mDiffuseTextures.size()>layer?mDiffuseTextures[layer]:NULL;}

	void setDetailScale(const Vector3 &scale){mDetailScale=scale;}
	bool setDetailTexture(const String &name);
	bool setDetailTexture(Texture *texture);

	Material::ptr getMaterial(TerrainPatchComponent *patch);
	
protected:
	class TextureRequest:public Object,public ResourceRequest{
	public:
		TOADLET_IOBJECT(TextureRequest);

		TextureRequest(DiffuseTerrainMaterialSource *parent,int layer=-1):mParent(parent),mLayer(layer){}

		void resourceReady(Resource *resource){if(mLayer>=0){mParent->setDiffuseTexture(mLayer,(Texture*)resource);}else{mParent->setDetailTexture((Texture*)resource);}}
		void resourceException(const Exception &ex){}

	protected:
		DiffuseTerrainMaterialSource::ptr mParent;
		int mLayer;
	};

	Engine *mEngine;
	Shader::ptr mDiffuseVertexShader,mDiffuseBaseFragmentShader,mDiffuseLayerFragmentShader;
	ShaderState::ptr mDiffuseBaseState,mDiffuseLayerState;
	Vector3 mDiffuseScale,mDetailScale;
	Collection<Texture::ptr> mDiffuseTextures;
	Texture::ptr mDetailTexture;
};

}
}
}

#endif
