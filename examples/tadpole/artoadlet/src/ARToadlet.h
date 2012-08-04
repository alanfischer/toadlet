#include <aruco/aruco.h>
#include <toadlet/toadlet.h>
#include <opencv2/core/core.hpp>

class ARToadlet:public Object,public Applet{
public:
	TOADLET_OBJECT(ARToadlet);

	ARToadlet(Application *app);

	void create();	
	void destroy();

	void update(int dt);
	void updateMarkers();
	void render();
	void resized(int width,int height);

	void focusGained(){}
	void focusLost(){}
	void keyPressed(int key);
	void keyReleased(int key){}
	void mousePressed(int x,int y,int button){}
	void mouseMoved(int x,int y){}
	void mouseReleased(int x,int y,int button){}
	void mouseScrolled(int x,int y,int scroll){}

protected:
	TextureFormat::ptr mTextureFormat;
	cv::Mat mTextureData;
	SharedPointer<cv::VideoCapture> mVideoCapture;
	aruco::CameraParameters mCameraParams;
	SharedPointer<aruco::MarkerDetector> mDetector;

	Application *mApp;
	Engine::ptr mEngine;
	Scene::ptr mScene;
	Camera::ptr mBackgroundCamera;
	Texture::ptr mBackgroundTexture;
	Node::ptr mBackground;
	Camera::ptr mCamera;
	Node::ptr mElco;
	Node::ptr mMerv;
	LightComponent::ptr mLight;
};
