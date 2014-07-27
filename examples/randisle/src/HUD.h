#ifndef HUD_H
#define HUD_H

#include <toadlet/toadlet.h>
#include "Resources.h"

namespace randisle{

using namespace toadlet;

class HUD:public Node{
public:
	TOADLET_OBJECT(HUD);

	HUD(Scene *scene,Node *player,Camera *camera,Resources *resources);

protected:
	AudioComponent::ptr mDogSound;

	Node::ptr mCompass;
	Node::ptr mChomp;
	Node::ptr mAcorn;
	Node::ptr mFPS;
};

}

#endif
