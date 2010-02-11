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
import org.toadlet.egg.Error;
import org.toadlet.egg.Logger;
import org.toadlet.peeper.*;
import javax.microedition.khronos.opengles.*;
import static javax.microedition.khronos.opengles.GL11.*;

public class GLTexturePeer implements Texture.Peer{
	public GLTexturePeer(GLRenderer renderer,Texture texture){
		GL10 gl=renderer.gl;
		GL11 gl11=renderer.gl11;
		this.renderer=renderer;
		CapabilitySet capabilitySet=renderer.getCapabilitySet();

		int width=texture.getWidth();
		int height=texture.getHeight();
		int depth=texture.getDepth();
		textureTarget=getGLTarget(texture,capabilitySet);
		int format=getGLFormat(texture.getFormat());
		int internalFormat=format;
		int type=getGLType(texture.getFormat());

		gl.glGenTextures(1,textureHandle,0);
		gl.glBindTexture(textureTarget,textureHandle[0]);

		gl.glTexParameterx(textureTarget,GL_TEXTURE_WRAP_S,getGLWrap(texture.getSAddressMode()));
		gl.glTexParameterx(textureTarget,GL_TEXTURE_WRAP_T,getGLWrap(texture.getTAddressMode()));

		gl.glTexParameterx(textureTarget,GL_TEXTURE_MIN_FILTER,getGLMinFilter(texture.getMinFilter(),texture.getMipFilter()));
		gl.glTexParameterx(textureTarget,GL_TEXTURE_MAG_FILTER,getGLMagFilter(texture.getMagFilter()));

		manuallyBuildMipMaps=texture.getAutoGenerateMipMaps();
		if(manuallyBuildMipMaps){
			if(gl11!=null){
				gl11.glTexParameteri(textureTarget,GL_GENERATE_MIPMAP,GL_TRUE);
				manuallyBuildMipMaps=false;
			}
		}

		android.graphics.Bitmap image=texture.getImage();
		if(image!=null){
			if(	(Math.isPowerOf2(width)==false || Math.isPowerOf2(height)==false || Math.isPowerOf2(depth)==false) &&
				capabilitySet.textureNonPowerOf2==false)
			{
				Error.unknown(Categories.TOADLET_PEEPER,
					"GLTexturePeer: Cannot load a non power of 2 texture");
				return;
			}
			else{
				int pixelSize=2;
				if(image.getConfig()==android.graphics.Bitmap.Config.ALPHA_8) pixelSize=1;
				else if(image.getConfig()==android.graphics.Bitmap.Config.ARGB_8888) pixelSize=4;
				java.nio.ByteBuffer data=java.nio.ByteBuffer.allocateDirect(width*height*pixelSize).order(java.nio.ByteOrder.nativeOrder());
				image.copyPixelsToBuffer(data);
				data.rewind();
				// TODO: I believe this needs to go back in.  The textures we are currently
				//  using seem to work without any swapping, so we'll just leave it out for speed.
				/*if(image.getConfig()==android.graphics.Bitmap.Config.ARGB_4444){
					java.nio.ShortBuffer shortData=data.asShortBuffer();
					for(int i=0;i<shortData.limit();++i){
						int s=(int)shortData.get(i);
						shortData.put(i,(short)((0xFFF0&(s<<4))|(0xF&(s>>12))));
					}
				}
				else if(image.getConfig()==android.graphics.Bitmap.Config.ARGB_8888){
					java.nio.IntBuffer intData=data.asIntBuffer();
					for(int i=0;i<intData.limit();++i){
						int s=intData.get(i);
						intData.put(i,(int)((0xFFFFFF00&(s<<8))|(0xFF&(s>>24))));
					}
				}*/

				switch(textureTarget){
					case GL_TEXTURE_2D:
						gl.glTexImage2D(textureTarget,0,internalFormat,width,height,0,format,type,data);
					break;
				}

				if(manuallyBuildMipMaps){
					generateMipMaps();
				}
			}
		}

		TOADLET_CHECK_GLERROR("GLTexturePeer::GLTexturePeer");
	}

	public void destroy(){
		GL10 gl=renderer.gl;
		if(textureHandle[0]!=0){
			gl.glDeleteTextures(1,textureHandle,0);
			textureHandle[0]=0;
		}

		TOADLET_CHECK_GLERROR("GLTexturePeer::~GLTexturePeer");
	}

	public RenderTarget.Peer castToRenderTargetPeer(){return null;}

	public void generateMipMaps(){
		Logger.warning(Categories.TOADLET_PEEPER,
			"GLTexturePeer::generateMipMaps: Not implemented");
	}

	public boolean isValid(){return true;}

