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
	mesh::Mesh::ptr createSkyBox(scalar size,bool unfolded,bool invert,peeper::Texture::ptr bottom=NULL,peeper::Texture::ptr top=NULL,peeper::Texture::ptr left=NULL,peeper::Texture::ptr right=NULL,peeper::Texture::ptr back=NULL,peeper::Texture::ptr front=NULL);
	mesh::Mesh::ptr createSphere(const Sphere &sphere,int numSegments=16,int numRings=16);
	mesh::Mesh::ptr createSkyDome(peeper::VertexFormat::ptr format,const Sphere &sphere,int numSegments=16,int numRings=16,scalar fade=0.5,peeper::Texture::ptr texture=NULL);
	mesh::Mesh::ptr createGeoSphere(const Sphere &sphere,int depth,bool icosahedron);
	mesh::Mesh::ptr createTorus(scalar majorRadius,scalar minorRadius,int numMajor,int numMinor);

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
