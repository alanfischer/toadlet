#include "MyPlanet.h"

inline float scurve(float t){ return t*t*(3.0 - 2.0*t);}

using namespace toadlet::tadpole::Math;

/// @todo: Background appears to have some faces inverted when in Skybox mode

class Planet:public ParentNode{
public:
	TOADLET_NODE(Planet,ParentNode);

	Planet():super(),
		//mMeshNode,
		//mSparkNode,
		mStartTime(0),
		mFadeOutSparkTime(0)
	{}

	Node *create(Scene *scene){
		super::create(scene);

		Mesh::ptr mesh=getEngine()->getMeshManager()->createSphere(Sphere(Math::ONE));
		{
			LightEffect lightEffect(Colors::BLUE);
			mesh->subMeshes[0]->material->setLightEffect(lightEffect);
		}
		mMeshNode=getEngine()->createNodeType(MeshNode::type(),getScene());
		mMeshNode->setMesh(mesh);
		Matrix3x3 rotate; setMatrix3x3FromX(rotate,HALF_PI);
		mMeshNode->setRotate(rotate);
		attach(mMeshNode);

		mesh=MyPlanet::createDisc(getEngine(),Math::ONE);
		{
			TextureStage::ptr textureStage=getEngine()->getMaterialManager()->createTextureStage(mEngine->getTextureManager()->findTexture("spark"));
			textureStage->setBlend(TextureBlend(TextureBlend::Operation_MODULATE,TextureBlend::Source_PRIMARY_COLOR,TextureBlend::Source_TEXTURE));
			mesh->subMeshes[0]->material->setTextureStage(0,textureStage);
			mesh->subMeshes[0]->material->setDepthWrite(false);
			mesh->subMeshes[0]->material->setBlend(Blend::Combination_COLOR_ADDITIVE);
		}
		mSparkNode=getEngine()->createNodeType(MeshNode::type(),getScene());
		mSparkNode->setMesh(mesh);
		mSparkNode->setCameraAligned(true);
		attach(mSparkNode);

		mStartTime=getScene()->getTime();

		return this;
	}

	scalar getSize() const{return mMeshNode->getScale().x;}
	void setSize(scalar s){mMeshNode->setScale(s,s,s);}

	void setColor(const Vector4 &color){mMeshNode->getSubMesh(0)->material->setLightEffect(LightEffect(color));}
	const Vector4 &getColor() const{return mMeshNode->getSubMesh(0)->material->getLightEffect().diffuse;}
	
	void frameUpdate(int dt,int scope){
		super::frameUpdate(dt,scope);
	
		if(mSparkNode!=NULL){
			scalar value=Math::fromMilli(getScene()->getTime()-mStartTime);
			Matrix4x4 matrix;
			setMatrix4x4FromTranslate(matrix,0,-value/2,0);
			setMatrix4x4FromScale(matrix,4,0.5,1);
			mSparkNode->getSubMesh(0)->getRenderMaterial()->getTextureStage(0)->setCalculation(TextureStage::Calculation_NORMAL,matrix);

			scalar maxSize=0.4;
			if(mFadeOutSparkTime>0){
				scalar value=Math::fromMilli(getScene()->getTime()-mFadeOutSparkTime);
				scalar size=Math::lerp(maxSize,0,value);
				if(size<=0){
					mSparkNode->destroy();
					mSparkNode=NULL;
				}
				else{
					mSparkNode->setScale(size,size,size);
				}
			}
			else{
				scalar size=Math::clamp(0,maxSize,value/5);
				mSparkNode->setScale(size,size,size);
			}
		}
	}

	Material::ptr getMaterial(){return mMeshNode->getSubMesh(0)->material;}

	void fadeOutSpark(){mFadeOutSparkTime=getScene()->getTime();}

	MeshNode::ptr mMeshNode;
	MeshNode::ptr mSparkNode;
	int mStartTime;
	int mFadeOutSparkTime;
};
TOADLET_NODE_IMPLEMENT(Planet,"Planet");

class Orbit:public ParentNode{
public:
	TOADLET_NODE(Orbit,ParentNode);

	Orbit():super(),
		//mMeshNode,
		//mMaterial,
		//mStartColor,mEndColor,
		//mLightEffect,
		mVisible(false),
		mDestroy(false),
		mEndDistance(0),
		mAlpha(0)
		//mPlanet
	{}

	Node *create(Scene *scene,Planet::ptr planet,scalar size,scalar thickness,scalar distance){
		super::create(scene);

		mPlanet=planet;
		attach(mPlanet);

		Mesh::ptr mesh=getEngine()->getMeshManager()->createTorus(size,thickness,16,4);
		{
			mMaterial=mesh->subMeshes[0]->material;
			mMaterial->setBlend(Blend::Combination_COLOR_ADDITIVE);
			mMaterial->setLightEffect(LightEffect(Colors::BLACK,Colors::BLACK,Colors::BLACK,0));
			mMaterial->setDepthWrite(false);
			{
				TextureStage::ptr textureStage=getEngine()->getMaterialManager()->createTextureStage(createDash(128,1));
				Matrix4x4 scale;
				Math::setMatrix4x4FromScale(scale,32,1,1);
				textureStage->setCalculation(TextureStage::Calculation_NORMAL,scale);
				mMaterial->setTextureStage(0,textureStage);
			}
		}

		mMeshNode=getEngine()->createNodeType(MeshNode::type(),getScene());
		mMeshNode->setMesh(mesh);
		attach(mMeshNode);

		mPlanet->setTranslate(distance,0,0);
		mMeshNode->setScale(distance,distance,distance);
		mEndDistance=distance;

		mLightEffect.set(mMaterial->getLightEffect());
		mStartColor=Colors::BLACK;
		mEndColor=Colors::AZURE;
		mVisible=true;

		return this;
	}

