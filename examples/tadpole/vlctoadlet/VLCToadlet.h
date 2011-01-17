#ifndef VLCTOADLET_H
#define VLCTOADLET_H

#include <toadlet/toadlet.h>
#ifdef TOADLET_PLATFORM_WIN32
	typedef toadlet::int64 int64_t;
	typedef toadlet::uint32 uint32_t;
#endif
#include <vlc/vlc.h>

class VLCToadlet:public Application,public ContextListener{
public:
	VLCToadlet();
	virtual ~VLCToadlet();

	void create();
	void destroy();
	void resized(int width,int height);
	void render(Renderer *renderer);
	void update(int dt);
	void keyPressed(int key);

	void preContextReset(Renderer *renderer);
	void postContextReset(Renderer *renderer);
	
	void preContextActivate(Renderer *renderer){}
	void postContextActivate(Renderer *renderer);
	void preContextDeactivate(Renderer *renderer);
	void postContextDeactivate(Renderer *renderer){}

	Scene::ptr scene;
	CameraNode::ptr cameraNode;
	MeshNode::ptr meshNode;
	PixelBuffer::ptr backBuffer;
	PixelBuffer::ptr buffer;

	libvlc_instance_t *vlc;
	libvlc_media_t *media;
	libvlc_media_player_t *mediaplayer;
	bool activated;
	bool plugin;
};

#endif
