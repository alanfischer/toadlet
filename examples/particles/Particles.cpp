#include "Particles.h"

Particles::Particles(Application *application){
	app=application;
}

Particles::~Particles(){
}

void Particles::create(){
	engine=app->getEngine();
	engine->getArchiveManager()->addDirectory("../../data");

	scene=new Scene(engine);

	scalar ten=Math::ONE*10;

	Vector3 pointPositions[]={
		Vector3(-ten,-ten,0),
		Vector3(ten,-ten,0),
		Vector3(ten,ten,ten),
		Vector3(-ten,ten,ten),
	};

	Vector3 beamPositions[]={
		Vector3(-ten,-ten,0),
		Vector3(ten,-ten,0),
		Vector3(ten,-ten,0),
		Vector3(ten,ten,ten),
		Vector3(ten,ten,ten),
		Vector3(-ten,ten,ten),
		Vector3(-ten,ten,ten),
		Vector3(-ten,-ten,0),
	};

	Material::ptr pointMaterial=engine->createPointSpriteMaterial(engine->getTextureManager()->findTexture("sparkle.png"),ten,false);
	if(pointMaterial!=NULL){
		pointMaterial->getPass()->setBlendState(BlendState::Combination_ALPHA_ADDITIVE);
		pointMaterial->getPass()->setRasterizerState(RasterizerState(RasterizerState::CullType_NONE));
		pointMaterial->getPass()->setMaterialState(MaterialState(false));
		pointMaterial->getPass()->setDepthState(DepthState(DepthState::DepthTest_LEQUAL,false));
	}

	Material::ptr spriteMaterial=engine->getMaterialManager()->findMaterial("sparkle.png");
	if(spriteMaterial!=NULL){
		spriteMaterial->getPass()->setBlendState(BlendState::Combination_ALPHA_ADDITIVE);
		spriteMaterial->getPass()->setRasterizerState(RasterizerState(RasterizerState::CullType_NONE));
		spriteMaterial->getPass()->setMaterialState(MaterialState(false));
		spriteMaterial->getPass()->setDepthState(DepthState(DepthState::DepthTest_LEQUAL,false));
	}

	Material::ptr beamMaterial=engine->getMaterialManager()->findMaterial("fancyGlow.png");
	if(beamMaterial!=NULL){
		beamMaterial->getPass()->setBlendState(BlendState::Combination_ALPHA_ADDITIVE);
		beamMaterial->getPass()->setRasterizerState(RasterizerState(RasterizerState::CullType_NONE));
		beamMaterial->getPass()->setMaterialState(MaterialState(false));
		beamMaterial->getPass()->setDepthState(DepthState(DepthState::DepthTest_LEQUAL,false));
	}

	Node::ptr pointsNode=new Node(scene);
	{
		points=new ParticleComponent(scene);
		points->setNumParticles(4,ParticleComponent::ParticleType_POINTSPRITE,Math::ONE,pointPositions);
		points->setMaterial(pointMaterial);
		pointsNode->attach(points);
	}
	pointsNode->setTranslate(-Math::fromInt(40),0,0);
	scene->getRoot()->attach(pointsNode);

	Node::ptr spritesNode=new Node(scene);
	{
 		sprites=new ParticleComponent(scene);
		sprites->setNumParticles(4,ParticleComponent::ParticleType_SPRITE,Math::ONE,pointPositions);
		sprites->setMaterial(spriteMaterial);
		spritesNode->attach(sprites);
	}
	spritesNode->setTranslate(0,0,0);
	scene->getRoot()->attach(spritesNode);

	Node::ptr beamsNode=new Node(scene);
	{
 		beams=new ParticleComponent(scene);
		beams->setNumParticles(8,8 | ParticleComponent::ParticleType_BIT_LOOP,Math::ONE,beamPositions);
		beams->setMaterial(beamMaterial);
		beamsNode->attach(beams);
	}
	beamsNode->setTranslate(Math::fromInt(40),0,0);
	scene->getRoot()->attach(beamsNode);

	camera=new Camera(engine);
	camera->setLookAt(Vector3(0,-Math::fromInt(150),0),Math::ZERO_VECTOR3,Math::Z_UNIT_VECTOR3);
	camera->setClearColor(Colors::BLACK);

	if(points){
		points->setWorldSpace(true);
		particles.add(points);
	}
	if(sprites){
		sprites->setWorldSpace(true);
		sprites->setVelocityAligned(true);
		particles.add(sprites);
	}
	if(beams){
		beams->setWorldSpace(true);
		particles.add(beams);
	}

	#if defined(LFX_DLL_NAME)
		lfxLibrary.load(LFX_DLL_NAME,"","","");
		lfxInitialize=(LFX2INITIALIZE)lfxLibrary.getSymbol(LFX_DLL_INITIALIZE);
		lfxRelease=(LFX2RELEASE)lfxLibrary.getSymbol(LFX_DLL_RELEASE);
		lfxReset=(LFX2RESET)lfxLibrary.getSymbol(LFX_DLL_RESET);
		lfxUpdate=(LFX2UPDATE)lfxLibrary.getSymbol(LFX_DLL_UPDATE);
		lfxLight=(LFX2LIGHT)lfxLibrary.getSymbol(LFX_DLL_LIGHT);

		lfxInitialize();
		lfxReset();
		lfxTime=0;
	#endif
}

