#ifndef ACORN_H
#define ACORN_H

#include "Squirrel.h"
#include "Resources.h"

class Acorn:public ParentNode{
public:
	TOADLET_NODE(Acorn,ParentNode);
	
	Acorn():super(),
		mFadeTime(0)
	{}
	
	Node *create(Scene *scene){
		super::create(scene);

		setScope(Squirrel::Scope_ACORN);

		SpriteNode::ptr sprite=mEngine->createNodeType(SpriteNode::type(),mScene);
		sprite->setMaterial(Resources::instance->acorn);
		sprite->setScale(10);
		attach(sprite);

		return this;
	}

	bool fading(){return mFadeTime>0;}

	void fade(){
		if(mFadeTime==0){
			mFadeTime=mScene->getLogicTime();
			activate();
		}
	}

	void logicUpdate(int dt,int scope){
		super::logicUpdate(dt,scope);

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