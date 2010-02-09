#include "SimpleSync.h"
#include <string.h>
#include <toadlet/egg/Error.h>
#include <toadlet/egg/io/FileStream.h>
#include <toadlet/egg/io/ZIPArchive.h>

// This event is sent from the client each frame with the client dt and commands
class ClientUpdateEvent:public Event{
public:
	TOADLET_SHARED_POINTERS(ClientUpdateEvent);

	ClientUpdateEvent(int type):Event(type),mCounter(0),mDT(0){}
	ClientUpdateEvent(int type,int counter,int dt):Event(type){
		mCounter=counter;
		mDT=dt;
	}

	int getCounter(){return mCounter;}
	int getDT(){return mDT;}

	virtual int read(DataStream *stream){return stream->readBigInt16(mCounter)+stream->readBigInt32(mDT);}
	virtual int write(DataStream *stream){return stream->writeBigInt16(mCounter)+stream->writeBigInt32(mDT);}

protected:
	short mCounter;
	int mDT;
};

// This event is sent from the server occasionally to update information about objects or possibly confirm the receipt of a ClientUpdateEvent
class ServerUpdateEvent:public Event{
public:
	TOADLET_SHARED_POINTERS(ServerUpdateEvent);

	ServerUpdateEvent(int type):Event(type),mLastCounter(0),mTime(0){}
	ServerUpdateEvent(int type,int lastCounter,int time):Event(type){
		mLastCounter=lastCounter;
		mTime=time;
	}

	int getLastClientUpdateCounter(){return mLastCounter;}
	int getTime(){return mTime;}

	virtual int read(DataStream *stream){return stream->readBigInt16(mLastCounter)+stream->readBigInt32(mTime);}
	virtual int write(DataStream *stream){return stream->writeBigInt16(mLastCounter)+stream->writeBigInt32(mTime);}

protected:
	short mLastCounter;
	int mTime;
};



class ConnectionEvent:public Event{
public:
	TOADLET_SHARED_POINTERS(ConnectionEvent);

	enum{
		EventType_CONNECTION=204
	};

	ConnectionEvent():Event(EventType_CONNECTION){}
	ConnectionEvent(int id):Event(EventType_CONNECTION){
		mID=id;
	}

	int getID(){return mID;}

	virtual int read(DataStream *stream){
		int amount=0;
		amount+=stream->readBigInt32(mID);
		return amount;
	}

	virtual int write(DataStream *stream){
		int amount=0;
		amount+=stream->writeBigInt32(mID);
		return amount;
	}

protected:
	int mID;
};

class ClientEvent:public ClientUpdateEvent{
public:
	TOADLET_SHARED_POINTERS(ClientEvent);

	enum{
		EventType_CLIENT=202
	};

	ClientEvent():ClientUpdateEvent(EventType_CLIENT){}
	ClientEvent(int counter,int dt,int flags):ClientUpdateEvent(EventType_CLIENT,counter,dt){
		mFlags=flags;
	}

	int getFlags(){return mFlags;}

	virtual int read(DataStream *stream){
		int amount=ClientUpdateEvent::read(stream);
		amount+=stream->readBigInt32(mFlags);
		return amount;
	}

	virtual int write(DataStream *stream){
		int amount=ClientUpdateEvent::write(stream);
		amount+=stream->writeBigInt32(mFlags);
		return amount;
	}

protected:
	int mFlags;
};

class ServerEvent:public ServerUpdateEvent{
public:
	TOADLET_SHARED_POINTERS(ServerEvent);

	enum{
		EventType_SERVER=203
	};

	ServerEvent():ServerUpdateEvent(EventType_SERVER){}
	ServerEvent(int lastCounter,int time):ServerUpdateEvent(EventType_SERVER,lastCounter,time){}

	void addUpdate(int id,const Vector3 &position,const Vector3 &velocity){
		mIDs.add(id);
		mPositions.add(position);
		mVelocitys.add(velocity);
	}

	int getNumUpdates(){return mIDs.size();}
	int getID(int i){return mIDs[i];}
	const Vector3 &getPosition(int i){return mPositions[i];}
	const Vector3 &getVelocity(int i){return mVelocitys[i];}

	virtual int read(DataStream *stream){
		int amount=ServerUpdateEvent::read(stream);
		int numUpdates=0;
		amount+=stream->readBigInt32(numUpdates);
		mIDs.resize(numUpdates);
		mPositions.resize(numUpdates);
		mVelocitys.resize(numUpdates);
		for(int i=0;i<numUpdates;++i){
			amount+=stream->readBigInt32(mIDs[i]);
			amount+=stream->readBigFloat(mPositions[i].x);
			amount+=stream->readBigFloat(mPositions[i].y);
			amount+=stream->readBigFloat(mPositions[i].z);
			amount+=stream->readBigFloat(mVelocitys[i].x);
			amount+=stream->readBigFloat(mVelocitys[i].y);
			amount+=stream->readBigFloat(mVelocitys[i].z);
		}
		return amount;
	}

