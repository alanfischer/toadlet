#include "SimpleSync.h"
#include <stdlib.h>

SimpleSync::SimpleSync():Application(){
}

SimpleSync::~SimpleSync(){
}

void SimpleSync::accept(int localPort){
	eventServer=EventServer::ptr(new SimpleEventServer(this,TCPConnector::ptr(new TCPConnector(localPort))));
}

void SimpleSync::connect(int remoteHost,int remotePort){
	eventClient=EventClient::ptr(new SimpleEventClient(this,TCPConnector::ptr(new TCPConnector(remoteHost,remotePort))));
}

void SimpleSync::create(){
	Application::create();

	HopScene::ptr scene(new HopScene(mEngine->createNodeType(SceneNode::type())));
	getEngine()->setScene(scene);
	scene->showCollisionVolumes(true,false);
	scene->setLogicDT(25);
	scene->setGravity(Vector3(0,0,-10));

	getEngine()->getScene()->getRootNode()->attach(getEngine()->createNodeType(LightNode::type()));

	cameraNode=getEngine()->createNodeType(CameraNode::type());
	cameraNode->setLookAt(Vector3(0,-Math::fromInt(50),Math::fromInt(25)),Math::ZERO_VECTOR3,Math::Z_UNIT_VECTOR3);
	cameraNode->setClearColor(Colors::BLUE);
	getEngine()->getScene()->getRootNode()->attach(cameraNode);

	HopEntity::ptr floor=getEngine()->createNodeType(HopEntity::type());
	floor->addShape(Shape::ptr(new Shape(AABox(-100,-100,-1,100,100,0))));
	{
		Mesh::ptr mesh=getEngine()->getMeshManager()->createBox(floor->getShape(0)->getAABox());
		mesh->subMeshes[0]->material->setLightEffect(Colors::GREY);
		MeshNode::ptr meshNode=getEngine()->createNodeType(MeshNode::type());
		meshNode->setMesh(mesh);
		floor->attach(meshNode);
	}
	floor->setInfiniteMass();
	floor->setLocalGravity(Math::ZERO_VECTOR3);
	getEngine()->getScene()->getRootNode()->attach(floor);

	HopEntity::ptr block=getEngine()->createNodeType(HopEntity::type());
	block->addShape(Shape::ptr(new Shape(AABox(-1,-1,-1,1,1,1))));
	{
		Mesh::ptr mesh=getEngine()->getMeshManager()->createBox(block->getShape(0)->getAABox());
		mesh->subMeshes[0]->material->setLightEffect(Colors::RED);
		MeshNode::ptr meshNode=getEngine()->createNodeType(MeshNode::type());
		meshNode->setMesh(mesh);
		block->attach(meshNode);
	}
	block->setTranslate(0,0,10);
	getEngine()->getScene()->getRootNode()->attach(block);

	int i;
	for(i=0;i<2;++i){
		HopEntity::ptr player=getEngine()->createNodeType(HopEntity::type());
		player->addShape(Shape::ptr(new Shape(AABox(-1,-1,0,1,1,4))));
		{
			Mesh::ptr mesh=getEngine()->getMeshManager()->createBox(player->getShape(0)->getAABox());
			mesh->subMeshes[0]->material->setLightEffect(i==0?Colors::GREEN:Colors::BLUE);
			MeshNode::ptr meshNode=getEngine()->createNodeType(MeshNode::type());
			meshNode->setMesh(mesh);
			player->attach(meshNode);
		}
		player->setTranslate(0,i==0?-20:20,0);
		getEngine()->getScene()->getRootNode()->attach(player);
	}
}

void SimpleSync::resized(int width,int height){
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

void SimpleSync::render(Renderer *renderer){
	renderer->beginScene();
		getEngine()->getScene()->render(renderer,cameraNode,NULL);
	renderer->endScene();
	renderer->swap();
}

void SimpleSync::update(int dt){
	getEngine()->getScene()->update(dt);
}

Event::ptr SimpleSync::createEventType(int type){
	return Event::ptr(new Event());
}

void SimpleSync::run(){
	create();

	start();

	destroy();
}

#if !defined(TOADLET_PLATFORM_OSX)
#if defined(TOADLET_PLATFORM_WINCE)
int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPTSTR lpCmdLine,int nCmdShow){
#else
int main(int argc,char **argv){
#endif
	uint32 defaultPort=20202;
	uint32 localPort=0;
	uint32 remoteHost=0,remotePort=0;

	if(argc<2){
		localPort=defaultPort;
		remoteHost=Socket::stringToIP("127.0.0.1");
		remotePort=defaultPort;
	}
	else if(strncmp(argv[1],"-s",2)==0){
		char *s=argv[1]+2;
		if(strlen(s)==0){
			localPort=defaultPort;
		}
		else{
			localPort=atoi(s);
		}
	}
	else if(strncmp(argv[1],"-c",2)==0){
		char *c=argv[1]+2;

		if(strlen(c)==0){
			remoteHost=Socket::stringToIP("127.0.0.1");
			remotePort=defaultPort;
		}
		else{
			char *p=strchr(c,':');
			if(p==NULL){
				remoteHost=Socket::stringToIP(c);
				remotePort=defaultPort;
			}
			else{
				*p=NULL;
				p++;

				if(strlen(c)==0){
					remoteHost=Socket::stringToIP("127.0.0.1");
				}
				else{
					remoteHost=Socket::stringToIP(c);
				}
				remotePort=atoi(p);
			}
		}
	}

	SimpleSync::ptr server;
	Thread::ptr serverThread;
	SimpleSync::ptr client;
	Thread::ptr clientThread;

	if(localPort!=0){
		server=SimpleSync::ptr(new SimpleSync());
		server->accept(localPort);
		server->setPosition(0,0);
		server->setSize(640,480);
		serverThread=Thread::ptr(new Thread(server));
		serverThread->start();
	}
	if(remotePort!=0){
		client=SimpleSync::ptr(new SimpleSync());
		client->connect(remoteHost,remotePort);
		if(server==NULL){
			client->setPosition(0,0);
		}
		else{
			client->setPosition(640,0);
		}
		client->setSize(640,480);
		clientThread=Thread::ptr(new Thread(client));
		clientThread->start();
	}

	while((serverThread!=NULL && serverThread->isAlive()) || clientThread!=NULL && clientThread->isAlive()){
		System::msleep(100);
	}

	return 0;

}
#endif
