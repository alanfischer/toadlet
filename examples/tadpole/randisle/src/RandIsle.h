#ifndef RANDISLE_H
#define RANDISLE_H

#include <toadlet/toadlet.h>
#include "PathSystem.h"
#include "PathClimber.h"
#include "SmoothFollower.h"
#include "Sky.h"
#include "Tree.h"

class HUD;

class RandIsle:public Object,public Applet,public TerrainNodeDataSource,public UpdateListener,public TerrainNodeListener,public InputDeviceListener,public PathClimberListener{
public:
	TOADLET_OBJECT(RandIsle);

	enum{
		Scope_BIT_TREE=			1<<1,
		Scope_BIT_ACORN=		1<<2,
		Scope_BIT_HUD=			1<<3,
		Scope_BIT_WATER=		1<<4,
		Scope_BIT_MAIN_CAMERA=	1<<5,

		Scope_TREE=				Scope_BIT_TREE,
		Scope_ACORN=			Scope_BIT_ACORN,
		Scope_HUD=				Scope_BIT_HUD,
	};

	RandIsle(Application *app,String path);
	virtual ~RandIsle();

	void create();
	void destroy();
	void resized(int width,int height){}
	void render();
	void update(int dt);
	void preLogicUpdate(int dt){}
	void logicUpdate(int dt);
	void postLogicUpdate(int dt){}
	void preFrameUpdate(int dt){}
	void frameUpdate(int dt);
	void postFrameUpdate(int dt){}
	void keyPressed(int key);
	void keyReleased(int key);
	void focusGained(){}
	void focusLost(){}
	void mousePressed(int x,int y,int button);
	void mouseReleased(int x,int y,int button);
	void mouseMoved(int x,int y);
	void mouseScrolled(int x,int y,int scroll){}
	void inputDetected(const InputData &data);

	void playerJump(Node *player);
	void playerMove(Node *player,scalar dr,scalar ds);

	float findPathSequence(egg::Collection<int> &sequence,PathClimber *climber,PathSystem::Path *path,int direction,scalar time);
	float findPathSequence(egg::Collection<int> &sequence,PathClimber *climber,const Vector3 &forward,PathSystem::Path *previous,PathSystem::Path *path,int direction,scalar time,bool first);
	void updateDanger(int dt);
	void updateClimber(PathClimber *climber,int dt);
	void updatePredictedPath(PathClimber *climber,int dt);

	void wiggleLeaves(Tree *tree,const Sphere &bound);

	void pathMounted(PathClimber *climber){}
	void pathDismounted(PathClimber *climber){}
	int atJunction(PathClimber *climber,PathSystem::Path *current,PathSystem::Path *next);

	bool updatePopulatePatches();
	void terrainUpdated(int oldX,int oldY,int newX,int newY){}
	void terrainPatchCreated(int x,int y,Bound *bound);
	void terrainPatchDestroyed(int x,int y,Bound *bound);

	int getPatchSize(){return mPatchSize;}
	const Vector3 &getPatchScale(){return mPatchScale;}
	bool getPatchHeightData(scalar *data,int px,int py);
	bool getPatchLayerData(tbyte *data,int px,int py);
	scalar terrainValue(float tx,float ty);
	scalar pathValue(float ty);

	RenderPath::ptr chooseBestPath(Material *material);

protected:
	class PopulatePatch{
	public:
		PopulatePatch():px(0),py(0),dx(0),dy(0),y(-1){}

		PopulatePatch(int px,int py,int dx,int dy,Bound *bound):y(-1){
			this->px=px,this->py=py;
			this->dx=dx,this->dy=dy;
			this->bound=bound;
		}

		int px,py;
		int dx,dy;
		Bound::ptr bound;
		int y;
	};

	Application *mApp;
	String mPath;
	Engine::ptr mEngine;
	Scene::ptr mScene;
	TerrainNode::ptr mTerrain;
	SmoothFollower::ptr mFollower;
	Node::ptr mFollowNode;
	Camera::ptr mCamera,mReflectCamera,mRefractCamera;
	Node::ptr mPlayer;
	int mMouseButtons;
	scalar mXJoy,mYJoy;
	egg::Collection<int> mPathSequence;
	egg::Collection<PopulatePatch> mPopulatePatches;

	VertexData::ptr mPredictedVertexData;
	IndexData::ptr mPredictedIndexData;
	Material::ptr mPredictedMaterial;

	BoundingVolumeSensor::ptr mSensor;
	int mPatchSize;
	Vector3 mPatchScale;
	Noise mPatchNoise;
	Sky::ptr mSky;
	AudioComponent::ptr mRustleSound;

//	IntrusivePointer<HUD> mHUD;
};

#endif
