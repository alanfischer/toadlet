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

#ifndef TOADLET_TADPOLE_SKYBOXMESHCREATOR_H
#define TOADLET_TADPOLE_SKYBOXMESHCREATOR_H

#include <toadlet/tadpole/BaseResourceCreator.h>
#include <toadlet/tadpole/Mesh.h>

namespace toadlet{
namespace tadpole{

class TOADLET_API SkyBoxMeshCreator:public BaseResourceCreator{
public:
	TOADLET_OBJECT(SkyBoxMeshCreator);

	SkyBoxMeshCreator(Engine *engine){
		mEngine=engine;
	}

	void destroy(){}

	Resource::ptr create(const String &name,ResourceData *data){
		Resource::ptr resource=createSkyBoxMesh(Math::ONE,false,false,NULL,NULL,NULL,NULL,NULL,NULL);
		resource->setName(name);
		return resource;
	}

	Mesh::ptr createSkyBoxMesh(scalar size,bool unfolded,bool invert,Material::ptr bottom,Material::ptr top,Material::ptr left,Material::ptr right,Material::ptr back,Material::ptr front);

protected:
	Engine *mEngine;
};

}
}

#endif
