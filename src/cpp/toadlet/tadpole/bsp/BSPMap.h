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

#ifndef TOADLET_TADPOLE_BSP_BSPMAP_H
#define TOADLET_TADPOLE_BSP_BSPMAP_H

#include <toadlet/egg/BaseResource.h>
#include <toadlet/tadpole/bsp/Brush.h>
#include <toadlet/tadpole/bsp/Face.h>
#include <toadlet/tadpole/bsp/Leaf.h>
#include <toadlet/tadpole/bsp/Node.h>

namespace toadlet{
namespace tadpole{
namespace bsp{

class TOADLET_API BSPMap:public egg::BaseResource{
	egg::Collection<Node> nodes;
	egg::Collection<Leaf> leaves;
	egg::Collection<Brush> brushes;
	egg::Collection<Face> faces;
	egg::Collection<Plane> planes;
	egg::Collection<Vector3> vertices;

	TOADLET_BASERESOURCE_PASSTHROUGH(BaseResource);
};

}
}
}

#endif
