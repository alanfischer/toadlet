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

#include <toadlet/egg/Logger.h>
#include <toadlet/tadpole/DecalShadowSceneRenderer.h>
#include <toadlet/tadpole/Engine.h>
#include <toadlet/tadpole/Scene.h>

using namespace toadlet;
using namespace toadlet::egg;
using namespace toadlet::peeper;
using namespace toadlet::tadpole::node;

namespace toadlet{
namespace tadpole{

DecalShadowSceneRenderer::DecalShadowSceneRenderer(Scene *scene):
	SceneRenderer(scene)
{
	mShadowMaterial=mScene->getEngine()->getMaterialManager()->createMaterial();
	mPlane.set(Math::Z_UNIT_VECTOR3,0);
}

DecalShadowSceneRenderer::~DecalShadowSceneRenderer(){
}

void DecalShadowSceneRenderer::renderScene(Renderer *renderer,Node *node,CameraNode *camera){
	gatherRenderables(mRenderableSet,node,camera);
	renderRenderables(mRenderableSet,renderer,camera);

	RenderableSet *set=mRenderableSet;
	LightNode *light=NULL;
	if(set->getLightQueue().size()>0){
		light=set->getLightQueue()[0].light;
	}

	if(light==NULL){
		return;
	}

	const LightState &state=light->getLightState();

	mShadowMaterial->setupRenderer(renderer);

	int i,j;
	for(i=0;i<set->getNumRenderableQueues();++i){
		const RenderableSet::RenderableQueue &queue=set->getRenderableQueue(i);
		for(j=0;j<queue.size();++j){
			const RenderableSet::RenderableQueueItem &item=queue[j];
			Renderable *renderable=item.renderable;

			Matrix4x4 m;
			if(state.type==LightState::Type_DIRECTION){
				Math::setMatrix4x4FromObliquePlane(m,mPlane,state.direction);
			}
			else{
				Math::setMatrix4x4FromPerspectivePlane(m,mPlane,state.position);
			}
			
			Matrix4x4 m2;
			renderable->getRenderTransform().getMatrix(m2);
			Math::postMul(m,m2);
			
			renderer->setModelMatrix(m);
			renderable->render(renderer);
		}
	}
}

}
}
