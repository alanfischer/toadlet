#ifndef VLCTOADLET_H
#define VLCTOADLET_H

#include <toadlet/toadlet.h>
#ifdef TOADLET_PLATFORM_WIN32
	typedef toadlet::int64 int64_t;
	typedef toadlet::uint32 uint32_t;
#endif
#include <vlc/vlc.h>

class VLCToadlet:public Applet,public ContextListener{
public:
	VLCToadlet(Application *application);
	virtual ~VLCToadlet();

	void create();
	void destroy();
	void resized(int width,int height);
	void render(RenderDevice *device);
	void update(int dt);

	void preContextReset(RenderDevice *device);
	void postContextReset(RenderDevice *device);
	
	void preContextActivate(RenderDevice *device){}
	void postContextActivate(RenderDevice *device);
	void preContextDeactivate(RenderDevice *device);
	void postContextDeactivate(RenderDevice *device){}

	void focusGained(){}
	void focusLost(){}

	void keyPressed(int key){}
	void keyReleased(int){}

	void mousePressed(int x,int y,int button){}
	void mouseMoved(int x,int y){}
	void mouseReleased(int x,int y,int button){}
	void mouseScrolled(int x,int y,int scroll){}

	void joyPressed(int button){}
	void joyMoved(scalar x,scalar y,scalar z,scalar r,scalar u,scalar v){}
	void joyReleased(int button){}

	Application *app;
	Scene::ptr scene;
	CameraNode::ptr cameraNode;
	MeshNode::ptr meshNode;
	int videoPixelFormat;
	Texture::ptr texture;
	PixelBuffer::ptr backBuffer;
	PixelBuffer::ptr conversionBuffer;

	libvlc_instance_t *vlc;
	libvlc_media_t *media;
	libvlc_media_player_t *mediaplayer;
	bool activated;
	bool plugin;
};

#endif
