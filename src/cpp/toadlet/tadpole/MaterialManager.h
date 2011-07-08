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

#ifndef TOADLET_TADPOLE_MATERIALMANAGER_H
#define TOADLET_TADPOLE_MATERIALMANAGER_H

#include <toadlet/tadpole/ResourceManager.h>
#include <toadlet/tadpole/material/Material.h>

namespace toadlet{
namespace tadpole{

class Engine;
class BufferManager;

class TOADLET_API MaterialManager:public ResourceManager,public RenderStateDestroyedListener,public ShaderStateDestroyedListener{
public:
	MaterialManager(Engine *engine,bool backable);

	void destroy();

	Material::ptr createMaterial(Material::ptr source=NULL);
	Material::ptr cloneMaterial(Material::ptr source);
	Material::ptr findMaterial(const String &name){return shared_static_cast<Material>(ResourceManager::find(name));}

	Material::ptr createDiffuseMaterial(Texture::ptr texture);
	Material::ptr createDiffusePointSpriteMaterial(Texture::ptr texture,scalar size,bool attenuated);
	Material::ptr createSkyboxMaterial(Texture::ptr texture);

	RenderState::ptr createRenderState();
	ShaderState::ptr createShaderState();

	void modifyRenderState(RenderState::ptr dst,RenderState::ptr src);
	void modifyShaderState(ShaderState::ptr dst,ShaderState::ptr src);

	void contextActivate(RenderDevice *renderDevice);
	void contextDeactivate(RenderDevice *renderDevice);

	void renderStateDestroyed(RenderState *renderState);
	void shaderStateDestroyed(ShaderState *shaderState);

	Resource::ptr unableToFindHandler(const String &name,const ResourceHandlerData *handlerData);

	void setDefaultSamplerState(const SamplerState &samplerState){mDefaultSamplerState.set(samplerState);}
	const SamplerState &getDefaultSamplerState(){return mDefaultSamplerState;}

	inline Engine *getEngine(){return mEngine;}
	BufferManager *getBufferManager();

protected:
	Engine *mEngine;
	bool mBackable;

	Collection<RenderState::ptr> mRenderStates;
	Collection<ShaderState::ptr> mShaderStates;

	SamplerState mDefaultSamplerState;
	Shader::ptr mFixedVertexShader,mFixedFragmentShader,mFixedGeometryShader;
};

}
}

#endif
