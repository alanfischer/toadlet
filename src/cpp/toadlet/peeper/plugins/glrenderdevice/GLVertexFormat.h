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

class GLRenderDevice;

class TOADLET_API GLVertexFormat:public VertexFormat{
public:
	GLVertexFormat(GLRenderDevice *renderDevice);
	virtual ~GLVertexFormat();

	VertexFormat *getRootVertexFormat(){return this;}

	void setVertexFormatDestroyedListener(VertexFormatDestroyedListener *listener){mListener=listener;}

	bool create();
	void destroy();

	bool addElement(int semantic,int index,int format){return addElement(semantic,(char*)NULL,index,format);}
	bool addElement(const egg::String &name,int index,int format){return addElement(Semantic_UNKNOWN,name,index,format);}
	bool addElement(int semantic,const egg::String &name,int index,int format);
	int getNumElements() const{return mSemantics.size();}
	int getSemantic(int i) const{return mSemantics[i];}
	egg::String getName(int i) const{return mNames[i];}
	int getIndex(int i) const{return mIndexes[i];}
	int getFormat(int i) const{return mFormats[i];}
	int getOffset(int i) const{return mOffsets[i];}
	int getVertexSize() const{return mVertexSize;}
	int findElement(int semantic);
	int findElement(const egg::String &name);

protected:
	GLRenderDevice *mDevice;

	VertexFormatDestroyedListener *mListener;
	egg::Collection<int> mSemantics;
	egg::Collection<egg::String> mNames;
	egg::Collection<int> mIndexes;
	egg::Collection<int> mFormats;
	egg::Collection<int> mOffsets;
	int mVertexSize;

	int mUniqueHandle;
	egg::Collection<GLenum> mGLDataTypes;
	egg::Collection<GLuint> mGLElementCounts;
	uint32 mSemanticBits;

	friend class GLRenderDevice;
	friend class GLBuffer;
	friend class GLSLProgram;
};

}
}

#endif
