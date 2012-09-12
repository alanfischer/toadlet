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

/// @todo: This does nothing but cast a mesh in findMesh.  This manager should be replace with just a ResourceManager.
/// @todo: Look for other managers in a similar state
class TOADLET_API MeshManager:public ResourceManager{
public:
	TOADLET_OBJECT(MeshManager);

	MeshManager(Engine *engine);

	Mesh::ptr findMesh(const String &name){return shared_static_cast<Mesh>(ResourceManager::find(name));}
};

}
}

#endif
