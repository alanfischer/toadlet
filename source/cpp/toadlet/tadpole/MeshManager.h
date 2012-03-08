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

#ifndef TOADLET_TADPOLE_MESHMANAGER_H
#define TOADLET_TADPOLE_MESHMANAGER_H

#include <toadlet/peeper/IndexBufferAccessor.h>
#include <toadlet/peeper/VertexBufferAccessor.h>
#include <toadlet/tadpole/ResourceManager.h>
#include <toadlet/tadpole/Mesh.h>

namespace toadlet{
namespace tadpole{

class Engine;

class TOADLET_API MeshManager:public ResourceManager{
public:
	TOADLET_OBJECT(MeshManager);

	MeshManager(Engine *engine);

	Mesh::ptr findMesh(const String &name){return shared_static_cast<Mesh>(ResourceManager::find(name));}
	Resource::ptr manage(const Resource::ptr &resource,const String &name=(char*)NULL);

	inline void setAABoxCreator(ResourceCreator::ptr creator){mAABoxCreator=creator;}
	inline void setSkyBoxCreator(ResourceCreator::ptr creator){mSkyBoxCreator=creator;}
	inline void setSkyDomeCreator(ResourceCreator::ptr creator){mSkyDomeCreator=creator;}
	inline void setSphereCreator(ResourceCreator::ptr creator){mSphereCreator=creator;}
	inline void setGridCreator(ResourceCreator::ptr creator){mGridCreator=creator;}

	Mesh::ptr createAABoxMesh(const AABox &box,Material::ptr material=Material::ptr());
	Mesh::ptr createSkyBoxMesh(scalar size,bool unfolded,bool invert,Material::ptr bottom,Material::ptr top,Material::ptr left,Material::ptr right,Material::ptr back,Material::ptr front);
	Mesh::ptr createSkyDomeMesh(const Sphere &sphere,int numSegments,int numRings,scalar fade,Material::ptr material=Material::ptr());
	Mesh::ptr createSphereMesh(const Sphere &sphere,Material::ptr material=Material::ptr());
	Mesh::ptr createGridMesh(scalar width,scalar height,int numWidth,int numHeight,Material::ptr material=Material::ptr());

protected:
	ResourceCreator::ptr mAABoxCreator,mSkyBoxCreator,mSkyDomeCreator,mSphereCreator,mGridCreator;
};

}
}

#endif
