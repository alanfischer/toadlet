#include "Video.h"

Video::Video(Application *application){
	app=application;
}

Video::~Video(){
}

void Video::create(){
	engine=app->getEngine();

	scene=Scene::ptr(new Scene(engine));

	texture=engine->getTextureManager()->createTexture(Texture::Usage_BIT_STREAM,TextureFormat::Dimension_D2,TextureFormat::Format_RGBA_8,256,256,1,1);

	meshNode=engine->createNodeType(MeshNode::type(),scene);
	meshNode->setMesh(engine->getMeshManager()->createAABoxMesh(AABox(-50,-50,-50,50,50,50),engine->getMaterialManager()->createDiffuseMaterial(texture)));
	scene->getRoot()->attach(meshNode);
	
	cameraNode=engine->createNodeType(CameraNode::type(),scene);
	cameraNode->setLookAt(Vector3(0,-Math::fromInt(150),0),Math::ZERO_VECTOR3,Math::Z_UNIT_VECTOR3);
	cameraNode->setClearColor(Colors::AZURE);
	scene->getRoot()->attach(cameraNode);

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
	device->beginScene();
		cameraNode->render(device);
	device->endScene();
 	device->swap();
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

Applet *createApplet(Application *app){return new Video(app);}
void destroyApplet(Applet *applet){delete applet;}
