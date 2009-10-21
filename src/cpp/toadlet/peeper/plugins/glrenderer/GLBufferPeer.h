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

#ifndef TOADLET_PEEPER_GLBUFFERPEER_H
#define TOADLET_PEEPER_GLBUFFERPEER_H

#include "GLIncludes.h"
#include <toadlet/peeper/Buffer.h>
#if defined(TOADLET_BIG_ENDIAN)
#	include <toadlet/peeper/VertexElement.h>
#endif

#if defined(GL_VERSION_1_1)
	// MapBuffers seems to lag on my ATI card when locking & unlocking lots of data
	//  the use of MapBuffers vs CopySubData should be an option that the renderer auto-sets with an
	//  intelligent default when I get the peeper rewrite finished
	//#define TOADLET_HAS_GLMAPBUFFER
#endif

namespace toadlet{
namespace peeper{

class GLRenderer;

class TOADLET_API GLBufferPeer:public BufferPeer{
public:
	GLBufferPeer(GLRenderer *renderer,Buffer *buffer);

	virtual ~GLBufferPeer();

	inline bool isValid() const{return bufferHandle!=0;}

	uint8 *lock(Buffer::LockType lockType);
	bool unlock();

	bool supportsRead(){
		#if defined(TOADLET_HAS_GLMAPBUFFER)
			return true;
		#else
			return false;
		#endif
	}

	static GLenum getBufferUsage(Buffer::UsageType usageType,Buffer::AccessType accessType);

	GLRenderer *renderer;
	Buffer::Type type;
	GLuint bufferHandle;
	GLenum bufferTarget;
	uint8 *data;
	bool bufferedData;
#	if !defined(TOADLET_HAS_GLMAPBUFFER)
		Buffer::UsageType usageType;
		Buffer::AccessType accessType;
		int bufferSize;
#	endif
#	if defined(TOADLET_BIG_ENDIAN)
		Buffer::LockType lockType;
		int numVertexes;
		short vertexSize;
		egg::Collection<VertexElement> colorElementsToEndianSwap;
#	endif
};

}
}

#endif
