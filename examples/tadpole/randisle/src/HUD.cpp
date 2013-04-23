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

class DangerComponent:public BaseComponent{
public:
	TOADLET_OBJECT(DangerComponent);

	DangerComponent(Node *player,Animation *danger,AudioComponent *dog,ActionComponent *chomp):
		mPlayer(player),
		mDanger(danger),
		mDog(dog),
		mChomp(chomp),
		mNextDogTime(0)
	{}

	void logicUpdate(int dt,int scope){
		scalar danger=0;

		danger+=Math::fromMilli(dt)*0.1;

		if(danger>=Math::ONE){
			if(mChomp->getActive()==false){
				mChomp->start();
				mDog->stop();
			}
		}
		else if(danger>0){
			if(mNextDogTime<mParent->getScene()->getLogicTime() && mDog->getPlaying()==false){
				mNextDogTime=mParent->getScene()->getLogicTime()+mRandom.nextInt(500,1500);
				mDog->play();
			}
		}

		mDanger->setValue(danger);
	}

protected:
	Node::ptr mPlayer;
	Animation::ptr mDanger;
	AudioComponent::ptr mDog;
	ActionComponent::ptr mChomp;
	int mNextDogTime;
	Random mRandom;
};

class ChompyComponent:public BaseComponent,public Animatable{
public:
	TOADLET_OBJECT(ChompyComponent);

	ChompyComponent(Engine *engine,SpriteComponent *sprite,AudioComponent *audio){
		Sequence::ptr sequence=new Sequence();
		{
			Track::ptr colorTrack=new Track(engine->getVertexFormats().COLOR);
			colorTrack->addKeyFrame(0,(void*)Colors::TRANSPARENT_RED.getData());
			colorTrack->addKeyFrame(1,(void*)Colors::RED.getData());
			sequence->addTrack(colorTrack);

			Track::ptr gainTrack=new Track(engine->getVertexFormats().COLOR);
			gainTrack->addKeyFrame(0,(void*)Vector4(0,0,0,0).getData());
			gainTrack->addKeyFrame(1,(void*)Vector4(1,0,0,0).getData());
			sequence->addTrack(gainTrack);

			Matrix4x4 startMatrix,endMatrix,temp;
			{
				scalar y=Math::ONE;
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

			int i;
			Vector3 position;
			Quaternion rotate;
			Vector3 scale;

			Track::ptr transformTrack=new Track(engine->getVertexFormats().POSITION_ROTATE_SCALE);
			VertexBufferAccessor &vba=transformTrack->getAccessor();

			Math::setTranslateFromMatrix4x4(position,startMatrix);
			Math::setQuaternionFromMatrix4x4(rotate,startMatrix);
			Math::setScaleFromMatrix4x4(scale,startMatrix);
			i=transformTrack->addKeyFrame(0);
			vba.set3(i,0,position);
			vba.set4(i,1,rotate);
			vba.set3(i,2,scale);

			Math::setTranslateFromMatrix4x4(position,endMatrix);
			Math::setQuaternionFromMatrix4x4(rotate,endMatrix);
			Math::setScaleFromMatrix4x4(scale,endMatrix);
			i=transformTrack->addKeyFrame(0.5);
			vba.set3(i,0,position);
			vba.set4(i,1,rotate);
			vba.set3(i,2,scale);

			Math::setTranslateFromMatrix4x4(position,startMatrix);
			Math::setQuaternionFromMatrix4x4(rotate,startMatrix);
			Math::setScaleFromMatrix4x4(scale,startMatrix);
			i=transformTrack->addKeyFrame(1);
			vba.set3(i,0,position);
			vba.set4(i,1,rotate);
			vba.set3(i,2,scale);

			sequence->addTrack(transformTrack);
		}

		mDangerAnimation=new CompositeAnimation();
		mDangerAnimation->attach(new MaterialStateAnimation(sprite->getSharedRenderState(),sequence,0));
		mDangerAnimation->attach(new AudioGainAnimation(audio,sequence,1));

		Shader::ShaderType type;
		int index;
		sprite->getMaterial()->getBestPath()->findTexture(type,index,"tex");
		TextureState textureState;
		sprite->getSharedRenderState()->getTextureState(type,index,textureState);
		textureState.calculation=TextureState::CalculationType_NORMAL;
		sprite->getSharedRenderState()->setTextureState(type,index,textureState);
		mChompAnimation=new TextureStateAnimation(sprite->getSharedRenderState(),type,index,sequence,2);
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

	int getNumAnimations(){return 2;}
	Animation *getAnimation(const String &name){
		if(name=="danger"){
			return mDangerAnimation;
		}
		else{
			return mChompAnimation;
		}
	}
	Animation *getAnimation(int index){
		if(index==0){
			return mDangerAnimation;
		}
		else{
			return mChompAnimation;
		}
	}

protected:
	CompositeAnimation::ptr mDangerAnimation;
	Animation::ptr mChompAnimation;
};

HUD::HUD(Scene *scene,Node *player,Camera *camera):Node(scene){
	mChomp=new Node(mScene);
	{
		mChomp->setName("chomp");

		SpriteComponent::ptr sprite=new SpriteComponent(mEngine);
		sprite->setMaterial(Resources::instance->fade);
		mChomp->attach(sprite);
		mChomp->setScale(Vector3(2,2,2));

		AudioComponent::ptr dog=new AudioComponent(mEngine);
		dog->setAudioBuffer(Resources::instance->dog);
		dog->setGlobal(true);
		mChomp->attach(dog);

		AudioComponent::ptr crunch=new AudioComponent(mEngine);
		crunch->setAudioBuffer(Resources::instance->crunch);
		crunch->setGlobal(true);
		mChomp->attach(crunch);

		ChompyComponent::ptr chompy=new ChompyComponent(mEngine,sprite,dog);
		mChomp->attach(chompy);

		CompositeAction::ptr chompAction=new CompositeAction();
		chompAction->setStopStyle(CompositeAction::StopStyle_ON_LAST);
		chompAction->attach(new AnimationAction(chompy->getAnimation("chomp")));
		chompAction->attach(new AudioAction(crunch));
		ActionComponent::ptr chomp=new ActionComponent("chomp",chompAction);
		mChomp->attach(chomp);

		DangerComponent::ptr danger=new DangerComponent(player,chompy->getAnimation("danger"),dog,chomp);
		mChomp->attach(danger);
	}
	attach(mChomp);

	mCompass=new Node(mScene);
	{
		SpriteComponent::ptr sprite=new SpriteComponent(mEngine);
		sprite->setMaterial(Resources::instance->compass);
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
			sprite->setMaterial(Resources::instance->acorn);
			node->attach(sprite);
			node->setTranslate(-0.75,0.75,0);
			node->setScale(0.25,0.20,0.25);
		}
		mAcorn->attach(node);

		LabelComponent::ptr label;
		node=new Node(mScene);
		{
			label=new LabelComponent(mEngine);
			label->setFont(Resources::instance->wooden);
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
