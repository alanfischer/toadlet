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

#ifndef TOADLET_TADPOLE_SIMPLERENDERMANAGER_H
#define TOADLET_TADPOLE_SIMPLERENDERMANAGER_H

#include <toadlet/tadpole/RenderManager.h>
#include <toadlet/tadpole/RenderableSet.h>

namespace toadlet{
namespace tadpole{

class TOADLET_API SimpleRenderManager:public Object,public RenderManager{
public:
	TOADLET_OBJECT(SimpleRenderManager);

	SimpleRenderManager(Scene *scene);
	virtual ~SimpleRenderManager();

	virtual void renderScene(RenderDevice *device,Node *node,Camera *camera);
	virtual RenderDevice *getDevice(){return mDevice;}
	virtual void setupPass(RenderPass *pass);
	virtual void setupPassForRenderable(RenderPass *pass,Renderable *renderable,const Vector4 &ambient);

protected:
	virtual void gatherRenderables(RenderableSet *set,Node *node,Camera *camera);
	virtual void renderRenderables(RenderableSet *set,RenderDevice *device,Camera *camera,bool useMaterials=true);
	virtual void renderDepthSortedRenderables(const RenderableSet::RenderableQueue &queue,bool useMaterials);
	virtual void renderQueueItems(Material *material,const RenderableSet::RenderableQueueItem *items,int numItems);

	void setupViewport(Camera *camera,RenderDevice *device);
	void setupLights(const RenderableSet::LightQueue &lightQueue,RenderDevice *device);
	void setupTextures(RenderPass *pass,int scope,RenderDevice *device);
	void setupVariableBuffers(RenderPass *pass,int scope,RenderDevice *device);

	Scene *mScene;
	RenderDevice *mDevice;
	SceneParameters::ptr mSceneParameters;
	RenderableSet::ptr mRenderableSet;
	RenderPass *mLastPass;
	RenderState *mLastRenderState;
	ShaderState *mLastShaderState;
};

}
}

#endif
