#ifndef SKY_H
#define SKY_H

#include <toadlet/tadpole.h>
#include "Clouds.h"

class Sky:public Node{
public:
	TOADLET_OBJECT(Sky);

	Sky(Scene *scene,int cloudSize,const Vector4 &skyColor,const Vector4 &fadeColor);

	void updateLightDirection(const Vector3 &lightDir);

	inline LightComponent *getLight(){return mLight;}
	inline SpriteComponent *getSun(){return mSun;}
	inline MeshComponent *getSkyDome(){return mSkyDome;}

protected:
	static tbyte *createGlow(TextureFormat *format);

	LightComponent::ptr mLight;
	Node::ptr mLightNode;
	SpriteComponent::ptr mSun;
	Node::ptr mSunNode;
	Clouds::ptr mClouds;
	MeshComponent::ptr mSkyDome;
};

#endif
