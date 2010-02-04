#include "SimpleSync.h"
#include <string.h>
#include <toadlet/egg/Error.h>
#include <toadlet/egg/io/FileStream.h>
#include <toadlet/egg/io/ZIPArchive.h>

class SyncEvent:public Event{
public:
	TOADLET_SHARED_POINTERS(SyncEvent);

	enum{
		EventType_SYNC=101
	};

	SyncEvent():Event(EventType_SYNC),
		playerID(-1),
		localTime(0),
		remoteTime(0)
	{}
	SyncEvent(int localTime):Event(EventType_SYNC),
		playerID(-1),
		localTime(0),
		remoteTime(0)
	{
		this->localTime=localTime;
	}

	void setPlayerID(int id){playerID=id;}
	int getPlayerID(){return playerID;}
	int getLocalTime(){return localTime;}
	void setLocalTime(int localTime){this->localTime=localTime;}
	int getRemoteTime(){return remoteTime;}
	void setRemoteTime(int remoteTime){this->remoteTime=remoteTime;}

	int read(DataStream *stream){
		int amount=0;
		amount+=stream->readBigInt32(playerID);
		amount+=stream->readBigInt32(localTime);
		amount+=stream->readBigInt32(remoteTime);
		return amount;
	}

	int write(DataStream *stream){
		int amount=0;
		amount+=stream->writeBigInt32(playerID);
		amount+=stream->writeBigInt32(localTime);
		amount+=stream->writeBigInt32(remoteTime);
		return amount;
	}

	String toString(){return String("localTime:")+localTime+",remoteTime:"+remoteTime;}

protected:
	int playerID;
	int localTime;
	int remoteTime;
};

class ClientUpdateEvent:public Event{
public:
	TOADLET_SHARED_POINTERS(ClientUpdateEvent);

	enum{
		EventType_CLIENTUPDATE=103
	};

	ClientUpdateEvent():Event(EventType_CLIENTUPDATE){}
	ClientUpdateEvent(int id,int movement):Event(EventType_CLIENTUPDATE){
		this->id=id;
		this->movement=movement;
	}

	int getID(){return id;}
	int getMovement(){return movement;}

	int read(DataStream *stream){
		int amount=0;
		amount+=stream->readBigInt32(id);
		amount+=stream->readBigInt32(movement);
		return amount;
	}

	int write(DataStream *stream){
		int amount=0;
		amount+=stream->writeBigInt32(id);
		amount+=stream->writeBigInt32(movement);
		return amount;
	}

protected:
	int id;
	int movement;
};

class UpdateEvent:public Event{
public:
	TOADLET_SHARED_POINTERS(UpdateEvent);

	enum{
		EventType_UPDATE=102
	};

	UpdateEvent():Event(EventType_UPDATE){}
	UpdateEvent(int time):Event(EventType_UPDATE){
		this->time=time;
	}

	void addUpdate(int id,const Vector3 &position,const Vector3 &velocity){
		ids.add(id);
		positions.add(position);
		velocities.add(velocity);
	}

	int getTime(){return time;}
	int getNumUpdates(){return ids.size();}
	int getID(int i){return ids[i];}
	const Vector3 &getPosition(int i){return positions[i];}
	const Vector3 &getVelocity(int i){return velocities[i];}

	int read(DataStream *stream){
		int amount=0;
		amount+=stream->readBigInt32(time);
		int numUpdates=0;
		amount+=stream->readBigInt32(numUpdates);
		ids.resize(numUpdates);
		positions.resize(numUpdates);
		velocities.resize(numUpdates);
		for(int i=0;i<numUpdates;++i){
			amount+=stream->readBigInt32(ids[i]);
			amount+=stream->readBigFloat(positions[i].x);
			amount+=stream->readBigFloat(positions[i].y);
			amount+=stream->readBigFloat(positions[i].z);
			amount+=stream->readBigFloat(velocities[i].x);
			amount+=stream->readBigFloat(velocities[i].y);
			amount+=stream->readBigFloat(velocities[i].z);
		}
		return amount;
	}

