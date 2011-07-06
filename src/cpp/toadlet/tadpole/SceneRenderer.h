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

#ifndef TOADLET_TADPOLE_SCENERENDERER_H
#define TOADLET_TADPOLE_SCENERENDERER_H

#include <toadlet/tadpole/RenderableSet.h>
#include <toadlet/tadpole/node/CameraNode.h>

namespace toadlet{
namespace tadpole{

class TOADLET_API SceneRenderer{
public:
	TOADLET_SHARED_POINTERS(SceneRenderer);

	SceneRenderer(Scene *scene);
	virtual ~SceneRenderer();

	virtual void renderScene(RenderDevice *device,Node *node,CameraNode *camera);
	virtual RenderDevice *getDevice(){return mDevice;}
	virtual void setupPass(RenderPass *pass);

protected:
	virtual void gatherRenderables(RenderableSet *set,Node *node,CameraNode *camera);
	virtual void renderRenderables(RenderableSet *set,RenderDevice *renderDevice,CameraNode *camera,bool useMaterials=true);
	virtual void renderDepthSortedRenderables(RenderableSet *set,RenderDevice *renderDevice,CameraNode *camera,bool useMaterials);
	virtual void renderQueueItems(Material *material,const RenderableSet::RenderableQueueItem *items,int numItems);

	void setupViewport(CameraNode *camera,RenderDevice *renderDevice);
	void setupLights(const RenderableSet::LightQueue &lightQueue,RenderDevice *renderDevice);

	Scene *mScene;
	RenderDevice *mDevice;
	SceneParameters::ptr mSceneParameters;
	RenderableSet::ptr mRenderableSet;
};

}
}

#endif
