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

#ifndef TOADLET_TADPOLE_ENTITY_SCENE_H
#define TOADLET_TADPOLE_ENTITY_SCENE_H

#include <toadlet/peeper/Color.h>
#include <toadlet/peeper/RenderTarget.h>
#include <toadlet/peeper/TextureStage.h>
#include <toadlet/tadpole/Renderable.h>
#include <toadlet/tadpole/UpdateListener.h>
#include <toadlet/tadpole/entity/ParentEntity.h>
#include <toadlet/tadpole/entity/LightEntity.h>
#include <toadlet/tadpole/entity/ParticleEntity.h>

namespace toadlet{
namespace tadpole{
namespace entity{

class CameraEntity;

/// @todo  I think a good way to let us use EventKeyFrames with the Controller framework is to:
///   1: Have a way of ensuring that the EventKeyFrames are aligned on LogicDT times.
///   2: Have the AnimationControllerEntity not issue a visualUpdate until we have enough logicUpdates to keep up with it.
///  That way, entities without EventKeyFrames should update smoothly if something keeps the logic from flowing, but
///   we will still be able to have any logic dependent stuff happen at the correct times.
///
///  Or perhaps, we could use the above tactic for all the update entities.  There is just 1 function, update
///   But it stops being called if logic updates cease due to networking issues.
///   Down sides are that its harder to do logic based updating, since you'd have to handle the checking to see if its
///    been a logicDT yourself.

class TOADLET_API Scene:public ParentEntity{
public:
	TOADLET_ENTITY(Scene,ParentEntity);

	Scene();
	virtual ~Scene();
	virtual Entity *create(Engine *engine);
	virtual void destroy();

	inline const ParentEntity::ptr &getBackground() const{return mBackground;}

	virtual void setAmbientColor(peeper::Color ambientColor);
	const peeper::Color &getAmbientColor(){return mAmbientColor;}

	virtual void setLayerClearing(int layer,bool clear);
	inline bool getLayerClearing(int layer){return getRenderLayer(layer)->clearLayer;}

	virtual void setIdealParticleViewTransform(const Matrix4x4 &transform){mIdealParticleViewTransform.set(transform);}
	virtual const Matrix4x4 &getIdealParticleViewTransform() const{return mIdealParticleViewTransform;}

	virtual void setExcessiveDT(int dt){mExcessiveDT=dt;}
	inline int getExcessiveDT() const{return mExcessiveDT;}
	virtual void setLogicDT(int dt);
	inline int getLogicDT() const{return mLogicDT;}
	virtual void setLogicTimeAndFrame(int time,int frame);
	inline int getLogicTime() const{return mLogicTime;}
	inline int getLogicFrame() const{return mLogicFrame;}
	inline int getVisualTime() const{return mLogicTime+mAccumulatedDT;}
	inline int getVisualFrame() const{return mVisualFrame;}

	virtual void update(int dt);

	virtual void preLogicUpdateLoop(int dt);
	virtual void logicUpdate(int dt);
	virtual void postLogicUpdateLoop(int dt);
	virtual void visualUpdate(int dt);

	virtual void render(peeper::Renderer *renderer,CameraEntity *cameraEntity);

	virtual void queueRenderable(Renderable *renderable);
	inline CameraEntity *getCamera() const{return mCamera;} // Only valid during rendering operations

	virtual void setUpdateListener(UpdateListener *updateListener);
	inline UpdateListener *getUpdateListener() const{return mUpdateListener;}

	virtual ParticleEntity::ParticleSimulator::ptr newParticleSimulator(ParticleEntity *particleEntity){return NULL;}

	virtual bool remove(Entity *entity);

protected:
	class RenderLayer{
	public:
		RenderLayer():
			forceRender(false),
			clearLayer(true){}

		egg::Collection<Renderable*> renderables;

		bool forceRender;
		bool clearLayer;
	};

	inline RenderLayer *getRenderLayer(int layer){
		layer-=Material::MIN_LAYER;
		if(mRenderLayers.size()<=layer){
			mRenderLayers.resize(layer+1,NULL);
		}

		RenderLayer *renderLayer=mRenderLayers[layer];
		if(renderLayer==NULL){
			renderLayer=new RenderLayer();
			mRenderLayers[layer]=renderLayer;
		}
		return renderLayer;
	}

	void resetModifiedFrames(Entity *entity);

	void logicUpdate(Entity::ptr entity,int dt);
	void visualUpdate(Entity::ptr entity,int dt);

	void queueRenderables(Entity *entity);
	bool culled(Entity *entity);

	virtual bool preLayerRender(peeper::Renderer *renderer,int layer);
	virtual bool postLayerRender(peeper::Renderer *renderer,int layer);

	int mExcessiveDT;
	int mLogicDT;
	int mLogicTime;
	int mLogicFrame;
	int mAccumulatedDT;
	int mVisualFrame;

	ParentEntity::ptr mBackground;

	egg::Collection<Entity::ptr> mUpdateEntities;
	UpdateListener *mUpdateListener;

	peeper::Color mAmbientColor;
	LightEntity::ptr mLight;

	Matrix4x4 mIdealParticleViewTransform;

	CameraEntity *mCamera;
	Material *mPreviousMaterial;

	egg::Collection<RenderLayer*> mRenderLayers;

	peeper::Viewport cache_render_viewport;
};

}
}
}

#endif
