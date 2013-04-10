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

class AcornCountComponent:public BaseComponent{
public:
	TOADLET_OBJECT(AcornCountComponent);

	AcornCountComponent(LabelComponent *label):
		mCount(0),
		mLabel(label){}

	void addAcorns(int count){
		mCount+=count;
		String text;
		int digits=0;
		int i;
		for(i=mCount;i>0;i/=10,digits++);
		for(i=0;i<3-digits;++i) text=text+"0";
		if(mCount>0) text=text+mCount;
		mLabel->setText(text);
	}

protected:
	int mCount;
	LabelComponent::ptr mLabel;
};

class ChompComponent:public BaseComponent,public Animatable{
public:
	TOADLET_OBJECT(ChompComponent);

	ChompComponent(Engine *engine,SpriteComponent *sprite){
		Sequence::ptr sequence=new Sequence();
		{
			Track::ptr colorTrack=new Track(engine->getVertexFormats().COLOR);
			colorTrack->addKeyFrame(0,(void*)Colors::TRANSPARENT_RED.getData());
			colorTrack->addKeyFrame(1,(void*)Colors::RED.getData());
			sequence->addTrack(colorTrack);

			Matrix4x4 startMatrix,endMatrix,temp;
			{
				scalar y=0;
				Math::setMatrix4x4FromTranslate(startMatrix,0,Math::HALF,0);
				Math::setMatrix4x4FromScale(temp,Math::ONE,y,Math::ONE);
				Math::postMul(startMatrix,temp);
				Math::setMatrix4x4FromTranslate(temp,0,-Math::mul(Math::HALF,y),0);
				Math::postMul(startMatrix,temp);

				y=4;
				Math::setMatrix4x4FromTranslate(endMatrix,0,Math::HALF,0);
				Math::setMatrix4x4FromScale(temp,Math::ONE,y,Math::ONE);
				Math::postMul(endMatrix,temp);
				Math::setMatrix4x4FromTranslate(temp,0,-Math::mul(Math::HALF,y),0);
				Math::postMul(endMatrix,temp);
			}

			Vector3 position;
			Quaternion rotate;
			Vector3 scale;

			Track::ptr transformTrack=new Track(engine->getVertexFormats().POSITION_ROTATE_SCALE);
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

		mColorAnimation=new MaterialStateAnimation(sprite->getSharedRenderState(),sequence,0);

		Shader::ShaderType type;
		int index;
		sprite->getMaterial()->getBestPath()->findTexture(type,index,"tex");
		TextureState textureState;
		sprite->getSharedRenderState()->getTextureState(type,index,textureState);
		textureState.calculation=TextureState::CalculationType_NORMAL;
		sprite->getSharedRenderState()->setTextureState(type,index,textureState);
		mTransformAnimation=new TextureStateAnimation(sprite->getSharedRenderState(),type,index,sequence,1);

		mCompositeAnimation=new CompositeAnimation();
		mCompositeAnimation->attach(mColorAnimation);
		mCompositeAnimation->attach(mTransformAnimation);
	}

	void parentChanged(Node *node){
		if(mParent!=NULL){
			mParent->animatableRemoved(this);
		}

		BaseComponent::parentChanged(node);

		if(mParent!=NULL){
			mParent->animatableAttached(this);
		}
	}

	int getNumAnimations(){return 1;}
	Animation *getAnimation(const String &name){return NULL;}
	Animation *getAnimation(int index){return mCompositeAnimation;}

	/*
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

protected:
	Animation::ptr mColorAnimation,mTransformAnimation;
	CompositeAnimation::ptr mCompositeAnimation;
};

HUD::HUD(Scene *scene,Node *player,Camera *camera):Node(scene){
	mChomp=new Node(mScene);
	{
		mChomp->setName("chomp");

		SpriteComponent::ptr sprite=new SpriteComponent(mEngine);
		sprite->setMaterial(Resources::instance->hudFade);
		mChomp->attach(sprite);
		mChomp->setScale(Vector3(2,2,2));

		AudioComponent::ptr dog=new AudioComponent(mEngine);
		dog->setAudioBuffer(Resources::instance->dog);
		dog->setGlobal(true);
		mChomp->attach(dog);

		AudioComponent::ptr shark=new AudioComponent(mEngine);
		shark->setAudioBuffer(Resources::instance->shark);
		shark->setGlobal(true);
		mChomp->attach(shark);

		ChompComponent::ptr chomp=new ChompComponent(mEngine,sprite);//,dog,shark);
		mChomp->attach(chomp);

		AnimationAction::ptr action=new AnimationAction();
		action->attach(chomp->getAnimation(0));
		action->attach(chomp->getAnimation(1));
		action->setCycling(AnimationAction::Cycling_REFLECT);
		attach(new ActionComponent("action",action));
	}
	attach(mChomp);

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
		Node::ptr node=new Node(mScene);
		{
			SpriteComponent::ptr sprite=new SpriteComponent(mEngine);
			sprite->setMaterial(Resources::instance->hudAcorn);
			node->attach(sprite);
			node->setTranslate(-0.75,0.75,0);
			node->setScale(0.25,0.20,0.25);
		}
		mAcorn->attach(node);

		LabelComponent::ptr label;
		node=new Node(mScene);
		{
			label=new LabelComponent(mEngine);
			label->setFont(Resources::instance->hudWooden);
			label->setAlignment(Font::Alignment_BIT_VCENTER|Font::Alignment_BIT_LEFT);
			label->getSharedRenderState()->setMaterialState(MaterialState(Colors::BROWN));
			node->attach(label);
			node->setTranslate(-0.60,0.80,0);
			node->setScale(0.3,0.3,0.3);
		}
		mAcorn->attach(node);

		AcornCountComponent::ptr count=new AcornCountComponent(label);
		count->setName("count");
		mAcorn->attach(count);
	}
	attach(mAcorn);
}
