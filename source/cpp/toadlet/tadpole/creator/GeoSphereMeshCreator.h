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

#ifndef TOADLET_TADPOLE_CREATOR_GEOSPHEREMESHCREATOR
#define TOADLET_TADPOLE_CREATOR_GEOSPHEREMESHCREATOR

#include <toadlet/tadpole/Engine.h>
#include <toadlet/tadpole/ResourceCreator.h>
#include <toadlet/tadpole/Mesh.h>

namespace toadlet{
namespace tadpole{
namespace creator{

class TOADLET_API GeoSphereMeshCreator:public ResourceCreator{
public:
	TOADLET_SPTR(GeoSphereMeshCreator);

	GeoSphereMeshCreator(Engine *engine){
		mEngine=engine;
	}

	void destroy(){}

	Resource::ptr create(const String &name,ResourceData *data,ProgressListener *listener){
		Resource::ptr resource=createGeoSphereMesh(Sphere(Math::ONE),3,false,Material::ptr());
		resource->setName(name);
		return resource;
	}

	inline int getGeoSphereInitialTriangleCount(bool icosahedron){return icosahedron?20:8;}
	inline int getGeoSphereInitialVertexCount(bool icosahedron){return icosahedron?12:6;}
	inline int getGeoSphereTriangleCount(int depth,bool icosahedron){return getGeoSphereInitialTriangleCount(icosahedron)<<(depth*2);}
	int getGeoSphereVertexCount(int depth,bool icosahedron){return getGeoSphereInitialTriangleCount(icosahedron) * (((1 << ((depth+1) * 2)) - 1) / (4 - 1) - 1) + getGeoSphereInitialVertexCount(icosahedron);}
	int getGeoSphereIndexCount(int depth,bool icosahedron){return getGeoSphereTriangleCount(depth,icosahedron)*3;}

	Mesh::ptr createGeoSphereMesh(VertexBuffer::ptr vertexBuffer,IndexBuffer::ptr indexBuffer,const Sphere &sphere,int depth,bool icosahedron);
	Mesh::ptr createGeoSphereMesh(const Sphere &sphere,int depth,bool icosahedron,Material::ptr material);

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
	Sphere currentSphere;
	VertexBufferAccessor vba;
	IndexBufferAccessor iba;
};

}
}
}

#endif