void Particles::destroy(){
	#if defined(LFX_DLL_NAME)
		lfxRelease();
	#endif

	scene->destroy();
}

void Particles::render(){
	RenderDevice *device=engine->getRenderDevice();

	device->beginScene();
		camera->render(device,scene);
	device->endScene();
 	device->swap();
}

void Particles::update(int dt){
	scalar fdt=Math::fromMilli(dt);
	scalar edge=Math::fromInt(60);

	int i,j;
	for(i=0;i<particles.size();++i){
		ParticleComponent *pc=particles[i];
		for(j=0;j<pc->getNumParticles();++j){
			ParticleComponent::Particle *p=pc->getParticle(j);
			p->vz-=Math::mul(Math::fromFloat(9.8),fdt);

			p->x+=Math::mul(p->vx,fdt);
			p->y+=Math::mul(p->vy,fdt);
			p->z+=Math::mul(p->vz,fdt);

			if(p->x<-edge){p->x=-edge;p->vx*=-1;}
			if(p->x>edge){p->x=edge;p->vx*=-1;}
			if(p->y<-edge){p->y=-edge;p->vy*=-1;}
			if(p->y>edge){p->y=edge;p->vy*=-1;}
			if(p->z<-edge){p->z=-edge;p->vz*=-1;}
		}
	}

	scene->update(dt);

	#if defined(LFX_DLL_NAME)
		lfxTime+=dt;
		if(lfxTime>200){
			lfxTime=0;

			scalar avgz=0;
			int total=0;
			for(i=0;i<particles.size();++i){
				ParticleComponent *pc=particles[i];
				for(j=0;j<pc->getNumParticles();++j){
					ParticleComponent::Particle *p=pc->getParticle(j);
					avgz+=p->z;
					total++;
				}
			}

			avgz/=(scalar)total;
			unsigned int intensity=(Math::clamp(0,1,-avgz/edge)*0xFF);
			lfxLight(LFX_ALL,(intensity<<24)|LFX_PINK);
			lfxUpdate();
		}
	#endif
}

void Particles::keyPressed(int key){
	if(key==Application::Key_ESC){
		app->stop();
	}
	if(key==Application::Key_SPACE){
		scalar v=Math::fromInt(4);
		int i,j;
		for(i=0;i<particles.size();++i){
			ParticleComponent *pc=particles[i];
			for(j=0;j<pc->getNumParticles();++j){
				ParticleComponent::Particle *p=pc->getParticle(j);
				p->vx+=random.nextScalar(-v,v);
				p->vy+=random.nextScalar(-v,v);
				p->vz+=random.nextScalar(-v,v);
			}
		}
	}
}

void Particles::mousePressed(int x,int y,int button){
	keyPressed(Application::Key_SPACE);
}

Applet *createApplet(Application *app){return new Particles(app);}
