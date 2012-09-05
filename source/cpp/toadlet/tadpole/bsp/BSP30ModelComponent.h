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

#ifndef TOADLET_TADPOLE_BSP_BSP30MODELCOMPONENT_H
#define TOADLET_TADPOLE_BSP_BSP30MODELCOMPONENT_H

#include <toadlet/peeper/IndexData.h>
#include <toadlet/peeper/VertexData.h>
#include <toadlet/tadpole/Traceable.h>
#include <toadlet/tadpole/Renderable.h>
#include <toadlet/tadpole/Visible.h>
#include <toadlet/tadpole/BaseComponent.h>
#include <toadlet/tadpole/bsp/BSP30Map.h>

namespace toadlet{
namespace tadpole{
namespace bsp{

class TOADLET_API BSP30ModelComponent:public BaseComponent,public Traceable,public Visible{
public:
	TOADLET_OBJECT(BSP30ModelComponent);

	class TOADLET_API SubModel:public Object,public Renderable{
	public:
		TOADLET_OBJECT(SubModel);

		SubModel(BSP30ModelComponent *parent,BSP30Map *map);
		void destroy(){mMaterial=NULL;}

		Material *getRenderMaterial() const{return mMaterial;}
		const Transform &getRenderTransform() const{return mParent->getParent()->getWorldTransform();}
		Bound *getRenderBound() const{return mParent->getParent()->getWorldBound();}
		void render(RenderManager *manager) const;

	protected:
		BSP30ModelComponent *mParent;
		BSP30Map *mMap;
		Material::ptr mMaterial;
		BSP30Map::facedata *mFaces;

		friend class BSP30ModelComponent;
	};

	BSP30ModelComponent(Engine *engine);
	virtual ~BSP30ModelComponent();
	void destroy();

	bool parentChanged(Node *node);

	void setModel(BSP30Map *map,const String &name);
	void setModel(BSP30Map *map,int index);
	int getModel() const{return mModelIndex;}
	BSP30Map *getMap(){return mMap;}

	inline int getNumSubModels() const{return mSubModels.size();}
	SubModel *getSubModel(int i){return mSubModels[i];}

	void showPlanarFaces(int plane);

	bool getActive() const{return false;}
	Bound *getBound() const{return mBound;}

	// Visible
	bool getRendered() const{return mRendered;}
	void setRendered(bool rendered){mRendered=rendered;}
	RenderState::ptr getSharedRenderState();
	void gatherRenderables(Camera *camera,RenderableSet *set);

	// Traceable
	Bound *getTraceableBound() const{return mBound;}
	void traceSegment(PhysicsCollision &result,const Vector3 &position,const Segment &segment,const Vector3 &size);

protected:
	Engine *mEngine;
	Bound::ptr mBound;
	BSP30Map::ptr mMap;
	int mModelIndex;
	Collection<SubModel::ptr> mSubModels;
	RenderState::ptr mSharedRenderState;
	bool mRendered;
};

}
}
}

#endif