	virtual int write(DataStream *stream){
		int amount=ServerUpdateEvent::write(stream);
		amount+=stream->writeBigInt32(mIDs.size());
		for(int i=0;i<mIDs.size();++i){
			amount+=stream->writeBigInt32(mIDs[i]);
			amount+=stream->writeBigFloat(mPositions[i].x);
			amount+=stream->writeBigFloat(mPositions[i].y);
			amount+=stream->writeBigFloat(mPositions[i].z);
			amount+=stream->writeBigFloat(mVelocitys[i].x);
			amount+=stream->writeBigFloat(mVelocitys[i].y);
			amount+=stream->writeBigFloat(mVelocitys[i].z);
		}
		return amount;
	}

protected:
	Collection<int> mIDs;
	Collection<Vector3> mPositions;
	Collection<Vector3> mVelocitys;
};

SimpleSync::SimpleSync():Application(),
	nextUpdateTime(0),
	clientServerTimeDifference(0),
	clientLeadTime(0),
	debugUpdateMin(0),
	debugUpdateMax(0),
	packetDelay(0),
	packetDelayVariance(0),
	movement(0)
{
	lastClientUpdateCounter[0]=0;
	lastClientUpdateCounter[1]=0;
}

SimpleSync::~SimpleSync(){
}

void SimpleSync::accept(int localPort){
	TCPConnector::ptr connector(new TCPConnector());
	if(connector->accept(localPort)==false){
		Error::unknown("error accepting server connections");
		return;
	}

	server=SimpleServer::ptr(new SimpleServer(this,connector));
	connector->addConnectionListener(this,false);
//	debugUpdateMin=200;
//	debugUpdateMax=200;
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
		scene->setLogicDT(0);
	}
	else{
		scene->setLogicDT(0);
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

	playerCollision=1<<0;
	if(server!=NULL){
		player[0]->setCollisionBits(playerCollision);
		player[1]->setCollisionBits(playerCollision);
	}

	mutex.unlock();
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
		int currentFrame=scene->getLogicFrame()+1;
		ClientEvent::ptr clientEvent(new ClientEvent(currentFrame,dt,movement));
		sentClientEvents.add(clientEvent);
		client->send(clientEvent);
	}

	if(server!=NULL && nextUpdateTime<=scene->getLogicTime()){
		nextUpdateTime=scene->getLogicTime()+random.nextInt(debugUpdateMin,debugUpdateMax);

		EventConnection::ptr eventConnection=NULL;
		int i=0;
		for(eventConnection=server->getClient(0);eventConnection!=NULL;eventConnection=server->getClient(++i)){
			ServerEvent::ptr serverEvent(new ServerEvent(lastClientUpdateCounter[i],scene->getLogicTime()));
			serverEvent->addUpdate(-1,block->getPosition(),block->getVelocity());
			serverEvent->addUpdate(0,player[0]->getPosition(),player[0]->getVelocity());
			serverEvent->addUpdate(1,player[1]->getPosition(),player[1]->getVelocity());
			eventConnection->send(serverEvent);
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
	int i;

	if(client!=NULL){
		int eventStart=sentClientEvents.size()-1;

		Event::ptr event=NULL;
		while((event=client->receive())!=NULL){
			if(event->getType()==ConnectionEvent::EventType_CONNECTION){
				ConnectionEvent::ptr connectionEvent=shared_static_cast<ConnectionEvent>(event);
				client->setClientID(connectionEvent->getID());
				player[client->getClientID()]->setMass(0);
				player[client->getClientID()]->setCollisionBits(playerCollision);
				player[client->getClientID()]->getSolid()->setAlwaysActive(true);
			}
			else if(event->getType()==ServerEvent::EventType_SERVER){
				ServerEvent::ptr serverEvent=shared_static_cast<ServerEvent>(event);

				int u=0;
				for(u=0;u<serverEvent->getNumUpdates();++u){
					int id=serverEvent->getID(u);
					Solid::ptr solid=id>=0?player[id]->getSolid():block->getSolid();
					if(solid!=NULL){
						// Update object
						solid->setPosition(serverEvent->getPosition(u));
						solid->setVelocity(serverEvent->getVelocity(u));
					}
				}

				int clientTime=scene->getLogicTime();
				int serverTime=serverEvent->getTime();

				// If the client is behind the server, then we always jump up to be in sync
				if(clientTime<serverTime){
					clientServerTimeDifference=serverTime-clientTime;
				}
				else{
					int idealDifference=serverTime-clientTime;
					scalar differenceWeighting=Math::fromMilli(100);
					clientServerTimeDifference=Math::toMilli(Math::mul(Math::fromMilli(clientServerTimeDifference-idealDifference),differenceWeighting));
				}
				clientTime+=clientServerTimeDifference;

				// TODO: Looks like we need to have the server do some dead reckogning on other clients when no updates have been sent.
				//  These updates will just be temporary/visual only, and sent to other clients to keep the movement smooth on them & on the sever
				int updateDT=0;
				int minDT=scene->getLogicDT()!=0?scene->getLogicDT():10;
				for(updateDT=(int)Math::minVal(minDT,clientTime-serverTime);serverTime<clientTime;updateDT=(int)Math::minVal(minDT,clientTime-serverTime)){
					scene->getSimulator()->update(updateDT,~player[client->getClientID()]->getCollisionBits(),NULL);
					serverTime+=updateDT;
				}

				// Remove any old events
				int lastReceivedUpdateCounter=serverEvent->getLastClientUpdateCounter();
				while(sentClientEvents.size()>0 && sentClientEvents[0]->getCounter()<=lastReceivedUpdateCounter){
					sentClientEvents.removeAt(0);
				}

				eventStart=0;
			}
		}

		if(client->getClientID()>=0){
			for(i=eventStart;i<sentClientEvents.size();++i){
				updatePlayerMovement(sentClientEvents[i]->getFlags(),player[client->getClientID()]->getSolid());
				scene->getSimulator()->update(sentClientEvents[i]->getDT(),0,player[client->getClientID()]->getSolid());
			}
		}
	}

	if(server!=NULL){
		EventConnection::ptr eventConnection=NULL;
		int i=0;
		for(eventConnection=server->getClient(0);eventConnection!=NULL;eventConnection=server->getClient(++i)){
			Event::ptr event;
			while((event=eventConnection->receive())!=NULL){
				if(event->getType()==ClientEvent::EventType_CLIENT){
					ClientEvent::ptr clientEvent=shared_static_cast<ClientEvent>(event);
					lastClientUpdateCounter[i]=clientEvent->getCounter();
					updatePlayerMovement(clientEvent->getFlags(),player[i]->getSolid());
					scene->getSimulator()->update(clientEvent->getDT(),0,player[i]->getSolid());
				}
			}
		}
	}

	scene->getSimulator()->update(dt,~playerCollision,NULL);

	for(i=scene->getNumHopEntities()-1;i>=0;--i){
		HopEntity *entity=scene->getHopEntity(i);
		entity->postLogicUpdate(dt);
	}
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

	Logger::alert(String("packetDelay:")+packetDelay+" packetDelayVariance:"+packetDelayVariance);
	if(client!=NULL){
		shared_static_cast<TCPConnection>(client->getConnection())->debugSetPacketDelayTime(packetDelay-packetDelayVariance/2,packetDelay+packetDelayVariance/2);
	}
	else{
		EventConnection::ptr eventConnection=NULL;
		int i=0;
		for(eventConnection=server->getClient(0);eventConnection!=NULL;eventConnection=server->getClient(++i)){
			shared_static_cast<TCPConnection>(shared_static_cast<SimpleEventConnection>(eventConnection)->getConnection())->debugSetPacketDelayTime(packetDelay-packetDelayVariance/2,packetDelay+packetDelayVariance/2);
		}
	}

	if(client==NULL){
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
	else{
		if(key=='w'){
			movement|=(1<<0);
		}
		else if(key=='s'){
			movement|=(1<<1);
		}
		else if(key=='a'){
			movement|=(1<<2);
		}
		else if(key=='d'){
			movement|=(1<<3);
		}
	}
}

void SimpleSync::keyReleased(int key){
	if(key=='w'){
		movement&=~(1<<0);
	}
	else if(key=='s'){
		movement&=~(1<<1);
	}
	else if(key=='a'){
		movement&=~(1<<2);
	}
	else if(key=='d'){
		movement&=~(1<<3);
	}
}

void SimpleSync::mousePressed(int x,int y,int button){
}

void SimpleSync::mouseMoved(int x,int y){
}

void SimpleSync::mouseReleased(int x,int y,int button){
}

Event::ptr SimpleSync::createEventType(int type){
	if(type==ConnectionEvent::EventType_CONNECTION){
		return Event::ptr(new ConnectionEvent());
	}
	else if(type==ClientEvent::EventType_CLIENT){
		return Event::ptr(new ClientEvent());
	}
	else if(type==ServerEvent::EventType_SERVER){
		return Event::ptr(new ServerEvent());
	}
	else{
		return Event::ptr(new Event());
	}
}

void SimpleSync::connected(Connection::ptr connection){
	if(server!=NULL){
		Logger::alert("Client connected");

		EventConnection::ptr client=server->getClient(connection);
		int i=0;
		for(EventConnection::ptr eventConnection=server->getClient(0);client!=eventConnection;eventConnection=server->getClient(++i));

		client->send(Event::ptr(new ConnectionEvent(i)));
	}
}

void SimpleSync::disconnected(Connection::ptr connection){
	if(server!=NULL){
		Logger::alert("Client disconnected");
	}

	if(client!=NULL){
		stop();
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
