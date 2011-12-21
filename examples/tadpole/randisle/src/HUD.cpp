#include "HUD.h"
#include "PathClimber.h"
#include "Resources.h"

TOADLET_NODE_IMPLEMENT(HUD,"HUD");

HUD::HUD():super(),
	mAcornCount(0),
	mChompTime(0),
	mNextBarkTime(0),
	mWaterAmount(0)
{
}

Node *HUD::create(Scene *scene){
	super::create(scene);

	mDogSound=mEngine->createNodeType(AudioNode::type(),mScene);
	mDogSound->setAudioBuffer(Resources::instance->dog);
	mScene->getRoot()->attach(mDogSound);
	mSharkSound=mEngine->createNodeType(AudioNode::type(),mScene);
	mSharkSound->setAudioBuffer(Resources::instance->shark);
	mScene->getRoot()->attach(mSharkSound);

	mFadeSprite=mEngine->createNodeType(SpriteNode::type(),mScene);
	mFadeSprite->setMaterial(Resources::instance->hudFade);
	mFadeSprite->setScale(Vector3(2,2,2));
	attach(mFadeSprite);

	mCompassSprite=mEngine->createNodeType(SpriteNode::type(),mScene);
	mCompassSprite->setMaterial(Resources::instance->hudCompass);
	mCompassSprite->setTranslate(0.75,0.75,0);
	mCompassSprite->setScale(.25,.25,.25);
	attach(mCompassSprite);

	mAcornSprite=mEngine->createNodeType(SpriteNode::type(),mScene);
	mAcornSprite->setMaterial(Resources::instance->hudAcorn);
	mAcornSprite->setTranslate(-0.75,0.75,0);
	mAcornSprite->setScale(0.25,0.20,0.25);
	attach(mAcornSprite);

	mAcornLabel=mEngine->createNodeType(LabelNode::type(),mScene);
	mAcornLabel->setFont(Resources::instance->hudWooden);
	mAcornLabel->setAlignment(Font::Alignment_BIT_VCENTER|Font::Alignment_BIT_LEFT);
	mAcornLabel->setTranslate(-0.60,0.80,0);
	mAcornLabel->setScale(0.3,0.3,0.3);
	mAcornLabel->getSharedRenderState()->setMaterialState(MaterialState(Colors::BROWN));
	attach(mAcornLabel);
	updateAcornCount(0);

	mSystemLabel=mEngine->createNodeType(LabelNode::type(),mScene);
	mSystemLabel->setFont(Resources::instance->hudSystem);
	mSystemLabel->setTranslate(-0.25,-0.80,0);
	mSystemLabel->setScale(0.1,0.1,0.1);
	attach(mSystemLabel);

	return this;
}

void HUD::frameUpdate(int dt,int scope){
	if(mPlayer!=NULL){
		Vector3 right,forward,up;
		Math::setAxesFromQuaternion(mCamera->getWorldRotate(),forward,right,up);
		forward.z=0;
		Math::normalizeCarefully(forward,0);
		scalar angle=-Math::atan2(forward.y,forward.x);
		mCompassSprite->setRotate(Math::Z_UNIT_VECTOR3,angle);

		MaterialState materialState;
		if(mFadeSprite->getMaterial()!=NULL){
			mFadeSprite->getMaterial()->getPass()->getMaterialState(materialState);
		}
		Vector4 dangerColor=materialState.ambient;
		dangerColor.w=mPlayer->getDanger();

		/// @todo:
/*
		if(mChompTime>0){
			scalar chompamount=Math::fromMilli(mScene->getTime()-mChompTime);
			scalar amount=powf(5.0,Math::sin(Math::mul(chompamount,Math::PI)));

			TextureState textureState;
			mFadeSprite->getMaterial()->getPass()->getTextureState(0,textureState);
			textureState.calculation=TextureState::CalculationType_NORMAL;
			Matrix4x4 matrix,temp;
			Math::setMatrix4x4FromTranslate(matrix,0,Math::HALF,0);
			Math::setMatrix4x4FromScale(temp,Math::ONE,amount,Math::ONE);
			Math::postMul(matrix,temp);
			Math::setMatrix4x4FromTranslate(temp,0,-Math::mul(Math::HALF,amount),0);
			Math::postMul(matrix,temp);
			textureState.matrix.set(matrix);
			mFadeSprite->getMaterial()->getPass()->setTextureState(0,textureState);

			if(chompamount>=Math::HALF){
				dangerColor.w=Math::clamp(0,Math::ONE,(Math::ONE-chompamount*2)+Math::ONE);
			}
		}
*/

		materialState.set(dangerColor);
		if(mFadeSprite->getMaterial()!=NULL){
			mFadeSprite->getMaterial()->getPass()->setMaterialState(materialState);
		}
		if(dangerColor.w>0){
			mFadeSprite->setScope(-1);
		}
		else{
			mFadeSprite->setScope(0);
		}
	}

	super::frameUpdate(dt,scope);
}

