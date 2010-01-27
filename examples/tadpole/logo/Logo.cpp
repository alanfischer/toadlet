#include "Logo.h"
#include "lt_xmsh.h"
#include "lt_mmsh.h"

// To keep this example as simple as possible, it does not require any other data files, instead getting its data from lt_xmsh
Logo::Logo():Application(){
}

Logo::~Logo(){
}

void Logo::create(){
	Application::create();

	getEngine()->setScene(mEngine->createNodeType(SceneNode::type()));

	MemoryStream::ptr in(new MemoryStream(lt_mmsh::data,lt_mmsh::length,lt_mmsh::length,false));
	Mesh::ptr mesh=shared_static_cast<Mesh>(getEngine()->getMeshManager()->getHandler("mmsh")->load(in,NULL));
	//MemoryInputStream::ptr in(new MemoryInputStream(lt_xmsh::data,lt_xmsh::length));
	//Mesh::ptr mesh=shared_static_cast<Mesh>(getEngine()->getMeshManager()->getHandler("xmsh")->load(in,NULL));

	meshNode=getEngine()->createNodeType(MeshNode::type());
	meshNode->setMesh(mesh);
	meshNode->getAnimationController()->start();
	meshNode->getAnimationController()->setCycling(MeshNode::MeshAnimationController::Cycling_REFLECT);
	getEngine()->getScene()->getRootNode()->attach(meshNode);

	cameraNode=getEngine()->createNodeType(CameraNode::type());
	cameraNode->setLookDir(Vector3(Math::fromInt(25),-Math::fromInt(100),0),Math::Y_UNIT_VECTOR3,Math::Z_UNIT_VECTOR3);
	cameraNode->setClearColor(Colors::BLUE);
	getEngine()->getScene()->getRootNode()->attach(cameraNode);
}

void Logo::resized(int width,int height){
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

void Logo::render(Renderer *renderer){
	renderer->beginScene();
		getEngine()->getScene()->render(renderer,cameraNode,NULL);
	renderer->endScene();
	renderer->swap();
}

void Logo::update(int dt){
	getEngine()->getScene()->update(dt);
}

#if !defined(TOADLET_PLATFORM_OSX)
#if defined(TOADLET_PLATFORM_WINCE)
#include <windows.h>
int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPTSTR lpCmdLine,int nCmdShow){
	Logo app;
	app.setFullscreen(true);
#else
int main(int argc,char **argv){
	Logo app;
#endif
	app.create();
	app.start();
	app.destroy();
	return 0;
}
#endif