	void frameUpdate(int dt,int scope){
		super::frameUpdate(dt,scope);

		int time=getScene()->getTime();

		if(mVisible){
			mAlpha+=Math::fromMilli(dt*4);
		}
		else{
			mAlpha-=Math::fromMilli(dt*4);
		}
		mAlpha=Math::clamp(0,Math::ONE,mAlpha);
		Math::lerp(mLightEffect.ambient,mStartColor,mEndColor,mAlpha);
		mMaterial->setLightEffect(mLightEffect);

		scalar distance=mPlanet->getTranslate().x+Math::mul(mEndDistance-mPlanet->getTranslate().x,Math::fromMilli(dt*4));
		mPlanet->setTranslate(distance,0,0);
		mMeshNode->setScale(distance,distance,distance);
		mMeshNode->setRotate(Vector3(0,0,Math::ONE),-Math::fromMilli(time));

		if(mAlpha==0 && mDestroy==true){
			destroy();
		}
	}

	Texture::ptr createDash(int width,int height){
		Image::ptr image(Image::createAndReallocate(Image::Dimension_D2,Image::Format_L_8,width,height));

		int lower=width/3;
		int upper=width*2/3;
		int border=width/8;

		uint8 *data=image->getData();
		int x=0,y=0;
		for(y=0;y<height;y++){
			for(x=0;x<width;x++){
				float c=1.0;

				if(x<lower-border || x>upper+border){
					c=0;
				}
				else if(x<lower){
					c=(float)(x-(lower-border))/(float)border;
				}
				else if(x>upper){
					c=1-(float)(x-upper)/(float)border;
				}
				else{
					c=1;
				}

				data[y*width+x]=c*255;
			}
		}

		return getEngine()->getTextureManager()->createTexture(image);
	}

	void setDistance(scalar distance){
		mEndDistance=distance;
		/// @todo: Perhaps a more sensical mapping instead of just straight HSV
		Vector4 color;
		Math::setHSVA(color,Vector4(Math::clamp(0,Math::ONE,(distance-Math::ONE)/Math::fromInt(12)),Math::ONE,Math::ONE,Math::ONE));
		mPlanet->setColor(color);
	}
	scalar getDistance() const{return mPlanet->getTranslate().x;}

	void setVisible(bool visible){mVisible=visible;}
	bool getVisible() const{return mVisible;}

	void fadeOutAndDestroy(){
		mVisible=false;
		mDestroy=true;
	}

protected:
	MeshNode::ptr mMeshNode;
	Material::ptr mMaterial;
	Vector4 mStartColor,mEndColor;
	LightEffect mLightEffect;
	bool mVisible;
	bool mDestroy;
	scalar mEndDistance;
	scalar mAlpha;
	Planet::ptr mPlanet;
};
TOADLET_NODE_IMPLEMENT(Orbit,"Orbit");

class SunListener:public NodeListener{
public:
	SunListener(TextureStage::ptr primaryFlareStage1,TextureStage::ptr secondaryFlareStage1,TextureStage::ptr primarySunStage1,TextureStage::ptr secondarySunStage1){
		primaryFlareStage=primaryFlareStage1;
		secondaryFlareStage=secondaryFlareStage1;
		primarySunStage=primarySunStage1;
		secondarySunStage=secondarySunStage1;
	}
	
	void nodeDestroyed(Node *node){}
	void transformUpdated(Node *node,int tu){}
	void logicUpdated(Node *node,int dt){}
	void frameUpdated(Node *node,int dt){
		scalar value=Math::fromMilli(node->getScene()->getTime());
	
		matrix.reset();
		setMatrix4x4FromTranslate(matrix,value/32,Math::sin(value/8),0);
		primarySunStage->setCalculation(TextureStage::Calculation_NORMAL,matrix);
		setMatrix4x4FromTranslate(matrix,Math::cos(-value/12),Math::cos(-value/8),0);
		secondarySunStage->setCalculation(TextureStage::Calculation_NORMAL,matrix);

		matrix.reset();
		setMatrix4x4FromZ(matrix,Math::sin(value/2));
		scalar s=mul(Math::sin(value*2),fromMilli(100))+fromMilli(1100);
		setMatrix4x4FromScale(scale,s,s,0);
		postMul(matrix,scale);
		setMatrix4x4AsTextureRotation(matrix);
		primaryFlareStage->setCalculation(TextureStage::Calculation_NORMAL,matrix);

		matrix.reset();
		setMatrix4x4FromZ(matrix,-Math::sin(value/2));
		s=mul(Math::sin(value*2),-fromMilli(100))+fromMilli(1100);
		setMatrix4x4FromScale(scale,s,s,0);
		postMul(matrix,scale);
		setMatrix4x4AsTextureRotation(matrix);
		secondaryFlareStage->setCalculation(TextureStage::Calculation_NORMAL,matrix);
	}

protected:
	Matrix4x4 matrix;
	Matrix4x4 scale;
	TextureStage::ptr primaryFlareStage;
	TextureStage::ptr secondaryFlareStage;
	TextureStage::ptr primarySunStage;
	TextureStage::ptr secondarySunStage;
};

enum{
	Mode_END=0,
	Mode_FOCUS_SUN,
	Mode_CREATE_PLANET,
	Mode_FOCUS_PLANET,
	Mode_CREATE_LAND,
};

MyPlanet::MyPlanet():Application(){
	mMode=Mode_FOCUS_SUN;
	mRandom.setSeed(System::mtime());
	mCreatePlanetStartTime=0;
	mPlanetDiskFadeOutTime=0;
}

MyPlanet::~MyPlanet(){
}

