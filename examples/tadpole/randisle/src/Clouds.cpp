#include "Clouds.h"
#include <toadlet/tadpole/plugins/SkyDomeMeshCreator.h>

Clouds::Clouds(Scene *scene,int cloudSize,const Vector4 &skyColor,const Vector4 &fadeColor):BaseComponent(),
	mCloudData(NULL),
	mBumpData(NULL),
	mFadeData(NULL),
	mCompositeData(NULL)
{
	mEngine=scene->getEngine();
	mScene=scene;
	mSkyColor=skyColor;

	Sphere sphere(Vector3(0,0,0),512);
	bool advanced=true; // Use realtime bumpmapping, or precalculated

	SkyDomeMeshCreator::ptr skyDomeCreator=new SkyDomeMeshCreator(mEngine);
 	int numSegments=16,numRings=16;
	VertexBuffer::ptr vertexBuffer=mEngine->getBufferManager()->createVertexBuffer(Buffer::Usage_BIT_STREAM,Buffer::Access_READ_WRITE,mEngine->getVertexFormats().POSITION_COLOR_TEX_COORD,skyDomeCreator->getSkyDomeVertexCount(numSegments,numRings));
	IndexBuffer::ptr indexBuffer=mEngine->getBufferManager()->createIndexBuffer(Buffer::Usage_BIT_STATIC,Buffer::Access_BIT_WRITE,IndexBuffer::IndexFormat_UINT16,skyDomeCreator->getSkyDomeIndexCount(numSegments,numRings));

	mCloudFormat=new TextureFormat(TextureFormat::Dimension_D2,TextureFormat::Format_RGBA_8,cloudSize,cloudSize,1,0);
	mCloudData=new tbyte[mCloudFormat->getDataSize()];
	createCloud(mCloudFormat,mCloudData,16,9,0.45,0.000025,0.75);
	mBumpFormat=new TextureFormat(TextureFormat::Dimension_D2,TextureFormat::Format_RGB_8,cloudSize,cloudSize,1,0);
	mBumpData=new tbyte[mBumpFormat->getDataSize()];
	createBump(mBumpFormat,mBumpData,mCloudData,-1,-1,-32,4); // To debug any bump issues, try disabling fadeStage, make bump/cloud stage modulate, add a rotating sun, and use a zscale of 1

	Material::ptr material;
	if(advanced){
		mCloudTexture=mEngine->getTextureManager()->createTexture(mCloudFormat,mCloudData);
		mBumpTexture=mEngine->getTextureManager()->createTexture(mBumpFormat,mBumpData);

		mFadeFormat=new TextureFormat(TextureFormat::Dimension_D2,TextureFormat::Format_RGBA_8,cloudSize/2,cloudSize/2,1,0);
		mFadeData=new tbyte[mFadeFormat->getDataSize()];
		createFade(mFadeFormat,mFadeData,Vector4(fadeColor.x,fadeColor.y,fadeColor.z,0),fadeColor,1.1,0.01);
		mFadeTexture=mEngine->getTextureManager()->createTexture(mFadeFormat,mFadeData);

		material=mEngine->getMaterialManager()->createMaterial();
		if(mEngine->hasShader(Shader::ShaderType_VERTEX) && mEngine->hasShader(Shader::ShaderType_FRAGMENT)){
			RenderPath::ptr shaderPath=material->addPath();
			RenderPass::ptr pass=shaderPath->addPass();

			pass->setBlendState(BlendState());
			pass->setDepthState(DepthState(DepthState::DepthTest_ALWAYS,false));
			pass->setRasterizerState(RasterizerState());

			String profiles[]={
				"glsl",
				"hlsl"
			};

			String vertexCodes[]={
				"attribute vec4 POSITION;\n"
				"attribute vec4 COLOR;\n"
				"attribute vec2 TEXCOORD0;\n"
				"varying vec4 color;\n"
				"varying vec2 texCoord0;\n"
				"varying vec2 texCoord1;\n"

				"uniform mat4 modelViewProjectionMatrix;\n"
				"uniform mat4 textureMatrix;\n"

				"void main(){\n"
					"gl_Position=modelViewProjectionMatrix * POSITION;\n"
					"color=COLOR;\n"
					"texCoord0=(textureMatrix * vec4(TEXCOORD0,0.0,1.0)).xy;\n "
					"texCoord1=TEXCOORD0;\n"
				"}",



				"struct VIN{\n"
					"float4 position : POSITION;\n"
					"float4 color : COLOR;\n"
					"float2 texCoord: TEXCOORD0;\n"
				"};\n"
				"struct VOUT{\n"
					"float4 position : SV_POSITION;\n"
					"float4 color : COLOR;\n"
					"float2 texCoord0: TEXCOORD0;\n"
					"float2 texCoord1: TEXCOORD1;\n"
				"};\n"

				"float4x4 modelViewProjectionMatrix;\n"
				"float4x4 textureMatrix;\n"

				"VOUT main(VIN vin){\n"
					"VOUT vout;\n"
					"vout.position=mul(modelViewProjectionMatrix,vin.position);\n"
					"vout.color=vin.color;\n"
					"vout.texCoord0=mul(textureMatrix,float4(vin.texCoord,0.0,1.0));\n "
					"vout.texCoord1=vin.texCoord;\n "
					"return vout;\n"
				"}"
			};

			String fragmentCodes[]={\
				"#if defined(GL_ES)\n"
					"precision mediump float;\n"
				"#endif\n"
				
				"varying vec4 color;\n"
				"varying vec2 texCoord0;\n"
				"varying vec2 texCoord1;\n"

				"uniform vec4 skyColor;\n"
				"uniform sampler2D bumpTexture,cloudTexture,fadeTexture;\n"

				"void main(){\n"
					"vec4 bump=texture2D(bumpTexture,texCoord0);\n"
					"vec4 cloud=texture2D(cloudTexture,texCoord0);\n"
					"vec4 fade=texture2D(fadeTexture,texCoord1);\n"
					"vec4 color=vec4(dot((color.xyz-0.5)*2.0,(bump.xyz-0.5)*2.0));\n"
					"color=vec4(color.xyz+cloud.xyz,cloud.a);\n"
					"color=mix(skyColor,color,cloud.w);\n"
					"color=mix(fade,color,fade.w);\n"
					"gl_FragColor = color;\n"
				"}",



				"struct PIN{\n"
					"float4 position: SV_POSITION;\n"
					"float4 color: COLOR;\n"
					"float2 texCoord0: TEXCOORD0;\n"
					"float2 texCoord1: TEXCOORD1;\n"
				"};\n"

				"float4 skyColor;\n"
				"Texture2D bumpTexture,cloudTexture,fadeTexture;\n"
				"SamplerState bumpSamp,cloudSamp,fadeSamp;\n"

				"float4 main(PIN pin): SV_TARGET{\n"
					"float4 bump=bumpTexture.Sample(bumpSamp,pin.texCoord0);\n"
					"float4 cloud=cloudTexture.Sample(cloudSamp,pin.texCoord0);\n"
					"float4 fade=fadeTexture.Sample(fadeSamp,pin.texCoord1);\n"
					"float4 color=dot((pin.color.xyz-0.5)*2,(bump.xyz-0.5)*2);\n"
					"color=float4(color.xyz+cloud.xyz,cloud.a);\n"
					"color=lerp(skyColor,color,cloud.w);\n"
					"color=lerp(fade,color,fade.w);\n"
					"return color;\n"
				"}"
			};

			Shader::ptr vertexShader,fragmentShader;
			TOADLET_TRY
				vertexShader=mEngine->getShaderManager()->createShader(Shader::ShaderType_VERTEX,profiles,vertexCodes,2);
			TOADLET_CATCH_ANONYMOUS(){}
			TOADLET_TRY
				fragmentShader=mEngine->getShaderManager()->createShader(Shader::ShaderType_FRAGMENT,profiles,fragmentCodes,2);
			TOADLET_CATCH_ANONYMOUS(){}

			if(vertexShader!=NULL){
				pass->setShader(Shader::ShaderType_VERTEX,vertexShader);
			}
			if(fragmentShader!=NULL){
				pass->setShader(Shader::ShaderType_FRAGMENT,fragmentShader);
			}

			RenderVariableSet::ptr variables=pass->makeVariables();
			variables->addVariable("modelViewProjectionMatrix",RenderVariable::ptr(new MVPMatrixVariable()),Material::Scope_RENDERABLE);
			variables->addVariable("textureMatrix",RenderVariable::ptr(new TextureMatrixVariable(Shader::ShaderType_VERTEX,0)),Material::Scope_MATERIAL);
			variables->addVariable("skyColor",RenderVariable::ptr(new Vector4Variable(mSkyColor)),Material::Scope_MATERIAL);

			variables->addTexture("bumpTexture",mBumpTexture,"bumpSamp",SamplerState(),TextureState());
			variables->addTexture("cloudTexture",mCloudTexture,"cloudSamp",SamplerState(),TextureState());
			variables->addTexture("fadeTexture",mFadeTexture,"fadeSamp",SamplerState(),TextureState());
		}

		if(mEngine->hasFixed(Shader::ShaderType_VERTEX) && mEngine->hasFixed(Shader::ShaderType_FRAGMENT)){
			RenderPath::ptr fixedPath=material->addPath();
			RenderPass::ptr pass=fixedPath->addPass();

			pass->setBlendState(BlendState());
			pass->setDepthState(DepthState(DepthState::DepthTest_ALWAYS,false));
			pass->setRasterizerState(RasterizerState());
			pass->setMaterialState(MaterialState(false,true));

			int state=0;
			TextureState bumpState;
			bumpState.colorOperation=TextureState::Operation_DOTPRODUCT;
			bumpState.colorSource1=TextureState::Source_PREVIOUS;
			bumpState.colorSource2=TextureState::Source_TEXTURE;
			pass->setTexture(Shader::ShaderType_FRAGMENT,state,mBumpTexture,SamplerState(),bumpState);
			pass->setTextureLocationName(Shader::ShaderType_FRAGMENT,state,"bumpTexture");
			state++;

			TextureState cloudState;
			cloudState.colorOperation=TextureState::Operation_ADD;
			cloudState.colorSource1=TextureState::Source_PREVIOUS;
			cloudState.colorSource2=TextureState::Source_TEXTURE;
			cloudState.alphaOperation=TextureState::Operation_REPLACE;
			cloudState.alphaSource1=TextureState::Source_TEXTURE;
			pass->setTexture(Shader::ShaderType_FRAGMENT,state,mCloudTexture,SamplerState(),cloudState);
			pass->setTextureLocationName(Shader::ShaderType_FRAGMENT,state,"cloudTexture");
			state++;

			TextureState colorState;
			colorState.constantColor.set(mSkyColor);
			colorState.colorOperation=TextureState::Operation_ALPHABLEND;
			colorState.colorSource1=TextureState::Source_PREVIOUS;
			colorState.colorSource2=TextureState::Source_CONSTANT_COLOR;
			colorState.colorSource3=TextureState::Source_PREVIOUS;
			colorState.alphaOperation=TextureState::Operation_REPLACE;
			colorState.alphaSource1=TextureState::Source_PREVIOUS;
			pass->setTexture(Shader::ShaderType_FRAGMENT,state,mCloudTexture,SamplerState(),colorState); // Need a texture for this state to function on OpenGL currently
			pass->setTextureLocationName(Shader::ShaderType_FRAGMENT,state,"colorTexture");
			state++;

			TextureState fadeState;
			fadeState.colorOperation=TextureState::Operation_ALPHABLEND;
			fadeState.colorSource1=TextureState::Source_PREVIOUS;
			fadeState.colorSource2=TextureState::Source_TEXTURE;
			fadeState.colorSource3=TextureState::Source_TEXTURE;
			fadeState.alphaOperation=TextureState::Operation_REPLACE;
			fadeState.alphaSource1=TextureState::Source_TEXTURE;
			pass->setTexture(Shader::ShaderType_FRAGMENT,state,mFadeTexture,SamplerState(),fadeState);
			pass->setTextureLocationName(Shader::ShaderType_FRAGMENT,state,"fadeTexture");
			state++;
		}

		material->compile();
	}
	else{
		mCompositeFormat=new TextureFormat(TextureFormat::Dimension_D2,TextureFormat::Format_RGB_8,cloudSize,cloudSize,1,0);
		mCompositeData=new tbyte[mCompositeFormat->getDataSize()];
		createComposite(mCompositeFormat,mCompositeData,mCloudData,mBumpData,Math::Z_UNIT_VECTOR3,mSkyColor);
		mCompositeTexture=mEngine->getTextureManager()->createTexture(Texture::Usage_BIT_AUTOGEN_MIPMAPS|Texture::Usage_BIT_DYNAMIC,mCompositeFormat,mCompositeData);

		material=mEngine->createSkyBoxMaterial(mCompositeTexture,false);
		material->getPass()->setTextureLocationName(Shader::ShaderType_FRAGMENT,0,"compositeTexture");
		material->getPass()->setBlendState(BlendState::Combination_ALPHA);
		material->getPass()->setMaterialState(MaterialState(false,true));
	}
	mMaterial=material;
	mMaterial->setLayer(-2);

	mMesh=skyDomeCreator->createSkyDomeMesh(vertexBuffer,indexBuffer,sphere,numSegments,numRings,0.35);
	Transform transform;
	transform.setScale(1,1,0.5f);
	mMesh->setTransform(transform);
	mMesh->getSubMesh(0)->material=mMaterial;
}

