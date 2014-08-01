#include "Sandbox.h"

class SnowComponent:public BaseComponent{
public:
	SnowComponent(ParticleComponent *particles,int skip):
		mParticles(particles),
		mSkip(skip),
		mNext(0)
	{
		mSnowData.resize(mParticles->getNumParticles());
		for(int i=0;i<mParticles->getNumParticles();++i){
			ParticleComponent::Particle *p=mParticles->getParticle(i);
			mSnowData[i].oldPosition=mSnowData[i].newPosition=Vector3(p->x,p->y,p->z);
		}
	}

	void frameUpdate(int dt,int scope){
		Scene *scene=mParent->getScene();

		for(int i=0;i<mParticles->getNumParticles();i++){
			ParticleComponent::Particle *p=mParticles->getParticle(i);

			Vector3 position(p->x,p->y,p->z);

			float t=1.0 - (float((i-mNext+mSkip)%mSkip) + 1.0 - scene->getLogicFraction())/float(mSkip);
			if(mSnowData[i].traceTime>0){
				t/=mSnowData[i].traceTime;
			}
			Math::lerp(position,mSnowData[i].oldPosition,mSnowData[i].newPosition,t);

			p->x=position.x;p->y=position.y;p->z=position.z;
		}
	}

	void logicUpdate(int dt,int scope){
		Scene *scene=mParent->getScene();

		for(int i=0;i<mParticles->getNumParticles();i++){
			ParticleComponent::Particle *p=mParticles->getParticle(i);

			Vector3 position(p->x,p->y,p->z);
			Vector3 velocity(p->vx,p->vy,p->vz);

			float t=float((i-mNext+mSkip)%mSkip)/float(mSkip);
			t = 1.0 - t;
			if(mSnowData[i].traceTime>0){
				t = Math::clamp(0, Math::ONE, t / mSnowData[i].traceTime);
			}
			Math::lerp(position,mSnowData[i].oldPosition,mSnowData[i].newPosition,t);

			if((i%mSkip)==mNext){
				Segment segment;
				segment.origin=position;
				segment.direction=velocity*Math::fromMilli(dt) * mSkip;

				PhysicsCollision result;
				scene->getPhysicsManager()->traceSegment(result,segment,-1,NULL);
				mSnowData[i].traceTime=result.time;

				mSnowData[i].oldPosition=mSnowData[i].newPosition;

				if(result.time<1){
					mSnowData[i].newPosition=result.point;
				}
				else{
					mSnowData[i].newPosition=position+segment.direction;
				}
			}

			p->x=position.x;p->y=position.y;p->z=position.z;
			p->vx=velocity.x;p->vy=velocity.y;p->vz=velocity.z;
		}

		mNext++;
		if(mNext>=mSkip){
			mNext=0;
		}
	}

protected:
	class SnowData{
	public:
		SnowData():traceTime(0){}

		Vector3 oldPosition;
		Vector3 newPosition;
		scalar traceTime;
	};

	ParticleComponent::ptr mParticles;
	Collection<SnowData> mSnowData;
	int mSkip,mNext;
};

Sandbox::Sandbox(Application *app){
	this->app=app;
}

Sandbox::~Sandbox(){
}

void Sandbox::create(){
	engine=app->getEngine();
//	engine->setHasMaximumShader(false);
	engine->getArchiveManager()->addDirectory("..");

	scene=new Scene(engine);

	Node::ptr node=new Node(scene);
	{
		LightComponent::ptr light=new LightComponent();
		LightState state;
		state.type=LightState::Type_DIRECTION;
		state.direction=Math::NEG_Z_UNIT_VECTOR3;
		light->setLightState(state);
		node->attach(light);
	}
	scene->getRoot()->attach(node);

	Node::ptr lt=new Node(scene);
	{
// 		MeshComponent::ptr mesh=new MeshComponent(engine);
//		mesh->setMesh("spider.obj");
//		lt->attach(mesh);

//		PhysicsComponent::ptr physics=scene->getPhysicsManager()->createPhysicsComponent();
//		physics->setMass(1);
//		physics->setBound(new Bound(Sphere(1)));
//		lt->attach(physics);
	}
	scene->getRoot()->attach(lt);

	node=new Node(scene);
	{
		camera=new CameraComponent(new Camera(engine));
		camera->setClearColor(Colors::BLUE);
		node->attach(camera);
		camera->setLookAt(Vector3(0,-Math::fromInt(50),0),Math::ZERO_VECTOR3,Math::Z_UNIT_VECTOR3);
	}
	scene->getRoot()->attach(node);

	Node::ptr snow=new Node(scene);
	{
		ParticleComponent::ptr particles=new ParticleComponent(scene);
		Texture::ptr texture=shared_static_cast<Texture>(engine->getTextureManager()->find("C:\\Users\\siralanf\\toadlet\\examples\\randisle\\res\\acorn.png"));
		particles->setMaterial(engine->createPointSpriteMaterial(texture,1,true));
		particles->setNumParticles(10000,ParticleComponent::ParticleType_POINTSPRITE,1);
		Random r;
		for(int i=0;i<particles->getNumParticles();++i){
			ParticleComponent::Particle *p=particles->getParticle(i);
			p->x=r.nextFloat(-100,100);p->y=r.nextFloat(-100,100);p->z=r.nextFloat(-100,100);
			p->vx=r.nextFloat(-1,1);p->vy=r.nextFloat(-1,1);p->vz=r.nextFloat(0,-15);
		}
		snow->attach(particles);

		snow->attach(new SnowComponent(particles,10));
	}
	scene->getRoot()->attach(snow);
}

void Sandbox::destroy(){
	scene->destroy();
}

void Sandbox::render(){
	RenderDevice *renderDevice=engine->getRenderDevice();

	renderDevice->beginScene();
		camera->getCamera()->render(renderDevice,scene);
	renderDevice->endScene();
	renderDevice->swap();
}

void Sandbox::update(int dt){
	scene->update(dt);
}

Applet *createApplet(Application *app){return new Sandbox(app);}