Mesh::ptr MyPlanet::createDisc(Engine *engine,scalar size){
	VertexBuffer::ptr vertexBuffer=engine->getBufferManager()->createVertexBuffer(Buffer::Usage_BIT_STATIC,Buffer::Access_BIT_WRITE,engine->getVertexFormats().POSITION_COLOR_TEX_COORD,7);
	{
		VertexBufferAccessor vba;
		vba.lock(vertexBuffer,Buffer::Access_BIT_WRITE);

		int i=0;
		scalar out=size;
		scalar mid=size*0.6;
		scalar third=Math::ONE/3;
		scalar third2=Math::ONE*2/3;
		vba.set3(i,0, 0,0,0);		vba.setRGBA(i,1,0xFFFFFFFF); vba.set2(i,2,Math::HALF,Math::ONE); ++i;
		vba.set3(i,0, 0,out,0);		vba.setRGBA(i,1,0x00000000); vba.set2(i,2,third,0); ++i;
		vba.set3(i,0, out,mid,0);	vba.setRGBA(i,1,0x00000000); vba.set2(i,2,third2,0); ++i;
		vba.set3(i,0, out,-mid,0);	vba.setRGBA(i,1,0x00000000); vba.set2(i,2,third,0); ++i;
		vba.set3(i,0, 0,-out,0);	vba.setRGBA(i,1,0x00000000); vba.set2(i,2,third2,0); ++i;
		vba.set3(i,0, -out,-mid,0);	vba.setRGBA(i,1,0x00000000); vba.set2(i,2,third,0); ++i;
		vba.set3(i,0, -out,mid,0);	vba.setRGBA(i,1,0x00000000); vba.set2(i,2,third2,0); ++i;

		vba.unlock();
	}

	IndexBuffer::ptr indexBuffer=engine->getBufferManager()->createIndexBuffer(Buffer::Usage_BIT_STATIC,Buffer::Access_BIT_WRITE,IndexBuffer::IndexFormat_UINT8,18);
	{
		IndexBufferAccessor iba;
		iba.lock(indexBuffer,Buffer::Access_BIT_WRITE);

		int i=0;
		iba.set(i++,1);	iba.set(i++,0); iba.set(i++,2);
		iba.set(i++,2);	iba.set(i++,0); iba.set(i++,3);
		iba.set(i++,3);	iba.set(i++,0); iba.set(i++,4);
		iba.set(i++,4);	iba.set(i++,0); iba.set(i++,5);
		iba.set(i++,5);	iba.set(i++,0); iba.set(i++,6);
		iba.set(i++,6);	iba.set(i++,0); iba.set(i++,1);

		iba.unlock();
	}

	Mesh::SubMesh::ptr subMesh(new Mesh::SubMesh());
	subMesh->indexData=IndexData::ptr(new IndexData(IndexData::Primitive_TRIS,indexBuffer));
	subMesh->material=engine->getMaterialManager()->createMaterial();
	subMesh->material->retain();
	subMesh->material->setFaceCulling(Renderer::FaceCulling_BACK);
	subMesh->material->setLighting(false);

	Mesh::ptr mesh(new Mesh());
	mesh->subMeshes.resize(1);
	mesh->subMeshes[0]=subMesh;
	mesh->staticVertexData=VertexData::ptr(new VertexData(vertexBuffer));

	return mesh;
}

Texture::ptr MyPlanet::createPerlin(Engine *engine,int width,int height,int scale,int seed){
	Image::ptr image(Image::createAndReallocate(Image::Dimension_D2,Image::Format_L_8,width,height));

	Noise noise(4,scale,1,seed,256);

	uint8 *data=image->getData();
	int x=0,y=0;
	for(y=0;y<height;y++){
		for(x=0;x<width;x++){
			float n=noise.tileablePerlin2((float)x/(float)width,(float)y/(float)height,1,1)*0.5 + 0.5;
			data[y*width+x]=255*n;
		}
	}

	return engine->getTextureManager()->createTexture(image);
}

Texture::ptr MyPlanet::createNebula(Engine *engine,int width,int height,int scale,int seed,float falloff){
	Image::ptr image(Image::createAndReallocate(Image::Dimension_D2,Image::Format_L_8,width,height));

	Noise noise(4,scale,1,seed,256);

	uint8 *data=image->getData();
	int x=0,y=0;
	for(y=0;y<height;y++){
		for(x=0;x<width;x++){
			float n=(noise.perlin2((float)x/(float)width,(float)y/(float)height)*0.5 + 0.5);
			float n2=(noise.noise2((float)x/(4*(float)width),(float)y/(4*(float)height))*0.5 + 0.5);
			float v=n*n2*falloff;
			v=pow(v,2);
			v=v*(1.0-Math::length(Vector2(x-width/2,y-height/2))/(width/2));
			if(v<0 || x==0 || y==0 || x==width-1 || y==height-1) v=0;
			if(v>1) v=1;

			data[y*width+x]=255*v;
		}
	}

	return engine->getTextureManager()->createTexture(image);
}

Texture::ptr MyPlanet::createFlare(Engine *engine,int width,int height,int scale,int seed,float falloff){
	Image::ptr image(Image::createAndReallocate(Image::Dimension_D2,Image::Format_L_8,width,height));

	Noise noise(4,scale,1,seed,256);

	uint8 *data=image->getData();
	int x=0,y=0;
	for(y=0;y<height;y++){
		for(x=0;x<width;x++){
			float n=(noise.perlin2((float)x/(float)width,(float)y/(float)height)*0.5 + 0.5);
			float v=n*falloff;
			v=pow(v,2);
			v=v*(1.0-Math::length(Vector2(x-width/2,y-height/2))/(width/2));
			if(v<0 || x==0 || y==0 || x==width-1 || y==height-1) v=0;
			if(v>1) v=1;

			data[y*width+x]=255*v;
		}
	}

	return engine->getTextureManager()->createTexture(image);
}

