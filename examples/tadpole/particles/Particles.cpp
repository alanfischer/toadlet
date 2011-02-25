#include "Particles.h"

Particles::Particles():Application(){
}

Particles::~Particles(){
}

void Particles::create(){
	Application::create();

	mEngine->setDirectory("../../data");

	scene=Scene::ptr(new Scene(mEngine));

	Vector3 pointPositions[]={
		Vector3(-10,-10,0),
		Vector3(10,-10,0),
		Vector3(10,10,10),
		Vector3(-10,10,10),
	};

	Vector3 beamPositions[]={
		Vector3(-10,-10,0),
		Vector3(10,-10,0),
		Vector3(10,-10,0),
		Vector3(10,10,10),
		Vector3(10,10,10),
		Vector3(-10,10,10),
		Vector3(-10,10,10),
		Vector3(-10,-10,0),
	};

	Material::ptr pointMaterial=mEngine->getMaterialManager()->findMaterial("sparkle.png");
	pointMaterial->setPointState(PointState(true,10,false,1,1,1,1,100));
	pointMaterial->setBlend(Blend::Combination_ALPHA_ADDITIVE);
	pointMaterial->setFaceCulling(Renderer::FaceCulling_NONE);
	pointMaterial->setDepthWrite(false);

	Material::ptr beamMaterial=mEngine->getMaterialManager()->findMaterial("fancyGlow.png");
	beamMaterial->setBlend(Blend::Combination_ALPHA_ADDITIVE);
	beamMaterial->setFaceCulling(Renderer::FaceCulling_NONE);
	beamMaterial->setDepthWrite(false);

 	pointNode=getEngine()->createNodeType(ParticleNode::type(),scene);
	pointNode->setNumParticles(4,ParticleNode::ParticleType_POINTSPRITE,pointPositions);
	pointNode->setMaterial(pointMaterial);
	pointNode->setTranslate(-40,0,0);
	scene->getRoot()->attach(pointNode);

 	spriteNode=getEngine()->createNodeType(ParticleNode::type(),scene);
	spriteNode->setNumParticles(4,ParticleNode::ParticleType_SPRITE,pointPositions);
	spriteNode->setMaterial(pointMaterial);
	spriteNode->setTranslate(0,0,0);
	scene->getRoot()->attach(spriteNode);

 	beamNode=getEngine()->createNodeType(ParticleNode::type(),scene);
	beamNode->setNumParticles(8,ParticleNode::ParticleType_BEAM,beamPositions);
	beamNode->setMaterial(beamMaterial);
	beamNode->setTranslate(40,0,0);
	scene->getRoot()->attach(beamNode);

	cameraNode=getEngine()->createNodeType(CameraNode::type(),scene);
	cameraNode->setLookAt(Vector3(0,-Math::fromInt(150),0),Math::ZERO_VECTOR3,Math::Z_UNIT_VECTOR3);
	cameraNode->setClearColor(Colors::BLACK);
	scene->getRoot()->attach(cameraNode);
}

void Particles::resized(int width,int height){
	if(cameraNode!=NULL && width>0 && height>0){
		if(width>=height){
			cameraNode->setProjectionFovY(Math::degToRad(Math::fromInt(45)),Math::div(Math::fromInt(width),Math::fromInt(height)),Math::fromInt(10),Math::fromInt(200));
		}
		else{
			cameraNode->setProjectionFovX(Math::degToRad(Math::fromInt(45)),Math::div(Math::fromInt(height),Math::fromInt(width)),Math::fromInt(10),Math::fromInt(200));
		}
		cameraNode->setViewport(Viewport(0,0,width,height));
	}
}

void Particles::render(Renderer *renderer){
	renderer->beginScene();
		cameraNode->render(renderer);
	renderer->endScene();
	renderer->swap();
}

void Particles::update(int dt){
	scalar fdt=Math::fromMilli(dt);

	int i,j;
	for(i=0;i<simulatedParticles.size();++i){
		ParticleNode *particles=simulatedParticles[i];
		for(j=0;j<particles->getNumParticles();++j){
			ParticleNode::Particle *p=particles->getParticle(j);
			p->vz-=9.8*fdt;

			p->x+=p->vx*fdt;
			p->y+=p->vy*fdt;
			p->z+=p->vz*fdt;

			if(p->x<-60){p->x=-60;p->vx*=-1;}
			if(p->x>60){p->x=60;p->vx*=-1;}
			if(p->y<-60){p->y=-60;p->vy*=-1;}
			if(p->y>60){p->y=60;p->vy*=-1;}
			if(p->z<-20){p->z=-20;p->vz*=-1;}
		}
	}

	scene->update(dt);
}

void Particles::keyPressed(int key){
	if(key==Key_ESC){
		stop();
	}
	else if(key==Key_SPACE){
		pointNode->setWorldSpace(true);
		spriteNode->setWorldSpace(true);
		spriteNode->setVelocityAligned(true);
		beamNode->setWorldSpace(true);

		addSimulatedParticles(pointNode);
		addSimulatedParticles(spriteNode);
		addSimulatedParticles(beamNode);
	}
}

void Particles::addSimulatedParticles(ParticleNode::ptr particles){
	int i;
	for(i=0;i<particles->getNumParticles();++i){
		ParticleNode::Particle *p=particles->getParticle(i);
		p->vx=random.nextFloat(-4,4);
		p->vy=random.nextFloat(-4,4);
		p->vz=random.nextFloat(-4,4);
	}

	simulatedParticles.add(particles);
}

int toadletMain(int argc,char **argv){
	Particles app;
	app.create();
	app.start();
	app.destroy();
	return 0;
}