void Clouds::destroy(){
	if(mCloudData!=NULL){
		delete[] mCloudData;
		mCloudData=NULL;
	}

	if(mBumpData!=NULL){
		delete[] mBumpData;
		mBumpData=NULL;
	}

	if(mFadeData!=NULL){
		delete[] mFadeData;
		mFadeData=NULL;
	}

	if(mCompositeData!=NULL){
		delete[] mCompositeData;
		mCompositeData=NULL;
	}

	if(mMesh!=NULL){
		mMesh->destroy();
		mMesh=NULL;
	}

	if(mMaterial!=NULL){
		mMaterial->destroy();
		mMaterial=NULL;
	}

	BaseComponent::destroy();
}

void Clouds::setLightDirection(const Vector3 &lightDir){
	VertexBuffer *buffer=mMesh->getStaticVertexData()->getVertexBuffer(0);
	VertexBufferAccessor vba(buffer,Buffer::Access_READ_WRITE);
	int ip=buffer->getVertexFormat()->findElement(VertexFormat::Semantic_POSITION);
	int ic=buffer->getVertexFormat()->findElement(VertexFormat::Semantic_COLOR);

	Shader::ShaderType type;
	int index;
	bool hasBump=mMaterial->getPass()->findTexture("bumpTexture",type,index);

	Vector3 pos,dir;
	int i;
	for(i=0;i<vba.getSize();++i){
		vba.get3(i,ip,pos);
		Math::normalize(pos);
		if(hasBump){
			pos.z=0;
			Math::sub(dir,pos,lightDir);
			Math::normalize(dir);
			vba.setRGBA(i,ic,Vector4(dir.x*0.5+Math::HALF,dir.y*0.5+Math::HALF,dir.z*0.5+Math::HALF,Math::ONE).getRGBA());
		}
		else{
			scalar alpha=Math::clamp(0,Math::ONE,(pos.z-0)/0.75);
			vba.setRGBA(i,ic,Vector4(Math::ONE,Math::ONE,Math::ONE,alpha).getRGBA());
		}
	}
	vba.unlock();

	if(mCompositeTexture!=NULL){
		createComposite(mCompositeFormat,mCompositeData,mCloudData,mBumpData,lightDir,mSkyColor);
		mEngine->getTextureManager()->textureLoad(mCompositeTexture,mCompositeFormat,mCompositeData);
	}
}

