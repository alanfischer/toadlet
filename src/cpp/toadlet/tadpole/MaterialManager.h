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
#include <toadlet/tadpole/material/MaterialResource.h>

namespace toadlet{
namespace tadpole{

class Engine;

class TOADLET_API MaterialManager:public ResourceManager,public RenderStateDestroyedListener,public ShaderStateDestroyedListener{
public:
	MaterialManager(Engine *engine,bool backable);

	void destroy();

	MaterialResource::ptr getMaterialResource(int handle){return shared_static_cast<MaterialResource>(ResourceManager::get(handle));}
	MaterialResource::ptr findMaterialResource(const String &name){return shared_static_cast<MaterialResource>(ResourceManager::find(name));}
	MaterialResource::ptr createMaterialResource(Material *material,const String &name=(char*)NULL);

	Material::ptr createMaterial(int handle){return createMaterial(getMaterialResource(handle));}
	Material::ptr createMaterial(const String &name){return createMaterial(findMaterialResource(name));}
	Material::ptr createMaterial(MaterialResource *resource=NULL);
/*
	Material::ptr createMaterial(Texture::ptr texture=Texture::ptr(),bool clamped=false,Material::ptr shareSource=Material::ptr());
 
	Material::ptr findMaterial(const String &name){return shared_static_cast<Material>(ResourceManager::find(name));}

	Material::ptr cloneMaterial(Material::ptr material,bool manage,Material::ptr sharedSource=Material::ptr());
*/
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

protected:
	Engine *mEngine;
	bool mBackable;

	Collection<RenderState::ptr> mRenderStates;
	Collection<ShaderState::ptr> mShaderStates;

	SamplerState mDefaultSamplerState;
};

}
}

#endif
