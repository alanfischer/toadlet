#ifndef ANIMATIONCONTROLLER_H
#define ANIMATIONCONTROLLER_H

#include <toadlet/tadpole.h>

class AnimationController:public Controller{
public:
	AnimationController(HopEntity *node,MeshNode *mesh){
		mNode=node;
		mMesh=mesh;
		
		attach(new MeshAnimation(mesh,1));
	}
	
	void logicUpdate(int dt,int scope){
		if(Math::square(mNode->getVelocity().x)+Math::square(mNode->getVelocity().y)<0.05){
			setCycling(Cycling_NONE);
		}
		else{
			setCycling(Cycling_LOOP);
			if(getActive()==false){
				start();
			}
		}
	}

protected:
	HopEntity *mNode;
	MeshNode *mMesh;
};

#endif