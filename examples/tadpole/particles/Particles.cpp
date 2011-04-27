#include "Particles.h"

Particles::Particles():Application(){
}

Particles::~Particles(){
}

void Particles::create(){
	Application::create();

	mEngine->setDirectory("../../../data");

	scene=Scene::ptr(new Scene(mEngine));

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

	Material::ptr pointMaterial=mEngine->getMaterialManager()->findMaterial("sparkle.png");
	pointMaterial->setPointState(PointState(true,ten,false,Math::ONE,Math::ONE,Math::ONE,Math::ONE,Math::fromInt(100)));
	pointMaterial->setBlendState(BlendState::Combination_ALPHA_ADDITIVE);
	pointMaterial->setRasterizerState(RasterizerState(RasterizerState::CullType_NONE));
	pointMaterial->setDepthState(DepthState(DepthState::DepthTest_LEQUAL,false));

	Material::ptr beamMaterial=mEngine->getMaterialManager()->findMaterial("fancyGlow.png");
	beamMaterial->setBlendState(BlendState::Combination_ALPHA_ADDITIVE);
	beamMaterial->setRasterizerState(RasterizerState(RasterizerState::CullType_NONE));
	beamMaterial->setDepthState(DepthState(DepthState::DepthTest_LEQUAL,false));

 	pointNode=getEngine()->createNodeType(ParticleNode::type(),scene);
	pointNode->setNumParticles(4,ParticleNode::ParticleType_POINTSPRITE,Math::ONE,pointPositions);
	pointNode->setMaterial(pointMaterial);
	pointNode->setTranslate(-Math::fromInt(40),0,0);
	scene->getRoot()->attach(pointNode);

 	spriteNode=getEngine()->createNodeType(ParticleNode::type(),scene);
	spriteNode->setNumParticles(4,ParticleNode::ParticleType_SPRITE,Math::ONE,pointPositions);
	spriteNode->setMaterial(pointMaterial);
	spriteNode->setTranslate(0,0,0);
	scene->getRoot()->attach(spriteNode);

 	beamNode=getEngine()->createNodeType(ParticleNode::type(),scene);
	beamNode->setNumParticles(8,ParticleNode::ParticleType_BEAM,Math::ONE,beamPositions);
	beamNode->setMaterial(beamMaterial);
	beamNode->setTranslate(Math::fromInt(40),0,0);
	scene->getRoot()->attach(beamNode);

	cameraNode=getEngine()->createNodeType(CameraNode::type(),scene);
	cameraNode->setLookAt(Vector3(0,-Math::fromInt(150),0),Math::ZERO_VECTOR3,Math::Z_UNIT_VECTOR3);
	cameraNode->setClearColor(Colors::BLACK);
	scene->getRoot()->attach(cameraNode);
}

void Particles::destroy(){
	scene->destroy();

	Application::destroy();
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
	scalar edge=Math::fromInt(60);

	int i,j;
	for(i=0;i<simulatedParticles.size();++i){
		ParticleNode *particles=simulatedParticles[i];
		for(j=0;j<particles->getNumParticles();++j){
			ParticleNode::Particle *p=particles->getParticle(j);
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
	scalar v=Math::fromInt(4);
	int i;
	for(i=0;i<particles->getNumParticles();++i){
		ParticleNode::Particle *p=particles->getParticle(i);
		p->vx=random.nextScalar(-v,v);
		p->vy=random.nextScalar(-v,v);
		p->vz=random.nextScalar(-v,v);
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
