#ifndef RANDISLE_H
#define RANDISLE_H

#include <toadlet/toadlet.h>
#include <toadlet/tadpole/plugins/hop/HopScene.h>
#include <toadlet/tadpole/plugins/hop/HopEntity.h>
#include "PathSystem.h"
#include "PathClimberListener.h"
#include "SmoothFollower.h"
#include "GroundProjector.h"
#include "Sky.h"
#include "Tree.h"

class HUD;

class RandIsle:public Application,public TerrainNodeDataSource,public UpdateListener,public TerrainNodeListener,public PathClimberListener,public RenderPathChooser{
public:
	enum{
		Scope_BIT_TREE=			1<<1,
		Scope_BIT_ACORN=		1<<2,
		Scope_BIT_HUD=			1<<3,
		Scope_BIT_ABOVEWATER=	1<<4,

		Scope_TREE=				Scope_BIT_TREE,
		Scope_ACORN=			Scope_BIT_ACORN,
		Scope_HUD=				Scope_BIT_HUD,
	};

	RandIsle();
	virtual ~RandIsle();

	void create(const String &directory);
	void destroy();
	void resized(int width,int height);
	void render(RenderDevice *renderDevice);
	void update(int dt);
	void preLogicUpdate(int dt){}
	void logicUpdate(int dt);
	void postLogicUpdate(int dt){}
	void preFrameUpdate(int dt){}
	void frameUpdate(int dt);
	void postFrameUpdate(int dt){}
	void keyPressed(int key);
	void keyReleased(int key);
	void mousePressed(int x,int y,int button);
	void mouseReleased(int x,int y,int button);
	void mouseMoved(int x,int y);
	void joyPressed(int button);
	void joyMoved(scalar x,scalar y,scalar z,scalar r,scalar u,scalar v);
	void joyReleased(int button);

	void playerJump();
	void playerMove(scalar dr,scalar ds);

	float findPathSequence(egg::Collection<int> &sequence,PathClimber *player,PathSystem::Path *path,int direction,scalar time);
	float findPathSequence(egg::Collection<int> &sequence,PathClimber *player,const Vector3 &forward,PathSystem::Path *previous,PathSystem::Path *path,int direction,scalar time,bool first);
	void updateDanger(int dt);
	void updatePredictedPath();

	void wiggleLeaves(Tree *tree,const Sphere &bound);

	void pathMounted(PathClimber *climber);
	void pathDismounted(PathClimber *climber);
	int atJunction(PathClimber *climber,PathSystem::Path *current,PathSystem::Path *next);

	bool updatePopulatePatches();
	void terrainUpdated(int oldX,int oldY,int newX,int newY){}
	void terrainPatchCreated(int x,int y,const Bound &bound);
	void terrainPatchDestroyed(int x,int y,const Bound &bound);

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
		PopulatePatch():px(0),py(0),dx(0),dy(0),y(-1)
		{}

		PopulatePatch(int px,int py,int dx,int dy,const Bound &bound):y(-1){
			this->px=px,this->py=py;
			this->dx=dx,this->dy=dy;
			this->bound.set(bound);
		}

		int px,py;
		int dx,dy;
		Bound bound;
		int y;
	};

	HopScene::ptr mScene;
	TerrainNode::ptr mTerrain;
	SmoothFollower::ptr mFollower;
	ParentNode::ptr mFollowNode;
	CameraNode::ptr mCamera;
	IntrusivePointer<PathClimber> mPlayer;
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
	AudioNode::ptr mRustleSound;

	IntrusivePointer<HUD> mHUD;
};

#endif
