#include "SimpleSync.h"
#include <string.h>
#include <toadlet/egg/Error.h>
#include <toadlet/egg/io/FileStream.h>
#include <toadlet/egg/io/ZIPArchive.h>

class PingEvent:public Event{
public:
	TOADLET_SHARED_POINTERS(PingEvent);

	enum{
		EventType_PING=101
	};

	PingEvent():Event(EventType_PING),
		mLocalTime(0),
		mRemoteTime(0)
	{}
	PingEvent(int localTime,int remoteTime=0):Event(EventType_PING),
		mLocalTime(0),
		mRemoteTime(0)
	{
		mLocalTime=localTime;
		mRemoteTime=remoteTime;
	}

	int getLocalTime(){return mLocalTime;}
	int getRemoteTime(){return mRemoteTime;}

	int read(DataStream *stream){
		int amount=0;
		amount+=stream->readBigInt32(mLocalTime);
		amount+=stream->readBigInt32(mRemoteTime);
		return amount;
	}

	int write(DataStream *stream){
		int amount=0;
		amount+=stream->writeBigInt32(mLocalTime);
		amount+=stream->writeBigInt32(mRemoteTime);
		return amount;
	}

protected:
	int mLocalTime;
	int mRemoteTime;
};

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

	int read(DataStream *stream){return stream->readBigInt16(mCounter)+stream->readBigInt32(mDT);}
	int write(DataStream *stream){return stream->writeBigInt16(mCounter)+stream->writeBigInt32(mDT);}

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

	int read(DataStream *stream){return stream->readBigInt16(mLastCounter)+stream->readBigInt32(mTime);}
	int write(DataStream *stream){return stream->writeBigInt16(mLastCounter)+stream->writeBigInt32(mTime);}

protected:
	short mLastCounter;
	int mTime;
};


class ClientEvent:public ClientUpdateEvent{
public:
	TOADLET_SHARED_POINTERS(ClientEvent);

	enum{
		EventType_CLIENT=201
	};

	ClientEvent():ClientUpdateEvent(EventType_CLIENT){}
	ClientEvent(int dt,int counter,int flags):ClientUpdateEvent(counter,flags,EventType_CLIENT){
		mFlags=flags;
	}

	int getFlags(){return mFlags;}

	int read(DataStream *stream){
		int amount=ClientUpdateEvent::read(stream);
		amount+=stream->readBigInt32(mFlags);
		return amount;
	}

	int write(DataStream *stream){
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
		EventType_SERVER=202
	};

	ServerEvent():ServerUpdateEvent(EventType_SERVER){}
	ServerEvent(int time,int lastCounter):ServerUpdateEvent(EventType_SERVER,time,lastCounter){}

	void addUpdate(int id,const Vector3 &position,const Vector3 &velocity){
		mIDs.add(id);
		mPositions.add(position);
		mVelocitys.add(velocity);
	}

	int getNumUpdates(){return mIDs.size();}
	int getID(int i){return mIDs[i];}
	const Vector3 &getPosition(int i){return mPositions[i];}
	const Vector3 &getVelocity(int i){return mVelocitys[i];}

