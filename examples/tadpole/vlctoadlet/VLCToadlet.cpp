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
Logger::alert("LOCKED");
return NULL;
	VLCToadlet *self=(VLCToadlet*)data;
	if(self->activated==false){
		self->app->getRenderDevice()->activateAdditionalContext();
		self->activated=true;
	}

//	*p_pixels=self->backBuffer->lock(Buffer::Access_BIT_WRITE);

Logger::alert("LOCK DONE");
	return NULL;
}

static void unlock(void *data, void *id, void *const *p_pixels){
Logger::alert("UNLOCK");
return;
	VLCToadlet *self=(VLCToadlet*)data;

	PixelBuffer *backBuffer=self->backBuffer;
	int pixelFormat=self->backBuffer->getTextureFormat()->pixelFormat;
	if(pixelFormat!=self->videoPixelFormat){
		int width=self->backBuffer->getTextureFormat()->width,height=self->backBuffer->getTextureFormat()->height;
		tbyte *conversionData=self->conversionBuffer->lock(Buffer::Access_BIT_WRITE);
		int srcPitch=ImageFormatConversion::getRowPitch(pixelFormat,width);
		int dstPitch=ImageFormatConversion::getRowPitch(self->videoPixelFormat,width);
		ImageFormatConversion::convert((tbyte*)*p_pixels,pixelFormat,srcPitch,srcPitch*height,conversionData,self->videoPixelFormat,dstPitch,dstPitch*height,width,height,1);
		self->conversionBuffer->unlock();
		backBuffer=self->conversionBuffer;
	}
//	self->backBuffer->unlock();
//	self->getRenderDevice()->copyPixelBuffer(self->texture->getMipPixelBuffer(0,0),backBuffer);
Logger::alert("UNLOCK DONE");
}

static void display(void *data, void *id){}

VLCToadlet::VLCToadlet(Application *application){
	app=application;
	plugin=false;
	activated=false;
}

VLCToadlet::~VLCToadlet(){
	scene=NULL;
}

void VLCToadlet::create(){
	String url="../../data/video.3gp";

	Engine *engine=app->getEngine();

	scene=Scene::ptr(new Scene(engine));

	videoPixelFormat=TextureFormat::Format_BGRA_8;
	int format=app->getRenderDevice()->getClosePixelFormat(videoPixelFormat,Texture::Usage_BIT_STREAM);
	texture=engine->getTextureManager()->createTexture(Texture::Usage_BIT_STREAM,TextureFormat::Dimension_D2,format,128,128,1,1);
	backBuffer=engine->getBufferManager()->createPixelBuffer(Buffer::Usage_BIT_STAGING,Buffer::Access_BIT_WRITE,TextureFormat::Dimension_D2,format,128,128);
	conversionBuffer=engine->getBufferManager()->createPixelBuffer(Buffer::Usage_BIT_STAGING,Buffer::Access_BIT_WRITE,texture->getFormat()->pixelFormat,128,128,1);

	Mesh::ptr mesh=engine->getMeshManager()->createAABoxMesh(AABox(-10,-10,-10,10,10,10),engine->getMaterialManager()->createDiffuseMaterial(texture));
	meshNode=engine->createNodeType(MeshNode::type(),scene);
	meshNode->setMesh(mesh);
	meshNode->addNodeListener(NodeListener::ptr(new Spinner()));
	scene->getRoot()->attach(meshNode);

	cameraNode=engine->createNodeType(CameraNode::type(),scene);
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

	app->getEngine()->addContextListener(this);
}

void VLCToadlet::destroy(){
	app->getEngine()->removeContextListener(this);

	libvlc_media_player_stop(mediaplayer);
	libvlc_media_player_release(mediaplayer);
	libvlc_release(vlc);

	scene->destroy();
	texture->destroy();
	backBuffer->destroy();
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

void VLCToadlet::render(RenderDevice *device){
	device->beginScene();
		cameraNode->render(device);
	device->endScene();
	device->swap();
}

void VLCToadlet::update(int dt){
	scene->update(dt);
}

/// @todo: I should just be able to use player_pause here, but that doesnt seem to stop the thread like I imagine.  I need to look into this vlc bug
void VLCToadlet::preContextReset(RenderDevice *device){
	libvlc_media_player_stop(mediaplayer);
}

void VLCToadlet::postContextReset(RenderDevice *device){
	libvlc_media_player_play(mediaplayer);
}

void VLCToadlet::postContextActivate(RenderDevice *device){
	activated=false;

	libvlc_media_player_play(mediaplayer);
}

void VLCToadlet::preContextDeactivate(RenderDevice *device){
	libvlc_media_player_stop(mediaplayer);
}

Applet *createApplet(Application *app){return new VLCToadlet(app);}
void destroyApplet(Applet *applet){delete applet;}