	int write(DataStream *stream){
		int amount=0;
		amount+=stream->writeBigInt32(time);
		amount+=stream->writeBigInt32(ids.size());
		for(int i=0;i<ids.size();++i){
			amount+=stream->writeBigInt32(ids[i]);
			amount+=stream->writeBigFloat(positions[i].x);
			amount+=stream->writeBigFloat(positions[i].y);
			amount+=stream->writeBigFloat(positions[i].z);
			amount+=stream->writeBigFloat(velocities[i].x);
			amount+=stream->writeBigFloat(velocities[i].y);
			amount+=stream->writeBigFloat(velocities[i].z);
		}
		return amount;
	}

protected:
	int time;
	Collection<int> ids;
	Collection<Vector3> positions;
	Collection<Vector3> velocities;
};

SimpleSync::SimpleSync():Application(),
	nextUpdateTime(0),
	clientServerTimeDifference(0),
	clientLeadTime(0),
	debugUpdateMin(0),
	debugUpdateMax(0),
	packetDelay(0),
	packetDelayVariance(0),
	localID(0),
	predictedTimeStart(0)
{
	playerMovement[0]=0;
	playerMovement[1]=0;
	predictedMovement.resize(100,0);
}

SimpleSync::~SimpleSync(){
}

void SimpleSync::accept(int localPort){
	TCPConnector::ptr connector(new TCPConnector());
	if(connector->accept(localPort)==false){
		Error::unknown("error accepting server connections");
		return;
	}

	connector->addConnectionListener(this,false);
	server=SimpleServer::ptr(new SimpleServer(this,connector));
//	debugUpdateMin=500;
//	debugUpdateMax=500;
}

void SimpleSync::connect(int remoteHost,int remotePort){
	TCPConnector::ptr connector(new TCPConnector());
	if(connector->connect(remoteHost,remotePort)==false){
		Error::unknown("error connecting to server");
		return;
	}

	connector->addConnectionListener(this,false);
	client=SimpleClient::ptr(new SimpleClient(this,connector));
//	shared_static_cast<TCPConnection>(client->getConnection())->debugSetPacketDelayTime(100,130);
}

void SimpleSync::create(){
	mutex.lock();

	Application::create();

	scene=HopScene::ptr(new HopScene(mEngine->createNodeType(SceneNode::type())));
	getEngine()->setScene(scene);
	scene->setUpdateListener(this);
	scene->setGravity(Vector3(0,0,-10));
	//scene->showCollisionVolumes(true,false);
	scene->getSimulator()->setMicroCollisionThreshold(5);
	if(client!=NULL){
		scene->setLogicDT(10);
	}
	else{
		scene->setLogicDT(10);
	}

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
		player[i]=getEngine()->createNodeType(HopEntity::type());
		player[i]->addShape(Shape::ptr(new Shape(AABox(-1,-1,0,1,1,4))));
		{
			Mesh::ptr mesh=getEngine()->getMeshManager()->createBox(player[i]->getShape(0)->getAABox());
			mesh->subMeshes[0]->material->setLightEffect(i==0?Colors::GREEN:Colors::BLUE);
			MeshNode::ptr meshNode=getEngine()->createNodeType(MeshNode::type());
			meshNode->setMesh(mesh);
			player[i]->attach(meshNode);
		}
		player[i]->setTranslate(0,i==0?-20:20,0);
		scene->getRootNode()->attach(player[i]);
	}

	mutex.unlock();

	// Send initial sync message
	if(client!=NULL){
		SyncEvent::ptr syncEvent(new SyncEvent(scene->getLogicTime()));
		Logger::alert(String("sending inital SyncEvent:")+syncEvent->toString());
		client->send(syncEvent);
	}
}

void SimpleSync::destroy(){
	if(client!=NULL){
		client->close();
	}

	if(server!=NULL){
		server->close();
	}

	Application::destroy();
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

	if(client!=NULL){
		client->update();
	}
	if(server!=NULL){
		server->update();
	}

	mutex.unlock();
}

