#include "VLCToadlet.h"

#pragma comment(lib,"c:\\program files\\videolan\\vlc\\sdk\\lib\\libvlc.lib")

/*
	This example requires that you copy libvlc.dll, libvlccore.dll, lua, and the plugins directory to the vlctoadlet executable directory
*/

class Spinner:public NodeListener{
public:
	Spinner(){
	}

	void nodeDestroyed(Node *node){}

	void logicUpdate(Node *node,int dt){}

	void frameUpdate(Node *node,int dt){
		Vector3 axis(1,1,1);
		Math::normalize(axis);
		node->setRotate(axis,Math::fromMilli(node->getScene()->getTime()));
	}
};

static void *lock(void *data, void **p_pixels){
	VLCToadlet *self=(VLCToadlet*)data;
	if(self->activated==false){
		self->getRenderer()->activateAdditionalContext();
		self->activated=true;
	}

	*p_pixels=self->backBuffer->lock(Buffer::Access_BIT_WRITE);

	return NULL;
}

static void unlock(void *data, void *id, void *const *p_pixels){
	VLCToadlet *self=(VLCToadlet*)data;

	self->backBuffer->unlock();

	self->getRenderer()->copyPixelBuffer(self->buffer,self->backBuffer);
}

static void display(void *data, void *id){}

VLCToadlet::VLCToadlet():Application(){
	activated=false;
}

VLCToadlet::~VLCToadlet(){
}

void VLCToadlet::create(){
	Application::create();

	String file="/home/siralanf/bigstuff_backup/ashleigh/Ashleigh mac/Movies/mvi_0811.avi";

	scene=Scene::ptr(new Scene(mEngine));

	int format=mRenderer->getClosestTextureFormat(Texture::Format_RGBA_8);
	Texture::ptr texture=mEngine->getTextureManager()->createTexture(Texture::Usage_BIT_RENDERTARGET,Texture::Dimension_D2,format,128,128,1,1);
	buffer=texture->getMipPixelBuffer(0,0);
	backBuffer=PixelBuffer::ptr(mRenderer->createPixelBuffer());
	backBuffer->create(Buffer::Usage_BIT_DYNAMIC,Buffer::Access_BIT_WRITE,texture->getFormat(),texture->getWidth(),texture->getHeight(),1);

	Mesh::ptr mesh=mEngine->getMeshManager()->createBox(AABox(-10,-10,-10,10,10,10));
	mesh->subMeshes[0]->material->setTextureStage(0,mEngine->getMaterialManager()->createTextureStage(texture));

	meshNode=getEngine()->createNodeType(MeshNode::type(),scene);
	meshNode->setMesh(mesh);
	meshNode->addNodeListener(NodeListener::ptr(new Spinner()));
	scene->getRoot()->attach(meshNode);

	cameraNode=getEngine()->createNodeType(CameraNode::type(),scene);
	cameraNode->setLookAt(Vector3(0,-Math::fromInt(150),0),Math::ZERO_VECTOR3,Math::Z_UNIT_VECTOR3);
	cameraNode->setClearColor(Colors::BLUE);
	scene->getRoot()->attach(cameraNode);

	vlc=libvlc_new(0,NULL);
	media=libvlc_media_new_path(vlc,file);
	mediaplayer=libvlc_media_player_new_from_media(media);
	libvlc_media_release(media);

	libvlc_video_set_callbacks(mediaplayer, lock, unlock, display, this);
	libvlc_video_set_format(mediaplayer,"RV32",128,128,128*4);
	libvlc_media_player_play(mediaplayer);
}

void VLCToadlet::destroy(){
	libvlc_media_player_stop(mediaplayer);
	libvlc_media_player_release(mediaplayer);
	libvlc_release(vlc);

	Application::destroy();
}

void VLCToadlet::resized(int width,int height){
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

void VLCToadlet::render(Renderer *renderer){
	renderer->beginScene();
		scene->render(renderer,cameraNode,NULL);
	renderer->endScene();
	renderer->swap();
}

void VLCToadlet::update(int dt){
	scene->update(dt);
}

#if !defined(TOADLET_PLATFORM_OSX)
#if defined(TOADLET_PLATFORM_WINCE)
#include <windows.h>
int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPTSTR lpCmdLine,int nCmdShow){
	VLCToadlet app;
	app.setFullscreen(true);
#else
int main(int argc,char **argv){
	VLCToadlet app;
#endif
	app.create();
	app.start();
	app.destroy();
	return 0;
}
#endif
