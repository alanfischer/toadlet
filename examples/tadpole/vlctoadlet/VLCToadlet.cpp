#include "VLCToadlet.h"

/*
	This example requires that you copy libvlc.dll, libvlccore.dll, lua, and the plugins directory to the vlctoadlet executable directory
*/

class Spinner:public NodeListener{
public:
	Spinner(){}

	void nodeDestroyed(Node *node){}

	void transformUpdated(Node *node,int tu){}

	void logicUpdated(Node *node,int dt){}

	void frameUpdated(Node *node,int dt){
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

	PixelBuffer *backBuffer=self->backBuffer;
	if(self->backBuffer->getPixelFormat()!=self->videoPixelFormat){
		int width=self->backBuffer->getWidth(),height=self->backBuffer->getHeight();
		tbyte *conversionData=self->conversionBuffer->lock(Buffer::Access_BIT_WRITE);
		int srcPitch=ImageFormatConversion::getRowPitch(self->backBuffer->getPixelFormat(),width);
		int dstPitch=ImageFormatConversion::getRowPitch(self->videoPixelFormat,width);
		ImageFormatConversion::convert((tbyte*)*p_pixels,self->backBuffer->getPixelFormat(),srcPitch,srcPitch*height,conversionData,self->videoPixelFormat,dstPitch,dstPitch*height,width,height,1);
		self->conversionBuffer->unlock();
		backBuffer=self->conversionBuffer;
	}
	self->backBuffer->unlock();
	self->getRenderer()->copyPixelBuffer(self->texture->getMipPixelBuffer(0,0),backBuffer);
}

static void display(void *data, void *id){}

VLCToadlet::VLCToadlet():Application(){
	plugin=false;
	activated=false;
}

VLCToadlet::~VLCToadlet(){
	scene=NULL;
}

void VLCToadlet::create(){
#if defined(TOADLET_PLATFORM_WIN32)
	setBackable(true);
#endif

	Application::create();

	String url="../../data/video.3gp";

	scene=Scene::ptr(new Scene(mEngine));

	videoPixelFormat=Texture::Format_BGRA_8;
	int format=mRenderer->getCloseTextureFormat(videoPixelFormat,Texture::Usage_BIT_STREAM);
	texture=mEngine->getTextureManager()->createTexture(Texture::Usage_BIT_STREAM,Texture::Dimension_D2,format,128,128,1,1);
	backBuffer=mEngine->getBufferManager()->createPixelBuffer(Buffer::Usage_BIT_STAGING,Buffer::Access_BIT_WRITE,texture->getFormat(),texture->getWidth(),texture->getHeight(),1);
	conversionBuffer=mEngine->getBufferManager()->createPixelBuffer(Buffer::Usage_BIT_STAGING,Buffer::Access_BIT_WRITE,texture->getFormat(),texture->getWidth(),texture->getHeight(),1);

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
	media=libvlc_media_new_path(vlc,url);
	mediaplayer=libvlc_media_player_new_from_media(media);
	libvlc_media_release(media);

	libvlc_video_set_callbacks(mediaplayer, lock, unlock, display, this);
	libvlc_video_set_format(mediaplayer,"RV32",128,128,128*4);
	int result=libvlc_media_player_play(mediaplayer);
	if(result<0){
		Error::unknown("unable to start media");
		return;
	}

	mEngine->setContextListener(this);
}

void VLCToadlet::destroy(){
	mEngine->setContextListener(NULL);

	libvlc_media_player_stop(mediaplayer);
	libvlc_media_player_release(mediaplayer);
	libvlc_release(vlc);

	scene->destroy();
	texture->destroy();
	backBuffer->destroy();

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

void VLCToadlet::keyPressed(int key){
#if defined(TOADLET_PLATFORM_WIN32)
	if(key==' '){
		if(plugin){
			changeRendererPlugin("gl");
		}
		else{
			changeRendererPlugin("d3d10");
		}
		plugin=!plugin;
	}
#endif
}

/// @todo: I should just be able to use player_pause here, but that doesnt seem to stop the thread like I imagine.  I need to look into this vlc bug
void VLCToadlet::preContextReset(Renderer *renderer){
	libvlc_media_player_stop(mediaplayer);
}

void VLCToadlet::postContextReset(Renderer *renderer){
	libvlc_media_player_play(mediaplayer);
}

void VLCToadlet::postContextActivate(Renderer *renderer){
	activated=false;

	libvlc_media_player_play(mediaplayer);
}

void VLCToadlet::preContextDeactivate(Renderer *renderer){
	libvlc_media_player_stop(mediaplayer);
}

int toadletMain(int argc,char **argv){
	VLCToadlet app;
	app.create();
	app.start();
	app.destroy();
	return 0;
}
