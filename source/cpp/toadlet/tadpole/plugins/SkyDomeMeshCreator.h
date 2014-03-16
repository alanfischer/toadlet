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

#ifndef TOADLET_TADPOLE_SKYDOMEMESHCREATOR_H
#define TOADLET_TADPOLE_SKYDOMEMESHCREATOR_H

#include <toadlet/tadpole/BaseResourceCreator.h>
#include <toadlet/tadpole/Mesh.h>

namespace toadlet{
namespace tadpole{

class TOADLET_API SkyDomeMeshCreator:public BaseResourceCreator{
public:
	TOADLET_OBJECT(SkyDomeMeshCreator);

	SkyDomeMeshCreator(Engine *engine){
		mEngine=engine;
	}

	void destroy(){}

	Resource::ptr create(const String &name,ResourceData *data){
		Resource::ptr resource=createSkyDomeMesh(Sphere(Math::ONE),16,16,Math::HALF,NULL);
		resource->setName(name);
		return resource;
	}

	int getSkyDomeVertexCount(int numSegments,int numRings){return (numRings+1)*(numSegments+1);}
	int getSkyDomeIndexCount(int numSegments,int numRings){return 6*numRings*(numSegments+1);}

	Mesh::ptr createSkyDomeMesh(VertexBuffer::ptr vertexBuffer,IndexBuffer::ptr indexBuffer,const Sphere &sphere,int numSegments,int numRings,scalar fade);
	Mesh::ptr createSkyDomeMesh(const Sphere &sphere,int numSegments,int numRings,scalar fade,Material::ptr material);

protected:
	Engine *mEngine;
};

}
}

#endif
