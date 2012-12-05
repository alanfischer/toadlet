#include "Video.h"

extern "C" VideoDevice *new_MFVideoDevice();

Video::Video(Application *application){
	app=application;
}

Video::~Video(){
}

void Video::create(){
	engine=app->getEngine();

	scene=Scene::ptr(new Scene(engine));

	VideoDevice *device=new_MFVideoDevice();
	device->create();
	device->setListener(this);

	TextureFormat::ptr format(new TextureFormat(TextureFormat::Dimension_D2,TextureFormat::Format_RGBA_8,640,480,1,1));
	texture=engine->getTextureManager()->createTexture(Texture::Usage_BIT_STREAM,format);

	meshNode=engine->createNodeType(MeshNode::type(),scene);
	meshNode->setMesh(engine->getMeshManager()->createAABoxMesh(AABox(-50,-50,-50,50,50,50),engine->getMaterialManager()->createDiffuseMaterial(texture)));
	scene->getRoot()->attach(meshNode);
	
	cameraNode=engine->createNodeType(CameraNode::type(),scene);
	cameraNode->setLookAt(Vector3(0,-Math::fromInt(150),0),Math::ZERO_VECTOR3,Math::Z_UNIT_VECTOR3);
	cameraNode->setClearColor(Colors::AZURE);
	scene->getRoot()->attach(cameraNode);

	device->start();

//	audioNode=engine->createNodeType(AudioNode::type(),scene);
//	scene->getRoot()->attach(audioNode);
}

void Video::destroy(){
	if(controller!=NULL){
		controller->destroy();
		controller=NULL;
	}

	scene->destroy();
}

void Video::resized(int width,int height){
	if(cameraNode!=NULL && width>0 && height>0){
		if(width>=height){
			cameraNode->setProjectionFovY(Math::degToRad(Math::fromInt(45)),Math::div(Math::fromInt(width),Math::fromInt(height)),Math::fromInt(10),Math::fromInt(200));
		}
		else{
			cameraNode->setProjectionFovX(Math::degToRad(Math::fromInt(45)),Math::div(Math::fromInt(height),Math::fromInt(width)),Math::fromInt(10),Math::fromInt(200));
		}
		cameraNode->setViewport(Viewport(0,0,width,height));
	}
}

void Video::render(RenderDevice *device){
	mutex.lock();
	device->beginScene();
		cameraNode->render(device);
	device->endScene();
 	device->swap();
	mutex.unlock();
}

void Video::update(int dt){
	scene->update(dt);

	if(controller!=NULL){
		controller->update(dt);
	}
}

void Video::keyPressed(int key){
	if(controller!=NULL){
		controller->destroy();
		controller=NULL;
	}

	Stream::ptr stream=engine->openStream("video.avi");
	if(stream!=NULL){
		TOADLET_TRY
			controller=engine->getTextureManager()->getVideoHandler()->open(stream);
			controller->setTexture(texture);
			controller->start();
		TOADLET_CATCH(const Exception &){controller=NULL;}
	}
}

void Video::frameReceived(TextureFormat::ptr format,tbyte *data){
	mutex.lock();
	engine->getTextureManager()->textureLoad(texture,format,data);
	mutex.unlock();
}

Applet *createApplet(Application *app){return new Video(app);}
