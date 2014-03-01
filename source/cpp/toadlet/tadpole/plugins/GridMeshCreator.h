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

#ifndef TOADLET_TADPOLE_GRIDMESHCREATOR_H
#define TOADLET_TADPOLE_GRIDMESHCREATOR_H

#include <toadlet/tadpole/ResourceCreator.h>
#include <toadlet/tadpole/Mesh.h>

namespace toadlet{
namespace tadpole{

class TOADLET_API GridMeshCreator:public Object,public ResourceCreator{
public:
	TOADLET_IOBJECT(GridMeshCreator);

	GridMeshCreator(Engine *engine){
		mEngine=engine;
	}

	void destroy(){}

	Resource::ptr create(const String &name,ResourceData *data,ProgressListener *listener){
		Resource::ptr resource=createGridMesh(Math::ONE,Math::ONE,4,4,Material::ptr());
		resource->setName(name);
		return resource;
	}

	int getGridVertexCount(int numWidth,int numHeight){return numWidth*numHeight;}
	int getGridIndexCount(int numWidth,int numHeight){return (numWidth*2) * (numHeight-1) + (numHeight-2);}

	Mesh::ptr createGridMesh(VertexBuffer::ptr vertexBuffer,IndexBuffer::ptr indexBuffer,scalar width,scalar height,int numWidth,int numHeight);
	Mesh::ptr createGridMesh(scalar width,scalar height,int numWidth,int numHeight,Material::ptr material);

protected:
	Engine *mEngine;
};

}
}

#endif
