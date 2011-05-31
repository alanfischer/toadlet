#include "Sky.h"

TOADLET_NODE_IMPLEMENT(Sky,"Sky");

Node *Sky::create(Scene *scene,const Vector4 &skyColor,const Vector4 &fadeColor){
	super::create(scene);

	int cloudSize=256;
	Sphere sphere(Vector3(0,0,0),512);
	Vector3 lightDir(1,1,0.5);
	bool advanced=true; // Use realtime bumpmapping, or precalculated

	VertexFormat::ptr vertexFormat=mEngine->getBufferManager()->createVertexFormat();
	vertexFormat->addElement(VertexFormat::Semantic_POSITION,0,VertexFormat::Format_BIT_FLOAT_32|VertexFormat::Format_BIT_COUNT_3);
	vertexFormat->addElement(VertexFormat::Semantic_COLOR,0,VertexFormat::Format_COLOR_RGBA);
	vertexFormat->addElement(VertexFormat::Semantic_TEX_COORD,0,VertexFormat::Format_BIT_FLOAT_32|VertexFormat::Format_BIT_COUNT_2);

 	int numSegments=16,numRings=16;
	VertexBuffer::ptr vertexBuffer=mEngine->getBufferManager()->createVertexBuffer(Buffer::Usage_BIT_STREAM,Buffer::Access_READ_WRITE,vertexFormat,mEngine->getMeshManager()->getSkyDomeVertexCount(numSegments,numRings));
	IndexBuffer::ptr indexBuffer=mEngine->getBufferManager()->createIndexBuffer(Buffer::Usage_BIT_STATIC,Buffer::Access_BIT_WRITE,IndexBuffer::IndexFormat_UINT16,mEngine->getMeshManager()->getSkyDomeIndexCount(numSegments,numRings));

	Image::ptr cloud=createCloud(cloudSize,cloudSize,16,9,0.45,0.000025,0.75);
	Image::ptr bump=createBump(cloud,-1,-1,-32,4); // To debug any bump issues, try disabling fadeStage, make bump/cloud stage modulate, add a rotating sun, and use a zscale of 1

	mSkyMaterial=mEngine->getMaterialManager()->createMaterial();
	mSkyMaterial->setDepthState(DepthState(DepthState::DepthTest_NEVER,false));
	mSkyMaterial->setLayer(-1);
	mSkyMaterial->setMaterialState(MaterialState(false,true));
	mSkyMaterial->setFogState(FogState(FogState::FogType_NONE,1,0,0,Colors::BLACK));
	int state=0;
	if(advanced){
		Image::ptr fade=createFade(cloudSize/2,cloudSize/2,Vector4(fadeColor.x,fadeColor.y,fadeColor.z,0),fadeColor,1.1,0.01);

		Texture::ptr bumpTexture=mEngine->getTextureManager()->createTexture(bump);
		Texture::ptr cloudTexture=mEngine->getTextureManager()->createTexture(cloud);
		Texture::ptr fadeTexture=mEngine->getTextureManager()->createTexture(fade);

		TextureState bumpState;
		bumpState.colorOperation=TextureState::Operation_DOTPRODUCT;
		bumpState.colorSource1=TextureState::Source_PREVIOUS;
		bumpState.colorSource2=TextureState::Source_TEXTURE;
		mSkyMaterial->setTexture(state,bumpTexture);
		mSkyMaterial->setTextureState(state,bumpState);
		mBumpAccessor=Matrix4x4Accessor::ptr(new TextureStateMatrix4x4Accessor(mSkyMaterial,state++));

		TextureState cloudState;
		cloudState.colorOperation=TextureState::Operation_ADD;
		cloudState.colorSource1=TextureState::Source_PREVIOUS;
		cloudState.colorSource2=TextureState::Source_TEXTURE;
		cloudState.alphaOperation=TextureState::Operation_REPLACE;
		cloudState.alphaSource1=TextureState::Source_TEXTURE;
		mSkyMaterial->setTexture(state,cloudTexture);
		mSkyMaterial->setTextureState(state,cloudState);
		mCloudAccessor=Matrix4x4Accessor::ptr(new TextureStateMatrix4x4Accessor(mSkyMaterial,state++));

		TextureState colorState;
		colorState.constantColor.set(skyColor);
		colorState.colorOperation=TextureState::Operation_ALPHABLEND;
		colorState.colorSource1=TextureState::Source_PREVIOUS;
		colorState.colorSource2=TextureState::Source_CONSTANT_COLOR;
		colorState.colorSource3=TextureState::Source_PREVIOUS;
		colorState.alphaOperation=TextureState::Operation_REPLACE;
		colorState.alphaSource1=TextureState::Source_PREVIOUS;
		mSkyMaterial->setTexture(state,cloudTexture); // Need a texture for this state to function on OpenGL currently
		mSkyMaterial->setTextureState(state,colorState);
		mColorAccessor=Matrix4x4Accessor::ptr(new TextureStateMatrix4x4Accessor(mSkyMaterial,state++));

		TextureState fadeState;
		fadeState.colorOperation=TextureState::Operation_ALPHABLEND;
		fadeState.colorSource1=TextureState::Source_PREVIOUS;
		fadeState.colorSource2=TextureState::Source_TEXTURE;
		fadeState.colorSource3=TextureState::Source_TEXTURE;
		fadeState.alphaOperation=TextureState::Operation_REPLACE;
		fadeState.alphaSource1=TextureState::Source_TEXTURE;
		mSkyMaterial->setTexture(state,fadeTexture);
		mSkyMaterial->setTextureState(state,fadeState);
		mFadeAccessor=Matrix4x4Accessor::ptr(new TextureStateMatrix4x4Accessor(mSkyMaterial,state++));
	}
	else{
		mSkyMaterial->setBlendState(BlendState::Combination_ALPHA);

		Image::ptr composite=createComposite(cloud,bump,lightDir,skyColor);
		Texture::ptr compositeTexture=mEngine->getTextureManager()->createTexture(composite);

		mSkyMaterial->setTexture(state,compositeTexture);
		mCompositeAccessor=Matrix4x4Accessor::ptr(new TextureStateMatrix4x4Accessor(mSkyMaterial,state++));
	}
	mSkyMaterial->retain();

	Mesh::ptr mesh=mEngine->getMeshManager()->createSkyDome(vertexBuffer,indexBuffer,sphere,numSegments,numRings,0.35);
	Transform transform;
	transform.setScale(1,1,0.5f);
	mesh->setTransform(transform);
	mesh->getSubMesh(0)->material=mSkyMaterial;
	mesh->getSubMesh(0)->material->retain();

	mSkyDome=mEngine->createNodeType(MeshNode::type(),mScene);
	mSkyDome->setMesh(mesh);
	attach(mSkyDome);

	Image::ptr glow=createGlow(96,96);

	Material::ptr sunMaterial=mEngine->getMaterialManager()->createMaterial(mEngine->getTextureManager()->createTexture(glow));
	sunMaterial->setBlendState(BlendState::Combination_COLOR_ADDITIVE);
	sunMaterial->setMaterialState(MaterialState(false));
	sunMaterial->setDepthState(DepthState(DepthState::DepthTest_NEVER,false));
	sunMaterial->setLayer(-1);

	mSun=mEngine->createNodeType(SpriteNode::type(),mScene);
	mSun->setMaterial(sunMaterial);
	mSun->setScale(128,128,128);
	attach(mSun);

	mLight=mEngine->createNodeType(LightNode::type(),mScene);
	LightState lightState;
	lightState.diffuseColor.set(Colors::WHITE);
	mLight->setLightState(lightState);
	attach(mLight);

	Math::normalize(lightDir);
	updateLightDirection(lightDir);

	return this;
}