void Clouds::frameUpdate(int dt,int scope){
	BaseComponent::frameUpdate(dt,scope);

	Vector3 offset;
	Math::mul(offset,mParent->getWorldTranslate(),0.0001); // Since the sky is in the background, it's world translate will be the viewer's world translate
	offset.x+=Math::fromMilli(mScene->getTime())*0.01;
	Matrix4x4 matrix;
	Math::setMatrix4x4FromTranslate(matrix,offset);

	setTextureMatrix("bumpTexture",mMaterial,matrix);
	setTextureMatrix("cloudTexture",mMaterial,matrix);
	setTextureMatrix("compositeTexture",mMaterial,matrix);
}

void Clouds::setTextureMatrix(const String &name,Material *material,const Matrix4x4 &matrix){
	Shader::ShaderType type;
	int index;
	RenderPass *pass=material->getPath()->findTexture(name,type,index);
	if(pass!=NULL){
		TextureState state;
		pass->getTextureState(type,index,state);
		state.matrix.set(matrix);
		pass->setTextureState(type,index,state);
	}
}

void Clouds::createCloud(TextureFormat *format,tbyte *data,int scale,int seed,float cover,float sharpness,float brightness){
	int width=format->getWidth(),height=format->getHeight();

	Noise noise(4,scale,1,seed,256);

	int x=0,y=0;
	for(y=0;y<height;y++){
		for(x=0;x<width;x++){
			float n=(noise.tileablePerlin2((float)x/(float)width,(float)y/(float)height,1,1)*0.5 + 0.5);
			n-=cover;
			if(n<0){n=0;}
			float density=1.0-pow(sharpness,n);

			data[(y*width+x)*4+0]=brightness*255;
			data[(y*width+x)*4+1]=brightness*255;
			data[(y*width+x)*4+2]=brightness*255;
			data[(y*width+x)*4+3]=density*255;
		}
	}
}