void SimpleSync::preLogicUpdate(int dt){
	scene->preLogicUpdate(dt);

	if(client!=NULL){
		Event::ptr update(new ClientUpdateEvent(localID,predictedMovement[0]));
		client->send(update);
	}

	if(server!=NULL){
		if(nextUpdateTime<=scene->getLogicTime()){
			UpdateEvent::ptr updateEvent(new UpdateEvent(scene->getLogicTime()));
			updateEvent->addUpdate(-1,block->getPosition(),block->getVelocity());
			updateEvent->addUpdate(0,player[0]->getPosition(),player[0]->getVelocity());
			updateEvent->addUpdate(1,player[1]->getPosition(),player[1]->getVelocity());
			server->broadcast(updateEvent);

			nextUpdateTime=scene->getLogicTime()+random.nextInt(debugUpdateMin,debugUpdateMax);
		}
	}
}

void SimpleSync::logicUpdate(int dt){
	scene->logicUpdate(dt);
}

void updatePlayerMovement(int movement,Solid::ptr player){
	if(movement&(1<<0)){
		player->setVelocity(Vector3(0,10,0));
	}
	if(movement&(1<<1)){
		player->setVelocity(Vector3(0,-10,0));
	}
	if(movement&(1<<2)){
		player->setVelocity(Vector3(-10,0,0));
	}
	if(movement&(1<<3)){
		player->setVelocity(Vector3(10,0,0));
	}
}

void SimpleSync::postLogicUpdate(int dt){
	if(client!=NULL){
		Event::ptr event=NULL;
		while((event=client->receive())!=NULL){
			if(event->getType()==SyncEvent::EventType_SYNC){
				SyncEvent::ptr syncEvent=shared_static_cast<SyncEvent>(event);

				// We now know the times for the client & server, and the ping, so we can calculate our 'lead ahead time'
				int oldClientTime=syncEvent->getRemoteTime();
				int newClientTime=scene->getLogicTime();
				int serverTime=syncEvent->getLocalTime();
				int ping=(newClientTime-oldClientTime);
				Logger::alert(String("received SyncEvent:")+syncEvent->toString());
				Logger::alert(String("ping:")+ping);

				clientServerTimeDifference=serverTime-newClientTime;
				clientLeadTime=ping/2;
Logger::alert(String("TD:")+(clientServerTimeDifference));

				localID=syncEvent->getPlayerID();
Logger::alert(String("localID:")+(localID));
			}
			else if(event->getType()==UpdateEvent::EventType_UPDATE){
				UpdateEvent::ptr updateEvent=shared_static_cast<UpdateEvent>(event);

				int u=0;
				for(u=0;u<updateEvent->getNumUpdates();++u){
					Solid::ptr solid=updateEvent->getID(u)>=0?player[updateEvent->getID(u)]->getSolid():block->getSolid();
					if(solid!=NULL){
						// Update object
						solid->setPosition(updateEvent->getPosition(u));
						solid->setVelocity(updateEvent->getVelocity(u));
					}
				}

				// Now simulate till we're back to where we need to be
				int clientTime=scene->getLogicTime()+clientServerTimeDifference+20;//clientLeadTime;
				int serverTime=updateEvent->getTime();
				int updateDT=0;
				int minDT=scene->getLogicDT()!=0?scene->getLogicDT():10;
int timesToUpdate=(clientTime-serverTime)/minDT;
int i=0;
				for(updateDT=(int)Math::minVal(minDT,clientTime-serverTime);serverTime<clientTime;updateDT=(int)Math::minVal(minDT,clientTime-serverTime)){
updatePlayerMovement(predictedMovement[timesToUpdate-i],player[localID]->getSolid());
//Logger::alert(String("i:")+(timesToUpdate-i));
i++;
					for(u=0;u<updateEvent->getNumUpdates();++u){
						Solid::ptr solid=updateEvent->getID(u)>=0?player[updateEvent->getID(u)]->getSolid():block->getSolid();
						scene->getSimulator()->update(updateDT,solid);
					}
					serverTime+=updateDT;
				}
			}
		}

		memcpy(&predictedMovement[1],&predictedMovement[0],sizeof(int)*99);
	}

	if(server!=NULL){
		EventConnection::ptr eventConnection=NULL;
		int i=0;
		for(eventConnection=server->getClient(0);eventConnection!=NULL;eventConnection=server->getClient(++i)){
			Event::ptr event;
			while((event=eventConnection->receive())!=NULL){
				if(event->getType()==SyncEvent::EventType_SYNC){
					SyncEvent::ptr syncEvent=shared_static_cast<SyncEvent>(event);
					Logger::alert(String("received SyncEvent:")+syncEvent->toString());

					// Flip syncEvent to be from the server's view
					int clientTime=syncEvent->getLocalTime();
					syncEvent->setLocalTime(scene->getLogicTime());
					syncEvent->setRemoteTime(clientTime);
					syncEvent->setPlayerID(i);
					Logger::alert(String("ASSIGNING ID:")+i);

					Logger::alert(String("sending SyncEvent:")+syncEvent->toString());
					eventConnection->send(syncEvent);
				}
				else if(event->getType()==ClientUpdateEvent::EventType_CLIENTUPDATE){
					ClientUpdateEvent::ptr updateEvent=shared_static_cast<ClientUpdateEvent>(event);
					playerMovement[updateEvent->getID()]=updateEvent->getMovement();
				}
			}
		}

		for(i=0;i<2;++i){
			updatePlayerMovement(playerMovement[i],player[i]->getSolid());
		}
	}

	scene->postLogicUpdate(dt);
}