Texture::ptr MyPlanet::createPoint(Engine *engine,int width,int height){
	Image::ptr image(Image::createAndReallocate(Image::Dimension_D2,Image::Format_L_8,width,height));

	uint8 *data=image->getData();
	int x=0,y=0;
	for(y=0;y<height;y++){
		for(x=0;x<width;x++){
			float v=1.0;
			v=v*(1.0-Math::length(Vector2(x-width/2,y-height/2))/(width/2));
			if(v<0) v=0;
			if(v>1) v=1;
			v=scurve(scurve(pow(v,1.25f)));

			data[y*width+x]=255*v;
		}
	}

	return engine->getTextureManager()->createTexture(image);
}

void MyPlanet::getSolarSystemPoint(Vector3 &result,int x,int y,scalar height){
	int vx=mCamera->getViewportX();
	int vy=mCamera->getViewportY();
	int vw=mCamera->getViewportWidth();
	int vh=mCamera->getViewportHeight();

	Segment segment;
	Matrix4x4 projViewMatrix,scratch;
	Math::mul(projViewMatrix,mCamera->getProjectionMatrix(),mCamera->getViewMatrix());
	Math::unprojectSegment(segment,projViewMatrix,x,y,vx,vy,vw,vh,scratch);
	Plane plane(Math::Z_UNIT_VECTOR3,height);
	Vector3 normal;
	Math::findIntersection(segment,plane,result,normal);
}

ParentNode::ptr MyPlanet::createSun(scalar size){
	ParentNode::ptr sun=mEngine->createNodeType(ParentNode::type(),mScene);
	{
		SpriteNode::ptr flareNode=mEngine->createNodeType(SpriteNode::type(),mScene);
		Material::ptr material=mEngine->getMaterialManager()->createMaterial();
		TextureStage::ptr primaryStage=mEngine->getMaterialManager()->createTextureStage(mEngine->getTextureManager()->findTexture("sunflare1"),true);
		primaryStage->setBlend(TextureBlend(TextureBlend::Operation_MODULATE,TextureBlend::Source_PREVIOUS,TextureBlend::Source_TEXTURE));
		material->setTextureStage(0,primaryStage);
		TextureStage::ptr secondaryStage=mEngine->getMaterialManager()->createTextureStage(mEngine->getTextureManager()->findTexture("sunflare2"),true);
		secondaryStage->setBlend(TextureBlend(TextureBlend::Operation_MODULATE,TextureBlend::Source_PREVIOUS,TextureBlend::Source_TEXTURE));
		material->setTextureStage(1,secondaryStage);

		LightEffect lightEffect(Colors::ORANGE);
		lightEffect.emissive.set(Colors::ORANGE);
		material->setLightEffect(lightEffect);
		material->setBlend(Blend::Combination_COLOR_ADDITIVE);
		material->setLighting(true);
		material->setDepthWrite(false);
		flareNode->setMaterial(material);

		scalar flareSize=Math::mul(size,Math::fromMilli(3200));
		flareNode->setScale(flareSize,flareSize,ONE);
		sun->attach(flareNode);

		Mesh::ptr mesh=getEngine()->getMeshManager()->createSphere(Sphere(size));
		{
			LightEffect lightEffect(Colors::YELLOW);
			lightEffect.emissive.set(Colors::YELLOW);
			mesh->subMeshes[0]->material->setLightEffect(lightEffect);

			TextureStage::ptr textureStage=getEngine()->getMaterialManager()->createTextureStage(mEngine->getTextureManager()->findTexture("sun1"));
			textureStage->setBlend(TextureBlend(TextureBlend::Operation_MODULATE,TextureBlend::Source_PRIMARY_COLOR,TextureBlend::Source_TEXTURE));
			mesh->subMeshes[0]->material->setTextureStage(0,textureStage);

			textureStage=getEngine()->getMaterialManager()->createTextureStage(mEngine->getTextureManager()->findTexture("sun2"));
			textureStage->setBlend(TextureBlend(TextureBlend::Operation_ADD,TextureBlend::Source_PREVIOUS,TextureBlend::Source_TEXTURE));
			mesh->subMeshes[0]->material->setTextureStage(1,textureStage);
		}
		MeshNode::ptr meshNode=mEngine->createNodeType(MeshNode::type(),mScene);
		meshNode->setMesh(mesh);
		Matrix3x3 rotate; setMatrix3x3FromX(rotate,HALF_PI);
		meshNode->setRotate(rotate);
		sun->attach(meshNode);

		LightNode::ptr light=mEngine->createNodeType(LightNode::type(),mScene);
		LightState lightState;
		lightState.type=LightState::Type_POINT;
		lightState.diffuseColor.set(Math::ONE_VECTOR4);
		lightState.linearAttenuation=fromMilli(10);
		light->setLightState(lightState);
		sun->attach(light);

		if(flareNode->getMaterial()!=NULL){
			sun->addNodeListener(NodeListener::ptr(new SunListener(
				flareNode->getMaterial()->getTextureStage(0),
				flareNode->getMaterial()->getTextureStage(1),
				meshNode->getSubMesh(0)->getRenderMaterial()->getTextureStage(0),
				meshNode->getSubMesh(0)->getRenderMaterial()->getTextureStage(1)
			)));
		}
	}

	return sun;
}

