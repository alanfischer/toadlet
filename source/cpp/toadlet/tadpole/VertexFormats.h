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

#ifndef TOADLET_TADPOLE_VERTEXFORMATS_H
#define TOADLET_TADPOLE_VERTEXFORMATS_H

#include <toadlet/tadpole/Types.h>
#include <toadlet/peeper/VertexFormat.h>

namespace toadlet{
namespace tadpole{

class VertexFormats{
public:
	VertexFormats(){}

	VertexFormat::ptr POSITION;
	VertexFormat::ptr POSITION_NORMAL;
	VertexFormat::ptr POSITION_COLOR;
	VertexFormat::ptr POSITION_TEX_COORD;
	VertexFormat::ptr POSITION_NORMAL_COLOR;
	VertexFormat::ptr POSITION_NORMAL_TEX_COORD;
	VertexFormat::ptr POSITION_COLOR_TEX_COORD;
	VertexFormat::ptr POSITION_NORMAL_COLOR_TEX_COORD;
	VertexFormat::ptr POSITION_ROTATE_SCALE;
	VertexFormat::ptr ROTATE;
	VertexFormat::ptr SCALE;
	VertexFormat::ptr COLOR;
};

}
}

#endif
