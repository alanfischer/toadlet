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
	DiffuseTerrainMaterialSource(Engine *engine,const String &name,const Vector3 &scale=Vector3(Math::THIRD,Math::THIRD,0),const Vector3 &offset=Vector3(-Math::TWO_THIRDS,-Math::TWO_THIRDS,0));
	DiffuseTerrainMaterialSource(Engine *engine,Texture *texture,const Vector3 &scale=Vector3(Math::THIRD,Math::THIRD,0),const Vector3 &offset=Vector3(-Math::TWO_THIRDS,-Math::TWO_THIRDS,0));
	virtual ~DiffuseTerrainMaterialSource(){}

	void createShaders();
	void destroy();

	void setDiffuseScale(const Vector3 &scale){mDiffuseScale=scale;}
	void setDiffuseOffset(const Vector3 &offset){mDiffuseOffset=offset;}
	bool setDiffuseTexture(int layer,const String &name);
	bool setDiffuseTexture(int layer,Texture *texture);
	Texture::ptr getDiffuseTexture(int layer){return mDiffuseTextures.size()>layer?mDiffuseTextures[layer]:NULL;}

	void setDetailScale(const Vector3 &scale){mDetailScale=scale;}
	void setDetailOffset(const Vector3 &offset){mDetailOffset=offset;}
	bool setDetailTexture(const String &name);
	bool setDetailTexture(Texture *texture);

	Material::ptr getMaterial(TerrainPatchComponent *patch);
	void getTextureMatrix(Matrix4x4 &result,TerrainPatchComponent *patch){getTextureMatrix(result,patch,false);}
	void getTextureMatrix(Matrix4x4 &result,TerrainPatchComponent *patch,bool detail);
	
protected:
	class TextureRequest:public Object,public ResourceRequest{
	public:
		TOADLET_IOBJECT(TextureRequest);

		TextureRequest(DiffuseTerrainMaterialSource *parent,int layer=-1):mParent(parent),mLayer(layer){}

		void resourceReady(Resource *resource){if(mLayer>=0){mParent->setDiffuseTexture(mLayer,(Texture*)resource);}else{mParent->setDetailTexture((Texture*)resource);}}
		void resourceException(const Exception &ex){}
		void resourceProgress(float progress){}

	protected:
		DiffuseTerrainMaterialSource *mParent;
		int mLayer;
	};

	Engine *mEngine;
	Shader::ptr mDiffuseVertexShader,mDiffuseBaseFragmentShader,mDiffuseLayerFragmentShader;
	ShaderState::ptr mDiffuseBaseState,mDiffuseLayerState;
	Vector3 mDiffuseScale,mDetailScale;
	Vector3 mDiffuseOffset,mDetailOffset;
	Collection<Texture::ptr> mDiffuseTextures;
	Texture::ptr mDetailTexture;
};

}
}
}

#endif
