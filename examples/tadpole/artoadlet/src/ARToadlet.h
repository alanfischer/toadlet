#include <toadlet/toadlet.h>
#include <ARToolKitPlus/TrackerSingleMarker.h>

using namespace ARToolKitPlus;

class ARToadlet:public Applet,public VideoDeviceListener{
public:
	ARToadlet(Application *app);

	void create();	
	void destroy();
	
	void frameReceived(TextureFormat::ptr format,tbyte *data);

	void update(int dt);
	void updateMarkers();
	void render(RenderDevice *device);
	void resized(int width,int height);

	void focusGained(){}
	void focusLost(){}
	void keyPressed(int key){}
	void keyReleased(int key){}
	void mousePressed(int x,int y,int button){}
	void mouseMoved(int x,int y){}
	void mouseReleased(int x,int y,int button){}
	void mouseScrolled(int x,int y,int scroll){}

	static PIXEL_FORMAT getARPixelFormatFromPixelFormat(int format);
	
protected:
	TextureFormat::ptr mTextureFormat;
	uint8 *mTextureData;
	VideoDevice *mVideoDevice;

	SharedPointer<TrackerSingleMarker> mTracker;

	Application *mApp;
	Engine *mEngine;
	Scene::ptr mScene;
	CameraNode::ptr mOrthoCamera;
	Texture::ptr mBackgroundTexture;
	MeshNode::ptr mBackground;
	CameraNode::ptr mCamera;
	ParentNode::ptr mElco;
	ParentNode::ptr mMerv;
	LightNode::ptr mLight;

	Mutex mMutex;
};