void SimpleSync::keyPressed(int key){
	if(key=='o'){
		packetDelay-=50;
	}
	else if(key=='p'){
		packetDelay+=50;
	}
	else if(key=='O'){
		packetDelayVariance-=50;
	}
	else if(key=='P'){
		packetDelayVariance+=50;
	}

	if(client!=NULL){
Logger::alert(String("packetDelay:")+packetDelay+" packetDelayVariance:"+packetDelayVariance);
		shared_static_cast<TCPConnection>(client->getConnection())->debugSetPacketDelayTime(packetDelay-packetDelayVariance/2,packetDelay+packetDelayVariance/2);
	}

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
	else if(key=='w'){
		predictedMovement[0]|=(1<<0);
	}
	else if(key=='s'){
		predictedMovement[0]|=(1<<1);
	}
	else if(key=='a'){
		predictedMovement[0]|=(1<<2);
	}
	else if(key=='d'){
		predictedMovement[0]|=(1<<3);
	}
}

void SimpleSync::keyReleased(int key){
	if(key=='w'){
		predictedMovement[0]&=~(1<<0);
	}
	else if(key=='s'){
		predictedMovement[0]&=~(1<<1);
	}
	else if(key=='a'){
		predictedMovement[0]&=~(1<<2);
	}
	else if(key=='d'){
		predictedMovement[0]&=~(1<<3);
	}
}

void SimpleSync::mousePressed(int x,int y,int button){
}

void SimpleSync::mouseMoved(int x,int y){
}

void SimpleSync::mouseReleased(int x,int y,int button){
}

Event::ptr SimpleSync::createEventType(int type){
	if(type==SyncEvent::EventType_SYNC){
		return Event::ptr(new SyncEvent());
	}
	else if(type==UpdateEvent::EventType_UPDATE){
		return Event::ptr(new UpdateEvent());
	}
	else if(type==ClientUpdateEvent::EventType_CLIENTUPDATE){
		return Event::ptr(new ClientUpdateEvent());
	}
	else{
		return Event::ptr(new Event());
	}
}

void SimpleSync::connected(Connection::ptr connection){
}

void SimpleSync::disconnected(Connection::ptr connection){
	stop();
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

	TOADLET_TRY
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
				localPort=String(s).toInt32();
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
					remotePort=String(p).toInt32();
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
	TOADLET_CATCH(const Exception &ex){}

	return 0;

}
#endif
