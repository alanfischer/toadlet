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
#include <toadlet/tadpole/DetailTraceable.h>
#include <toadlet/tadpole/MeshComponent.h>
#include <toadlet/tadpole/PartitionNode.h>
#include <toadlet/tadpole/bsp/BSP30Streamer.h>
#include <toadlet/tadpole/bsp/BSP30Map.h>

namespace toadlet{
namespace tadpole{
namespace bsp{

class TOADLET_API BSP30Node:public PartitionNode,public ResourceRequest,public StreamRequest,public Renderable,public PhysicsTraceable,public DetailTraceable{
public:
	TOADLET_INODE(BSP30Node);

	BSP30Node(Scene *scene=NULL);

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
	bool findAmbientForBound(Vector4 &r,Bound *bound);

	void resourceReady(Resource *resource){setMap((BSP30Map*)resource);}
	void resourceException(const Exception &ex){}
	void resourceProgress(float progress){}

	void streamReady(Stream *stream){static_pointer_cast<ResourceStreamer>(mStreamer)->load(stream,NULL,this);}
	void streamException(const Exception &ex){}
	void streamProgress(float progress){}

	// Renderable items
	Material *getRenderMaterial() const{return NULL;}
	Transform *getRenderTransform() const{return mWorldTransform;}
	Bound *getRenderBound() const{return mWorldBound;}
	void render(RenderManager *manager) const;

	// Traceable items
	Bound *getPhysicsBound() const{return Node::getBound();}
	void tracePhysicsSegment(PhysicsCollision &result,const Vector3 &position,const Segment &segment,const Vector3 &size);
	Bound *getDetailBound() const{return Node::getBound();}
	void traceDetailSegment(PhysicsCollision &result,const Vector3 &position,const Segment &segment,const Vector3 &size);

protected:
	class SkyMeshRequest:public Object,public ResourceRequest{
	public:
		TOADLET_IOBJECT(SkyMeshRequest);

		SkyMeshRequest(Engine *engine,MeshComponent::ptr mesh,RenderState::ptr state,const String &skyDown,const String &skyUp,const String &skyWest,const String &skyEasy,const String &skySouth,const String &skyNorth);

		void request();

		void resourceReady(Resource *resource);
		void resourceException(const Exception &ex);
		void resourceProgress(float progress){}

	protected:
		Engine *mEngine;
		MeshComponent::ptr mSkyMesh;
		RenderState::ptr mRenderState;
		String mTextureNames[6];
		Material::ptr mMaterials[6];
		int mIndex;
	};

	void addLeafToVisible(bleaf *leaf,const Vector3 &cameraPosition);
	void findBoundLeafs(Collection<int> &leafs,Node *node);

	BSP30Streamer::ptr mStreamer;
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
