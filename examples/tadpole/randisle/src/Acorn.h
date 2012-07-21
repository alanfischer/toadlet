#ifndef ACORN_H
#define ACORN_H

#include "RandIsle.h"
#include "Resources.h"

class Acorn:public Node{
public:
	Acorn(Scene *scene):Node(scene),
		mFadeTime(0)
	{
		setScope(RandIsle::Scope_ACORN);

		SpriteComponent::ptr sprite=new SpriteComponent(mEngine);
		sprite->setMaterial(Resources::instance->acorn);
	//	sprite->setScale(10);
		attach(sprite);
	}

	bool fading(){return mFadeTime>0;}

	void fade(){
		if(mFadeTime==0){
			mFadeTime=mScene->getLogicTime();
			activate();
		}
	}

	void logicUpdate(int dt,int scope){
		Node::logicUpdate(dt,scope);

		if(mFadeTime>0){
			scalar size=Math::ONE-Math::fromMilli(mScene->getLogicTime()-mFadeTime)*4;
			if(size<=0){
				destroy();
			}
			else{
				setScale(size);
			}
		}
	}

protected:
	int mFadeTime;
};

#endif