void Sky::destroy(){
	mSkyMaterial->release();

	super::destroy();
}

void Sky::updateLightDirection(const Vector3 &lightDir){
	LightState state=mLight->getLightState();
	state.direction=-lightDir;
	mLight->setLightState(state);

	mSun->setTranslate(lightDir*256);

	VertexBuffer *buffer=mSkyDome->getMesh()->getStaticVertexData()->getVertexBuffer(0);
	VertexBufferAccessor vba(buffer,Buffer::Access_READ_WRITE);
	int ip=buffer->getVertexFormat()->findSemantic(VertexFormat::Semantic_POSITION);
	int ic=buffer->getVertexFormat()->findSemantic(VertexFormat::Semantic_COLOR);
	Vector3 pos,dir;
	int i;
	for(i=0;i<vba.getSize();++i){
		vba.get3(i,ip,pos);
		Math::normalize(pos);
		if(mBumpAccessor!=NULL){
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
}

void Sky::frameUpdate(int dt,int scope){
	super::frameUpdate(dt,scope);

	Vector3 offset;
	Math::mul(offset,getWorldTranslate(),0.0001); // Since the sky is in the background, it's world translate will be the viewer's world translate
	offset.x+=Math::fromMilli(mScene->getTime())*0.01;
	Matrix4x4 matrix;
	Math::setMatrix4x4FromTranslate(matrix,offset);
	if(mBumpAccessor!=NULL){
		mBumpAccessor->setMatrix4x4(matrix);
	}
	if(mCloudAccessor!=NULL){
		mCloudAccessor->setMatrix4x4(matrix);
	}
	if(mCompositeAccessor!=NULL){
		mCompositeAccessor->setMatrix4x4(matrix);
	}
}

Image::ptr Sky::createCloud(int width,int height,int scale,int seed,float cover,float sharpness,float brightness){
	Image::ptr image(Image::createAndReallocate(Image::Dimension_D2,Image::Format_RGBA_8,width,height));

	Noise noise(4,scale,1,seed,256);

	uint8 *data=image->getData();
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

	return image;
}

Image::ptr Sky::createBump(Image *in,float xscale,float yscale,float zscale,int spread){
	int width=in->getWidth(),height=in->getHeight();
	Image::ptr image(Image::createAndReallocate(Image::Dimension_D2,Image::Format_RGB_8,width,height));

	Pixel<tbyte> ip,xp,yp;
	uint8 *data=image->getData();
	int x=0,y=0;
	for(y=0;y<height;y++){
		for(x=0;x<width;x++){
			int sx=(x+1)%width;
			int sy=(y+1)%height;

			in->getPixel(ip,x,y);
			in->getPixel(xp,sx,y);
			in->getPixel(yp,x,sy);
			Vector3 xv(xscale,0,xp.a-ip.a);Math::normalize(xv);
			Vector3 yv(0,yscale,yp.a-ip.a);Math::normalize(yv);
			Vector3 zv;Math::cross(zv,xv,yv);
			zv.z*=zscale;Math::normalize(zv);

			data[(y*width+x)*3+0]=(zv.x/2+0.5)*255;
			data[(y*width+x)*3+1]=(zv.y/2+0.5)*255;
			data[(y*width+x)*3+2]=(zv.z/2+0.5)*255;
		}
	}

	Image::ptr blurImage(Image::createAndReallocate(Image::Dimension_D2,Image::Format_RGB_8,width,height));
	uint8 *blurData=blurImage->getData();
	int i=0,j=0;
	for(y=0;y<height;y++){
		for(x=0;x<width;x++){
			int tx=0,ty=0,tz=0;
			for(i=-spread;i<=spread;++i){
				for(j=-spread;j<=spread;++j){
					int sx=(x+i+width)%width;
					int sy=(y+j+height)%height;

					tx+=data[(sy*width+sx)*3+0];
					ty+=data[(sy*width+sx)*3+1];
					tz+=data[(sy*width+sx)*3+2];
				}
			}
			tx/=((spread*2+1)*(spread*2+1));
			ty/=((spread*2+1)*(spread*2+1));
			tz/=((spread*2+1)*(spread*2+1));
			
			blurData[(y*width+x)*3+0]=tx;
			blurData[(y*width+x)*3+1]=ty;
			blurData[(y*width+x)*3+2]=tz;
		}
	}

	return blurImage;
}

Image::ptr Sky::createFade(int width,int height,const Vector4 &start,const Vector4 &end,float falloff,float sharpness){
	Image::ptr image(Image::createAndReallocate(Image::Dimension_D2,Image::Format_RGBA_8,width,height));

	uint8 *data=image->getData();
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

	return image;
}

Image::ptr Sky::createComposite(Image *cloud,Image *bump,const Vector3 &lightDir,const Vector4 &skyColor){
	int width=cloud->getWidth(),height=cloud->getHeight();
	Image::ptr image(Image::createAndReallocate(Image::Dimension_D2,Image::Format_RGB_8,width,height));

	Vector3 dir;
	Math::neg(dir,lightDir);

	uint8 *bumpData=bump->getData();
	uint8 *cloudData=cloud->getData();
	uint8 *data=image->getData();
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

	return image;
}

Image::ptr Sky::createGlow(int width,int height){
	Image::ptr image(Image::createAndReallocate(Image::Dimension_D2,Image::Format_L_8,width,height));

	uint8 *data=image->getData();
	int x=0,y=0;
	for(y=0;y<height;y++){
		for(x=0;x<width;x++){
			float v=1.0;
			v=v*(1.0-Math::length(Vector2(x-width/2,y-height/2))/(width/2));
			v=pow(v*2,3);
			v=Math::clamp(0,1,v);

			data[y*width+x]=255*v;
		}
	}

	return image;
}
