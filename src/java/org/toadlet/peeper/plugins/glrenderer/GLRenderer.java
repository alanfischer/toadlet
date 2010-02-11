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

package org.toadlet.peeper.plugins.glrenderer;

#include <org/toadlet/peeper/Types.h>
#include <org/toadlet/peeper/plugins/glrenderer/GLIncludes.h>

import org.toadlet.egg.Categories;
import org.toadlet.egg.Logger;
import org.toadlet.egg.Error;
import org.toadlet.egg.MathConversion;
import org.toadlet.peeper.*;
import javax.microedition.khronos.opengles.*;
import static javax.microedition.khronos.opengles.GL11.*;

public class GLRenderer implements Renderer{
	public GL10 gl;
	public GL11 gl11;
	public int gl_version=0;
	
	class Option{
		public final static int USE_PBUFFERS=1;
		public final static int USE_FBOS=2;
		public final static int USE_HARDWARE_BUFFERS=3;
	};
	
	public GLRenderer(){}

	public void destroy(){
		assert(mShutdown);
	}

	// Startup/Shutdown
	public boolean startup(RenderContext renderContext,int[] options){
		Logger.alert(Categories.TOADLET_PEEPER,
			"GLRenderer: Startup started");

		boolean usePBuffers=true;
		boolean useFBOs=true;
		boolean useHardwareBuffers=true;
		if(options!=null){
			int i=0;
			while(options[i]!=0){
				switch(options[i++]){
					case Option.USE_PBUFFERS:
						usePBuffers=options[i++]>0;
						Logger.alert(Categories.TOADLET_PEEPER,
							"Setting PBuffer usage to:"+usePBuffers);
					break;
					case Option.USE_FBOS:
						useFBOs=options[i++]>0;
						Logger.alert(Categories.TOADLET_PEEPER,
							"Setting FBO usage to:"+useFBOs);
					break;
					case Option.USE_HARDWARE_BUFFERS:
						useHardwareBuffers=options[i++]>0;
						Logger.alert(Categories.TOADLET_PEEPER,
							"Setting Hardware Buffer usage to:"+useHardwareBuffers);
					break;
				}
			}
		}

		#if defined(TOADLET_HAS_EGL)
			gl=(GL10)((EGLRenderContextPeer)renderContext.internal_getRenderTargetPeer()).getEGLContext().getGL();
			gl_version=10;
			try{
				gl11=(GL11)gl;
				gl_version=11;
			}
			catch(Exception ex){}
		#endif

		Logger.alert(Categories.TOADLET_PEEPER,
			"CALCULATED GL VERSION:"+(gl_version/10)+"."+(gl_version%10));

		mShutdown=false;

		mRenderContext=renderContext;
		mRenderTarget=renderContext;

		// Crashes on Android 1.1
		#if !defined(TOADLET_PLATFORM_ANDROID)
			Logger.alert(Categories.TOADLET_PEEPER,
				"GL_VENDOR:" + gl.glGetString(GL_VENDOR));
			Logger.alert(Categories.TOADLET_PEEPER,
				"GL_RENDERER:" + gl.glGetString(GL_RENDERER));
			Logger.alert(Categories.TOADLET_PEEPER,
				"GL_VERSION:" + gl.glGetString(GL_VERSION));
			Logger.alert(Categories.TOADLET_PEEPER,
				"GL_EXTENSIONS:" + gl.glGetString(GL_EXTENSIONS));
		#endif

		mCapabilitySet.hardwareTextures=true;
		
		int[] maxTextureStages=new int[1];
		gl.glGetIntegerv(GL_MAX_TEXTURE_UNITS,maxTextureStages,0);
		if(maxTextureStages[0]<=0){
			maxTextureStages[0]=1;
		}
		mCapabilitySet.maxTextureStages=maxTextureStages[0];

		mLastTextures=new Texture[mCapabilitySet.maxTextureStages];
		mLastTexTargets=new int[mCapabilitySet.maxTextureStages];
		mTexCoordIndexes=new short[mCapabilitySet.maxTextureStages]; for(int i=0;i<mTexCoordIndexes.length;++i){mTexCoordIndexes[i]=-1;}
		mLastTexCoordIndexes=new short[mCapabilitySet.maxTextureStages]; for(int i=0;i<mLastTexCoordIndexes.length;++i){mLastTexCoordIndexes[i]=-1;}

		int[] maxTextureSize=new int[1];
		gl.glGetIntegerv(GL_MAX_TEXTURE_SIZE,maxTextureSize,0);
		if(maxTextureSize[0]<=0){
			maxTextureSize[0]=256;
		}
		mCapabilitySet.maxTextureSize=maxTextureSize[0];

		int[] maxLights=new int[1];
		gl.glGetIntegerv(GL_MAX_LIGHTS,maxLights,0);
		if(maxLights[0]<=0){
			maxLights[0]=8;
		}
		mCapabilitySet.maxLights=maxLights[0];

//		mPBufferRenderToTexture=usePBuffers && GLPBufferRenderTexturePeer.available(this);
//		#if defined(TOADLET_HAS_GLEW) || defined(TOADLET_HAS_EAGL)
//			mFBORenderToTexture=useFBOs && GLFBORenderTexturePeer.available(this);
//		#else
//			mFBORenderToTexture=false;
//		#endif

		mCapabilitySet.renderToTexture=mPBufferRenderToTexture || mFBORenderToTexture;

		mCapabilitySet.renderToDepthTexture=mFBORenderToTexture;

		#if defined(TOADLET_HAS_GLEW)
			mCapabilitySet.textureDot3=(GLEW_ARB_texture_env_dot3!=0);
		#elif defined(TOADLET_HAS_GLES)
			mCapabilitySet.textureDot3=(gl_version>=11);
		#endif

		#if defined(TOADLET_HAS_GLEW)
			// Usefully, GL_TEXTURE_RECTANGLE_ARB == GL_TEXTURE_RECTANGLE_EXT == GL_TEXTURE_RECTANGLE_NV
			mCapabilitySet.textureNonPowerOf2Restricted=(GLEW_ARB_texture_rectangle!=0) || (GLEW_EXT_texture_rectangle!=0) || (GLEW_NV_texture_rectangle!=0);
			mCapabilitySet.renderToTextureNonPowerOf2Restricted=mFBORenderToTexture && mCapabilitySet.textureNonPowerOf2Restricted;
			mCapabilitySet.textureNonPowerOf2=(GLEW_ARB_texture_non_power_of_two!=0);
		#endif

		#if defined(TOADLET_HAS_GLEW)
			mCapabilitySet.hardwareIndexBuffers=(mCapabilitySet.hardwareVertexBuffers=(useHardwareBuffers && GLEW_ARB_vertex_buffer_object));
		#elif defined(TOADLET_HAS_GLES)
			mCapabilitySet.hardwareIndexBuffers=(mCapabilitySet.hardwareVertexBuffers=(useHardwareBuffers && gl_version>=11));
		#endif

		#if defined(TOADLET_HAS_GLEW)
			mCapabilitySet.pointSprites=(GLEW_ARB_point_parameters!=0);
		#elif defined(TOADLET_HAS_GLES)
			mCapabilitySet.pointSprites=(gl_version>=11);
		#endif

		#if defined(TOADLET_HAS_GLEW)
			mCapabilitySet.vertexShaders=(GLEW_ARB_vertex_program>0);
			if(mCapabilitySet.vertexShaders) glGetProgramivARB(GL_VERTEX_PROGRAM_ARB,GL_MAX_PROGRAM_ENV_PARAMETERS_ARB,(GLint*)&mCapabilitySet.maxVertexShaderLocalParameters);
		#endif

		#if defined(TOADLET_HAS_GLEW)
			mCapabilitySet.fragmentShaders=(GLEW_ARB_fragment_program>0);
			if(mCapabilitySet.fragmentShaders) glGetProgramivARB(GL_FRAGMENT_PROGRAM_ARB,GL_MAX_PROGRAM_ENV_PARAMETERS_ARB,(GLint*)&mCapabilitySet.maxFragmentShaderLocalParameters);
		#endif

		#if defined(TOADLET_HAS_GLES) && defined(TOADLET_FIXED_POINT)
			mCapabilitySet.idealFormatBit=VertexElement.Format.BIT_FIXED_32;
		#else
			mCapabilitySet.idealFormatBit=VertexElement.Format.BIT_FLOAT_32;
		#endif

		setDefaultStates();

		TOADLET_CHECK_GLERROR("startup");

		Logger.alert(Categories.TOADLET_PEEPER,
			"GLRenderer: Startup finished");

		return true;
	}

	public boolean shutdown(){
		Logger.alert(Categories.TOADLET_PEEPER,
			"GLRenderer: Shutdown finished");

		mShutdown=true;
		return true;
	}

	public RendererStatus getStatus(){
		return RendererStatus.OK;
	}

	public boolean reset(){
		// No device reset necessary

		setDefaultStates();

		return true;
	}