void MyPlanet::create(){
	int i;

	Application::setBackable(false);

	Application::create("d3d9");

	mScene=Scene::ptr(new Scene(mEngine));

	mOverlay=mEngine->createNodeType(CameraNode::type(),mScene);
	mOverlay->setClearFlags(Renderer::ClearFlag_DEPTH);
	mOverlay->setScope(1<<1);
	mScene->getRoot()->attach(mOverlay);

	mEngine->getTextureManager()->manage(createPerlin(mEngine,128,128,8,2),"sun1");
	mEngine->getTextureManager()->manage(createPerlin(mEngine,128,128,8,2),"sun2");
	mEngine->getTextureManager()->manage(createFlare(mEngine,128,128,8,mRandom.nextInt(),3),"sunflare1");
	mEngine->getTextureManager()->manage(createFlare(mEngine,128,128,8,mRandom.nextInt(),3),"sunflare2");
	mEngine->getTextureManager()->manage(createPerlin(mEngine,128,128,8,1),"spark");

	mSun=createSun(Math::ONE);
	mScene->getRoot()->attach(mSun);

	mCameraOrbit=mEngine->createNodeType(ParentNode::type(),mScene);
	mSun->attach(mCameraOrbit);

	mCamera=mEngine->createNodeType(CameraNode::type(),mScene);
	mCamera->setScope(1<<0);
	mCamera->setNearAndFarDist(Math::fromInt(1),Math::fromInt(200));
	mCameraOrbit->attach(mCamera);

	Node::ptr background=createBackground();
	mScene->getBackground()->attach(background);
	bool backgroundToSkybox=true;
	if(backgroundToSkybox){
		Mesh::ptr skyBoxMesh=mCamera->renderToSkyBox(mRenderer,Image::Format_RGB_5_6_5,2048,fromInt(10));
		MeshNode::ptr skyBox=mEngine->createNodeType(MeshNode::type(),mScene);
		skyBox->setMesh(skyBoxMesh);
		background->destroy();
		mScene->getBackground()->attach(skyBox);
		mCamera->setClearFlags(Renderer::ClearFlag_DEPTH);
	}

	mPlanetDisk=mEngine->createNodeType(ParticleNode::type(),mScene);
	{
		mPlanetDisk->setNumParticles(300,ParticleNode::ParticleType_SPRITE,fromMilli(250));
		{
			Material::ptr material=mEngine->getMaterialManager()->createMaterial();
			material->setFaceCulling(Renderer::FaceCulling_NONE);
			material->setLightEffect(LightEffect(true));
			material->setLighting(true);
			material->setTextureStage(0,getEngine()->getMaterialManager()->createTextureStage(createPoint(mEngine,64,64)));
			material->setBlend(Blend::Combination_COLOR);
			material->setDepthWrite(false);
			material->setLayer(1);
			mPlanetDisk->setMaterial(material);
		}
	}
	for(i=0;i<mPlanetDisk->getNumParticles();++i){
		ParticleNode::Particle *p=mPlanetDisk->getParticle(i);
		p->color=Colors::CYAN.getRGBA();
		scalar d=mRandom.nextScalar(fromMilli(4000),fromMilli(5000));
		scalar a=mRandom.nextScalar(0,TWO_PI);
		p->x=mul(Math::sin(a),d);
		p->y=mul(Math::cos(a),d);
		p->z=0;
		a+=Math::HALF_PI;
		scalar s=mRandom.nextFloat(2.5,4);
		p->vx=mul(Math::sin(a),s);
		p->vy=mul(Math::cos(a),s);
		p->vz=0;
	}
	mSun->attach(mPlanetDisk);

	backgroundAnimation();
	startAnimation();
}

void MyPlanet::destroy(){
	mScene->destroy();

	Application::destroy();
}

void MyPlanet::resized(int width,int height){
	if(mCamera!=NULL && width>0 && height>0){
		if(width>=height){
			mCamera->setProjectionFovY(degToRad(fromInt(60)),Math::div(fromInt(width),fromInt(height)),mCamera->getNearDist(),mCamera->getFarDist());
		}
		else{
			mCamera->setProjectionFovX(degToRad(fromInt(60)),Math::div(fromInt(height),fromInt(width)),mCamera->getNearDist(),mCamera->getFarDist());
		}
		mCamera->setViewport(Viewport(0,0,width,height));
	}

	// Create overlay
	if(mOverlay!=NULL && width>0 && height>0){
		mOverlay->setProjectionOrtho(0,fromInt(width),fromInt(height),0,0,ONE);
		mOverlay->setViewport(Viewport(0,0,width,height));

		mOverlay->destroyAllChildren();
	}
}

void MyPlanet::render(Renderer *renderer){
	renderer->beginScene();
		mCamera->render(renderer);
	renderer->endScene();
	renderer->swap();
}

void MyPlanet::update(int dt){
	int i;

	scalar fdt=fromMilli(dt);
	scalar ftime=fromMilli(mScene->getTime());

	if(mMode==Mode_CREATE_PLANET){
		if(mOrbit!=NULL && mOrbit->getDistance()<=Math::fromMilli(1000)){
			burnupAnimation();
		}
		if(mOrbit!=NULL && mOrbit->getDistance()>=Math::fromMilli(11000)){
			focusPlanet();
		}
		if(mCreatePlanetStartTime>0 && mScene->getLogicTime()-mCreatePlanetStartTime>20000){
			focusPlanet();
		}
	}

	for(i=0;i<mPlanetDisk->getNumParticles();++i){
		ParticleNode::Particle *p=mPlanetDisk->getParticle(i);

		if(p->scale==0){
			continue;
		}

		bool pulled=false;
		if(mMode==Mode_CREATE_PLANET && mPlanet!=NULL){
			Vector3 planetOrigin=mPlanet->getWorldTranslate();
			Vector3 pullPointGravity(planetOrigin.x-p->x,planetOrigin.y-p->y,planetOrigin.z-p->z);
			scalar l=Math::length(pullPointGravity);
			if(l<1.5){
				p->vx=(p->vx*(1-fdt))+pullPointGravity.x*fdt*4;
				p->vy=(p->vy*(1-fdt))+pullPointGravity.y*fdt*4;
				p->vz=(p->vz*(1-fdt))+pullPointGravity.z*fdt*4;

				pulled=true;
			}
		}
		if(!pulled){
			Vector3 gravity(-p->x,-p->y,-p->z);
			scalar l=Math::length(gravity);
			if(l<Math::fromMilli(500)){
				p->scale=0;
			}
			else{
				Math::div(gravity,l);

				p->vx+=gravity.x*fdt;
				p->vy+=gravity.y*fdt;
				p->vz+=gravity.z*fdt;
			}
		}

		if(mMode==Mode_CREATE_PLANET && mPlanet!=NULL){
			Vector3 planetOrigin=mPlanet->getWorldTranslate();
			Vector3 pullPointGravity(planetOrigin.x-p->x,planetOrigin.y-p->y,planetOrigin.z-p->z);
			scalar l=Math::length(pullPointGravity);
			if(l<mPlanet->getSize() || l<0.25){
				p->scale=0;

				scalar size=mPlanet->getSize();
				size+=0.001;
				mPlanet->setSize(size);
			}
		}

		if(mPlanetDiskFadeOutTime>0){
			p->scale-=Math::fromMilli(dt);
			if(p->scale<0){
				p->scale=0;
			}
		}

		// Update position
		p->x+=p->vx*fdt;
		p->y+=p->vy*fdt;
		p->z+=p->vz*fdt;
	}

	mScene->update(dt);
}

