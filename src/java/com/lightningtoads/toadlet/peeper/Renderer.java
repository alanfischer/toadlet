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

package com.lightningtoads.toadlet.peeper;

#include <com/lightningtoads/toadlet/peeper/Types.h>

public interface Renderer{
	public enum RendererStatus{
		OK,
		UNAVAILABLE,
		NEEDSRESET,
	}

	public class ClearFlag{
		public static int COLOR=1;
		public static int DEPTH=2;
		public static int STENCIL=4;
	}

	public enum FaceCulling{
		NONE,
		CCW,
		CW,
		FRONT,
		BACK,
	}

	public enum DepthTest{
		NONE,
		NEVER,
		LESS,
		EQUAL,
		LEQUAL,
		GREATER,
		NOTEQUAL,
		GEQUAL,
		ALWAYS,
	}

	public enum AlphaTest{
		NONE,
		LESS,
		EQUAL,
		LEQUAL,
		GREATER,
		NOTEQUAL,
		GEQUAL,
		ALWAYS,
	}

	public enum TexCoordGen{
		DISABLED,
		OBJECTSPACE,
		CAMERASPACE,
	}

	public enum Fog{
		NONE,
		LINEAR,
	}

	public enum Fill{
		POINT,
		LINE,
		SOLID,
	}

	public enum Shading{
		FLAT,
		SMOOTH,
	}

	public enum Normalize{
		NONE,
		RESCALE,
		NORMALIZE,
	}

	// Startup/Shutdown
	public boolean startup(RenderContext renderContext,int[] options);
	public boolean shutdown();
	public RendererStatus getStatus();
	public boolean reset();

	// Resource operations
	public Texture.Peer createTexturePeer(Texture texture);
	public Buffer.Peer createBufferPeer(Buffer buffer);
	//public Program.Peer createProgramPeer(Program program);
	//public Shader.Peer createShaderPeer(Shader shader);

	// Matrix operations
	public void setModelMatrix(Matrix4x4 matrix);
	public void setViewMatrix(Matrix4x4 matrix);
	public void setProjectionMatrix(Matrix4x4 matrix);

	// Rendering operations
	public RenderContext getRenderContext();
	public boolean setRenderTarget(RenderTarget target);
	public RenderTarget getRenderTarget();
	public void setViewport(Viewport viewport);
	public void clear(int clearFlags,Color clearColor);
	public void swap();
	public void beginScene();
	public void endScene();
	public void renderPrimitive(VertexData vertexData,IndexData indexData);

	// Render state operations
	public void setDefaultStates();
	public void setAlphaTest(AlphaTest alphaTest,scalar cutoff);
	public void setBlend(Blend blend);
	public void setFaceCulling(FaceCulling faceCulling);
	public void setDepthTest(DepthTest depthTest);
	public void setDepthWrite(boolean depthWrite);
	public void setDithering(boolean dithering);
	public void setFogParameters(Fog fog,scalar nearDistance,scalar farDistance,Color color);
	public void setLightEffect(LightEffect lightEffect);
	public void setFill(Fill fill);
	public void setLighting(boolean lighting);
	public void setShading(Shading shading);
	public void setColorWrite(boolean color);
	public void setNormalize(Normalize normalize);
	public void setDepthBias(scalar constant,scalar slope);
	public void setTexturePerspective(boolean texturePerspective);
	public void setPointParameters(boolean sprite,scalar size,boolean attenuated,scalar constant,scalar linear,scalar quadratic,scalar minSize,scalar maxSize);
	public void setTextureStage(int stage,TextureStage textureStage);
	//public void setProgram(Program program);
	public void setTexCoordGen(int stage,TexCoordGen texCoordGen,Matrix4x4 matrix);
	public void getShadowBiasMatrix(Texture shadowTexture,Matrix4x4 result);
	 // A workaround here, since OpenGL & Direct3D have different shadow comparison methods
	public void setShadowComparisonMethod(boolean enabled);

	// Light operations
	public void setLight(int i,Light light);
	public void setLightEnabled(int i,boolean enable);
	public void setAmbientColor(Color ambient);

	// Misc operations
	public void setMirrorY(boolean mirrorY);
	public void copyFrameBufferToTexture(Texture texture);

	public StatisticsSet getStatisticsSet();
	public CapabilitySet getCapabilitySet();
}