	// Resource operations
	public Texture.Peer createTexturePeer(Texture texture){
		if(texture==null){
			Error.nullPointer(Categories.TOADLET_PEEPER,
				"Texture is NULL");
			return null;
		}

		switch(texture.getType()){
			case NORMAL:{
				return new GLTexturePeer(this,texture);
			}
			case RENDER:{
				RenderTexture renderTexture=(RenderTexture)texture;
				#if defined(TOADLET_HAS_GLEW)
					if(mFBORenderToTexture){
						GLTexturePeer peer=new GLFBORenderTexturePeer(this,renderTexture);
						if(peer.isValid()==false){
							peer=null;
						}
						return peer;
					}
					else
				#endif
//				if(mPBufferRenderToTexture){
//					GLTexturePeer peer=new GLPBufferRenderTexturePeer(this,renderTexture);
//					if(peer.isValid()==false){
//						peer=null;
//					}
//					return peer;
//				}
				Error.unknown(Categories.TOADLET_PEEPER,
					"TT_RENDER specified, but no render to texture support");
				return null;
			}
			case ANIMATED:{
				// Dont load animated textures, since they are just a collection of normal textures
				return null;
			}
			default:
				return null;
		}
	}

	public Buffer.Peer createBufferPeer(Buffer buffer){
		if(buffer==null){
			Error.nullPointer(Categories.TOADLET_PEEPER,
				"Buffer is NULL");
			return null;
		}

		if(	(buffer.getType()==Buffer.Type.INDEX && mCapabilitySet.hardwareIndexBuffers==true) ||
			(buffer.getType()==Buffer.Type.VERTEX && mCapabilitySet.hardwareVertexBuffers==true) ){
			GLBufferPeer peer=new GLBufferPeer(this,buffer);
			if(peer.isValid()==false){
				peer=null;
			}
			return peer;
		}

		return null;
	}

	//public Program.Peer createProgramPeer(Program program){return null;}
	//public Shader.Peer createShaderPeer(Shader shader){return null;}

	// Matrix operations
	public void setModelMatrix(Matrix4x4 matrix){
		if(mMatrixMode!=GL_MODELVIEW){
			mMatrixMode=GL_MODELVIEW;
			gl.glMatrixMode(mMatrixMode);
		}

		#if defined(TOADLET_FIXED_POINT)
			#if defined(TOADLET_HAS_GLES)
				mModelMatrix.set(matrix);

				gl.glLoadMatrixx(mViewMatrix.getData(),0);
				gl.glMultMatrixx(mModelMatrix.getData(),0);
			#else
				MathConversion.scalarToFloat(mModelMatrix,matrix);

				gl.glLoadMatrixf(mViewMatrix.getData(),0);
				gl.glMultMatrixf(mModelMatrix.getData(),0);
			#endif
		#else
			mModelMatrix.set(matrix);

			gl.glLoadMatrixf(mViewMatrix.getData(),0);
			gl.glMultMatrixf(mModelMatrix.getData(),0);
		#endif
	}

	public void setViewMatrix(Matrix4x4 matrix){
		if(mMatrixMode!=GL_MODELVIEW){
			mMatrixMode=GL_MODELVIEW;
			gl.glMatrixMode(mMatrixMode);
		}

		#if defined(TOADLET_FIXED_POINT)
			#if defined(TOADLET_HAS_GLES)
				mViewMatrix.set(matrix);

				gl.glLoadMatrixx(mViewMatrix.getData(),0);
				gl.glMultMatrixx(mModelMatrix.getData(),0);
			#else
				MathConversion.scalarToFloat(mViewMatrix,matrix);

				gl.glLoadMatrixf(mViewMatrix.getData(),0);
				gl.glMultMatrixf(mModelMatrix.getData(),0);
			#endif
		#else
			mViewMatrix.set(matrix);

			gl.glLoadMatrixf(mViewMatrix.getData(),0);
			gl.glMultMatrixf(mModelMatrix.getData(),0);
		#endif
	}

	public void setProjectionMatrix(Matrix4x4 matrix){
		if(mMatrixMode!=GL_PROJECTION){
			mMatrixMode=GL_PROJECTION;
			gl.glMatrixMode(mMatrixMode);
		}

		if(mMirrorY){
			#if defined(TOADLET_FIXED_POINT)
				#if defined(TOADLET_HAS_GLES)
					cacheMatrix4x4.set(matrix);
					cacheMatrix4x4.setAt(1,0,-cacheMatrix4x4.at(1,0));
					cacheMatrix4x4.setAt(1,1,-cacheMatrix4x4.at(1,1));
					cacheMatrix4x4.setAt(1,2,-cacheMatrix4x4.at(1,2));
					cacheMatrix4x4.setAt(1,3,-cacheMatrix4x4.at(1,3));

					gl.glLoadMatrixx(cacheMatrix4x4.getData(),0);
				#else
					MathConversion.scalarToFloat(cacheMatrix4x4,matrix);
					cacheMatrix4x4.setAt(1,0,-cacheMatrix4x4.at(1,0));
					cacheMatrix4x4.setAt(1,1,-cacheMatrix4x4.at(1,1));
					cacheMatrix4x4.setAt(1,2,-cacheMatrix4x4.at(1,2));
					cacheMatrix4x4.setAt(1,3,-cacheMatrix4x4.at(1,3));

					gl.glLoadMatrixf(cacheMatrix4x4.getData(),0);
				#endif
			#else
				cacheMatrix4x4.set(matrix);
				cacheMatrix4x4.setAt(1,0,-cacheMatrix4x4.at(1,0));
				cacheMatrix4x4.setAt(1,1,-cacheMatrix4x4.at(1,1));
				cacheMatrix4x4.setAt(1,2,-cacheMatrix4x4.at(1,2));
				cacheMatrix4x4.setAt(1,3,-cacheMatrix4x4.at(1,3));

				gl.glLoadMatrixf(cacheMatrix4x4.getData(),0);
			#endif
		}
		else{
			#if defined(TOADLET_FIXED_POINT)
				#if defined(TOADLET_HAS_GLES)
					gl.glLoadMatrixx(matrix.getData(),0);
				#else
					MathConversion.scalarToFloat(cacheMatrix4x4,matrix);
					gl.glLoadMatrixf(cacheMatrix4x4.getData(),0);
				#endif
			#else
				gl.glLoadMatrixf(matrix.getData(),0);
			#endif
		}
	}

	// Rendering operations
	public RenderContext getRenderContext(){
		return mRenderContext;
	}

	public boolean setRenderTarget(RenderTarget target){
		TOADLET_CHECK_GLERROR("before setRenderTarget");

		if(target==null){
			mRenderTarget=null;
			Error.nullPointer(Categories.TOADLET_PEEPER,
				"RenderTarget is NULL");
			return false;
		}

		GLRenderTargetPeer peer=(GLRenderTargetPeer)target.internal_getRenderTargetPeer();
		if(peer!=null){
			peer.makeCurrent();

			if(peer.initialized==false){
				setDefaultStates();
				peer.initialized=true;
			}

			// It is known that GL_LINE_SMOOTH or GL_POLYGON_SMOOTH cause software rendering on some devices, so disable antialiasing
			/*
			if(peer.getAntialiased()){
				gl.glEnable(GL_LINE_SMOOTH);
				#if defined(TOADLET_HAS_GLEW)
					gl.glEnable(GL_POLYGON_SMOOTH);
					if(GLEW_ARB_multisample){
						glEnable(GL_MULTISAMPLE_ARB);
					}
					if(GLEW_NV_multisample_filter_hint){
						glHint(GL_MULTISAMPLE_FILTER_HINT_NV,GL_NICEST);
					}
				#endif
			}
			else{
				gl.glDisable(GL_LINE_SMOOTH);
				#if defined(TOADLET_HAS_GLEW)
					glDisable(GL_POLYGON_SMOOTH);
					if(GLEW_ARB_multisample){
						glDisable(GL_MULTISAMPLE_ARB);
					}
					if(GLEW_NV_multisample_filter_hint){
						glHint(GL_MULTISAMPLE_FILTER_HINT_NV,GL_FASTEST);
					}
				#endif
			}
			*/
		}
		else{
			mRenderTarget=null;
			Error.invalidParameters(Categories.TOADLET_PEEPER,
				"setRenderTarget: target peer is not a GLRenderTargetPeer");
			return false;
		}

		mRenderTarget=target;

		TOADLET_CHECK_GLERROR("setRenderTarget");

		return true;
	}

	public RenderTarget getRenderTarget(){
		return mRenderTarget;
	}

	public void setViewport(Viewport viewport){
		if(mRenderTarget!=null){
			int rtwidth=mRenderTarget.getWidth();
			int rtheight=mRenderTarget.getHeight();

			gl.glViewport(viewport.x,rtheight-(viewport.y+viewport.height),viewport.width,viewport.height);
			if(viewport.x==0 && viewport.y==0 && viewport.width==rtwidth && viewport.height==rtheight){
				gl.glDisable(GL_SCISSOR_TEST);
			}
			else{
				gl.glScissor(viewport.x,rtheight-(viewport.y+viewport.height),viewport.width,viewport.height);
				gl.glEnable(GL_SCISSOR_TEST);
			}
		}
		else{
			gl.glViewport(0,0,viewport.width,viewport.height);
			gl.glDisable(GL_SCISSOR_TEST);

			Error.unknown(Categories.TOADLET_PEEPER,
				"setViewport called without a valid render target");
		}
	}

	public void clear(int clearFlags,Color clearColor){
		int bufferBits=0;

		if((clearFlags & ClearFlag.COLOR)>0){
			bufferBits|=GL_COLOR_BUFFER_BIT;
			gl.glClearColor(clearColor.r,clearColor.g,clearColor.b,clearColor.a);
		}
		if((clearFlags & ClearFlag.DEPTH)>0){
			bufferBits|=GL_DEPTH_BUFFER_BIT;
		}
		if((clearFlags & ClearFlag.STENCIL)>0){
			bufferBits|=GL_STENCIL_BUFFER_BIT;
		}

		gl.glClear(bufferBits);
	}

