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
#include <toadlet/tadpole/material/RenderPathChooser.h>

namespace toadlet{
namespace tadpole{

class Engine;
class BufferManager;

class TOADLET_API MaterialManager:public ResourceManager,public RenderStateDestroyedListener,public ShaderStateDestroyedListener{
public:
	MaterialManager(Engine *engine,bool backable);

	void destroy();

	Material::ptr createMaterial(Material::ptr source=Material::ptr());
	Material::ptr cloneMaterial(Material::ptr source);
	Material::ptr findMaterial(const String &name){return shared_static_cast<Material>(ResourceManager::find(name));}

	inline void setDiffuseCreator(ResourceCreator::ptr creator){mDiffuseCreator=creator;}
	inline void setSkyBoxCreator(ResourceCreator::ptr creator){mSkyBoxCreator=creator;}
	Material::ptr createDiffuseMaterial(Texture::ptr texture);
	Material::ptr createPointSpriteMaterial(Texture::ptr texture,scalar size,bool attenuated);
	Material::ptr createSkyBoxMaterial(Texture::ptr texture);

	RenderState::ptr createRenderState();
	ShaderState::ptr createShaderState();

	void modifyRenderState(RenderState::ptr dst,RenderState::ptr src);
	void modifyShaderState(ShaderState::ptr dst,ShaderState::ptr src);

	void setRenderPathChooser(RenderPathChooser *chooser);
	inline RenderPathChooser *getRenderPathChooser() const{return mRenderPathChooser;}

	void setDefaultSamplerState(const SamplerState &samplerState){mDefaultSamplerState.set(samplerState);}
	inline const SamplerState &getDefaultSamplerState() const{return mDefaultSamplerState;}

	void contextActivate(RenderDevice *renderDevice);
	void contextDeactivate(RenderDevice *renderDevice);

	void renderStateDestroyed(RenderState *renderState);
	void shaderStateDestroyed(ShaderState *shaderState);

	Resource::ptr unableToFindStreamer(const String &name,ResourceData *data);

	bool isPathUseable(RenderPath *path,const RenderCaps &caps);
	bool compileMaterial(Material *material);

	inline Engine *getEngine(){return mEngine;}
	BufferManager *getBufferManager();

protected:
	Engine *mEngine;
	bool mBackable;

	Collection<RenderState::ptr> mRenderStates;
	Collection<ShaderState::ptr> mShaderStates;

	RenderPathChooser *mRenderPathChooser;
	SamplerState mDefaultSamplerState;
	ResourceCreator::ptr mDiffuseCreator,mSkyBoxCreator;
};

}
}

#endif
