#include "SimpleSync.h"
#include <stdlib.h>
#include <toadlet/egg/io/FileStream.h>
#include <toadlet/egg/io/ZIPArchive.h>

class UpdateEvent:public Event{
public:
	TOADLET_SHARED_POINTERS(UpdateEvent);

	enum{
		EventType_UPDATE=101
	};

	UpdateEvent():Event(EventType_UPDATE){}
	UpdateEvent(int time,const Vector3 &position,const Vector3 &velocity):Event(EventType_UPDATE){
		this->time=time;
		this->position=position;
		this->velocity=velocity;
	}

	int getTime(){return time;}
	const Vector3 &getPosition(){return position;}
	const Vector3 &getVelocity(){return velocity;}

	int read(DataStream *stream){
		int amount=0;
		amount+=stream->readBigInt32(time);
		amount+=stream->readBigFloat(position.x);
		amount+=stream->readBigFloat(position.y);
		amount+=stream->readBigFloat(position.z);
		amount+=stream->readBigFloat(velocity.x);
		amount+=stream->readBigFloat(velocity.y);
		amount+=stream->readBigFloat(velocity.z);
		return amount;
	}

	int write(DataStream *stream){
		int amount=0;
		amount+=stream->writeBigInt32(time);
		amount+=stream->writeBigFloat(position.x);
		amount+=stream->writeBigFloat(position.y);
		amount+=stream->writeBigFloat(position.z);
		amount+=stream->writeBigFloat(velocity.x);
		amount+=stream->writeBigFloat(velocity.y);
		amount+=stream->writeBigFloat(velocity.z);
		return amount;
	}

protected:
	int time;
	Vector3 position;
	Vector3 velocity;
};

SimpleSync::SimpleSync():Application(),
	nextUpdateTime(0),
	debugUpdateMin(0),
	debugUpdateMax(0)
{
}

SimpleSync::~SimpleSync(){
}

void SimpleSync::accept(int localPort){
	server=SimpleServer::ptr(new SimpleServer(this,TCPConnector::ptr(new TCPConnector(localPort))));
	//debugUpdateMin=100;
	//debugUpdateMax=100;
}

void SimpleSync::connected(Connection::ptr connection){
}
void SimpleSync::disconnected(Connection::ptr connection){
}

void SimpleSync::connect(int remoteHost,int remotePort){
	client=SimpleClient::ptr(new SimpleClient(this,TCPConnector::ptr(new TCPConnector(remoteHost,remotePort))));
	//shared_static_cast<TCPConnection>(client->getConnection())->debugSetPacketDelayTime(100,500);
}

void SimpleSync::create(){
	mutex.lock();

	Application::create();

	scene=HopScene::ptr(new HopScene(mEngine->createNodeType(SceneNode::type())));
	getEngine()->setScene(scene);
	scene->setUpdateListener(this);
	scene->setGravity(Vector3(0,0,-10));
	scene->showCollisionVolumes(true,false);
	scene->getSimulator()->setMicroCollisionThreshold(5);
	scene->setLogicDT(50);
	//scene->setLogicDT(0);

	scene->getRootNode()->attach(getEngine()->createNodeType(LightNode::type()));

	cameraNode=getEngine()->createNodeType(CameraNode::type());
	cameraNode->setLookAt(Vector3(0,-Math::fromInt(50),Math::fromInt(25)),Math::ZERO_VECTOR3,Math::Z_UNIT_VECTOR3);
	cameraNode->setClearColor(Colors::BLUE);
	scene->getRootNode()->attach(cameraNode);

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
	scene->getRootNode()->attach(floor);

	block=getEngine()->createNodeType(HopEntity::type());
	block->addShape(Shape::ptr(new Shape(AABox(-1,-1,-1,1,1,1))));
	{
		Mesh::ptr mesh=getEngine()->getMeshManager()->createBox(block->getShape(0)->getAABox());
		mesh->subMeshes[0]->material->setLightEffect(Colors::RED);
		MeshNode::ptr meshNode=getEngine()->createNodeType(MeshNode::type());
		meshNode->setMesh(mesh);
		block->attach(meshNode);
	}
	block->setTranslate(0,0,10);
	scene->getRootNode()->attach(block);

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
		scene->getRootNode()->attach(player);
	}

	mutex.unlock();

	// Cheap way to initially synchronize
	if(client!=NULL){
		client->send(Event::ptr(new Event()));
		scene->setLogicTimeAndFrame(200,0);
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
	mutex.lock();

	renderer->beginScene();
		scene->render(renderer,cameraNode,NULL);
	renderer->endScene();
	renderer->swap();

	mutex.unlock();
}

void SimpleSync::update(int dt){
	mutex.lock();

	scene->update(dt);

	mutex.unlock();
}

void SimpleSync::preLogicUpdate(int dt){
	scene->preLogicUpdate(dt);
}

void SimpleSync::logicUpdate(int dt){
	scene->logicUpdate(dt);
}

void SimpleSync::postLogicUpdate(int dt){
	if(client!=NULL){
		Event::ptr event=NULL;
		while((event=client->receive())!=NULL){
			if(event->getType()==UpdateEvent::EventType_UPDATE){
				UpdateEvent::ptr update=shared_static_cast<UpdateEvent>(event);

				// Update block
				block->getSolid()->setPosition(update->getPosition());
				block->getSolid()->setVelocity(update->getVelocity());

				// Now simulate block till we're back to where we need to be
	Logger::alert(String("TIME:")+(scene->getLogicTime()-update->getTime()));
				int time=update->getTime();
				int updateDT=0;
				int minDT=scene->getLogicDT()!=0?scene->getLogicDT():10;
				do{
					updateDT=(int)Math::minVal(minDT,scene->getLogicTime()-time);
					scene->getSimulator()->update(updateDT);
					time+=updateDT;
				}while(updateDT>0);
			}
		}
	}

	if(server!=NULL){
		if(server->receive()!=NULL){
			scene->setLogicTimeAndFrame(0,0);
		}

		if(nextUpdateTime<=scene->getLogicTime()){
			Event::ptr update(new UpdateEvent(scene->getLogicTime(),block->getPosition(),block->getVelocity()));
			server->broadcast(update);
			nextUpdateTime=scene->getLogicDT()+random.nextInt(debugUpdateMin,debugUpdateMax);
		}
	}

	scene->postLogicUpdate(dt);
}

void SimpleSync::keyPressed(int key){
	if(key=='r'){
		block->setPosition(Vector3(0,0,10));
		block->setVelocity(Vector3(0,0,0));
		block->clearForce();
	}
	else if(key=='p'){
		block->setPosition(Vector3(random.nextScalar(-20,20),random.nextScalar(-20,20),random.nextScalar(0,20)));
	}
	else if(key=='v'){
		block->setVelocity(Vector3(random.nextScalar(-20,20),random.nextScalar(-20,20),random.nextScalar(-20,20)));
	}
	else if(key=='f'){
		block->addForce(Vector3(random.nextScalar(-20,20),random.nextScalar(-20,20),random.nextScalar(-20,20))*10);
	}
}

void SimpleSync::keyReleased(int key){
}

void SimpleSync::mousePressed(int x,int y,int button){
}

void SimpleSync::mouseMoved(int x,int y){
}

void SimpleSync::mouseReleased(int x,int y,int button){
}

Event::ptr SimpleSync::createEventType(int type){
	if(type==UpdateEvent::EventType_UPDATE){
		return Event::ptr(new UpdateEvent());
	}
	else{
		return Event::ptr(new Event());
	}
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
		if(client->client->getConnection()==NULL){
			Logger::alert("Error connecting");
			return -1;
		}
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
