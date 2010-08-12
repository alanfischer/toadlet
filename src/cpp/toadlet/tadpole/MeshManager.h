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
#include <toadlet/tadpole/mesh/Mesh.h>

namespace toadlet{
namespace tadpole{

class Engine;

class TOADLET_API MeshManager:public ResourceManager{
public:
	MeshManager(Engine *engine);

	mesh::Mesh::ptr findMesh(const egg::String &name){return egg::shared_static_cast<mesh::Mesh>(ResourceManager::find(name));}
	egg::Resource::ptr manage(const egg::Resource::ptr &resource,const egg::String &name=(char*)NULL);

	mesh::Mesh::ptr createBox(const AABox &box);
	mesh::Mesh::ptr createSkyBox(scalar size,bool unfolded,bool invert,Material::ptr bottom=NULL,Material::ptr top=NULL,Material::ptr left=NULL,Material::ptr right=NULL,Material::ptr back=NULL,Material::ptr front=NULL);

	int getSphereVertexCount(int numSegments,int numRings){return (numRings+1)*(numSegments+1);}
	int getSphereIndexCount(int numSegments,int numRings){return 6*numRings*(numSegments+1);}
	mesh::Mesh::ptr createSphere(const Sphere &sphere,int numSegments=16,int numRings=16,Material::ptr material=NULL);
	mesh::Mesh::ptr createSphere(peeper::VertexBuffer::ptr vertexBuffer,peeper::IndexBuffer::ptr indexBuffer,const Sphere &sphere,int numSegments,int numRings);

	int getSkyDomeVertexCount(int numSegments,int numRings){return (numRings+1)*(numSegments+1);}
	int getSkyDomeIndexCount(int numSegments,int numRings){return 6*numRings*(numSegments+1);}
	mesh::Mesh::ptr createSkyDome(const Sphere &sphere,int numSegments=16,int numRings=16,scalar fade=0.5,Material::ptr material=NULL);
	mesh::Mesh::ptr createSkyDome(peeper::VertexBuffer::ptr vertexBuffer,peeper::IndexBuffer::ptr indexBuffer,const Sphere &sphere,int numSegments,int numRings,scalar fade);

	inline int getGeoSphereInitialTriangleCount(bool icosahedron){return icosahedron?20:8;}
	inline int getGeoSphereInitialVertexCount(bool icosahedron){return icosahedron?12:6;}
	inline int getGeoSphereTriangleCount(int depth,bool icosahedron){return getGeoSphereInitialTriangleCount(icosahedron)<<(depth*2);}
	int getGeoSphereVertexCount(int depth,bool icosahedron){return getGeoSphereInitialTriangleCount(icosahedron) * (((1 << ((depth+1) * 2)) - 1) / (4 - 1) - 1) + getGeoSphereInitialVertexCount(icosahedron);}
	int getGeoSphereIndexCount(int depth,bool icosahedron){return getGeoSphereTriangleCount(depth,icosahedron)*3;}
	mesh::Mesh::ptr createGeoSphere(const Sphere &sphere,int depth=3,bool icosahedron=false,Material::ptr material=NULL);
	mesh::Mesh::ptr createGeoSphere(peeper::VertexBuffer::ptr vertexBuffer,peeper::IndexBuffer::ptr indexBuffer,const Sphere &sphere,int depth,bool icosahedron);

	int getTorusVertexCount(int numMajor,int numMinor){return numMajor*(numMinor+1)*2;}
	mesh::Mesh::ptr createTorus(scalar majorRadius,scalar minorRadius,int numMajor=16,int numMinor=16,Material::ptr material=NULL);
	mesh::Mesh::ptr createTorus(peeper::VertexBuffer::ptr vertexBuffer,scalar majorRadius,scalar minorRadius,int numMajor,int numMinor);

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
	peeper::VertexBufferAccessor vba;
	peeper::IndexBufferAccessor iba;
	Sphere currentSphere;
};

}
}

#endif
