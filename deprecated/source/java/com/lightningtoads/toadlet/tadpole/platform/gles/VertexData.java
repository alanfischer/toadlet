/********** Copyright header - do not remove **********
 *
 * The Toadlet Engine
 *
 * Copyright (C) 2006, Lightning Toads Productions, LLC
 *
 * Author(s): Alan Fischer
 *
 * All source code for the Toadlet Engine, including
 * this file, is the sole property of Lightning Toads
 * Productions, LLC. It has been developed on our own
 * time, with our own tools, on our own hardware. None
 * of this code may be observed, used or changed
 * without our explicit permission.
 *
 *  - Lightning Toads Productions, LLC
 *
 ********** Copyright header - do not remove **********/

package com.lightningtoads.toadlet.tadpole;

import javax.microedition.khronos.opengles.*;

public final class VertexData{
	public VertexData(){
		mVertexBuffers=new Buffer[0];
	}

	public VertexData(int numVertexBuffers){
		mVertexBuffers=new Buffer[numVertexBuffers];
	}

	public VertexData(Buffer vertexBuffer){
		mVertexBuffers=new Buffer[1];
		addVertexBuffer(vertexBuffer);
	}

	public VertexFormat getVertexFormat(){return mVertexFormat;}

	public void addVertexBuffer(Buffer vertexBuffer){
		VertexFormat vertexFormat=vertexBuffer.getVertexFormat();

		int i;
		for(i=0;i<vertexFormat.getNumVertexElements();++i){
			mVertexFormat.addVertexElement(vertexFormat.getVertexElement(i));
		}

		if(mVertexBuffers.length<mNumVertexBuffers+1){
			Buffer[] vertexBuffers=new Buffer[mVertexBuffers.length+1];
			System.arraycopy(mVertexBuffers,0,vertexBuffers,0,mVertexBuffers.length);
			mVertexBuffers=vertexBuffers;
		}
		mVertexBuffers[mNumVertexBuffers]=vertexBuffer;
		mNumVertexBuffers++;
	}

	public int getNumVertexBuffers(){return mNumVertexBuffers;}
	public Buffer getVertexBuffer(int index){return mVertexBuffers[index];}

	public void bind(GL10 gl){
		int i;
		for(i=0;i<mNumVertexBuffers;++i){
			mVertexBuffers[i].bind(gl);
		}
	}

	public void unbind(GL10 gl){
		int i;
		for(i=0;i<mNumVertexBuffers;++i){
			mVertexBuffers[i].unbind(gl);
		}
	}

	VertexFormat mVertexFormat=new VertexFormat();
	Buffer[] mVertexBuffers=null;
	int mNumVertexBuffers=0;
}