	public void swap(){
		if(mRenderTarget!=null && mRenderTarget.internal_getRenderTargetPeer()!=null){
			((GLRenderTargetPeer)mRenderTarget.internal_getRenderTargetPeer()).swap();
		}

		TOADLET_CHECK_GLERROR("swap");
	}

	public void beginScene(){
		TOADLET_CHECK_GLERROR("before beginScene");

		#if defined(TOADLET_DEBUG)
			mBeginEndCounter++;
			if(mBeginEndCounter!=1){
				mBeginEndCounter--;
				Error.unknown(Categories.TOADLET_PEEPER,
					"GLRenderer::beginScene: Unmached beginScene/endScene");
			}
		#endif
		
		mStatisticsSet.reset();

		TOADLET_CHECK_GLERROR("beginScene");
	}

	public void endScene(){
		#if defined(TOADLET_DEBUG)
			mBeginEndCounter--;
			if(mBeginEndCounter!=0){
				mBeginEndCounter++;
				Error.unknown(Categories.TOADLET_PEEPER,
					"GLRenderer::endScene: Unmached beginScene/endScene");
			}
		#endif

		if(mLastVertexData!=null){
			unsetVertexData(mLastVertexData);
			mLastVertexData=null;
		}
		if(mCapabilitySet.hardwareVertexBuffers){
			gl11.glBindBuffer(GL_ARRAY_BUFFER,0);
		}
		if(mCapabilitySet.hardwareIndexBuffers){
			gl11.glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
		}

		int i;
		for(i=0;i<mCapabilitySet.maxTextureStages;++i){
			setTextureStage(i,null);
		}

		if(mRenderTarget!=null){
			Texture texture=mRenderTarget.castToTexture();
			if(texture!=null && texture.getAutoGenerateMipMaps()){
				GLTexturePeer peer=(GLTexturePeer)texture.internal_getTexturePeer();
				if(peer.manuallyBuildMipMaps){
					peer.generateMipMaps();
				}
			}
		}

		TOADLET_CHECK_GLERROR("endScene");
	}

