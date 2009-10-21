/********** Copyright header - do not remove **********
 *
 * The Toadlet Engine 
 *
 * Copyright (C) 2005, Lightning Toads Productions, LLC
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

#ifndef TOADLET_PEEPER_NDSRENDERER_H
#define TOADLET_PEEPER_NDSRENDERER_H

#include "../../Renderer.h"
#include "../../Blend.h"
#include "../../LightEffect.h"
#include "../../TextureStage.h"
#include "../../StatisticsSet.h"
#include "../../CapabilitySet.h"
#include "../../VertexElement.h"
#include "NDSIncludes.h"

namespace toadlet{
namespace peeper{

//class VertexBuffer;
//class NDSBufferPeer;

class NDSRenderer:public Renderer{
public:
	NDSRenderer();
	virtual ~NDSRenderer();

	// Startup/Shutdown
	bool startup(RenderContext *renderContext,Options *options);
	bool shutdown();
	Status getStatus();
	bool reset();

	// Resource operations
	TexturePeer *createTexturePeer(Texture *texture);
	BufferPeer *createBufferPeer(Buffer *buffer);
	ProgramPeer *createProgramPeer(Program *program);
	ShaderPeer *createShaderPeer(Shader *shader);

	// Matrix operations
	void setModelMatrix(const Matrix4x4 &matrix);
	void setViewMatrix(const Matrix4x4 &matrix);
	void setProjectionMatrix(const Matrix4x4 &matrix);

	// Rendering operations
	bool setRenderTarget(RenderTarget *target);
	RenderTarget *getRenderTarget();
	void setViewport(const Viewport &viewport);
	void clearRenderTargetBuffers(int buffers,const Color &color);
	void swapRenderTargetBuffers();
	void beginScene();
	void endScene();
	void renderPrimitive(const VertexData *vertexData,const IndexData *indexData);

	// Render state operations
	void setDefaultStates();
	void setColor(const Color &color);
	void setAlphaTest(const AlphaTest &alphaTest,scalar cutoff);
	void setBlend(const Blend &blend);
	void setFaceCulling(const FaceCulling &faceCulling);
	void setDepthTest(const DepthTest &depthTest);
	void setDepthWrite(bool depthWrite);
	void setDithering(bool dithering);
	void setFogParameters(const FogType &fogType,scalar nearDistance,scalar farDistance,const Color &color);
	void setLightEffect(const LightEffect &lightEffect);
	void setFillType(const FillType &fill);
	void setLighting(bool lighting);
	void setShading(const Shading &shading);
	void setColorWrite(bool color);
	void setNormalizeNormals(NormalizeType normalizeType);
	void setDepthBias(scalar constant,scalar slope);
	void setTextureStage(int stage,const TextureStage &textureStage);
	void disableTextureStage(int stage);
	void setProgram(const Program *program);
	void setTextureCoordinateGeneration(int stage,const TextureCoordinateGeneration &tcg,const Matrix4x4 &matrix);
	void getShadowBiasMatrix(const Texture *shadowTexture,Matrix4x4 &result);
	void setShadowComparisonMethod(bool enabled);

	// Light operations
	void setLight(int i,const Light &light);
	void setLightEnabled(int i,bool enable);
	void setAmbientColor(const Color &ambient);

	// Misc operations
	void copyFrameBufferToTexture(Texture *texture);

	const StatisticsSet &getStatisticsSet();
	const CapabilitySet &getCapabilitySet();

private:
	void initState();

	bool mShutdown;
	GL_MATRIX_MODE_ENUM mMatrixMode;

	int mRenderShiftBits;

	// Cached render state attributes
	AlphaTest mAlphaTest;
	scalar mAlphaCutoff;
	Blend mBlend;
	FaceCulling mFaceCulling;
	bool mLighting;
	Color mAmbientColor;

	uint32 mPolyFmt;
	uint32 mLightsFmt;
	Color mMaterialAmbientColor;

	Matrix4x4 mModelMatrix;
	Matrix4x4 mViewMatrix;
	Vector3 mModelScale;

	StatisticsSet mStatisticsSet;
	CapabilitySet mCapabilitySet;

	int mBeginEndCounter;
};

}
}

#endif