void MyPlanet::mousePressed(int x,int y,int button){
	if(mMode==Mode_CREATE_PLANET){
		if(mOrbit==NULL){
			Vector3 result;
			getSolarSystemPoint(result,x,y,0);
			scalar distance=Math::clamp(2,6,Math::length(result));
			Math::normalize(result);
			scalar angle=Math::atan2(result.y,result.x);

			mPlanet=mEngine->createNodeType(Planet::type(),mScene);
			mPlanet->setSize(0);
			mOrbit=(Orbit*)mEngine->allocNodeType(Orbit::type())->create(mScene,mPlanet,Math::ONE,0.0125,distance);
			mOrbit->setRotate(Vector3(0,0,Math::ONE),angle);
			mSun->attach(mOrbit);

			mCreatePlanetStartTime=mScene->getLogicTime();
		}
		else{
			Vector3 result;
			getSolarSystemPoint(result,x,y,0);
			scalar distance=Math::clamp(0,12,Math::length(result));
			mOrbit->setDistance(distance);
			mOrbit->setVisible(true);
		}
	}

	mMouseX=x;
	mMouseY=y;
}

void MyPlanet::mouseMoved(int x,int y){
	if(mMode==Mode_CREATE_PLANET){
		if(mOrbit!=NULL && mOrbit->getVisible()){
			Vector3 result;
			getSolarSystemPoint(result,x,y,0);
			scalar distance=Math::clamp(0,12,Math::length(result));
			mOrbit->setDistance(distance);
		}
	}

	mMouseX=x;
	mMouseY=y;
}

void MyPlanet::mouseReleased(int x,int y,int button){
	if(mMode==Mode_CREATE_PLANET){
		if(mOrbit!=NULL){
			mOrbit->setVisible(false);
		}
	}

	mMouseX=x;
	mMouseY=y;
}

Node::ptr MyPlanet::createBackground(){
	ParentNode::ptr node=mEngine->createNodeType(ParentNode::type(),mScene);
	mScene->getBackground()->attach(node);

	CameraNode::ptr camera=mEngine->createNodeType(CameraNode::type(),mScene);
	camera->setProjectionFovX(Math::degToRad(Math::fromInt(90)),Math::ONE,Math::fromInt(10),Math::fromInt(1000));
	camera->setAlignmentCalculationsUseOrigin(true);
	mScene->getRoot()->attach(camera);

	Random random(System::mtime());
	Material::ptr starMaterial=mEngine->getMaterialManager()->createMaterial();
	TextureStage::ptr starTextureStage1=mEngine->getMaterialManager()->createTextureStage(createPoint(mEngine,128,128));
	starMaterial->setTextureStage(0,starTextureStage1);
	TextureStage::ptr starTextureStage2=mEngine->getMaterialManager()->createTextureStage(createPoint(mEngine,128,128));
	Matrix4x4 scale;
	Math::setMatrix4x4FromScale(scale,0.7,0.7,0.7);
	starTextureStage2->setCalculation(TextureStage::Calculation_NORMAL,scale);
	starTextureStage2->setBlend(TextureBlend(TextureBlend::Operation_MODULATE,TextureBlend::Source_PREVIOUS,TextureBlend::Source_TEXTURE));
	starMaterial->setTextureStage(1,starTextureStage2);
	starMaterial->setDepthWrite(false);
	starMaterial->setBlend(Blend::Combination_COLOR_ADDITIVE);
	starMaterial->setLighting(true);
	starMaterial->setLightEffect(true);

	ParticleNode::ptr stars=mEngine->createNodeType(ParticleNode::type(),mScene);
	stars->setNumParticles(2000,ParticleNode::ParticleType_SPRITE,fromMilli(4000));
	int i;
	for(i=0;i<stars->getNumParticles();++i){
		ParticleNode::Particle *p=stars->getParticle(i);
		Vector4 color;
		float s=scurve(scurve(scurve(random.nextFloat())));
		if(s<0.5){
			Math::lerp(color,Colors::WHITE,Colors::BLUE,s*2);
		}
		else{
			Math::lerp(color,Colors::RED,Colors::WHITE,s*2-1);
		}
		color.x=color.x*1.5 + 0.3;
		color.y=color.y*1.5 + 0.3;
		color.z=color.z*1.5 + 0.3;
		p->color=color.getRGBA();
		Vector3 offset(random.nextFloat(-1,1),random.nextFloat(-1,1),random.nextFloat(-1,1));
		Math::normalize(offset);
		Math::mul(offset,random.nextFloat(150,600));
		p->x=offset.x;
		p->y=offset.y;
		p->z=offset.z;
	}
	stars->setMaterial(starMaterial);
	node->attach(stars);

	for(i=0;i<16;++i){
		Vector3 offset(random.nextFloat(-1,1),random.nextFloat(-1,1),random.nextFloat(-1,1));
		Math::normalize(offset);
		Math::mul(offset,random.nextFloat(60,190));

		float size=random.nextFloat(5,50);

		{
			SpriteNode::ptr flare=mEngine->createNodeType(SpriteNode::type(),mScene);
			Material::ptr material=mEngine->getMaterialManager()->createMaterial();
			TextureStage::ptr textureStage(new TextureStage(createNebula(mEngine,64,64,6,random.nextInt(),1.5)));
			textureStage->setMinFilter(TextureStage::Filter_LINEAR);
			textureStage->setMagFilter(TextureStage::Filter_LINEAR);
			textureStage->setBlend(TextureBlend(TextureBlend::Operation_MODULATE_4X,TextureBlend::Source_PREVIOUS,TextureBlend::Source_TEXTURE));
			material->setTextureStage(0,textureStage);
			material->setDepthWrite(false);
			flare->setMaterial(material);
			flare->getMaterial()->setBlend(Blend::Combination_ALPHA_ADDITIVE);
			Vector4 color;
			Math::lerp(color,Colors::ORANGE,Colors::YELLOW,random.nextFloat());
			flare->getMaterial()->setLightEffect(LightEffect(color));
			flare->getMaterial()->setLighting(true);
			flare->setScale(size*random.nextFloat(0.8,1.0),size*random.nextFloat(0.8,1.0),ONE);
			flare->setTranslate(offset);
			node->attach(flare);
		}

		{
			SpriteNode::ptr flare=mEngine->createNodeType(SpriteNode::type(),mScene);
			Material::ptr material=mEngine->getMaterialManager()->createMaterial();
			TextureStage::ptr textureStage(new TextureStage(createNebula(mEngine,64,64,6,random.nextInt(),1.5)));
			textureStage->setMinFilter(TextureStage::Filter_LINEAR);
			textureStage->setMagFilter(TextureStage::Filter_LINEAR);
			textureStage->setBlend(TextureBlend(TextureBlend::Operation_MODULATE_4X,TextureBlend::Source_PREVIOUS,TextureBlend::Source_TEXTURE));
			material->setTextureStage(0,textureStage);
			material->setDepthWrite(false);
			flare->setMaterial(material);
			flare->getMaterial()->setBlend(Blend::Combination_ALPHA_ADDITIVE);
			Vector4 color;
			Math::lerp(color,Colors::CYAN,Colors::VIOLET,random.nextFloat());
			flare->getMaterial()->setLightEffect(LightEffect(color));
			flare->getMaterial()->setLighting(true);
			flare->setScale(size*random.nextFloat(1.0,1.2),size*random.nextFloat(1.0,1.2),ONE);
			flare->setTranslate(offset);
			node->attach(flare);
		}
	}

	return node;
}