	int read(DataStream *stream){
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

	int write(DataStream *stream){
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
{
	playerMovement[0]=0;
	playerMovement[1]=0;
	predictedMovement.resize(1000,0);
	predictedMovementTime.resize(1000,-1);
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

	// Send initial ping message
	if(client!=NULL){
		PingEvent::ptr pingEvent(new PingEvent(scene->getLogicTime()));
		client->send(pingEvent);
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
		predictedMovementTime[0]=scene->getLogicTime();
		Event::ptr update(new ClientEvent(dt,scene->getLogicFrame(),predictedMovement[0]));
		client->send(update);
	}

	if(server!=NULL && nextUpdateTime<=scene->getLogicTime()){
		nextUpdateTime=scene->getLogicTime()+random.nextInt(debugUpdateMin,debugUpdateMax);

		EventConnection::ptr eventConnection=NULL;
		int i=0;
		for(eventConnection=server->getClient(0);eventConnection!=NULL;eventConnection=server->getClient(++i)){
			ServerEvent::ptr serverEvent(new ServerEvent(scene->getLogicTime(),lastReceivedPlayerMovement[i]));
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
//Logger::alert("postLogicUpdate");
	if(client!=NULL){
		Event::ptr event=NULL;
		while((event=client->receive())!=NULL){
			if(event->getType()==PingEvent::EventType_PING){
				PingEvent::ptr pingEvent=shared_static_cast<PingEvent>(event);

				// We now know the times for the client & server, and the ping, so we can calculate our 'lead ahead time'
				int oldClientTime=pingEvent->getRemoteTime();
				int newClientTime=scene->getLogicTime();
				int serverTime=pingEvent->getLocalTime();
				int ping=(newClientTime-oldClientTime);
				Logger::alert(String("ping:")+ping);

				clientServerTimeDifference=serverTime-newClientTime;
				clientLeadTime=ping/2;
Logger::alert(String("TD:")+(clientServerTimeDifference));
			}
			else if(event->getType()==ServerEvent::EventType_SERVER){
				ServerEvent::ptr serverEvent=shared_static_cast<ServerEvent>(event);

				predictedLastAcknowledgedCounter=serverEvent->getLastClientUpdateCounter();

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

				// Now simulate till we're back to where we need to be
//				int clientTime=scene->getLogicTime()+clientServerTimeDifference+0;//clientLeadTime*2;
//				int serverTime=updateEvent->getTime();
int clientTime=scene->getLogicTime();
int serverTime=predictedLastAcknowledgedTime;

				int updateDT=0;
				int minDT=scene->getLogicDT()!=0?scene->getLogicDT():10;
int timesToUpdate=(clientTime-serverTime)/minDT;
int i=0;
//Logger::alert(String("last ack:")+predictedLastAcknowledgedTime+" curr:"+scene->getLogicTime()+" TTU:"+timesToUpdate);
				for(updateDT=(int)Math::minVal(minDT,clientTime-serverTime);serverTime<clientTime;updateDT=(int)Math::minVal(minDT,clientTime-serverTime)){
for(int z=0;z<1000;++z){
	if(predictedMovementTime[z]==serverTime){
		updatePlayerMovement(predictedMovement[z],player[client->getClientID()]->getSolid());
	}
}
//Logger::alert(String("PT:")+predictedMovementTime[timesToUpdate-i]);
//Logger::alert(String("i:")+(timesToUpdate-i));
i++;
					for(u=0;u<serverEvent->getNumUpdates();++u){
						Solid::ptr solid=serverEvent->getID(u)>=0?player[updateEvent->getID(u)]->getSolid():block->getSolid();
						scene->getSimulator()->update(updateDT,solid);
					}
					serverTime+=updateDT;
				}
			}
		}

		memcpy(&predictedMovement[1],&predictedMovement[0],sizeof(int)*999);
		memcpy(&predictedMovementTime[1],&predictedMovementTime[0],sizeof(int)*999);
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
					lastReceivedPlayerMovement[updateEvent->getID()]=updateEvent->getTime();
updatePlayerMovement(playerMovement[updateEvent->getID()],player[i]->getSolid());
scene->getSimulator()->update(dt,player[updateEvent->getID()]->getSolid());
				}
			}
		}

//		for(i=0;i<2;++i){
//			updatePlayerMovement(playerMovement[i],player[i]->getSolid());
//		}
	}

if(server!=NULL){
scene->getSimulator()->update(dt,block->getSolid());

int i;
for(i=scene->getNumHopEntities()-1;i>=0;--i){
	HopEntity *entity=scene->getHopEntity(i);
	entity->postLogicUpdate(dt);
}
}
else{
	scene->postLogicUpdate(dt);
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
	if(server!=NULL){
		Logger::alert("Client connected");
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
