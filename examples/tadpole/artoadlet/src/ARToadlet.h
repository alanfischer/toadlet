#include <toadlet/toadlet.h>
#include <AR/config.h>
#include <AR/video.h>
#include <AR/param.h>			// arParamDisp()
#include <AR/ar.h>

using namespace toadlet::egg;
using namespace toadlet::pad;
using namespace toadlet::peeper;
using namespace toadlet::tadpole::animation;
using namespace toadlet::tadpole::node;

static double PATTERN_WIDTH=80;
static double PATTERN_CENTER[2]={0,0};
static int THRESHOLD=150;

class ARToadlet:public Applet{
public:
	ARToadlet(Application *app);

	bool setupARCamera(const String &cameraParamFile,const String &config,ARParam *cameraParams);

	void create();	
	void destroy();
	
	void update(int dt);
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

	static int getPixelFormatFromARPixelFormat(int format);

	static void setMatrix4x4FromARProjection(Matrix4x4 &r,ARParam *cparam,const double minDistance,const double maxDistance);
	static void setMatrix4x4FromARMatrix(Matrix4x4 &r,const double para[3][4]);
	
protected:
	ARParam mARTCparam;
	Collection<int> mPatternIDs;
	TextureFormat::ptr mTextureFormat;
	ARUint8 *mLastImage;

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
};
