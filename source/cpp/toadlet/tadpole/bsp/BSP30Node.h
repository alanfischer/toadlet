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

#ifndef TOADLET_TADPOLE_BSP_BSP30NODE_H
#define TOADLET_TADPOLE_BSP_BSP30NODE_H

#include <toadlet/peeper/IndexData.h>
#include <toadlet/peeper/VertexData.h>
#include <toadlet/tadpole/Camera.h>
#include <toadlet/tadpole/Renderable.h>
#include <toadlet/tadpole/Visible.h>
#include <toadlet/tadpole/PhysicsTraceable.h>
#include <toadlet/tadpole/MeshComponent.h>
#include <toadlet/tadpole/PartitionNode.h>
#include <toadlet/tadpole/bsp/BSP30Map.h>

namespace toadlet{
namespace tadpole{
namespace bsp{

class TOADLET_API BSP30Node:public PartitionNode,public Renderable,public PhysicsTraceable{
public:
	TOADLET_OBJECT(BSP30Node);

	BSP30Node(Scene *scene);

	void setMap(const String &name);
	void setMap(BSP30Map *map);
	BSP30Map *getMap() const{return mMap;}

	void setSkyName(const String &skyName);
	const String &getSkyName() const{return mSkyName;}
	void setSkyTextures(const String &skyDown,const String &skyUp,const String &skyWest,const String &skyEast,const String &skySouth,const String &skyNorth);
	MeshComponent *getSkyMesh() const{return mSkyMesh;}

	void setStyleIntensity(int style,scalar intensity){if(mMap!=NULL){mMap->styleIntensities[style]=255*intensity;}}
	scalar getStyleIntensity(int style){return mMap!=NULL?(float)mMap->styleIntensities[style]/255.0:0;}

	// Node items
	void nodeAttached(Node *node);
	void nodeRemoved(Node *node);
	void insertNodeLeafIndexes(const Collection<int> &indexes,Node *node);
	void removeNodeLeafIndexes(const Collection<int> &indexes,Node *node);

	void nodeBoundChanged(Node *node);
	void gatherRenderables(Camera *camera,RenderableSet *set);

	bool senseBoundingVolumes(SensorResultsListener *listener,Bound *bound);
	bool sensePotentiallyVisible(SensorResultsListener *listener,const Vector3 &point);
	bool findAmbientForPoint(Vector4 &r,const Vector3 &point);

	// Renderable items
	Material *getRenderMaterial() const{return NULL;}
	Transform *getRenderTransform() const{return mWorldTransform;}
	Bound *getRenderBound() const{return mWorldBound;}
	void render(RenderManager *manager) const;

	// Traceable items
	Bound *getTraceableBound() const{return Node::getBound();}
	void traceSegment(PhysicsCollision &result,const Vector3 &position,const Segment &segment,const Vector3 &size);

protected:
	void addLeafToVisible(bleaf *leaf,const Vector3 &cameraPosition);
	void findBoundLeafs(Collection<int> &leafs,Node *node);

	BSP30Map::ptr mMap;
	String mSkyName;
	MeshComponent::ptr mSkyMesh;

	class childdata{
	public:
		childdata():counter(0){}
		Collection<int> leafs;
		int counter;
	};

	class leafdata{
	public:
		Collection<Node*> occupants;
	};

	Collection<leafdata> mLeafData;
	leafdata mGlobalLeafData;

	Camera::ptr mLocalCamera;
	uint8 *mMarkedFaces;
	Collection<BSP30Map::facedata*> mVisibleMaterialFaces;

	int mCounter;
	Collection<int> mLeafIndexes;
};

}
}
}

#endif
