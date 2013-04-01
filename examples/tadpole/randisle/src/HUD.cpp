#include "HUD.h"
#include "PathClimber.h"
#include "Resources.h"

class CompassComponent:public BaseComponent{
public:
	TOADLET_OBJECT(CompassComponent);

	CompassComponent(Camera *camera){
		mCamera=camera;
	}

	void logicUpdate(int dt,int scope){
		BaseComponent::logicUpdate(dt,scope);

		Vector3 forward=mCamera->getForward();
		forward.z=0;
		Math::normalizeCarefully(forward,0);
		scalar angle=-Math::atan2(forward.y,forward.x);
		mParent->setRotate(Math::Z_UNIT_VECTOR3,angle);\
	}

	bool getActive() const{return true;}

protected:
	Camera::ptr mCamera;
};

/* 
	FadeComponent::frameUpdate, replace with some animations
		MaterialState materialState;
		if(mFadeSprite->getMaterial()!=NULL){
			mFadeSprite->getMaterial()->getPass()->getMaterialState(materialState);
		}
		Vector4 dangerColor=materialState.ambient;
//		dangerColor.w=mPlayer->getDanger();

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
*/

/* AcornComponent
		int newAcornCount=mPlayer->getAcornCount();
		if(mAcornCount!=newAcornCount){
			updateAcornCount(newAcornCount);

			void HUD::updateAcornCount(int count){
				mAcornCount=count;
				String text;
				int digits=0;
				int i;
				for(i=mAcornCount;i>0;i/=10,digits++);
				for(i=0;i<3-digits;++i) text=text+"0";
				if(count>0) text=text+count;
				((LabelComponent*)mAcorns->getVisible(0))->setText(text);
			}
		}
*/

/* ChompComponent
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
*/

HUD::HUD(Scene *scene,Node *player,Camera *camera):Node(scene){
	Node::ptr sounds=new Node(mScene);
	{
		mDogSound=new AudioComponent(mEngine);
		mDogSound->setAudioBuffer(Resources::instance->dog);
		mDogSound->setGlobal(true);
		sounds->attach(mDogSound);

		mSharkSound=new AudioComponent(mEngine);
		mSharkSound->setAudioBuffer(Resources::instance->shark);
		mSharkSound->setGlobal(true);
		sounds->attach(mSharkSound);
	}
	attach(sounds);

	mFade=new Node(mScene);
	{
		SpriteComponent::ptr sprite=new SpriteComponent(mEngine);
		sprite->setMaterial(Resources::instance->hudFade);
		mFade->attach(sprite);
		mFade->setScale(Vector3(2,2,2));
	}
	attach(mFade);

	mCompass=new Node(mScene);
	{
		SpriteComponent::ptr sprite=new SpriteComponent(mEngine);
		sprite->setMaterial(Resources::instance->hudCompass);
		mCompass->attach(sprite);
		mCompass->setTranslate(0.75,0.75,0);
		mCompass->setScale(0.25,0.25,0.25);

		CompassComponent::ptr compass=new CompassComponent(camera);
		mCompass->attach(compass);
	}
	attach(mCompass);
	
	mAcorn=new Node(mScene);
	{
		SpriteComponent::ptr sprite=new SpriteComponent(mEngine);
		sprite->setMaterial(Resources::instance->hudAcorn);
		mAcorn->attach(sprite);
		mAcorn->setTranslate(-0.75,0.75,0);
		mAcorn->setScale(0.25,0.20,0.25);
	}
	attach(mAcorn);

	mAcorns=new Node(mScene);
	{
		LabelComponent::ptr label=new LabelComponent(mEngine);
		label->setFont(Resources::instance->hudWooden);
		label->setAlignment(Font::Alignment_BIT_VCENTER|Font::Alignment_BIT_LEFT);
		label->getSharedRenderState()->setMaterialState(MaterialState(Colors::BROWN));
		mAcorns->attach(label);
		mAcorns->setTranslate(-0.60,0.80,0);
		mAcorns->setScale(0.3,0.3,0.3);
	}
	attach(mAcorns);
}
