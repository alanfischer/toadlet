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

#include "../../IndexData.h"
#include "../../IndexBuffer.h"
#include "../../VertexData.h"
#include "../../VertexBuffer.h"
#include "../../Viewport.h"
#include "../../Program.h"
#include "../../Shader.h"
#include "../../Light.h"
#include "../../Texture.h"
#include "../../RenderTexture.h"
#include "../../../egg/Logger.h"
#include "NDSRenderer.h"
#include "NDSTexturePeer.h"

using namespace toadlet::egg;
using namespace toadlet::egg::image;

namespace toadlet{
namespace peeper{

TOADLET_C_API Renderer* new_NDSRenderer(){
	return new NDSRenderer();
}

#if defined(TOADLET_BUILD_DYNAMIC)
	TOADLET_C_API Renderer* new_Renderer(){
		return new NDSRenderer();
	}
#endif

NDSRenderer::NDSRenderer(){
	mShutdown=true;
	mMatrixMode=(GL_MATRIX_MODE_ENUM)-1;

	mRenderShiftBits=8;

	mAlphaTest=AT_NONE;
	mAlphaCutoff=0;
	mFaceCulling=FC_NONE;
	mLighting=false;
	mAmbientColor=Colors::COLOR_WHITE;

	mPolyFmt=POLY_ALPHA(31);
	mLightsFmt=0;
	mMaterialAmbientColor=Colors::COLOR_WHITE;

	mBeginEndCounter=0;
}

NDSRenderer::~NDSRenderer(){
	if(mShutdown==false){
		TOADLET_ERROR(TOADLET_EGG_CATEGORY,Error::TYPE_UNKNOWN,
			"NDSRenderer::~NDSRenderer: Failed to shutdown NDSRenderer");
	}
}

bool NDSRenderer::startup(RenderContext *renderContext,Options *options){
	TOADLET_LOG(TOADLET_PEEPER_CATEGORY,Logger::LEVEL_ALERT,
		"NDSRenderer: Startup started");

	mShutdown=false;

	glInit();

	mCapabilitySet.maxTextureStages=1;

	mCapabilitySet.maxTextureSize=1024;

	mCapabilitySet.maxLights=4;

	mCapabilitySet.renderToTexture=false;

	mCapabilitySet.renderToDepthTexture=false;

	mCapabilitySet.textureDot3=false;

	mCapabilitySet.textureNonPowerOf2Restricted=false;
	mCapabilitySet.renderToTextureNonPowerOf2Restricted=false;
	mCapabilitySet.textureNonPowerOf2=false;

	mCapabilitySet.hardwareVertexBuffers=false;
	mCapabilitySet.hardwareIndexBuffers=false;

	mCapabilitySet.vertexShaders=false;
	mCapabilitySet.maxVertexShaderLocalParameters=0;

	mCapabilitySet.fragmentShaders=false;
	mCapabilitySet.maxFragmentShaderLocalParameters=0;

	initState();

	TOADLET_LOG(TOADLET_PEEPER_CATEGORY,Logger::LEVEL_ALERT,
		"NDSRenderer: Startup finished");

	return true;
}

bool NDSRenderer::shutdown(){
	TOADLET_LOG(TOADLET_PEEPER_CATEGORY,Logger::LEVEL_ALERT,
		"NDSRenderer: Shutdown finished");

	mShutdown=true;
	return true;
}

void NDSRenderer::initState(){
	mAlphaTest=AT_GEQUAL;
	mAlphaCutoff=0;
	mFaceCulling=FC_NONE;
	mLighting=true;
	mBlend=Blend(Blend::COMBINATION_ALPHA);

	setDefaultStates();
}

Renderer::Status NDSRenderer::getStatus(){
	return STATUS_OK;
}

bool NDSRenderer::reset(){
	// Nothing necessary
	return true;
}

// Resource operations
TexturePeer *NDSRenderer::createTexturePeer(Texture *texture){
	if(texture==NULL){
		TOADLET_ERROR(TOADLET_EGG_CATEGORY,Error::TYPE_NULL_POINTER,
			"Texture is NULL");
		return NULL;
	}

	switch(texture->getTextureType()){
		case(Texture::TT_NORMAL):{
			return new NDSTexturePeer(this,texture);
		}
		case(Texture::TT_RENDER):{
			TOADLET_ERROR(TOADLET_EGG_CATEGORY,Error::TYPE_UNKNOWN,
				"TT_RENDER specified, but no render to texture support");
			return NULL;
		}
		case(Texture::TT_ANIMATED):{
			// Dont load animated textures, since they are just a collection of normal textures
			return NULL;
		}
		default:
			return NULL;
	}
}

BufferPeer *NDSRenderer::createBufferPeer(Buffer *buffer){
	return NULL;
}

ProgramPeer *NDSRenderer::createProgramPeer(Program *program){
	return NULL;
}

ShaderPeer *NDSRenderer::createShaderPeer(Shader *shader){
	return NULL;
}

// Matrix operations
void NDSRenderer::setModelMatrix(const Matrix4x4 &matrix){
	if(mMatrixMode!=GL_MODELVIEW){
		mMatrixMode=GL_MODELVIEW;
		glMatrixMode(mMatrixMode);
	}

	mModelMatrix=matrix;

	m4x4 dsmatrix;
	toNDSm4x4(mViewMatrix,dsmatrix,mRenderShiftBits);
	glLoadMatrix4x4(&dsmatrix);
	toNDSm4x4(mModelMatrix,dsmatrix,mRenderShiftBits);
	glMultMatrix4x4(&dsmatrix);

#	if 0
		// Used to grab scale from the model matrix to rescale normals
		Vector3 v0(mModelMatrix.at(0,0),mModelMatrix.at(1,0),mModelMatrix.at(2,0));
		Vector3 v1(mModelMatrix.at(0,1),mModelMatrix.at(1,1),mModelMatrix.at(2,1));
		Vector3 v2(mModelMatrix.at(0,2),mModelMatrix.at(1,2),mModelMatrix.at(2,2));
		Math::normalize(v0);
		Math::normalize(v1);
		Math::normalize(v2);
		Matrix3x3 matrix1(
			v0.x,v1.x,v2.x,
			v0.y,v1.y,v2.y,
			v0.z,v1.z,v2.z);
		Matrix3x3 matrix2;
		Math::setTranspose(matrix2,matrix1);
		matrix2.set(
			mModelMatrix.at(0,0),mModelMatrix.at(0,1),mModelMatrix.at(0,2),
			mModelMatrix.at(1,0),mModelMatrix.at(1,1),mModelMatrix.at(1,2),
			mModelMatrix.at(2,0),mModelMatrix.at(2,1),mModelMatrix.at(2,2));
		Matrix3x3 result;
		Math::mul(result,matrix1,matrix2);

		mModelScale.set(result.at(0,0),result.at(1,1),result.at(2,2));
#	endif
}

void NDSRenderer::setViewMatrix(const Matrix4x4 &matrix){
	if(mMatrixMode!=GL_MODELVIEW){
		mMatrixMode=GL_MODELVIEW;
		glMatrixMode(mMatrixMode);
	}

	mViewMatrix=matrix;

	m4x4 dsmatrix;
	toNDSm4x4(mViewMatrix,dsmatrix,mRenderShiftBits);
	glLoadMatrix4x4(&dsmatrix);
	toNDSm4x4(mModelMatrix,dsmatrix,mRenderShiftBits);
	glMultMatrix4x4(&dsmatrix);
}

void NDSRenderer::setProjectionMatrix(const Matrix4x4 &matrix){
	if(mMatrixMode!=GL_PROJECTION){
		mMatrixMode=GL_PROJECTION;
		glMatrixMode(mMatrixMode);
	}

	m4x4 dsmatrix;
	toProjectionNDSm4x4(matrix,dsmatrix,mRenderShiftBits);
	glLoadMatrix4x4(&dsmatrix);
}

bool NDSRenderer::setRenderTarget(RenderTarget *target){
	return true;
}

RenderTarget *NDSRenderer::getRenderTarget(){
	return NULL;
}

void NDSRenderer::setViewport(const Viewport &viewport){
	// The gl viewport command has its origin at the lower left, toadlet uses upper left
	glViewport(viewport.x,SCREEN_HEIGHT-(viewport.y+viewport.height),viewport.width,viewport.height);
}

static const fixed F31=Math::intToFixed(31);

void NDSRenderer::clearRenderTargetBuffers(int buffers,const Color &color){
	if(buffers & RTB_COLOR){
		glClearColor(
			Math::fixedToInt(Math::mul(color.r,F31)),
			Math::fixedToInt(Math::mul(color.g,F31)),
			Math::fixedToInt(Math::mul(color.b,F31)),
			Math::fixedToInt(Math::mul(color.a,F31))
		);
	}
	if(buffers & RTB_DEPTH){
		glClearDepth(GL_MAX_DEPTH);
	}
}

void NDSRenderer::beginScene(){
	mBeginEndCounter++;
	if(mBeginEndCounter!=1){
		mBeginEndCounter--;
		TOADLET_ERROR(TOADLET_EGG_CATEGORY,Error::TYPE_UNKNOWN,
			"NDSRenderer::beginScene: Unmached beginScene/endScene");
	}

	mStatisticsSet=StatisticsSet();
}

void NDSRenderer::endScene(){
	mBeginEndCounter--;
	if(mBeginEndCounter!=0){
		mBeginEndCounter++;
		TOADLET_ERROR(TOADLET_EGG_CATEGORY,Error::TYPE_UNKNOWN,
			"NDSRenderer::endScene: Unmached beginScene/endScene");
	}

	disableTextureStage(0);
}

void NDSRenderer::swapRenderTargetBuffers(){
	swiWaitForVBlank();

	glFlush(0);
}

void NDSRenderer::renderPrimitive(const VertexData *vertexData,const IndexData *indexData){
#	if defined(TOADLET_DEBUG)
		if(mBeginEndCounter!=1){
			TOADLET_ERROR(TOADLET_EGG_CATEGORY,Error::TYPE_UNKNOWN,
				"NDSRenderer::renderPrimitive called outside of begin/end scene");
		}
#	endif

	if(vertexData==NULL || indexData==NULL){
		TOADLET_ERROR(TOADLET_EGG_CATEGORY,Error::TYPE_NULL_POINTER,
			"VertexData or IndexData are NULL");
		return;
	}

	unsigned int count=indexData->getCount();

	IndexData::Primitive primitive=indexData->getPrimitive();

	GL_GLBEGIN_ENUM type=(GL_GLBEGIN_ENUM)0;
	switch(primitive){
		case(IndexData::PRIMITIVE_TRIS):
			type=GL_TRIANGLES;
			mStatisticsSet.triangleCount+=count/3;
		break;
		case(IndexData::PRIMITIVE_TRISTRIP):
			type=GL_TRIANGLE_STRIP;
			mStatisticsSet.triangleCount+=count-2;
		break;
		default:
			TOADLET_ERROR(TOADLET_EGG_CATEGORY,Error::TYPE_UNKNOWN,
				"NDSRenderer::renderPrimitive invalid primitive type");
			return;
		break;
	}

	VertexBuffer *vertexBuffer=vertexData->getVertexBuffer(0);
	VertexBufferAccessor vba(vertexBuffer,Buffer::LT_READ_ONLY);

	const VertexFormat &vertexFormat=vertexBuffer->getVertexFormat();
	bool hasColor=vertexFormat.hasVertexElementOfType(VertexElement::TYPE_COLOR);
	bool hasNormal=vertexFormat.hasVertexElementOfType(VertexElement::TYPE_NORMAL) && hasColor==false && mLighting==true && mLightsFmt>0;

	int renderShiftBits=mRenderShiftBits+4;

	::uint32 color=RGB15((Math::mul(mAmbientColor.r,mMaterialAmbientColor.r)*255)>>19,(Math::mul(mAmbientColor.g,mMaterialAmbientColor.g)*255)>>19,(Math::mul(mAmbientColor.b,mMaterialAmbientColor.b)*255)>>19);

	IndexBuffer *indexBuffer=indexData->getIndexBuffer();
	if(indexBuffer!=NULL){
		IndexBufferAccessor iba(indexBuffer,Buffer::LT_READ_ONLY);

		unsigned int i;
		glBegin(type);
			if(hasColor==false){
				glColor(color);
			}

			unsigned int start=indexData->getStart();
			unsigned int end=start+count;
			for(i=start;i<end;++i){
				int index=iba.index16(i);

				if(hasColor){
					ABGRByteColor color=vba.color4b(index);
					glColor3b((uint8)((color>>16)&0xFF),(uint8)((color>>8)&0xFF),(uint8)(color&0xFF));
				}

				if(hasNormal){
					const Vector3 &normal=vba.normal3x(index);
					// TODO: See if I can work out the toNDSv10 here, since its a if and could slow things down
					glNormal(NORMAL_PACK(toNDSv10(normal.x),toNDSv10(normal.y),toNDSv10(normal.z)));
				}

				const Vector3 &position=vba.position3x(index);
				glVertex3v16((position.x>>renderShiftBits),(position.y>>renderShiftBits),(position.z>>renderShiftBits)); // TODO: Scaling should be by 4, but DS doesnt have enough precision, so we are faking this
			}
		glEnd();
	}
	else{
		unsigned int index;
		glBegin(type);
			if(hasColor==false){
				glColor(color);
			}

			unsigned int start=indexData->getStart();
			unsigned int end=start+count;
			for(index=start;index<end;++index){
				if(hasColor){
					ABGRByteColor color=vba.color4b(index);
					glColor3b((uint8)((color>>16)&0xFF),(uint8)((color>>8)&0xFF),(uint8)(color&0xFF));
				}

				if(hasNormal){
					const Vector3 &normal=vba.normal3x(index);
					// TODO: See if I can work out the toNDSv10 here, since its a if and could slow things down
					glNormal(NORMAL_PACK(toNDSv10(normal.x),toNDSv10(normal.y),toNDSv10(normal.z)));
				}

				const Vector3 &position=vba.position3x(index);
				glVertex3v16(position.x>>renderShiftBits,position.y>>renderShiftBits,position.z>>renderShiftBits);
			}
		glEnd();
	}
}

void NDSRenderer::setDefaultStates(){
	setAlphaTest(AT_NONE,Math::HALF);
	setFaceCulling(FC_BACK);
	setFogParameters(FT_NONE,0,Math::ONE,Colors::COLOR_BLACK);
	setLighting(false);

	disableTextureStage(0);
}

void NDSRenderer::setAlphaTest(const AlphaTest &alphaTest,scalar cutoff){
	if(mAlphaTest==alphaTest && mAlphaCutoff==cutoff){
		return;
	}

	if(alphaTest==AT_NONE){
		glDisable(GL_ALPHA_TEST);
	}
	else{
		int alpha=cutoff<0xFFFF?cutoff:0xFFFF;
		glAlphaFunc(alpha);

		if(mAlphaTest==AT_NONE){
			glEnable(GL_ALPHA_TEST);
		}
	}

	mAlphaTest=alphaTest;
	mAlphaCutoff=cutoff;
}

void NDSRenderer::setBlend(const Blend &blend){
	if(mBlend==blend){
		return;
	}

	if(blend==DISABLED_BLEND){
		glDisable(GL_BLEND);
	}
	else if(mBlend==DISABLED_BLEND){
		glEnable(GL_BLEND);
	}

	mBlend=blend;
}

void NDSRenderer::setFaceCulling(const FaceCulling &faceCulling){
	if(mFaceCulling==faceCulling){
		return;
	}

	if(faceCulling==FC_NONE){
		mPolyFmt|=POLY_CULL_NONE;
	}
	else{
		switch(faceCulling){
			case(FC_FRONT):
				mPolyFmt&=~POLY_CULL_NONE;
				mPolyFmt|=POLY_CULL_FRONT;
				break;
			case(FC_BACK):
				mPolyFmt&=~POLY_CULL_NONE;
				mPolyFmt|=POLY_CULL_BACK;
				break;
			default:
				break;
		}
	}

	glPolyFmt(mPolyFmt);

	mFaceCulling=faceCulling;
}

void NDSRenderer::setDepthTest(const DepthTest &depthTest){
}

void NDSRenderer::setDepthWrite(bool depthWrite){
}

void NDSRenderer::setDithering(bool dithering){
}

void NDSRenderer::setFogParameters(const FogType &fogType,scalar nearDistance,scalar farDistance,const Color &color){
/*
	if(fogType==FT_NONE){
		glDisable(GL_FOG);
	}
	else{
		glEnable(GL_FOG);
#		if defined(TOADLET_PEEPER_FIXED_POINT)
			glFogx(GL_FOG_START,nearDistance);
			glFogx(GL_FOG_END,farDistance);
			glFogxv(GL_FOG_COLOR,color.getData());
#		else
			glFogf(GL_FOG_START,nearDistance);
			glFogf(GL_FOG_END,farDistance);
			glFogfv(GL_FOG_COLOR,color.getData());
#		endif
	}
*/
}

void NDSRenderer::setLightEffect(const LightEffect &lightEffect){
	glMaterialf(GL_AMBIENT,toNDSrgb(lightEffect.ambient));
	mMaterialAmbientColor=lightEffect.ambient;
	glMaterialf(GL_DIFFUSE,toNDSrgb(lightEffect.diffuse));
	glMaterialf(GL_SPECULAR,toNDSrgb(lightEffect.specular));
	glMaterialf(GL_EMISSION,toNDSrgb(lightEffect.emissive));
}

void NDSRenderer::setFillType(const FillType &fill){
}

void NDSRenderer::setLighting(bool lighting){
	if(mLighting==lighting){
		return;
	}

	if(lighting){
		mPolyFmt|=mLightsFmt;
	}
	else{
		mPolyFmt&=~(POLY_FORMAT_LIGHT0|POLY_FORMAT_LIGHT1|POLY_FORMAT_LIGHT2|POLY_FORMAT_LIGHT3);
	}

	glPolyFmt(mPolyFmt);

	mLighting=lighting;
}

void NDSRenderer::setNormalizeNormals(NormalizeType normalizeType){
}

void NDSRenderer::setTextureStage(int stage,const TextureStage &textureStage){
/*
	if(mCapabilitySet.maxTextureStages>1){
		glActiveTexture(GL_TEXTURE0+stage);
	}

	Texture *texture=textureStage.getTexture();
	GLuint textureDimension=0;
	if(texture!=NULL && texture->internal_getTexturePeer()!=NULL){
		GLTexturePeer *peer=(GLTexturePeer*)texture->internal_getTexturePeer();

		textureDimension=peer->textureDimension;

		glBindTexture(textureDimension,peer->textureHandle);
		if(textureStage.getTextureMatrixIdentity()==false
#			if defined(TOADLET_HAS_GLEW)
				|| textureDimension==GL_TEXTURE_RECTANGLE_ARB
#			endif
		){
			if(mMatrixMode!=GL_TEXTURE){
				mMatrixMode=GL_TEXTURE;
				glMatrixMode(mMatrixMode);
			}
#			if defined(TOADLET_PEEPER_FIXED_POINT)
				glLoadMatrixx(textureStage.getTextureMatrix().getData());
#			else
				glLoadMatrixf(textureStage.getTextureMatrix().getData());
#			endif
#			if defined(TOADLET_HAS_GLEW)
				if(textureDimension==GL_TEXTURE_RECTANGLE_ARB){
					glMultMatrixf(peer->textureRectangleMatrix.getData());
				}
#			endif
		}
		else{
			if(mMatrixMode!=GL_TEXTURE){
				mMatrixMode=GL_TEXTURE;
				glMatrixMode(mMatrixMode);
			}
			glLoadIdentity();
		}
	}
	else{
		textureDimension=GL_TEXTURE_2D;
		glBindTexture(textureDimension,0);
	}

	if(mLastTexDimensions[stage]!=textureDimension){
		if(mLastTexDimensions[stage]!=0){
			glDisable(mLastTexDimensions[stage]);
		}
		if(textureDimension!=0){
			glEnable(textureDimension);
		}
		mLastTexDimensions[stage]=textureDimension;
	}

	const TextureBlend &blend=textureStage.getBlend();
	if(blend.operation!=TextureBlend::OPERATION_UNSPECIFIED){
		int mode=0;
		switch(blend.operation){
			case(TextureBlend::OPERATION_REPLACE):
				mode=GL_REPLACE;
			break;
			case(TextureBlend::OPERATION_MODULATE):
				mode=GL_MODULATE;
			break;
			case(TextureBlend::OPERATION_ADD):
				mode=GL_ADD;
			break;
			case(TextureBlend::OPERATION_DOTPRODUCT):
				mode=GL_DOT3_RGB;
			break;
		}
		
		if(blend.source1==TextureBlend::SOURCE_UNSPECIFIED || blend.source2==TextureBlend::SOURCE_UNSPECIFIED){
			glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,mode);
		}
		else{
			glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_COMBINE);
			glTexEnvi(GL_TEXTURE_ENV,GL_COMBINE_RGB,mode);
			glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE0_RGB,getGLTextureBlendSource(blend.source1));
			glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE1_RGB,getGLTextureBlendSource(blend.source2));
			glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE2_RGB,GL_CONSTANT);
		}
	}
	else{
		glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
	}

	if(textureStage.getAddressModeSpecified()){
		glTexParameteri(textureDimension,GL_TEXTURE_WRAP_S,GLTexturePeer::getWrap(textureStage.getSAddressMode()));
		glTexParameteri(textureDimension,GL_TEXTURE_WRAP_T,GLTexturePeer::getWrap(textureStage.getTAddressMode()));
#		if defined(GL_VERSION_1_1)
			glTexParameteri(textureDimension,GL_TEXTURE_WRAP_R,GLTexturePeer::getWrap(textureStage.getRAddressMode()));
#		endif
	}

	if(textureStage.getFilterSpecified()){
		glTexParameteri(textureDimension,GL_TEXTURE_MIN_FILTER,GLTexturePeer::getMinFilter(textureStage.getMinFilter(),textureStage.getMipFilter()));
		glTexParameteri(textureDimension,GL_TEXTURE_MAG_FILTER,GLTexturePeer::getMagFilter(textureStage.getMagFilter()));
	}

#	if defined(GL_VERSION_1_1)
		if(texture!=NULL && (texture->getFormat()&Image::FORMAT_BIT_DEPTH)>0){
			// Enable shadow comparison
			glTexParameteri(textureDimension,GL_TEXTURE_COMPARE_MODE_ARB,GL_COMPARE_R_TO_TEXTURE);

			// Shadow comparison should be true (ie not in shadow) if r<=texture
			glTexParameteri(textureDimension,GL_TEXTURE_COMPARE_FUNC_ARB,GL_LEQUAL);

			// Shadow comparison should generate an INTENSITY result
			glTexParameteri(textureDimension,GL_DEPTH_TEXTURE_MODE_ARB,GL_INTENSITY);
		}
		else{
			// Only disable this TexParameter, the others are just parameters to this one
			glTexParameteri(textureDimension,GL_TEXTURE_COMPARE_MODE_ARB,GL_NONE);
		}
#	endif

	int texCoordIndex=textureStage.getTexCoordIndex();
	if(mTexCoordIndexes[stage]!=texCoordIndex){
		mTexCoordIndexes[stage]=texCoordIndex;
	}

	if(stage>=mMaxTexCoordIndex){
		mMaxTexCoordIndex=stage+1;
	}

	if(mCapabilitySet.maxTextureStages>1){
		glActiveTexture(GL_TEXTURE0);
	}
*/
}

void NDSRenderer::disableTextureStage(int stage){
/*
	if(mCapabilitySet.maxTextureStages>1){
		glActiveTexture(GL_TEXTURE0+stage);
		glClientActiveTexture(GL_TEXTURE0+stage);
	}

	if(mLastTexDimensions[stage]!=0){
		glDisable(mLastTexDimensions[stage]);
		mLastTexDimensions[stage]=0;
	}

	while(mMaxTexCoordIndex>0 && mLastTexDimensions[mMaxTexCoordIndex-1]==0){
		mMaxTexCoordIndex--;
	}

	if(mCapabilitySet.maxTextureStages>1){
		glActiveTexture(GL_TEXTURE0);
	}
*/
}

void NDSRenderer::setProgram(const Program *program){
}

void NDSRenderer::setShading(const Shading &shading){
}

void NDSRenderer::setColorWrite(bool color){
}

void NDSRenderer::setDepthBias(scalar constant,scalar slope){
}

void NDSRenderer::setTextureCoordinateGeneration(int stage,const TextureCoordinateGeneration &tcg,const Matrix4x4 &matrix){
}

void NDSRenderer::getShadowBiasMatrix(const Texture *shadowTexture,Matrix4x4 &result){
}

void NDSRenderer::setShadowComparisonMethod(bool enabled){
}

// Lighting is a little different than in regular OpenGL.  A glLight has the same ambient as diffuse color,
//  so it then falls on the Material to light things correctly
void NDSRenderer::setLight(int i,const Light &light){
	if(light.getType()!=Light::LT_DIRECTION){
		TOADLET_ERROR(TOADLET_PEEPER_CATEGORY,Error::TYPE_UNKNOWN,
			"Only LT_DIRECTION lights are supported");
		return;
	}

	if(i>=0 && i<4){
		glLight(i,toNDSrgb(light.getDiffuseColor()),toNDSv10(light.getDirection().x),toNDSv10(light.getDirection().y),toNDSv10(light.getDirection().z));
	}
}

void NDSRenderer::setLightEnabled(int i,bool enable){
	if(enable){
		switch(i){
			case(0):
				mLightsFmt|=POLY_FORMAT_LIGHT0;
			break;
			case(1):
				mLightsFmt|=POLY_FORMAT_LIGHT1;
			break;
			case(2):
				mLightsFmt|=POLY_FORMAT_LIGHT2;
			break;
			case(3):
				mLightsFmt|=POLY_FORMAT_LIGHT3;
			break;
		}
	}
	else{
		switch(i){
			case(0):
				mLightsFmt&=~POLY_FORMAT_LIGHT0;
			break;
			case(1):
				mLightsFmt&=~POLY_FORMAT_LIGHT1;
			break;
			case(2):
				mLightsFmt&=~POLY_FORMAT_LIGHT2;
			break;
			case(3):
				mLightsFmt&=~POLY_FORMAT_LIGHT3;
			break;
		}
	}

	if(mLighting){
		mPolyFmt&=~(POLY_FORMAT_LIGHT0|POLY_FORMAT_LIGHT1|POLY_FORMAT_LIGHT2|POLY_FORMAT_LIGHT3);
		mPolyFmt|=mLightsFmt;

		glPolyFmt(mPolyFmt);
	}
}

void NDSRenderer::setAmbientColor(const Color &ambient){
	mAmbientColor=ambient;
}

// Misc operations
void NDSRenderer::copyFrameBufferToTexture(Texture *texture){
}

const StatisticsSet &NDSRenderer::getStatisticsSet(){
	return mStatisticsSet;
}

const CapabilitySet &NDSRenderer::getCapabilitySet(){
	return mCapabilitySet;
}

}
}