	public static int getGLTarget(Texture texture,CapabilitySet capabilitySet){
		Texture.Dimension dimension=texture.getDimension();
		int target=0;

		switch(dimension){
			case D2:
				target=GL_TEXTURE_2D;
			break;
			default:
			break;
		}
		
		return target;
	}

	public static int getGLFormat(int textureFormat){
		int format=0;
		
		if((textureFormat&Texture.Format.BIT_L)>0){
			format=GL_LUMINANCE;
		}
		else if((textureFormat&Texture.Format.BIT_A)>0){
			format=GL_ALPHA;
		}
		else if((textureFormat&Texture.Format.BIT_LA)>0){
			format=GL_LUMINANCE_ALPHA;
		}
		else if((textureFormat&Texture.Format.BIT_RGB)>0){
			format=GL_RGB;
		}
		else if((textureFormat&Texture.Format.BIT_RGBA)>0){
			format=GL_RGBA;
		}

		if(format==0){
			Error.unknown(Categories.TOADLET_PEEPER,
				"GLTexturePeer::getGLFormat: Invalid format");
			return 0;
		}

		return format;
	}

	public static int getGLType(int textureFormat){
		int type=0;

		if((textureFormat&Texture.Format.BIT_UINT_8)>0){
			type=GL_UNSIGNED_BYTE;
		}
		else if((textureFormat&Texture.Format.BIT_FLOAT_32)>0){
			type=GL_FLOAT;
		}
		else if((textureFormat&Texture.Format.BIT_UINT_5_6_5)>0){
			type=GL_UNSIGNED_SHORT_5_6_5;
		}
		else if((textureFormat&Texture.Format.BIT_UINT_5_5_5_1)>0){
			type=GL_UNSIGNED_SHORT_5_5_5_1;
		}
		else if((textureFormat&Texture.Format.BIT_UINT_4_4_4_4)>0){
			type=GL_UNSIGNED_SHORT_4_4_4_4;
		}

		if(type==0){
			Error.unknown(Categories.TOADLET_PEEPER,
				"GLTexturePeer::getGLType: Invalid type");
			return 0;
		}

		return type;
	}

	public static int getGLWrap(Texture.AddressMode addressMode){
		int wrap=0;

		switch(addressMode){
			case REPEAT:
				wrap=GL_REPEAT;
			break;
			case CLAMP_TO_EDGE:
				wrap=GL_CLAMP_TO_EDGE;
			break;
			default:
			break;
		}

		if(wrap==0){
			Error.unknown(Categories.TOADLET_PEEPER,
				"GLTexturePeer::getGLWrap: Invalid address mode");
			return 0;
		}

		return wrap;
	}

	public static int getGLMinFilter(Texture.Filter minFilter,Texture.Filter mipFilter){
		int filter=0;

		switch(mipFilter){
			case NONE:
				switch(minFilter){
					case NEAREST:
						filter=GL_NEAREST;
					break;
					case LINEAR:
						filter=GL_LINEAR;
					break;
					default:
					break;
				}
			break;
			case NEAREST:
				switch(minFilter){
					case NEAREST:
						filter=GL_NEAREST_MIPMAP_NEAREST;
					break;
					case LINEAR:
						filter=GL_NEAREST_MIPMAP_LINEAR;
					break;
					default:
					break;
				}
			break;
			case LINEAR:
				switch(minFilter){
					case NEAREST:
						filter=GL_LINEAR_MIPMAP_NEAREST;
					break;
					case LINEAR:
						filter=GL_LINEAR_MIPMAP_LINEAR;
					break;
					default:
					break;
				}
			break;
		}

		if(filter==0){
			Error.unknown(Categories.TOADLET_PEEPER,
				"GLTexturePeer::getGLMinFilter: Invalid filter");
			return 0;
		}

		return filter;
	}

	public static int getGLMagFilter(Texture.Filter magFilter){
		int filter=0;

		switch(magFilter){
			case NEAREST:
				filter=GL_NEAREST;
			break;
			case LINEAR:
				filter=GL_LINEAR;
			break;
			default:
			break;
		}

		if(filter==0){
			Error.unknown(Categories.TOADLET_PEEPER,
				"GLTexturePeer::getGLMagFilter: Invalid filter");
			return 0;
		}

		return filter;
	}

	public static int getGLTextureBlendSource(TextureBlend.Source blend){
		switch(blend){
			case PREVIOUS:
				return GL_PREVIOUS;
			case TEXTURE:
				return GL_TEXTURE;
			case PRIMARY_COLOR:
				return GL_PRIMARY_COLOR;
			default:
				return 0;
		}
	}
	
	GLRenderer renderer=null;
	int[] textureHandle=new int[1];
	int textureTarget=0;
	Matrix4x4 textureMatrix=new Matrix4x4();
	boolean manuallyBuildMipMaps=false;
}
