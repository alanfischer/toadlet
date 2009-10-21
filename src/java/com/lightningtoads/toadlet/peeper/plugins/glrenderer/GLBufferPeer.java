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

package com.lightningtoads.toadlet.peeper.plugins.glrenderer;

#include <com/lightningtoads/toadlet/peeper/Types.h>
#include <com/lightningtoads/toadlet/peeper/plugins/glrenderer/GLIncludes.h>

import com.lightningtoads.toadlet.peeper.*;
import javax.microedition.khronos.opengles.*;
import static javax.microedition.khronos.opengles.GL11.*;

public class GLBufferPeer implements Buffer.Peer{
	public GLBufferPeer(GLRenderer renderer,Buffer buffer){
		this.renderer=renderer;
		GL11 gl=renderer.gl11;
		type=buffer.getType();

		if(type==Buffer.Type.INDEX){
			bufferTarget=GL_ELEMENT_ARRAY_BUFFER;
		}
		else if(type==Buffer.Type.VERTEX){
			bufferTarget=GL_ARRAY_BUFFER;
		}

		if(bufferTarget!=0){
			gl.glGenBuffers(1,bufferHandle,0);
			usageType=buffer.getUsageType();
			accessType=buffer.getAccessType();
			bufferSize=buffer.getBufferSize();

			java.nio.ByteBuffer bufferData=buffer.internal_getData();
			if(bufferData!=null){
//				java.nio.ByteBuffer data=lock(Buffer.LockType.WRITE_ONLY);
//				if(data!=null){
//					data.put(bufferData);
//					data.rewind();
//					bufferData.rewind();
//				}
//				unlock(data);
			}
		}

		TOADLET_CHECK_GLERROR("GLBufferPeer::GLBufferPeer");
	}

	public void destroy(){
		GL11 gl=renderer.gl11;
		if(bufferHandle[0]!=0){
			gl.glDeleteBuffers(1,bufferHandle,0);
			bufferHandle[0]=0;
		}

		TOADLET_CHECK_GLERROR("GLBufferPeer::~GLBufferPeer");
	}

	public boolean isValid(){return bufferHandle[0]!=0;}

	public java.nio.ByteBuffer lock(Buffer.LockType lockType){
		renderer.internal_getStatisticsSet().bufferLockCount++;

		// Temporary workaround until I get peers removed
		//data=renderer.allocScratchBuffer(bufferSize);
data=null;

		return data;
	}

	public boolean unlock(java.nio.ByteBuffer data){
		GL11 gl=renderer.gl11;
		if(bufferedData==false){
			bufferedData=true;
			int usage=getBufferUsage(usageType,accessType);
			gl.glBindBuffer(bufferTarget,bufferHandle[0]);
			gl.glBufferData(bufferTarget,bufferSize,data,usage);
		}
		else{
			gl.glBindBuffer(bufferTarget,bufferHandle[0]);
			gl.glBufferSubData(bufferTarget,0,bufferSize,data);
		}

		// Temporary workaround till I get peers removed
		//renderer.freeScratchBuffer(data);

		data=null;
		return true;
	}

	public boolean supportsRead(){return false;}

	public static int getBufferUsage(Buffer.UsageType usageType,Buffer.AccessType accessType){
		int usage=0;
		switch(usageType){
			case STATIC:
				usage=GL_STATIC_DRAW;
			break;
			case STREAM:
				usage=GL_DYNAMIC_DRAW;
			break;
			case DYNAMIC:
				usage=GL_DYNAMIC_DRAW;
			break;
		}
		return usage;
	}

	GLRenderer renderer=null;
	Buffer.Type type;
	int[] bufferHandle=new int[1];
	int bufferTarget=0;
	java.nio.ByteBuffer data=null;
	boolean bufferedData=false;
	Buffer.UsageType usageType=Buffer.UsageType.STATIC;
	Buffer.AccessType accessType=Buffer.AccessType.READ_ONLY;
	int bufferSize=0;
}
