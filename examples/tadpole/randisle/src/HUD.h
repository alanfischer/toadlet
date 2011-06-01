#ifndef HUD_H
#define HUD_H

#include "RandIsle.h"

class HUD:public CameraNode{
public:
	TOADLET_NODE(HUD,CameraNode);

	HUD();
	Node *create(Scene *scene);

	void setTarget(PathClimber *player,CameraNode *camera);

	void frameUpdate(int dt,int scope);
	void logicUpdate(int dt,int scope);

	void setProjectionOrtho(scalar leftDist,scalar rightDist,scalar bottomDist,scalar topDist,scalar nearDist,scalar farDist);

	void render(Renderer *renderer){super::render(renderer,this);}

	void updateAcornCount(int count);

protected:
	IntrusivePointer<PathClimber> mPlayer;
	CameraNode::ptr mCamera;
	AudioNode::ptr mDogSound;
	AudioNode::ptr mSharkSound;
	egg::Random mRandom;

	SpriteNode::ptr mFadeSprite;
	SpriteNode::ptr mCompassSprite;
	SpriteNode::ptr mAcornSprite;
	LabelNode::ptr mAcornLabel;
	int mAcornCount;
	int mChompTime;
	int mNextBarkTime;
	scalar mWaterAmount;

	LabelNode::ptr mSystemLabel;
};

#endif
