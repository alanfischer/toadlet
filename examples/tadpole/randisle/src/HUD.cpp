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
		mParent->setRotate(Math::Z_UNIT_VECTOR3,angle);
	}

	bool getActive() const{return true;}

protected:
	Camera::ptr mCamera;
};

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

		Sequence::ptr sequence=new Sequence();
		{
			Track::ptr colorTrack=new Track(mEngine->getVertexFormats().COLOR);
			colorTrack->addKeyFrame(0,(void*)Colors::TRANSPARENT_RED.getData());
			colorTrack->addKeyFrame(1,(void*)Colors::RED.getData());
			sequence->addTrack(colorTrack);

			Matrix4x4 startMatrix,endMatrix,temp;
			{
				float amount=0;
				Math::setMatrix4x4FromTranslate(startMatrix,0,Math::HALF,0);
				Math::setMatrix4x4FromScale(temp,Math::ONE,amount,Math::ONE);
				Math::postMul(startMatrix,temp);
				Math::setMatrix4x4FromTranslate(temp,0,-Math::mul(Math::HALF,amount),0);
				Math::postMul(startMatrix,temp);

				amount=Math::ONE;
				Math::setMatrix4x4FromTranslate(endMatrix,0,Math::HALF,0);
				Math::setMatrix4x4FromScale(temp,Math::ONE,amount,Math::ONE);
				Math::postMul(endMatrix,temp);
				Math::setMatrix4x4FromTranslate(temp,0,-Math::mul(Math::HALF,amount),0);
				Math::postMul(endMatrix,temp);
			}

			Vector3 position;
			Quaternion rotate;
			Vector3 scale;

			Track::ptr transformTrack=new Track(mEngine->getVertexFormats().POSITION_ROTATE_SCALE);
			VertexBufferAccessor &vba=transformTrack->getAccessor();

			Math::setTranslateFromMatrix4x4(position,startMatrix);
			Math::setQuaternionFromMatrix4x4(rotate,startMatrix);
			Math::setScaleFromMatrix4x4(scale,startMatrix);
			transformTrack->addKeyFrame(0);
			vba.set3(0,0,position);
			vba.set4(0,1,rotate);
			vba.set3(0,2,scale);

			Math::setTranslateFromMatrix4x4(position,endMatrix);
			Math::setQuaternionFromMatrix4x4(rotate,endMatrix);
			Math::setScaleFromMatrix4x4(scale,endMatrix);
			transformTrack->addKeyFrame(1);
			vba.set3(1,0,position);
			vba.set4(1,1,rotate);
			vba.set3(1,2,scale);

			sequence->addTrack(transformTrack);
		}

		Animation::ptr colorAnimation=new MaterialStateAnimation(sprite->getSharedRenderState(),sequence,0);

		TextureState textureState;
		sprite->getSharedRenderState()->getTextureState(Shader::ShaderType_FRAGMENT,0,textureState);
		textureState.calculation=TextureState::CalculationType_NORMAL;
		sprite->getSharedRenderState()->setTextureState(Shader::ShaderType_FRAGMENT,0,textureState);
		Animation::ptr transformAnimation=new TextureStateAnimation(sprite->getSharedRenderState(),Shader::ShaderType_FRAGMENT,0,sequence,1);

		AnimationAction::ptr action=new AnimationAction();
		action->attach(colorAnimation);
		action->attach(transformAnimation);
		attach(new ActionComponent("action",action));
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
