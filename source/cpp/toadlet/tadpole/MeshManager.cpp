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

#include <toadlet/tadpole/MeshManager.h>
#include <toadlet/tadpole/Engine.h>
#include <toadlet/tadpole/creator/AABoxMeshCreator.h>
#include <toadlet/tadpole/creator/SkyBoxMeshCreator.h>
#include <toadlet/tadpole/creator/SkyDomeMeshCreator.h>
#include <toadlet/tadpole/creator/SphereMeshCreator.h>
#include <toadlet/tadpole/creator/GridMeshCreator.h>

namespace toadlet{
namespace tadpole{

MeshManager::MeshManager(Engine *engine):ResourceManager(engine->getArchiveManager()){
	mEngine=engine;
}

Resource::ptr MeshManager::manage(const Resource::ptr &resource,const String &name){
	Mesh::ptr mesh=shared_static_cast<Mesh>(resource);
	ResourceManager::manage(mesh,name);
	return mesh;
}

Mesh::ptr MeshManager::createAABoxMesh(const AABox &box,Material::ptr material){
	return shared_static_cast<AABoxMeshCreator>(mAABoxCreator)->createAABoxMesh(box,material);
}

Mesh::ptr MeshManager::createSkyBoxMesh(scalar size,bool unfolded,bool invert,Material::ptr bottom,Material::ptr top,Material::ptr left,Material::ptr right,Material::ptr back,Material::ptr front){
	return shared_static_cast<SkyBoxMeshCreator>(mSkyBoxCreator)->createSkyBoxMesh(size,unfolded,invert,bottom,top,left,right,back,front);
}

Mesh::ptr MeshManager::createSkyDomeMesh(const Sphere &sphere,int numSegments,int numRings,scalar fade,Material::ptr material){
	return shared_static_cast<SkyDomeMeshCreator>(mSkyDomeCreator)->createSkyDomeMesh(sphere,numSegments,numRings,fade,material);
}

Mesh::ptr MeshManager::createSphereMesh(const Sphere &sphere,Material::ptr material){
	return shared_static_cast<SphereMeshCreator>(mSphereCreator)->createSphereMesh(sphere,16,16,material);
}

Mesh::ptr MeshManager::createGridMesh(scalar width,scalar height,int numWidth,int numHeight,Material::ptr material){
	return shared_static_cast<GridMeshCreator>(mGridCreator)->createGridMesh(width,height,numWidth,numHeight,material);
}

}
}