void HUD::logicUpdate(int dt,int scope){
	if(mPlayer!=NULL){
		int newAcornCount=mPlayer->getAcornCount();
		if(mAcornCount!=newAcornCount){
			updateAcornCount(newAcornCount);
		}

		if(mPlayer->getCoefficientOfGravity()==0){
			mWaterAmount+=Math::fromMilli(dt*4);
		}
		else{
			mWaterAmount-=Math::fromMilli(dt*4);
		}
		mWaterAmount=Math::clamp(0,Math::ONE,mWaterAmount);

		scalar danger=mPlayer->getDanger();
		if(mChompTime>0){
			scalar chompamount=Math::fromMilli(mScene->getTime()-mChompTime);
			if(chompamount>=Math::HALF && mPlayer->getHealth()>0){
				mDogSound->stop();
				mSharkSound->stop();

				mDogSound->setAudioBuffer(Resources::instance->crunch);
				mDogSound->setGain(Math::ONE);
				mDogSound->play();

				mPlayer->setHealth(0);
				mPlayer->setSpeed(0);
				mPlayer->setVelocity(Math::ZERO_VECTOR3);
				mPlayer->dismount();
			}
		}
		else if(danger>=Math::ONE){
			mChompTime=mScene->getTime();
		}
		else if(danger>0){
			mDogSound->setGain(Math::mul(danger,Math::ONE-mWaterAmount)*4);
			mSharkSound->setGain(Math::mul(Math::ONE,mWaterAmount)*4); // use Math::mul(danger,mWaterAmount) when the shark sound is constant volume

			if(mNextBarkTime<mScene->getLogicTime() && mDogSound->getPlaying()==false){
				mNextBarkTime=mScene->getLogicTime()+mRandom.nextInt(500,1500);
				mDogSound->play();
			}
			if(mSharkSound->getPlaying()==false){
				mSharkSound->play();
			}
		}
		else if(mNextBarkTime>0){
			mNextBarkTime=0;
			mDogSound->setGain(0);
			mDogSound->stop();
			mSharkSound->setGain(0);
			mSharkSound->stop();
		}
	}

	/// @todo: Why isn't this working?
//	mSystemLabel->setText(String("FPS:")+(int)getFramesPerSecond());

	super::logicUpdate(dt,scope);
}

void HUD::setTarget(PathClimber *player,CameraNode *camera){
	mPlayer=player;
	mCamera=camera;
}

void HUD::setProjectionOrtho(scalar leftDist,scalar rightDist,scalar bottomDist,scalar topDist,scalar nearDist,scalar farDist){
	super::setProjectionOrtho(leftDist,rightDist,bottomDist,topDist,nearDist,farDist);
	scalar width=rightDist*2,height=topDist*2;

	mFadeSprite->setScale(width,height,Math::ONE);
}

void HUD::updateAcornCount(int count){
	mAcornCount=count;
	String text;
	int digits=0;
	int i;
	for(i=mAcornCount;i>0;i/=10,digits++);
	for(i=0;i<3-digits;++i) text=text+"0";
	if(count>0) text=text+count;
	mAcornLabel->setText(text);
}