void Clouds::createBump(TextureFormat *format,tbyte *data,tbyte *cloudSrc,float xscale,float yscale,float zscale,int spread){
	int width=format->getWidth(),height=format->getHeight();
	uint8 *inData=cloudSrc;

	uint8 *rawData=new uint8[format->getDataSize()];

	uint8 ip,xp,yp;
	int x=0,y=0;
	for(y=0;y<height;y++){
		for(x=0;x<width;x++){
			int sx=(x+1)%width;
			int sy=(y+1)%height;

			ip=inData[(y*width+x)*4+3];
			xp=inData[(y*width+sx)*4+3];
			yp=inData[(sy*width+x)*4+3];

			Vector3 xv(xscale,0,xp-ip);Math::normalize(xv);
			Vector3 yv(0,yscale,yp-ip);Math::normalize(yv);
			Vector3 zv;Math::cross(zv,xv,yv);
			zv.z*=zscale;Math::normalize(zv);

			rawData[(y*width+x)*3+0]=(zv.x/2+0.5)*255;
			rawData[(y*width+x)*3+1]=(zv.y/2+0.5)*255;
			rawData[(y*width+x)*3+2]=(zv.z/2+0.5)*255;
		}
	}

	int i=0,j=0;
	for(y=0;y<height;y++){
		for(x=0;x<width;x++){
			int tx=0,ty=0,tz=0;
			for(i=-spread;i<=spread;++i){
				for(j=-spread;j<=spread;++j){
					int sx=(x+i+width)%width;
					int sy=(y+j+height)%height;

					tx+=rawData[(sy*width+sx)*3+0];
					ty+=rawData[(sy*width+sx)*3+1];
					tz+=rawData[(sy*width+sx)*3+2];
				}
			}
			tx/=((spread*2+1)*(spread*2+1));
			ty/=((spread*2+1)*(spread*2+1));
			tz/=((spread*2+1)*(spread*2+1));
			
			data[(y*width+x)*3+0]=tx;
			data[(y*width+x)*3+1]=ty;
			data[(y*width+x)*3+2]=tz;
		}
	}

	delete[] rawData;
}