void MyPlanet::backgroundAnimation(){
	Controller::ptr orbitController(new Controller());
	{
		NodePathAnimation::ptr orbitAnimation(new NodePathAnimation(mScene->getBackground()));
		{
			TransformTrack::ptr track(new TransformTrack());
			Quaternion rotate;Matrix3x3 matrix;

			Math::setMatrix3x3FromZ(matrix,0); Math::setQuaternionFromMatrix3x3(rotate,matrix);
			track->keyFrames.add(TransformKeyFrame(fromInt(0),Math::ZERO_VECTOR3,rotate));

			Math::setMatrix3x3FromZ(matrix,degToRad(120)); Math::setQuaternionFromMatrix3x3(rotate,matrix);
			track->keyFrames.add(TransformKeyFrame(fromInt(8),Math::ZERO_VECTOR3,rotate));

			Math::setMatrix3x3FromZ(matrix,degToRad(240)); Math::setQuaternionFromMatrix3x3(rotate,matrix);
			track->keyFrames.add(TransformKeyFrame(fromInt(16),Math::ZERO_VECTOR3,rotate));

			Math::setMatrix3x3FromZ(matrix,degToRad(360)); Math::setQuaternionFromMatrix3x3(rotate,matrix);
			track->keyFrames.add(TransformKeyFrame(fromInt(24),Math::ZERO_VECTOR3,rotate));

			track->compile();
			orbitAnimation->setTrack(track);
		}
		orbitController->attach(orbitAnimation);
		orbitController->setCycling(Controller::Cycling_LOOP);
		orbitController->start();
	}
	mScene->getBackground()->addController(orbitController);
}

void MyPlanet::startAnimation(){
	Controller::ptr orbitController(new Controller());
	{
		NodePathAnimation::ptr orbitAnimation(new NodePathAnimation(mCameraOrbit));
		{
			TransformTrack::ptr track(new TransformTrack());
			Quaternion rotate;Matrix3x3 matrix;

			Math::setMatrix3x3FromZ(matrix,0); Math::setQuaternionFromMatrix3x3(rotate,matrix);
			track->keyFrames.add(TransformKeyFrame(fromInt(0),Math::ZERO_VECTOR3,rotate));

			Math::setMatrix3x3FromZ(matrix,degToRad(-120)); Math::setQuaternionFromMatrix3x3(rotate,matrix);
			track->keyFrames.add(TransformKeyFrame(fromInt(12),Math::ZERO_VECTOR3,rotate));

			Math::setMatrix3x3FromZ(matrix,degToRad(-240)); Math::setQuaternionFromMatrix3x3(rotate,matrix);
			track->keyFrames.add(TransformKeyFrame(fromInt(24),Math::ZERO_VECTOR3,rotate));

			Math::setMatrix3x3FromZ(matrix,degToRad(-360)); Math::setQuaternionFromMatrix3x3(rotate,matrix);
			track->keyFrames.add(TransformKeyFrame(fromInt(36),Math::ZERO_VECTOR3,rotate));

			track->compile();
			orbitAnimation->setTrack(track);
		}
		orbitController->attach(orbitAnimation);
		orbitController->setCycling(Controller::Cycling_LOOP);
		orbitController->start();
	}
	mCamera->addController(orbitController);

	Controller::ptr focusController(new Controller());
	{
		NodePathAnimation::ptr path(new NodePathAnimation(mCamera));
		{
			TransformTrack::ptr track(new TransformTrack());

			mCamera->setLookAt(Vector3(0,fromInt(150),fromInt(100)),ZERO_VECTOR3,Z_UNIT_VECTOR3);
			track->keyFrames.add(TransformKeyFrame(fromInt(0),mCamera->getTranslate(),mCamera->getRotate(),mCamera->getScale()));

			mCamera->setLookAt(Vector3(fromMilli(-500),fromMilli(10000),fromMilli(5000)),ZERO_VECTOR3,Z_UNIT_VECTOR3);
			track->keyFrames.add(TransformKeyFrame(fromInt(12),mCamera->getTranslate(),mCamera->getRotate(),mCamera->getScale()));

			track->compile();
			path->setTrack(track);
		}
		focusController->attach(path);

		focusController->setInterpolation(Controller::Interpolation_COS);
		focusController->setControllerFinishedListener(new ControllerFinishedFunctor<MyPlanet>(this,&MyPlanet::createPlanet),true);
		focusController->start();
	}
	mCamera->addController(focusController);
}

