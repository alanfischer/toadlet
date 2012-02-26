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

#ifndef TOADLET_TADPOLE_CREATOR_TORUSMESHCREATOR
#define TOADLET_TADPOLE_CREATOR_TORUSMESHCREATOR

#include <toadlet/tadpole/Engine.h>
#include <toadlet/tadpole/ResourceCreator.h>
#include <toadlet/tadpole/Mesh.h>

namespace toadlet{
namespace tadpole{
namespace creator{

class TOADLET_API TorusMeshCreator:public ResourceCreator{
public:
	TOADLET_SPTR(TorusMeshCreator);

	TorusMeshCreator(Engine *engine){
		mEngine=engine;
	}

	void destroy(){}

	Resource::ptr create(const String &name,ResourceData *data,ProgressListener *listener){
		Resource::ptr resource=createTorusMesh(Math::ONE,Math::HALF,16,16,Material::ptr());
		resource->setName(name);
		return resource;
	}

	int getTorusVertexCount(int numMajor,int numMinor){return numMajor*(numMinor+1)*2;}

	Mesh::ptr createTorusMesh(VertexBuffer::ptr vertexBuffer,scalar majorRadius,scalar minorRadius,int numMajor,int numMinor);
	Mesh::ptr createTorusMesh(scalar majorRadius,scalar minorRadius,int numMajor,int numMinor,Material::ptr material);

protected:
	Engine *mEngine;
	VertexBufferAccessor vba;
};

}
}
}

#endif
