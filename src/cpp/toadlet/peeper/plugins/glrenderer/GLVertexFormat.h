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

#ifndef TOADLET_PEEPER_GLVERTEXFORMAT_H
#define TOADLET_PEEPER_GLVERTEXFORMAT_H

#include <toadlet/peeper/VertexFormat.h>
#include "GLIncludes.h"

namespace toadlet{
namespace peeper{

class GLRenderer;

class TOADLET_API GLVertexFormat:public VertexFormat{
public:
	GLVertexFormat(GLRenderer *renderer);
	virtual ~GLVertexFormat();

	VertexFormat *getRootVertexFormat(){return this;}

	void addElement(int semantic,int index,int format);
	bool create();
	void destroy();

	int getNumElements() const{return mSemantics.size();}
	int getSemantic(int i) const{return mSemantics[i];}
	int getIndex(int i) const{return mIndexes[i];}
	int getFormat(int i) const{return mFormats[i];}
	int getOffset(int i) const{return mOffsets[i];}
	int getVertexSize() const{return mVertexSize;}
	int findSemantic(int semantic);

protected:
	GLRenderer *mRenderer;

	egg::Collection<int> mSemantics;
	egg::Collection<int> mIndexes;
	egg::Collection<int> mFormats;
	egg::Collection<int> mOffsets;
	int mVertexSize;

	egg::Collection<GLenum> mGLDataTypes;
	egg::Collection<GLuint> mGLElementCounts;

	friend class GLRenderer;
};

}
}

#endif
