#include "Sandbox.h"

class SnowComponent:public BaseComponent{
public:
	SnowComponent(ParticleComponent *particles,int skip,PixelBuffer::ptr buffer):
		mParticles(particles),
		mSkip(skip),
		mNext(0),
		mBuffer(buffer)
	{
		mSnowData.resize(mParticles->getNumParticles());
		for(int i=0;i<mParticles->getNumParticles();++i){
			ParticleComponent::Particle *p=mParticles->getParticle(i);
			mSnowData[i].oldPosition=mSnowData[i].newPosition=Vector3(p->x,p->y,p->z);
		}

		mBufferData=new tbyte[buffer->getTextureFormat()->getDataSize()];
		memset(mBufferData,0,buffer->getTextureFormat()->getDataSize());
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

				int size=mBuffer->getTextureFormat()->getWidth();
				int s=8;
				if(result.time<1){
					position.z+=100;
					mSnowData[i].newPosition=position;
					int x=(0   + (result.point.x/200.0f + 0.5)) * size;
					int y=(1.0 - (result.point.y/200.0f + 0.5)) * size;
					for(int tx=-s;tx<=s;tx++){
						for(int ty=-s;ty<=s;ty++){
							int px=Math::intClamp(0,size,x+tx);
							int py=Math::intClamp(0,size,y+ty);
							int l=Math::intClamp(0,255,(1.0 - Math::length(Vector2(x,y),Vector2(px,py)) / (float)s) * 255);
							mBufferData[py * size + px + 0] = Math::intClamp(0,200,mBufferData[py * size + px + 0] + l);
						}
					}
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

		mBuffer->update(mBufferData,0,mBuffer->getTextureFormat()->getDataSize());
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
	PixelBuffer::ptr mBuffer;
	tbyte *mBufferData;
};

Sandbox::Sandbox(Application *app){
	this->app=app;
}

Sandbox::~Sandbox(){
}

void Sandbox::create(){
	engine=app->getEngine();
	engine->getArchiveManager()->addDirectory("..");

	scene=new Scene(engine,Scene::Option_BIT_NOBULLET);

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

	node=new Node(scene);
	{
		camera=new CameraComponent(new Camera(engine));
		camera->setClearColor(Colors::BLUE);
		node->attach(camera);
		camera->setLookAt(Vector3(0,-100,100),Math::ZERO_VECTOR3,Math::Z_UNIT_VECTOR3);
	}
	scene->getRoot()->attach(node);
		
	Texture::ptr snowflakeTexture=shared_static_cast<Texture>(engine->getTextureManager()->find("snowflake.png"));

	PixelBuffer::ptr buffer;
	Node::ptr ground=new Node(scene);
	{
		TextureFormat::ptr format=new TextureFormat(2,TextureFormat::Format_L_8,1024,1024,1,0);
		Texture::ptr texture=engine->getTextureManager()->createTexture(Texture::Usage_BIT_STREAM|Texture::Usage_BIT_AUTOGEN_MIPMAPS,format,(tbyte*)NULL);
		buffer=texture->getMipPixelBuffer(0,0);
		
		Material::ptr material=engine->createDiffuseMaterial(texture);

		MeshComponent::ptr mesh=new MeshComponent(engine);
		mesh->setMesh(engine->createAABoxMesh(AABox(-100,-100,-100,100,100,0),material));
		ground->attach(mesh);

		PhysicsComponent::ptr physics=scene->getPhysicsManager()->createPhysicsComponent();
		physics->setMass(-1);
		physics->setGravity(0);
		physics->setBound(mesh->getBound());
		ground->attach(physics);
	}
	scene->getRoot()->attach(ground);

	Node::ptr snow=new Node(scene);
	{
		Material::ptr material=engine->createPointSpriteMaterial(snowflakeTexture,1,true);
		material->getRenderState()->setRasterizerState(RasterizerState(RasterizerState::CullType_NONE));
		material->getRenderState()->setBlendState(BlendState::Combination_ALPHA_ADDITIVE);
		material->getRenderState()->setDepthState(DepthState(DepthState::DepthTest_LEQUAL,false));

		ParticleComponent::ptr particles=new ParticleComponent(scene);
		particles->setMaterial(material);
		particles->setNumParticles(10000,ParticleComponent::ParticleType_POINTSPRITE,2);
		particles->setVelocityAligned(true);
		Random r;
		for(int i=0;i<particles->getNumParticles();++i){
			ParticleComponent::Particle *p=particles->getParticle(i);
			p->x=r.nextFloat(-100,100);p->y=r.nextFloat(-100,100);p->z=r.nextFloat(0,100);
			p->vx=r.nextFloat(-1,1);p->vy=r.nextFloat(-1,1);p->vz=r.nextFloat(-10,-30);
		}
		snow->attach(particles);

		snow->attach(new SnowComponent(particles,10,buffer));
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
