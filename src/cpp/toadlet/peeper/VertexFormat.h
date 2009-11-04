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

#ifndef TOADLET_PEEPER_VERTEXFORMAT_H
#define TOADLET_PEEPER_VERTEXFORMAT_H

#include <toadlet/egg/Collection.h>
#include <toadlet/peeper/VertexElement.h>

namespace toadlet{
namespace peeper{

class TOADLET_API VertexFormat{
public:
	TOADLET_SHARED_POINTERS(VertexFormat,VertexFormat);

	VertexFormat(int numVertexElements=0);

	const VertexElement &addVertexElement(const VertexElement &element);
	inline int getNumVertexElements() const{return vertexElements.size();}
	inline const VertexElement &getVertexElement(int index) const{return vertexElements[index];}
	inline short getVertexSize() const{return vertexSize;}
	inline bool hasVertexElementOfType(VertexElement::Type type) const{return vertexElementsByType.size()>type && vertexElementsByType[type]!=-1;}
	inline int getVertexElementIndexOfType(VertexElement::Type type) const{return vertexElementsByType.size()>type ? vertexElementsByType[type] : -1;}

	bool equals(const VertexFormat &format) const;
	inline bool equals(const VertexFormat::ptr &format) const{return equals(*format);}
	inline bool operator==(const VertexFormat &format) const{return equals(format);}

	egg::Collection<VertexElement> vertexElements;
	short vertexSize;
	egg::Collection<short> vertexElementsByType;
};

}
}

#endif
