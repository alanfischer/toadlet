#include "Sandbox.h"
#include <toadlet/tadpole/plugins/PointTextureCreator.h>

class SnowComponent:public BaseComponent{
public:
	SnowComponent(ParticleComponent *particles,int skip,Texture::ptr texture):
		mParticles(particles),
		mSkip(skip),
		mNext(0),
		mTexture(texture)
	{
		mSnowData.resize(mParticles->getNumParticles());
		for(int i=0;i<mParticles->getNumParticles();++i){
			ParticleComponent::Particle *p=mParticles->getParticle(i);
			mSnowData[i].oldPosition=mSnowData[i].newPosition=Vector3(p->x,p->y,p->z);
		}

		mBufferData=new tbyte[texture->getFormat()->getDataSize()];
		memset(mBufferData,0,texture->getFormat()->getDataSize());
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
				((TerrainNode*)scene->getRoot())->traceDetailSegment(result,Vector3(),segment,Vector3());
//				scene->getPhysicsManager()->traceSegment(result,segment,-1,NULL);
				mSnowData[i].traceTime=result.time;

				mSnowData[i].oldPosition=mSnowData[i].newPosition;

				if(result.time<1){
					position.z+=100;
					mSnowData[i].newPosition=position;

					if(result.texCoord.x>=0 && result.texCoord.x<1 && result.texCoord.y>=0 && result.texCoord.y<1){
						int x=(result.texCoord.x) * mTexture->getFormat()->getWidth();
						int y=(result.texCoord.y) * mTexture->getFormat()->getHeight();
						int s=8;
						PointTextureCreator::createPointTexture(mTexture->getFormat(),mBufferData,x-s,y-s,s*2,s*2,0,1,0,0,1.25);
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

		mTexture->load(mTexture->getFormat(),mBufferData);
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
	Texture::ptr mTexture;
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

	TextureFormat::ptr format=new TextureFormat(2,TextureFormat::Format_L_8,1024,1024,1,0);
	Texture::ptr texture=engine->getTextureManager()->createTexture(Texture::Usage_BIT_STREAM|Texture::Usage_BIT_AUTOGEN_MIPMAPS,format,(tbyte*)NULL);

	TerrainNode::ptr terrain=new TerrainNode(scene);
	terrain->setDataSource(new TextureDataSource(engine,Vector3(.5,.5,12),0,"terrain.png"));
	terrain->setMaterialSource(new DiffuseTerrainMaterialSource(engine,texture));//,Vector3(.5,.5,1),Vector3(0,0,0)));
	terrain->setTolerance(0);
	scene->setRoot(terrain);

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
		
	TextureFormat::ptr snowflakeFormat=new TextureFormat(TextureFormat::Dimension_D2,TextureFormat::Format_LA_8,128,128,1,0);
	Texture::ptr snowflakeTexture=engine->createPointTexture(snowflakeFormat,1,0,0,1,1.25);

	Node::ptr snow=new Node(scene);
	{
		Material::ptr material=engine->createPointSpriteMaterial(snowflakeTexture,4,true);
		material->getRenderState()->setRasterizerState(RasterizerState(RasterizerState::CullType_NONE));
		material->getRenderState()->setBlendState(BlendState::Combination_ALPHA_ADDITIVE);
		material->getRenderState()->setDepthState(DepthState(DepthState::DepthTest_LEQUAL,false));

		ParticleComponent::ptr particles=new ParticleComponent(scene);
		particles->setMaterial(material);
		particles->setNumParticles(500,ParticleComponent::ParticleType_POINTSPRITE,1);
		Random r;
		for(int i=0;i<particles->getNumParticles();++i){
			ParticleComponent::Particle *p=particles->getParticle(i);
			p->x=r.nextFloat(-100,100);p->y=r.nextFloat(-100,100);p->z=r.nextFloat(0,100);
			p->vx=r.nextFloat(-1,1);p->vy=r.nextFloat(-1,1);p->vz=r.nextFloat(-10,-30);
		}
		snow->attach(particles);

		snow->attach(new SnowComponent(particles,10,texture));
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