void MyPlanet::createPlanet(){
	mCamera->setNearAndFarDist(Math::fromMilli(100),Math::fromMilli(100000));

	mMode=Mode_CREATE_PLANET;
}

void MyPlanet::burnupAnimation(){
	Vector3 planetOrigin=mPlanet->getWorldTranslate();
	mPlanet->setTranslate(planetOrigin);
	mSun->attach(mPlanet);

	// Remove any other camera animations
	mCamera->removeAllControllers();
	mOrbit->setVisible(false);
	mPlanet->fadeOutSpark();

	Controller::ptr burnupController(new Controller());
	{
		NodePathAnimation::ptr path(new NodePathAnimation(mCamera));
		{
			TransformTrack::ptr track(new TransformTrack());
			Quaternion rotate;

			track->keyFrames.add(TransformKeyFrame(fromInt(0),mCamera->getTranslate(),mCamera->getRotate(),mCamera->getScale()));

			Vector3 point(planetOrigin);
			Math::invert(rotate,mCameraOrbit->getRotate());
			Math::mul(point,rotate);
			point.z=Math::fromMilli(500);
			Math::mul(point,Math::fromMilli(3000));
			mCamera->setLookAt(point,ZERO_VECTOR3,Z_UNIT_VECTOR3);
			track->keyFrames.add(TransformKeyFrame(fromInt(2),mCamera->getTranslate(),mCamera->getRotate(),mCamera->getScale()));

			track->compile();
			path->setTrack(track);

			path->setLookAt(Math::ZERO_VECTOR3);
		}
		burnupController->attach(path);

		NodePathAnimation::ptr sink(new NodePathAnimation(mPlanet));
		{
			TransformTrack::ptr track(new TransformTrack());
			track->keyFrames.add(TransformKeyFrame(fromInt(0),planetOrigin));

			Vector3 origin=planetOrigin;
			Math::normalize(origin);
			Math::mul(origin,Math::fromMilli(1000)-mPlanet->getSize());
			track->keyFrames.add(TransformKeyFrame(fromInt(4),origin));

			track->compile();
			sink->setTrack(track);
		}
		burnupController->attach(sink);

//		Material::ptr material=mPlanet->getMaterial();
//		material->setBlend(Blend::Combination_ALPHA);
//		MaterialColorAnimation::ptr fade(new MaterialColorAnimation(material,Colors::TRANSPARENT_WHITE,Math::fromInt(3)));
//		burnupController->attach(fade);

		burnupController->setInterpolation(Controller::Interpolation_COS);
//		burnupController->setDestroyOnFinish(true);
		burnupController->start();
	}
	mCamera->addController(burnupController);

	mMode=Mode_END;
}

void MyPlanet::focusPlanet(){
	Matrix4x4 cameraMatrix,planetMatrix,matrixTo;
	mCamera->getTransform().getMatrix(cameraMatrix);
	mPlanet->getTransform().getMatrix(planetMatrix);
	Math::invert(matrixTo,planetMatrix);
	Math::postMul(matrixTo,cameraMatrix);
	Transform transform;
	transform.setMatrix(matrixTo);
	mCamera->setTransform(transform);
	mPlanet->attach(mCamera);

	// Remove any other camera animations
	mCamera->removeAllControllers();
	mOrbit->setVisible(false);
	mPlanet->fadeOutSpark();

	Controller::ptr focusController(new Controller());
	{
		NodePathAnimation::ptr path(new NodePathAnimation(mCamera));
		{
			TransformTrack::ptr track(new TransformTrack());

			track->keyFrames.add(TransformKeyFrame(fromInt(0),mCamera->getTranslate(),mCamera->getRotate(),mCamera->getScale()));

			Vector3 eye;
			Math::add(eye,Math::X_UNIT_VECTOR3,Math::Z_UNIT_VECTOR3);
			mCamera->setLookAt(eye,Math::ZERO_VECTOR3,Math::Z_UNIT_VECTOR3);
			track->keyFrames.add(TransformKeyFrame(fromInt(3),mCamera->getTranslate(),mCamera->getRotate(),mCamera->getScale()));

			track->compile();
			path->setTrack(track);
		}
		focusController->attach(path);

		focusController->setInterpolation(Controller::Interpolation_COS);
		focusController->setControllerFinishedListener(new ControllerFinishedFunctor<MyPlanet>(this,&MyPlanet::createLand),true);
		focusController->start();
	}
	mCamera->addController(focusController);
	focusController->start();

	mMode=Mode_FOCUS_PLANET;

	mPlanetDiskFadeOutTime=mScene->getTime();
}

void MyPlanet::createLand(){
	mMode=Mode_CREATE_LAND;
}

int toadletMain(int argc,char **argv){
	MyPlanet app;
	app.create();
	app.start();
	app.destroy();
	return 1;
}