	public void renderPrimitive(VertexData vertexData,IndexData indexData){
		#if defined(TOADLET_DEBUG)
			if(mBeginEndCounter!=1){
				Error.unknown(Categories.TOADLET_PEEPER,
					"GLRenderer::renderPrimitive called outside of begin/end scene");
			}
		#endif

		if(vertexData==null || indexData==null){
			Error.nullPointer(Categories.TOADLET_PEEPER,
				"VertexData or IndexData is NULL");
			return;
		}

		int count=indexData.getCount();

		if(count<=0){
			return;
		}

		IndexData.Primitive primitive=indexData.getPrimitive();

		int type=0;
		switch(primitive){
			case POINTS:
				type=GL_POINTS;
				mStatisticsSet.pointCount+=count;
			break;
			case LINES:
				type=GL_LINES;
				mStatisticsSet.lineCount+=count/2;
			break;
			case LINESTRIP:
				type=GL_LINE_STRIP;
				mStatisticsSet.lineCount+=count-1;
			break;
			case TRIS:
				type=GL_TRIANGLES;
				mStatisticsSet.triangleCount+=count/3;
			break;
			case TRISTRIP:
				type=GL_TRIANGLE_STRIP;
				mStatisticsSet.triangleCount+=count-2;
			break;
			case TRIFAN:
				type=GL_TRIANGLE_FAN;
				mStatisticsSet.triangleCount+=count-2;
			break;
		}

		boolean rebind=false;
		if(mLastVertexData!=vertexData){
			rebind=true;
		}
		else{
			int i;
			for(i=0;i<mMaxTexCoordIndex;++i){
				if(mLastTexCoordIndexes[i]!=mTexCoordIndexes[i]){
					rebind=true;
					break;
				}
			}
		}
		if(rebind){
			if(mLastVertexData!=null){
				unsetVertexData(mLastVertexData);
			}

			setVertexData(vertexData);
			mLastVertexData=vertexData;
		}

		IndexBuffer indexBuffer=indexData.getIndexBuffer();
		if(indexBuffer!=null){
			int indexType=0;
			switch(indexBuffer.getIndexFormat()){
				case UINT_8:
					indexType=GL_UNSIGNED_BYTE;
				break;
				case UINT_16:
					indexType=GL_UNSIGNED_SHORT;
				break;
				#if !defined(TOADLET_HAS_GLES)
					case UINT_32:
						indexType=GL_UNSIGNED_INT;
					break;
				#endif
				default:
				break;
			}

			// TODO: Unify
			GLBufferPeer indexBufferPeer=(GLBufferPeer)indexBuffer.internal_getBufferPeer();
			if(indexBufferPeer==null){
				if(mCapabilitySet.hardwareIndexBuffers){
					gl11.glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
				}

				gl.glDrawElements(type,indexData.getCount(),indexType,indexBuffer.internal_getData().position(indexData.getStart()*indexBuffer.getIndexFormat().ordinal()));
			}
			else{
				if(mCapabilitySet.hardwareIndexBuffers){
					gl11.glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,indexBufferPeer.bufferHandle[0]);
				}

				gl11.glDrawElements(type,indexData.getCount(),indexType,indexData.getStart()*indexBuffer.getIndexFormat().ordinal());
			}

			TOADLET_CHECK_GLERROR("glDrawElements");
		}
		else{
			gl.glDrawArrays(type,indexData.getStart(),indexData.getCount());

			TOADLET_CHECK_GLERROR("glDrawArrays");
		}
	}

	// Render state operations
	public void setDefaultStates(){
		// General states
		mAlphaTest=AlphaTest.GEQUAL;
		mAlphaCutoff=0;
		mDepthWrite=false;
		mDepthTest=DepthTest.NONE;
		mDithering=true;
		mFaceCulling=FaceCulling.NONE;
		mLighting=true;
		mBlend=new Blend(Blend.Combination.ALPHA);
		mShading=Shading.FLAT;
		mNormalize=Normalize.NONE;
		mTexturePerspective=false;

		setAlphaTest(AlphaTest.NONE,Math.HALF);
		setDepthWrite(true);
		setDepthTest(DepthTest.LEQUAL);
		setDithering(false);
		setFaceCulling(FaceCulling.BACK);
		setFogParameters(Fog.NONE,0,Math.ONE,Colors.BLACK);
		setLighting(false);
		setBlend(Blend.Combination.DISABLED);
		setShading(Shading.SMOOTH);
		setNormalize(Normalize.RESCALE);
		setTexturePerspective(true);

		int i;
		for(i=0;i<mCapabilitySet.maxTextureStages;++i){
			mLastTexTargets[i]=GL_TEXTURE_2D;
			setTextureStage(i,null);
		}

		setLightEffect(new LightEffect());
		setAmbientColor(Colors.WHITE);
		// We leave the current lights enabled because the SceneManager does not re-set the lights between layers

		// GL specific states
		{
			// Move specular to separate color
			#if !defined(TOADLET_HAS_GLES) && defined(TOADLET_HAS_GL_11)
			if(gl_version>=11){
				gl.glLightModeli(GL_LIGHT_MODEL_COLOR_CONTROL,GL_SEPARATE_SPECULAR_COLOR);
			}
			#endif

			// TODO: Change the texture loading to agree with this?
			gl.glPixelStorei(GL_PACK_ALIGNMENT,1);
			gl.glPixelStorei(GL_UNPACK_ALIGNMENT,1);
		}
	}

	public void setAlphaTest(AlphaTest alphaTest,scalar cutoff){
		if(mAlphaTest==alphaTest && mAlphaCutoff==cutoff){
			return;
		}

		if(alphaTest==AlphaTest.NONE){
			gl.glDisable(GL_ALPHA_TEST);
		}
		else{
			int func=0;
			switch(alphaTest){
				case LESS:
					func=GL_LESS;
				break;
				case EQUAL:
					func=GL_EQUAL;
				break;
				case LEQUAL:
					func=GL_LEQUAL;
				break;
				case GREATER:
					func=GL_GREATER;
				break;
				case NOTEQUAL:
					func=GL_NOTEQUAL;
				break;
				case GEQUAL:
					func=GL_GEQUAL;
				break;
				case ALWAYS:
					func=GL_ALWAYS;
				break;
				default:
				break;
			}

			#if defined(TOADLET_FIXED_POINT) && defined(TOADLET_HAS_GLES)
				gl.glAlphaFuncx(func,cutoff);
			#else
				gl.glAlphaFunc(func,MathConversion.scalarToFloat(cutoff));
			#endif

			if(mAlphaTest==AlphaTest.NONE){
				gl.glEnable(GL_ALPHA_TEST);
			}
		}

		mAlphaTest=alphaTest;
		mAlphaCutoff=cutoff;

		TOADLET_CHECK_GLERROR("setAlphaTest");
	}

	public void setBlend(Blend blend){
		if(mBlend.equals(blend)){
			return;
		}

		if(blend.equals(Blend.Combination.DISABLED)){
			gl.glDisable(GL_BLEND);
		}
		else{
			int src=getGLBlendOperation(blend.source);
			int dest=getGLBlendOperation(blend.dest);

			gl.glBlendFunc(src,dest);

			if(mBlend.equals(Blend.Combination.DISABLED)){
				gl.glEnable(GL_BLEND);
			}
		}

		mBlend.set(blend);

		TOADLET_CHECK_GLERROR("setBlendFunction");
	}

	public void setFaceCulling(FaceCulling faceCulling){
		if(mFaceCulling==faceCulling){
			return;
		}

		if(faceCulling==FaceCulling.NONE){
			gl.glDisable(GL_CULL_FACE);
		}
		else{
			switch(faceCulling){
				case CCW:
				case FRONT:
					gl.glCullFace(mMirrorY?GL_BACK:GL_FRONT);
				break;
				case CW:
				case BACK:
					gl.glCullFace(mMirrorY?GL_FRONT:GL_BACK);
				break;
				default:
				break;
			}

			if(mFaceCulling==FaceCulling.NONE){
				gl.glEnable(GL_CULL_FACE);
			}
		}

		mFaceCulling=faceCulling;

		TOADLET_CHECK_GLERROR("setFaceCulling");
	}

	public void setDepthTest(DepthTest depthTest){
		if(mDepthTest==depthTest){
			return;
		}

		if(depthTest==DepthTest.NONE){
			gl.glDisable(GL_DEPTH_TEST);
		}
		else{
			switch(depthTest){
				case NEVER:
					gl.glDepthFunc(GL_NEVER);
				break;
				case LESS:
					gl.glDepthFunc(GL_LESS);
				break;
				case EQUAL:
					gl.glDepthFunc(GL_EQUAL);
				break;
				case LEQUAL:
					gl.glDepthFunc(GL_LEQUAL);
				break;
				case GREATER:
					gl.glDepthFunc(GL_GREATER);
				break;
				case NOTEQUAL:
					gl.glDepthFunc(GL_NOTEQUAL);
				break;
				case GEQUAL:
					gl.glDepthFunc(GL_GEQUAL);
				break;
				case ALWAYS:
					gl.glDepthFunc(GL_ALWAYS);
				break;
				default:
				break;
			}

			if(mDepthTest==DepthTest.NONE){
				gl.glEnable(GL_DEPTH_TEST);
			}
		}

		mDepthTest=depthTest;

		TOADLET_CHECK_GLERROR("setDepthTest");
	}

	public void setDepthWrite(boolean depthWrite){
		if(mDepthWrite==depthWrite){
			return;
		}

		if(depthWrite){
			gl.glDepthMask(true);
		}
		else{
			gl.glDepthMask(false);
		}

		mDepthWrite=depthWrite;

		TOADLET_CHECK_GLERROR("setDepthWrite");
	}

	public void setDithering(boolean dithering){
		if(mDithering==dithering){
			return;
		}

		if(dithering){
			gl.glEnable(GL_DITHER);
		}
		else{
			gl.glDisable(GL_DITHER);
		}

		mDithering=dithering;

		TOADLET_CHECK_GLERROR("setDithering");
	}

	public void setFogParameters(Fog fog,scalar nearDistance,scalar farDistance,Color color){
		if(fog==Fog.NONE){
			gl.glDisable(GL_FOG);
		}
		else{
			gl.glEnable(GL_FOG);
			#if defined(TOADLET_FIXED_POINT)
				#if defined(TOADLET_HAS_GLES)
					gl.glFogx(GL_FOG_START,nearDistance);
					gl.glFogx(GL_FOG_END,farDistance);
					gl.glFogxv(GL_FOG_COLOR,colorArray(cacheArray,color),0);
				#else
					gl.glFogf(GL_FOG_START,MathConversion.scalarToFloat(nearDistance));
					gl.glFogf(GL_FOG_END,MathConversion.scalarToFloat(farDistance));
					gl.glFogfv(GL_FOG_COLOR,colorArray(cacheArray,color),0);
				#endif
			#else
				gl.glFogf(GL_FOG_START,nearDistance);
				gl.glFogf(GL_FOG_END,farDistance);
				gl.glFogfv(GL_FOG_COLOR,colorArray(cacheArray,color),0);
			#endif
		}

		TOADLET_CHECK_GLERROR("setFogParameters");
	}

	public void setLightEffect(LightEffect lightEffect){
		// The GL_COLOR_MATERIAL command must come before the actual Material settings
		if(lightEffect.trackColor){
			#if !defined(TOADLET_HAS_GLES) && defined(TOADLET_HAS_GL_11)
			if(gl_version>=11){
				gl.glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);
			}
			#endif
			gl.glEnable(GL_COLOR_MATERIAL);
		}
		else{
			gl.glDisable(GL_COLOR_MATERIAL);
		}

		#if defined(TOADLET_FIXED_POINT)
			#if defined(TOADLET_HAS_GLES)
				gl.glMaterialxv(GL_FRONT_AND_BACK,GL_AMBIENT,colorArray(cacheArray,lightEffect.ambient),0);
				gl.glMaterialxv(GL_FRONT_AND_BACK,GL_DIFFUSE,colorArray(cacheArray,lightEffect.diffuse),0);
				gl.glMaterialxv(GL_FRONT_AND_BACK,GL_SPECULAR,colorArray(cacheArray,lightEffect.specular),0);
				gl.glMaterialx(GL_FRONT_AND_BACK,GL_SHININESS,lightEffect.shininess);
				gl.glMaterialxv(GL_FRONT_AND_BACK,GL_EMISSION,colorArray(cacheArray,lightEffect.emissive),0);
			#else
				gl.glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT,colorArray(cacheArray,lightEffect.ambient),0);
				gl.glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,colorArray(cacheArray,lightEffect.diffuse),0);
				gl.glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,colorArray(cacheArray,lightEffect.specular),0);
				gl.glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,MathConversion.scalarToFloat(lightEffect.shininess));
				gl.glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,colorArray(cacheArray,lightEffect.emissive),0);
			#endif
		#else
			gl.glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT,colorArray(cacheArray,lightEffect.ambient),0);
			gl.glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,colorArray(cacheArray,lightEffect.diffuse),0);
			gl.glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,colorArray(cacheArray,lightEffect.specular),0);
			gl.glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,lightEffect.shininess);
			gl.glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,colorArray(cacheArray,lightEffect.emissive),0);
		#endif

		TOADLET_CHECK_GLERROR("setLightEffect");
	}

	public void setFill(Fill fill){
		#if defined(TOADLET_HAS_GLES)
			Error.unimplemented(Categories.TOADLET_PEEPER,
				"GLRenderer::setFillType: unimplemented");
		#else
			if(mFill==fill){
				return;
			}

			if(fill==Fill.POINT){
				gl.glPolygonMode(GL_FRONT_AND_BACK,GL_POINT);
			}
			else if(fill==Fill.LINE){
				gl.glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
			}
			else{
				gl.glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
			}

			mFill=fill;

			TOADLET_CHECK_GLERROR("setFillType");
		#endif
	}

	public void setLighting(boolean lighting){
		if(mLighting==lighting){
			return;
		}

		if(lighting){
			gl.glEnable(GL_LIGHTING);
		}
		else{
			gl.glDisable(GL_LIGHTING);
		}

		mLighting=lighting;

		TOADLET_CHECK_GLERROR("setLighting");
	}

	public void setShading(Shading shading){
		if(mShading==shading){
			return;
		}

		switch(shading){
			case FLAT:
				gl.glShadeModel(GL_FLAT);
			break;
			case SMOOTH:
				gl.glShadeModel(GL_SMOOTH);
			break;
		}

		mShading=shading;
	}

	public void setColorWrite(boolean color){
		if(color){
			gl.glColorMask(true,true,true,true);
		}
		else{
			gl.glColorMask(false,false,false,false);
		}
	}

	public void setNormalize(Normalize normalize){
		if(mNormalize==normalize){
			return;
		}

		switch(normalize){
			case NONE:
				gl.glDisable(GL_NORMALIZE);
				gl.glDisable(GL_RESCALE_NORMAL);
			break;
			case RESCALE:
				gl.glDisable(GL_NORMALIZE);
				gl.glEnable(GL_RESCALE_NORMAL);
			break;
			case NORMALIZE:
				gl.glEnable(GL_NORMALIZE);
				gl.glDisable(GL_RESCALE_NORMAL);
			break;
		}

		mNormalize=normalize;

		TOADLET_CHECK_GLERROR("setNormalizeNormals");
	}

	public void setDepthBias(scalar constant,scalar slope){
		if(constant!=0 && slope!=0){
			#if defined(TOADLET_FIXED_POINT) && defined(TOADLET_HAS_GLES)
				gl.glPolygonOffsetx(slope,constant);
			#else
				gl.glPolygonOffset(MathConversion.scalarToFloat(slope),MathConversion.scalarToFloat(constant));
			#endif
			gl.glEnable(GL_POLYGON_OFFSET_FILL);
		}
		else{
			gl.glDisable(GL_POLYGON_OFFSET_FILL);
		}
	}

	public void setTexturePerspective(boolean texturePerspective){
		if(mTexturePerspective==texturePerspective){
			return;
		}

		#if defined(TOADLET_HAS_GLES)
			gl.glHint(GL_PERSPECTIVE_CORRECTION_HINT,texturePerspective?GL_NICEST:GL_FASTEST);
		#endif

		mTexturePerspective=texturePerspective;

		TOADLET_CHECK_GLERROR("setTexturePerspective");
	}

	public void setPointParameters(boolean sprite,scalar size,boolean attenuated,scalar constant,scalar linear,scalar quadratic,scalar minSize,scalar maxSize){
		// pointsize = size / sqrt(constant + linear*d + quadratic*d*d)
		// if a&b = 0, then quadratic = 1/(C*C) where C = first component of projMatrix * 1/2 screen width
		if(mCapabilitySet.pointSprites){
			int value;
			if(sprite){
				gl.glEnable(GL_POINT_SPRITE);
				value=1;
			}
			else{
				gl.glDisable(GL_POINT_SPRITE);
				value=0;
			}
			int i;
			for(i=0;i<mCapabilitySet.maxTextureStages;++i){
				gl.glTexEnvi(GL_POINT_SPRITE,GL_COORD_REPLACE,value);
			}
		}
		
		gl.glPointSize(MathConversion.scalarToFloat(size));
		
		// Attenuation is unimplemented on android
		#if !defined(TOADLET_PLATFORM_ANDROID)
			if(attenuated){
				#if defined(TOADLET_FIXED_POINT)
					#if defined(TOADLET_HAS_GLES)
						cacheArray[0]=constant; cacheArray[1]=linear; cacheArray[2]=quadratic;
						gl.glPointParametersxv(GL_POINT_DISTANCE_ATTENUATION,cacheArray,0);
					#else
						cacheArray[0]=MathConversion.scalarToFloat(constant); cacheArray[1]=MathConversion.scalarToFloat(linear); cacheArray[2]=MathConversion.scalarToFloat(quadratic);
						gl.glPointParametersfv(GL_POINT_DISTANCE_ATTENUATION,cacheArray,0);
					#endif
				#else
					cacheArray[0]=constant; cacheArray[1]=linear; cacheArray[2]=quadratic;
					gl.glPointParametersfv(GL_POINT_DISTANCE_ATTENUATION,cacheArray,0);
				#endif
			}
		#endif

		TOADLET_CHECK_GLERROR("setPointSprite");
	}
	
	public void setTextureStage(int stage,TextureStage textureStage){
		mStatisticsSet.textureChangeCount++;

		if(mCapabilitySet.maxTextureStages>1){
			gl.glActiveTexture(GL_TEXTURE0+stage);
		}

		if(textureStage!=null){
			Texture texture=textureStage.getTexture();
			int textureTarget=0;
			if(texture!=null && texture.internal_getTexturePeer()!=null){
				GLTexturePeer peer=(GLTexturePeer)texture.internal_getTexturePeer();

				textureTarget=peer.textureTarget;

				if(mLastTextures[stage]!=texture){
					gl.glBindTexture(textureTarget,peer.textureHandle[0]);
					mLastTextures[stage]=texture;
				}

				if(textureStage.getTextureMatrixIdentity()==false
					|| texture.getType()==Texture.Type.RENDER
					#if defined(TOADLET_HAS_GLEW)
						|| textureTarget==GL_TEXTURE_RECTANGLE_ARB
					#endif
				){
					if(mMatrixMode!=GL_TEXTURE){
						mMatrixMode=GL_TEXTURE;
						gl.glMatrixMode(mMatrixMode);
					}
					#if defined(TOADLET_FIXED_POINT)
						#if defined(TOADLET_HAS_GLES)
							gl.glLoadMatrixx(textureStage.getTextureMatrix().getData(),0);
							gl.glMultMatrixx(peer.textureMatrix.getData(),0);
						#else
							gl.glLoadMatrixf(MathConversion.scalarToFloat(cacheMatrix4x4,textureStage.getTextureMatrix()).getData(),0);
							gl.glMultMatrixf(MathConversion.scalarToFloat(cacheMatrix4x4,peer->textureMatrix).getData(),0);
						#endif
					#else
						gl.glLoadMatrixf(textureStage.getTextureMatrix().getData(),0);
						gl.glMultMatrixf(peer.textureMatrix.getData(),0);
					#endif
				}
				else{
					if(mMatrixMode!=GL_TEXTURE){
						mMatrixMode=GL_TEXTURE;
						gl.glMatrixMode(mMatrixMode);
					}
					gl.glLoadIdentity();
				}
			}
			else{
				textureTarget=GL_TEXTURE_2D;
				if(mLastTextures[stage]!=texture){
					gl.glBindTexture(textureTarget,0);
					mLastTextures[stage]=texture;
				}
			}

			if(mLastTexTargets[stage]!=textureTarget){
				if(mLastTexTargets[stage]!=0){
					gl.glDisable(mLastTexTargets[stage]);
				}
				if(textureTarget!=0){
					gl.glEnable(textureTarget);
				}
				mLastTexTargets[stage]=textureTarget;
			}

			TextureBlend blend=textureStage.getBlend();
			if(blend.operation!=TextureBlend.Operation.UNSPECIFIED){
				int mode=0;
				switch(blend.operation){
					case REPLACE:
						mode=GL_REPLACE;
					break;
					case MODULATE:
						mode=GL_MODULATE;
					break;
					case ADD:
						mode=GL_ADD;
					break;
					case DOTPRODUCT:
						mode=GL_DOT3_RGB;
					break;
					default:
					break;
				}
				
				if(blend.source1==TextureBlend.Source.UNSPECIFIED || blend.source2==TextureBlend.Source.UNSPECIFIED){
					gl.glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,mode);
				}
				else{
					gl.glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_COMBINE);
					gl.glTexEnvi(GL_TEXTURE_ENV,GL_COMBINE_RGB,mode);
					gl.glTexEnvi(GL_TEXTURE_ENV,GL_SRC0_RGB,GLTexturePeer.getGLTextureBlendSource(blend.source1));
					gl.glTexEnvi(GL_TEXTURE_ENV,GL_SRC1_RGB,GLTexturePeer.getGLTextureBlendSource(blend.source2));
					gl.glTexEnvi(GL_TEXTURE_ENV,GL_SRC2_RGB,GL_CONSTANT);
				}
			}
			else{
				gl.glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
			}

			if(textureStage.getAddressModeSpecified()){
				gl.glTexParameteri(textureTarget,GL_TEXTURE_WRAP_S,GLTexturePeer.getGLWrap(textureStage.getSAddressMode()));
				gl.glTexParameteri(textureTarget,GL_TEXTURE_WRAP_T,GLTexturePeer.getGLWrap(textureStage.getTAddressMode()));
				#if defined(TOADLET_HAS_GL_12)
					if(gl_version>=12){
						gl.glTexParameteri(textureTarget,GL_TEXTURE_WRAP_R,GLTexturePeer.getGLWrap(textureStage.getRAddressMode()));
					}
				#endif
			}

			if(textureStage.getFilterSpecified()){
				gl.glTexParameteri(textureTarget,GL_TEXTURE_MIN_FILTER,GLTexturePeer.getGLMinFilter(textureStage.getMinFilter(),textureStage.getMipFilter()));
				gl.glTexParameteri(textureTarget,GL_TEXTURE_MAG_FILTER,GLTexturePeer.getGLMagFilter(textureStage.getMagFilter()));
			}

			#if !defined(TOADLET_HAS_GLES)
				if(texture!=null && (texture.getFormat()&Texture.Format.BIT_DEPTH)>0){
					// Enable shadow comparison
					gl.glTexParameteri(textureTarget,GL_TEXTURE_COMPARE_MODE_ARB,GL_COMPARE_R_TO_TEXTURE);

					// Shadow comparison should be true (ie not in shadow) if r<=texture
					gl.glTexParameteri(textureTarget,GL_TEXTURE_COMPARE_FUNC_ARB,GL_LEQUAL);

					// Shadow comparison should generate an INTENSITY result
					gl.glTexParameteri(textureTarget,GL_DEPTH_TEXTURE_MODE_ARB,GL_INTENSITY);
				}
				else{
					// Only disable this TexParameter, the others are just parameters to this one
					gl.glTexParameteri(textureTarget,GL_TEXTURE_COMPARE_MODE_ARB,GL_NONE);
				}
			#endif

			int texCoordIndex=textureStage.getTexCoordIndex();
			if(mTexCoordIndexes[stage]!=texCoordIndex){
				mTexCoordIndexes[stage]=(short)texCoordIndex;
			}

			if(stage>=mMaxTexCoordIndex){
				mMaxTexCoordIndex=(short)(stage+1);
			}
		}
		else{
			if(mLastTextures[stage]!=null){
				mLastTextures[stage]=null;
			}

			if(mLastTexTargets[stage]!=0){
				gl.glDisable(mLastTexTargets[stage]);
				mLastTexTargets[stage]=0;
			}

			while(mMaxTexCoordIndex>0 && mLastTexTargets[mMaxTexCoordIndex-1]==0){
				mMaxTexCoordIndex--;
			}
		}

		if(mCapabilitySet.maxTextureStages>1){
			gl.glActiveTexture(GL_TEXTURE0);
		}
	}

	//public void setProgram(Program program){}

	public void setTexCoordGen(int stage,TexCoordGen texCoordGen,Matrix4x4 matrix){
		#if defined(TOADLET_HAS_GLES)
			Error.unimplemented(Categories.TOADLET_PEEPER,
				"GLRenderer::setTexCoordGen: unimplemented");
		#else
			if(mCapabilitySet.maxTextureStages>1){
				gl.glActiveTexture(GL_TEXTURE0+stage);
			}

			if(texCoordGen!=TexCoordGen.DISABLED){
				if(mMatrixMode!=GL_MODELVIEW){
					mMatrixMode=GL_MODELVIEW;
					gl.glMatrixMode(mMatrixMode);
				}

				gl.glPushMatrix();

				gl.glLoadIdentity();

				int gltg=0;
				int glpl=0;
				switch(texCoordGen){
					case OBJECTSPACE:
						gltg=GL_OBJECT_LINEAR;
						glpl=GL_OBJECT_PLANE;
					break;
					case CAMERASPACE:
						gltg=GL_EYE_LINEAR;
						glpl=GL_EYE_PLANE;
					break;
					default:
					break;
				}

				// Set up texgen
				gl.glTexGeni(GL_S,GL_TEXTURE_GEN_MODE,gltg);
				cacheArray[0]=matrix.at(0,0);cacheArray[1]=matrix.at(0,1);cacheArray[2]=matrix.at(0,2);cacheArray[3]=matrix.at(0,3);
				gl.glTexGenfv(GL_S,glpl,cacheArray,0);
				gl.glEnable(GL_TEXTURE_GEN_S);

				gl.glTexGeni(GL_T,GL_TEXTURE_GEN_MODE,gltg);
				cacheArray[0]=matrix.at(1,0);cacheArray[1]=matrix.at(1,1);cacheArray[2]=matrix.at(1,2);cacheArray[3]=matrix.at(1,3);
				gl.glTexGenfv(GL_T,glpl,cacheArray,0);
				gl.glEnable(GL_TEXTURE_GEN_T);

				gl.glTexGeni(GL_R,GL_TEXTURE_GEN_MODE,gltg);
				cacheArray[0]=matrix.at(2,0);cacheArray[1]=matrix.at(2,1);cacheArray[2]=matrix.at(2,2);cacheArray[3]=matrix.at(2,3);
				gl.glTexGenfv(GL_R,glpl,cacheArray,0);
				gl.glEnable(GL_TEXTURE_GEN_R);

				gl.glTexGeni(GL_Q,GL_TEXTURE_GEN_MODE,gltg);
				cacheArray[0]=matrix.at(3,0);cacheArray[1]=matrix.at(3,1);cacheArray[2]=matrix.at(3,2);cacheArray[3]=matrix.at(3,3);
				gl.glTexGenfv(GL_Q,glpl,cacheArray,0);
				gl.glEnable(GL_TEXTURE_GEN_Q);

				gl.glPopMatrix();
			}
			else{
				// Disable texgen
				gl.glDisable(GL_TEXTURE_GEN_S);
				gl.glDisable(GL_TEXTURE_GEN_T);
				gl.glDisable(GL_TEXTURE_GEN_R);
				gl.glDisable(GL_TEXTURE_GEN_Q);
			}

			if(mCapabilitySet.maxTextureStages>1){
				gl.glActiveTexture(GL_TEXTURE0);
			}

			TOADLET_CHECK_GLERROR("setTexCoordGen");
		#endif
	}

	public void getShadowBiasMatrix(Texture shadowTexture,Matrix4x4 result){
		result.set( Math.HALF, 0,         0,         Math.HALF,
					0,         Math.HALF, 0,         Math.HALF,
					0,         0,         Math.HALF, Math.HALF,
					0,         0,         0,         Math.ONE);
	}

	 // A workaround here, since OpenGL & Direct3D have different shadow comparison methods
	public void setShadowComparisonMethod(boolean enabled){
		if(enabled){
			setAlphaTest(AlphaTest.GEQUAL,Math.HALF);
		}
		else{
			setAlphaTest(AlphaTest.NONE,Math.HALF);
		}
	}

	// Light operations
	public void setLight(int i,Light light){
		int l=GL_LIGHT0+i;

		if(mMatrixMode!=GL_MODELVIEW){
			mMatrixMode=GL_MODELVIEW;
			gl.glMatrixMode(mMatrixMode);
		}

		gl.glPushMatrix();
		#if	defined(TOADLET_FIXED_POINT) && defined(TOADLET_HAS_GLES)
			gl.glLoadMatrixx(mViewMatrix.getData(),0);
		#else
			gl.glLoadMatrixf(mViewMatrix.getData(),0);
		#endif

		switch(light.getType()){
			case DIRECTION:{
				#if defined(TOADLET_FIXED_POINT) && defined(TOADLET_HAS_GLES)
					gl.glLightxv(l,GL_POSITION,lightDirArray(cacheArray,light.getDirection()),0);

					gl.glLightx(l,GL_CONSTANT_ATTENUATION,Math.ONE);
					gl.glLightx(l,GL_LINEAR_ATTENUATION,0);
					gl.glLightx(l,GL_QUADRATIC_ATTENUATION,0);

					gl.glLightx(l,GL_SPOT_CUTOFF,Math.ONE*180);
					gl.glLightx(l,GL_SPOT_EXPONENT,0);

				#else
					gl.glLightfv(l,GL_POSITION,lightDirArray(cacheArray,light.getDirection()),0);

					gl.glLightf(l,GL_CONSTANT_ATTENUATION,Math.ONE);
					gl.glLightf(l,GL_LINEAR_ATTENUATION,0);
					gl.glLightf(l,GL_QUADRATIC_ATTENUATION,0);

					gl.glLightf(l,GL_SPOT_CUTOFF,MathConversion.scalarToFloat(Math.ONE)*180);
					gl.glLightf(l,GL_SPOT_EXPONENT,0);
				#endif

				break;
			}
			case POSITION:{
				#if defined(TOADLET_FIXED_POINT) && defined(TOADLET_HAS_GLES)
					gl.glLightxv(l,GL_POSITION,lightPosArray(cacheArray,light.getPosition()),0);

					gl.glLightx(l,GL_CONSTANT_ATTENUATION,0);
					gl.glLightx(l,GL_LINEAR_ATTENUATION,light.getLinearAttenuation());
					gl.glLightx(l,GL_QUADRATIC_ATTENUATION,light.getLinearAttenuation());

					gl.glLightx(l,GL_SPOT_CUTOFF,Math.ONE*180);
					gl.glLightx(l,GL_SPOT_EXPONENT,0);
				#else
					gl.glLightfv(l,GL_POSITION,lightPosArray(cacheArray,light.getPosition()),0);

					gl.glLightf(l,GL_CONSTANT_ATTENUATION,0);
					gl.glLightf(l,GL_LINEAR_ATTENUATION,MathConversion.scalarToFloat(light.getLinearAttenuation()));
					gl.glLightf(l,GL_QUADRATIC_ATTENUATION,MathConversion.scalarToFloat(light.getLinearAttenuation()));

					gl.glLightf(l,GL_SPOT_CUTOFF,MathConversion.scalarToFloat(Math.ONE)*180);
					gl.glLightf(l,GL_SPOT_EXPONENT,0);
				#endif
				break;
			}
			case SPOT:{
				#if defined(TOADLET_FIXED_POINT) && defined(TOADLET_HAS_GLES)
					gl.glLightxv(l,GL_POSITION,lightPosArray(cacheArray,light.getPosition()),0);

					gl.glLightx(l,GL_CONSTANT_ATTENUATION,0);
					gl.glLightx(l,GL_LINEAR_ATTENUATION,light.getLinearAttenuation());
					gl.glLightx(l,GL_QUADRATIC_ATTENUATION,light.getLinearAttenuation());
				#else
					gl.glLightfv(l,GL_POSITION,lightPosArray(cacheArray,light.getPosition()),0);

					gl.glLightf(l,GL_CONSTANT_ATTENUATION,0);
					gl.glLightf(l,GL_LINEAR_ATTENUATION,MathConversion.scalarToFloat(light.getLinearAttenuation()));
					gl.glLightf(l,GL_QUADRATIC_ATTENUATION,MathConversion.scalarToFloat(light.getLinearAttenuation()));
				#endif

				#if defined(TOADLET_FIXED_POINT) && defined(TOADLET_HAS_GLES)
					gl.glLightxv(l,GL_SPOT_DIRECTION,lightPosArray(cacheArray,light.getDirection()),0);

					gl.glLightx(l,GL_SPOT_CUTOFF,(Math.ONE-light.getSpotCutoff())*90);
					gl.glLightx(l,GL_SPOT_EXPONENT,Math.ONE*5);
				#else
					gl.glLightfv(l,GL_SPOT_DIRECTION,lightPosArray(cacheArray,light.getDirection()),0);

					gl.glLightf(l,GL_SPOT_CUTOFF,MathConversion.scalarToFloat(Math.ONE-light.getSpotCutoff())*90);
					gl.glLightf(l,GL_SPOT_EXPONENT,MathConversion.scalarToFloat(Math.ONE)*5);
				#endif

				break;
			}
		}

		gl.glPopMatrix();

		#if defined(TOADLET_FIXED_POINT)
			#if defined(TOADLET_HAS_GLES)
				gl.glLightxv(l,GL_SPECULAR,colorArray(cacheArray,light.getSpecularColor()),0);
				gl.glLightxv(l,GL_DIFFUSE,colorArray(cacheArray,light.getDiffuseColor()),0);
				// Ambient lighting works through the GL_LIGHT_MODEL_AMBIENT
				gl.glLightxv(l,GL_AMBIENT,colorArray(cacheArray,Colors.BLACK),0);
			#else
				gl.glLightfv(l,GL_SPECULAR,colorArray(cacheArray,light.getSpecularColor()),0);
				gl.glLightfv(l,GL_DIFFUSE,colorArray(cacheArray,light.getDiffuseColor()),0);
				// Ambient lighting works through the GL_LIGHT_MODEL_AMBIENT
				gl.glLightfv(l,GL_AMBIENT,colorArray(cacheArray,Colors.BLACK),0);
			#endif
		#else
			gl.glLightfv(l,GL_SPECULAR,colorArray(cacheArray,light.getSpecularColor()),0);
			gl.glLightfv(l,GL_DIFFUSE,colorArray(cacheArray,light.getDiffuseColor()),0);
			// Ambient lighting works through the GL_LIGHT_MODEL_AMBIENT
			gl.glLightfv(l,GL_AMBIENT,colorArray(cacheArray,Colors.BLACK),0);
		#endif

		TOADLET_CHECK_GLERROR("setLight");
	}

	public void setLightEnabled(int i,boolean enable){
		if(enable){
			gl.glEnable(GL_LIGHT0+i);
		}
		else{
			gl.glDisable(GL_LIGHT0+i);
		}

		TOADLET_CHECK_GLERROR("setLightEnabled");
	}

	public void setAmbientColor(Color ambient){
		#if defined(TOADLET_FIXED_POINT)
			#if defined(TOADLET_HAS_GLES)
				gl.glLightModelxv(GL_LIGHT_MODEL_AMBIENT,colorArray(cacheArray,ambient),0);
			#else
				gl.glLightModelfv(GL_LIGHT_MODEL_AMBIENT,colorArray(cacheArray,ambient),0);
			#endif
		#else
			gl.glLightModelfv(GL_LIGHT_MODEL_AMBIENT,colorArray(cacheArray,ambient),0);
		#endif

		TOADLET_CHECK_GLERROR("setAmbientColor");
	}

	// Misc operations
	public void setMirrorY(boolean mirrorY){
		mMirrorY=mirrorY;

		FaceCulling faceCulling=mFaceCulling;
		mFaceCulling=FaceCulling.NONE;
		setFaceCulling(faceCulling);
	}

	public void copyFrameBufferToTexture(Texture texture){
		GLTexturePeer peer=(GLTexturePeer)texture.internal_getTexturePeer();
		if(peer==null){
			Error.unknown(Categories.TOADLET_PEEPER,
				"GLRenderercopyBufferToTexture: invalid peer");
			return;
		}

		int width=texture.getWidth();
		int height=texture.getHeight();

		gl.glBindTexture(peer.textureTarget,peer.textureHandle[0]);
		gl.glCopyTexSubImage2D(peer.textureTarget,0,0,0,0,0,width,height);

		if(texture.getAutoGenerateMipMaps()){
			peer.generateMipMaps();
		}

		gl.glBindTexture(peer.textureTarget,0);

		TOADLET_CHECK_GLERROR("copyBufferToTexture");
	}

	public StatisticsSet getStatisticsSet(){return mStatisticsSet;}
	public CapabilitySet getCapabilitySet(){return mCapabilitySet;}

	#if defined(TOADLET_FIXED_POINT) && defined(TOADLET_HAS_GLES)
		org.toadlet.egg.mathfixed.Matrix4x4 cacheMatrix4x4=new org.toadlet.egg.mathfixed.Matrix4x4();
		fixed[] cacheArray=new fixed[4];
	#else
		org.toadlet.egg.math.Matrix4x4 cacheMatrix4x4=new org.toadlet.egg.math.Matrix4x4();
		float[] cacheArray=new float[4];
	#endif

	public StatisticsSet internal_getStatisticsSet(){return mStatisticsSet;}

	public fixed[] colorArray(fixed[] result,Color src){result[0]=MathConversion.scalarToFixed(src.r);result[1]=MathConversion.scalarToFixed(src.g);result[2]=MathConversion.scalarToFixed(src.b);result[3]=MathConversion.scalarToFixed(src.a);return result;}
	public float[] colorArray(float[] result,Color src){result[0]=MathConversion.scalarToFloat(src.r);result[1]=MathConversion.scalarToFloat(src.g);result[2]=MathConversion.scalarToFloat(src.b);result[3]=MathConversion.scalarToFloat(src.a);return result;}
	public fixed[] lightDirArray(fixed[] result,Vector3 src){result[0]=MathConversion.scalarToFixed(-src.x);result[1]=MathConversion.scalarToFixed(-src.y);result[2]=MathConversion.scalarToFixed(-src.z);result[3]=0;return result;}
	public float[] lightDirArray(float[] result,Vector3 src){result[0]=MathConversion.scalarToFloat(-src.x);result[1]=MathConversion.scalarToFloat(-src.y);result[2]=MathConversion.scalarToFloat(-src.z);result[3]=0;return result;}
	public fixed[] lightPosArray(fixed[] result,Vector3 src){result[0]=MathConversion.scalarToFixed(src.x);result[1]=MathConversion.scalarToFixed(-src.y);result[2]=MathConversion.scalarToFixed(-src.z);result[3]=MathConversion.scalarToFixed(Math.ONE);return result;}
	public float[] lightPosArray(float[] result,Vector3 src){result[0]=MathConversion.scalarToFloat(src.x);result[1]=MathConversion.scalarToFloat(-src.y);result[2]=MathConversion.scalarToFloat(-src.z);result[3]=MathConversion.scalarToFloat(Math.ONE);return result;}

	java.nio.ByteBuffer allocScratchBuffer(int size){
		int i;
		for(i=0;i<mScratchBuffers.size();++i){
			ScratchBuffer buffer=mScratchBuffers.get(i);
			if(buffer.free && buffer.size>=size){
				buffer.free=false;
				buffer.data.limit(size);
				return buffer.data;
			}
		}

		ScratchBuffer buffer=new ScratchBuffer();
		buffer.size=size;
		buffer.free=false;
		buffer.data=java.nio.ByteBuffer.allocateDirect(size).order(java.nio.ByteOrder.nativeOrder());
		mScratchBuffers.add(buffer);

		return buffer.data;
	}

	void freeScratchBuffer(java.nio.ByteBuffer data){
		int i;
		for(i=0;i<mScratchBuffers.size();++i){
			if(mScratchBuffers.get(i).data==data){
				mScratchBuffers.get(i).free=true;
				return;
			}
		}

		Error.unknown("invalid scratch buffer freed");
	}

	protected int getGLBlendOperation(Blend.Operation blend){
		switch(blend){
			case ONE:
				return GL_ONE;
			case ZERO:
				return GL_ZERO;
			case DEST_COLOR:
				return GL_DST_COLOR;
			case SOURCE_COLOR:
				return GL_SRC_COLOR;
			case ONE_MINUS_DEST_COLOR:
				return GL_ONE_MINUS_DST_COLOR;
			case ONE_MINUS_SOURCE_COLOR:
				return GL_ONE_MINUS_SRC_COLOR;
			case DEST_ALPHA:
				return GL_ONE_MINUS_DST_ALPHA;
			case SOURCE_ALPHA:
				return GL_SRC_ALPHA;
			case ONE_MINUS_DEST_ALPHA:
				return GL_ONE_MINUS_DST_ALPHA;
			case ONE_MINUS_SOURCE_ALPHA:
				return GL_ONE_MINUS_SRC_ALPHA;
			default:
				return 0;
		}
	}

	protected void setVertexData(VertexData vertexData){
		boolean hasColorData=false;
		int numVertexBuffers=vertexData.getNumVertexBuffers();
		int i,j;
		for(i=0;i<numVertexBuffers;++i){
			VertexBuffer vertexBuffer=vertexData.getVertexBuffer(i);
			GLBufferPeer vertexBufferPeer=(GLBufferPeer)vertexBuffer.internal_getBufferPeer();

			// TODO: Unify
			java.nio.ByteBuffer basePointer=null;
			if(vertexBufferPeer==null){
				if(mCapabilitySet.hardwareVertexBuffers){
					gl11.glBindBuffer(GL_ARRAY_BUFFER,0);
				}
				basePointer=vertexBuffer.internal_getData();
			}
			else{
				gl11.glBindBuffer(GL_ARRAY_BUFFER,vertexBufferPeer.bufferHandle[0]);
			}

			VertexFormat vertexFormat=vertexBuffer.getVertexFormat();
			int vertexSize=vertexFormat.getVertexSize();
			int numVertexFormats=vertexFormat.getNumVertexElements();
			for(j=0;j<numVertexFormats;++j){
				VertexElement vertexElement=vertexFormat.getVertexElement(j);

				int elementCount=getGLElementCount(vertexElement.format);
				int dataType=getGLDataType(vertexElement.format);

				if(vertexBufferPeer==null){
					switch(vertexElement.type){
						case POSITION:
							gl.glEnableClientState(GL_VERTEX_ARRAY);
							gl.glVertexPointer(elementCount,dataType,vertexSize,basePointer.position(vertexElement.offset));
						break;
						case NORMAL:
							gl.glEnableClientState(GL_NORMAL_ARRAY);
							gl.glNormalPointer(dataType,vertexSize,basePointer.position(vertexElement.offset));
						break;
						case COLOR:
							gl.glEnableClientState(GL_COLOR_ARRAY);
							gl.glColorPointer(elementCount,dataType,vertexSize,basePointer.position(vertexElement.offset));
							hasColorData=true;
						break;
						case POINT_SIZE:
							gl.glEnableClientState(GL_POINT_SIZE_ARRAY);
							gl11.glPointSizePointer(dataType,vertexSize,basePointer.position(vertexElement.offset));
							hasColorData=true;
						break;
						default:
						break;
					}
				}
				else{
					switch(vertexElement.type){
						case POSITION:
							gl.glEnableClientState(GL_VERTEX_ARRAY);
							gl11.glVertexPointer(elementCount,dataType,vertexSize,vertexElement.offset);
						break;
						case NORMAL:
							gl.glEnableClientState(GL_NORMAL_ARRAY);
							gl11.glNormalPointer(dataType,vertexSize,vertexElement.offset);
						break;
						case COLOR:
							gl.glEnableClientState(GL_COLOR_ARRAY);
							gl11.glColorPointer(elementCount,dataType,vertexSize,vertexElement.offset);
							hasColorData=true;
						break;
						case POINT_SIZE:
							gl.glEnableClientState(GL_POINT_SIZE_ARRAY);
							// glPointSizePointer with only an offset is not available in android
							gl11.glPointSizePointer(dataType,vertexSize,basePointer.position(vertexElement.offset));
							hasColorData=true;
						break;
						default:
						break;
					}
				}
			}

			// Bind Tex Coords separately because they depend on the texture units currently used
			mLastMaxTexCoordIndex=mMaxTexCoordIndex;
			for(j=0;j<mMaxTexCoordIndex;++j){
				int texCoordSet=mTexCoordIndexes[j];
				mLastTexCoordIndexes[j]=(short)texCoordSet;

				if(texCoordSet>=0 && texCoordSet<=vertexFormat.getMaxTexCoordIndex()){
					 VertexElement vertexElement=vertexFormat.getTexCoordElementByIndex(texCoordSet);

					if(mCapabilitySet.maxTextureStages>1){
						gl.glClientActiveTexture(GL_TEXTURE0+j);
					}

					gl.glEnableClientState(GL_TEXTURE_COORD_ARRAY);

					if(vertexBufferPeer==null){
						gl.glTexCoordPointer(
							getGLElementCount(vertexElement.format),
							getGLDataType(vertexElement.format),
							vertexSize,basePointer.position(vertexElement.offset));
					}
					else{
						gl11.glTexCoordPointer(
							getGLElementCount(vertexElement.format),
							getGLDataType(vertexElement.format),
							vertexSize,vertexElement.offset);
					}
				}
			}
		}

		if(mCapabilitySet.maxTextureStages>1){
			gl.glClientActiveTexture(GL_TEXTURE0);
		}

		if(hasColorData==false){
			gl.glColor4f(1.0f,1.0f,1.0f,1.0f);
		}

		TOADLET_CHECK_GLERROR("setVertexData");
	}
	
	protected void unsetVertexData(VertexData vertexData){
		int numVertexBuffers=vertexData.getNumVertexBuffers();
		int i,j;
		for(i=0;i<numVertexBuffers;++i){
			VertexBuffer vertexBuffer=vertexData.getVertexBuffer(i);

			VertexFormat vertexFormat=vertexBuffer.getVertexFormat();
			int numVertexElements=vertexFormat.getNumVertexElements();
			for(j=0;j<numVertexElements;++j){
				 VertexElement vertexElement=vertexFormat.getVertexElement(j);

				switch(vertexElement.type){
					case POSITION:
						gl.glDisableClientState(GL_VERTEX_ARRAY);
					break;
					case NORMAL:
						gl.glDisableClientState(GL_NORMAL_ARRAY);
					break;
					case COLOR:
						gl.glDisableClientState(GL_COLOR_ARRAY);
					break;
					case POINT_SIZE:
						gl.glDisableClientState(GL_POINT_SIZE_ARRAY);
					default:
					break;
				}
			}

			// Unbind Tex Coords separately because they depend on the texture units currently used
			for(j=0;j<mLastMaxTexCoordIndex;++j){
				int texCoordSet=mLastTexCoordIndexes[j];
				mLastTexCoordIndexes[j]=-1;

				if(texCoordSet>=0 && texCoordSet<=vertexFormat.getMaxTexCoordIndex()){
					// Unused
					//const VertexElement &vertexElement=vertexFormat.getTexCoordElementByIndex(texCoordSet);

					if(mCapabilitySet.maxTextureStages>1){
						gl.glClientActiveTexture(GL_TEXTURE0+j);
					}

					gl.glDisableClientState(GL_TEXTURE_COORD_ARRAY);
				}
			}
		}

		if(mCapabilitySet.maxTextureStages>1){
			gl.glClientActiveTexture(GL_TEXTURE0);
		}

		TOADLET_CHECK_GLERROR("unsetVertexData");
	}

	protected int getGLElementCount(int format){
		int count=0;

		if((format&VertexElement.Format.BIT_COUNT_1)>0){
			count=1;
		}
		else if((format&VertexElement.Format.BIT_COUNT_2)>0){
			count=2;
		}
		else if((format&VertexElement.Format.BIT_COUNT_3)>0){
			count=3;
		}
		else if((format&(VertexElement.Format.BIT_COUNT_4|VertexElement.Format.COLOR_RGBA))>0){
			count=4;
		}

		if(count==0){
			Error.unknown(Categories.TOADLET_PEEPER,
				"getGLElementCount: Invalid element count");
			return 0;
		}

		return count;
	}
	
	protected int getGLDataType(int format){
		int type=0;

		if((format&(VertexElement.Format.BIT_UINT_8|VertexElement.Format.COLOR_RGBA))>0){
			type=GL_UNSIGNED_BYTE;
		}
		else if((format&VertexElement.Format.BIT_INT_8)>0){
			type=GL_BYTE;
		}
		else if((format&VertexElement.Format.BIT_INT_16)>0){
			type=GL_SHORT;
		}
		#if !defined(TOADLET_HAS_GLES)
			else if((format&VertexElement.Format.BIT_INT_32)>0){
				type=GL_INT;
			}
		#endif
		#if defined(TOADLET_HAS_GLES)
			else if((format&VertexElement.Format.BIT_FIXED_32)>0){
				type=GL_FIXED;
			}
		#endif
		else if((format&VertexElement.Format.BIT_FLOAT_32)>0){
			type=GL_FLOAT;
		}
		#if !defined(TOADLET_HAS_GLES)
			else if((format&VertexElement.Format.BIT_DOUBLE_64)>0){
				type=GL_DOUBLE;
			}
		#endif

		if(type==0){
			Error.unknown(Categories.TOADLET_PEEPER,
				"getGLDataType: Invalid data type");
			return 0;
		}

		return type;
	}

	boolean mShutdown=true;
	int mMatrixMode=-1;

	boolean mFBORenderToTexture=false;
	boolean mPBufferRenderToTexture=false;

	// Cached render state attributes
	AlphaTest mAlphaTest=AlphaTest.NONE;
	scalar mAlphaCutoff=0;
	Blend mBlend=new Blend();
	DepthTest mDepthTest=DepthTest.NONE;
	boolean mDepthWrite=false;
	boolean mDithering=false;
	FaceCulling mFaceCulling=FaceCulling.NONE;
	Fill mFill=Fill.SOLID;
	boolean mLighting=false;
	Normalize mNormalize=Normalize.NONE;
	Shading mShading=Shading.SMOOTH;
	boolean mTexturePerspective=false;
	boolean mPointSprite=false;
	scalar mPointSize=0;

	Texture[] mLastTextures;

	short mMaxTexCoordIndex=0;
	short[] mTexCoordIndexes=new short[0];
	int mNumTexCoordIndexes=0;
	int[] mLastTexTargets=new int[0];
	int mNumLastTexTargets=0;

	VertexData mLastVertexData=null;
	IndexData mLastIndexData=null;
	short mLastMaxTexCoordIndex=0;
	short[] mLastTexCoordIndexes=new short[0];
	int mNumLastTexCoordIndexes=0;

	boolean mMirrorY=false;

	#if defined(TOADLET_FIXED_POINT) && defined(TOADLET_HAS_GLES)
		org.toadlet.egg.mathfixed.Matrix4x4 mModelMatrix=new org.toadlet.egg.mathfixed.Matrix4x4();
		org.toadlet.egg.mathfixed.Matrix4x4 mViewMatrix=new org.toadlet.egg.mathfixed.Matrix4x4();
	#else
		org.toadlet.egg.math.Matrix4x4 mModelMatrix=new org.toadlet.egg.math.Matrix4x4();
		org.toadlet.egg.math.Matrix4x4 mViewMatrix=new org.toadlet.egg.math.Matrix4x4();
	#endif

	StatisticsSet mStatisticsSet=new StatisticsSet();
	CapabilitySet mCapabilitySet=new CapabilitySet();

	RenderContext mRenderContext=null;
	RenderTarget mRenderTarget=null;

	class ScratchBuffer{
		public ScratchBuffer(){}
	
		public java.nio.ByteBuffer data;
		public int size;
		public boolean free;
	}
	java.util.Vector<ScratchBuffer> mScratchBuffers=new java.util.Vector<ScratchBuffer>();

	#if defined(TOADLET_DEBUG)
		int mBeginEndCounter=0;
	#endif
}
