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

#include <toadlet/peeper/BackableRenderState.h>
#include <toadlet/tadpole/Material.h>
#include <toadlet/tadpole/ResourceManager.h>
#include <toadlet/tadpole/TextureManager.h>

namespace toadlet{
namespace tadpole{

class Engine;

class TOADLET_API MaterialManager:public ResourceManager,public peeper::RenderStateDestroyedListener{
public:
	MaterialManager(Engine *engine,bool backable);

	void destroy();

	Material::ptr createMaterial(peeper::Texture::ptr texture=peeper::Texture::ptr(),bool clamped=false,Material::ptr shareSource=Material::ptr());

	Material::ptr findMaterial(const egg::String &name){return egg::shared_static_cast<Material>(ResourceManager::find(name));}

	Material::ptr cloneMaterial(Material::ptr material,bool manage,Material::ptr sharedSource=Material::ptr());

	peeper::RenderState::ptr createRenderState();

	void contextActivate(peeper::Renderer *renderer);
	void contextDeactivate(peeper::Renderer *renderer);

	void renderStateDestroyed(peeper::RenderState *renderState);

	egg::Resource::ptr unableToFindHandler(const egg::String &name,const ResourceHandlerData *handlerData);

	void setDefaultSamplerState(const peeper::SamplerState &samplerState){mDefaultSamplerState.set(samplerState);}
	const peeper::SamplerState &getDefaultSamplerState(){return mDefaultSamplerState;}

protected:
	Engine *mEngine;
	bool mBackable;

	egg::Collection<peeper::RenderState::ptr> mRenderStates;

	peeper::SamplerState mDefaultSamplerState;
};

}
}

#endif
