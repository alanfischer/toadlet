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

#include <toadlet/tadpole/ResourceManager.h>
#include <toadlet/tadpole/Mesh.h>
#include <toadlet/peeper/IndexBufferAccessor.h>
#include <toadlet/peeper/VertexBufferAccessor.h>

namespace toadlet{
namespace tadpole{

class Engine;

class MeshManager:public ResourceManager{
public:
	MeshManager(Engine *engine);

	Mesh::ptr createBox(const AABox &box);
	Mesh::ptr createSkyBox(scalar size,bool unfolded);
	Mesh::ptr createSphere(const Sphere &sphere,int numSegments=16,int numRings=16);
	Mesh::ptr createGeoSphere(const Sphere &sphere,int depth,bool icosahedron);

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
