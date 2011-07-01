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
	MeshManager(Engine *engine);

	Mesh::ptr findMesh(const String &name){return shared_static_cast<Mesh>(ResourceManager::find(name));}
	Resource::ptr manage(const Resource::ptr &resource,const String &name=(char*)NULL);

	Mesh::ptr createBox(const AABox &box,Material::ptr material=Material::ptr());
	Mesh::ptr createSkyBox(scalar size,bool unfolded,bool invert,Material::ptr bottom=Material::ptr(),Material::ptr top=Material::ptr(),Material::ptr left=Material::ptr(),Material::ptr right=Material::ptr(),Material::ptr back=Material::ptr(),Material::ptr front=Material::ptr());

	int getGridVertexCount(int numWidth,int numHeight){return numWidth*numHeight;}
	int getGridIndexCount(int numWidth,int numHeight){return (numWidth*2) * (numHeight-1) + (numHeight-2);}
	Mesh::ptr createGrid(scalar width,scalar height,int numWidth,int numHeight,Material::ptr material=Material::ptr());
	Mesh::ptr createGrid(VertexBuffer::ptr vertexBuffer,IndexBuffer::ptr indexBuffer,scalar width,scalar height,int numWidth,int numHeight);

	int getSphereVertexCount(int numSegments,int numRings){return (numRings+1)*(numSegments+1);}
	int getSphereIndexCount(int numSegments,int numRings){return 6*numRings*(numSegments+1);}
	Mesh::ptr createSphere(const Sphere &sphere,int numSegments=16,int numRings=16,Material::ptr material=Material::ptr());
	Mesh::ptr createSphere(VertexBuffer::ptr vertexBuffer,IndexBuffer::ptr indexBuffer,const Sphere &sphere,int numSegments,int numRings);

	int getSkyDomeVertexCount(int numSegments,int numRings){return (numRings+1)*(numSegments+1);}
	int getSkyDomeIndexCount(int numSegments,int numRings){return 6*numRings*(numSegments+1);}
	Mesh::ptr createSkyDome(const Sphere &sphere,int numSegments=16,int numRings=16,scalar fade=0.5,Material::ptr material=Material::ptr());
	Mesh::ptr createSkyDome(VertexBuffer::ptr vertexBuffer,IndexBuffer::ptr indexBuffer,const Sphere &sphere,int numSegments,int numRings,scalar fade);

	inline int getGeoSphereInitialTriangleCount(bool icosahedron){return icosahedron?20:8;}
	inline int getGeoSphereInitialVertexCount(bool icosahedron){return icosahedron?12:6;}
	inline int getGeoSphereTriangleCount(int depth,bool icosahedron){return getGeoSphereInitialTriangleCount(icosahedron)<<(depth*2);}
	int getGeoSphereVertexCount(int depth,bool icosahedron){return getGeoSphereInitialTriangleCount(icosahedron) * (((1 << ((depth+1) * 2)) - 1) / (4 - 1) - 1) + getGeoSphereInitialVertexCount(icosahedron);}
	int getGeoSphereIndexCount(int depth,bool icosahedron){return getGeoSphereTriangleCount(depth,icosahedron)*3;}
	Mesh::ptr createGeoSphere(const Sphere &sphere,int depth=3,bool icosahedron=false,Material::ptr material=Material::ptr());
	Mesh::ptr createGeoSphere(VertexBuffer::ptr vertexBuffer,IndexBuffer::ptr indexBuffer,const Sphere &sphere,int depth,bool icosahedron);

	int getTorusVertexCount(int numMajor,int numMinor){return numMajor*(numMinor+1)*2;}
	Mesh::ptr createTorus(scalar majorRadius,scalar minorRadius,int numMajor=16,int numMinor=16,Material::ptr material=Material::ptr());
	Mesh::ptr createTorus(VertexBuffer::ptr vertexBuffer,scalar majorRadius,scalar minorRadius,int numMajor,int numMinor);

protected:
	class IndexTri{
	public:
		void set(int i1,int i2,int i3){
			index[0]=i1;
			index[1]=i2;
			index[2]=i3;
		}

		int index[3];
	};

	void geoSet(int vertexIndex,const Vector3 &vec);

	Engine *mEngine;
	VertexBufferAccessor vba;
	IndexBufferAccessor iba;
	Sphere currentSphere;
};

}
}

#endif