void Clouds::createFade(TextureFormat *format,tbyte *data,const Vector4 &start,const Vector4 &end,float falloff,float sharpness){
	int width=format->getWidth(),height=format->getHeight();

	int x=0,y=0;
	for(y=0;y<height;y++){
		for(x=0;x<width;x++){
			float v=(1.0-Math::length(Vector2(x-width/2,y-height/2))*falloff/(width/2));
			v=1.0-pow(sharpness,v);
			if(v<0) v=0;
			if(v>1) v=1;

			uint32 color=Math::lerpColor(start.getRGBA(),end.getRGBA(),v);

			data[(y*width+x)*4+0]=(color>>24)&0xFF;
			data[(y*width+x)*4+1]=(color>>16)&0xFF;
			data[(y*width+x)*4+2]=(color>>8)&0xFF;
			data[(y*width+x)*4+3]=(color>>0)&0xFF;
		}
	}
}

void Clouds::createComposite(TextureFormat *format,tbyte *data,tbyte *cloudData,tbyte *bumpData,const Vector3 &lightDir,const Vector4 &skyColor){
	int width=format->getWidth(),height=format->getHeight();

	Vector3 dir;
	Math::neg(dir,lightDir);

	Vector3 n;
	Vector4 c,b,f;
	int x=0,y=0;
	for(y=0;y<height;y++){
		for(x=0;x<width;x++){
			tbyte br=bumpData[(y*width+x)*3+0];
			tbyte bg=bumpData[(y*width+x)*3+1];
			tbyte bb=bumpData[(y*width+x)*3+2];
			n.set(br/127.5-1.0, bg/127.5-1.0, bb/127.5-1.0);
			Math::mul(n,Math::HALF);

			tbyte cr=cloudData[(y*width+x)*4+0];
			tbyte cg=cloudData[(y*width+x)*4+1];
			tbyte cb=cloudData[(y*width+x)*4+2];
			tbyte ca=cloudData[(y*width+x)*4+3];
			c.set(cr/255.0,cg/255.0,cb/255.0,ca/255.0);

			scalar brightness=Math::clamp(0,Math::ONE,Math::dot(n,dir));
			b.set(brightness,brightness,brightness,Math::ONE);

			Math::add(f,c,b);
			if(f.x>Math::ONE) f.x=Math::ONE;
			if(f.y>Math::ONE) f.y=Math::ONE;
			if(f.z>Math::ONE) f.z=Math::ONE;
			if(f.w>Math::ONE) f.w=Math::ONE;
			Math::lerp(f,skyColor,f,c.w);

			data[(y*width+x)*3+0]=Math::clamp(0,Math::ONE,f.x)*255.0;
			data[(y*width+x)*3+1]=Math::clamp(0,Math::ONE,f.y)*255.0;
			data[(y*width+x)*3+2]=Math::clamp(0,Math::ONE,f.z)*255.0;
		}
	}
}

