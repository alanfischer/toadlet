#ifndef HUD_H
#define HUD_H

#include <toadlet/toadlet.h>

class HUD:public Node{
public:
	TOADLET_OBJECT(HUD);

	HUD(Scene *scene,Node *player,Camera *camera);

protected:
	AudioComponent::ptr mDogSound;

	Node::ptr mCompass;
	Node::ptr mChomp;
	Node::ptr mAcorn;
	Node::ptr mFPS;
};

#endif
