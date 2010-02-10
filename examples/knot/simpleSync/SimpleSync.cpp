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
	ConnectionEvent(int id,const Quaternion &look):Event(EventType_CONNECTION){
		mID=id;
		mLook=look;
	}

	int getID(){return mID;}
	const Quaternion &getLook(){return mLook;}

	virtual int read(DataStream *stream){
		int amount=0;
		amount+=stream->readBigInt32(mID);

		amount+=stream->readBigFloat(mLook.x);
		amount+=stream->readBigFloat(mLook.y);
		amount+=stream->readBigFloat(mLook.z);
		amount+=stream->readBigFloat(mLook.w);
		return amount;
	}

	virtual int write(DataStream *stream){
		int amount=0;
		amount+=stream->writeBigInt32(mID);

		amount+=stream->writeBigFloat(mLook.x);
		amount+=stream->writeBigFloat(mLook.y);
		amount+=stream->writeBigFloat(mLook.z);
		amount+=stream->writeBigFloat(mLook.w);
		return amount;
	}

protected:
	int mID;
	Quaternion mLook;
};

class ClientEvent:public ClientUpdateEvent{
public:
	TOADLET_SHARED_POINTERS(ClientEvent);

	enum{
		EventType_CLIENT=202
	};

	ClientEvent():ClientUpdateEvent(EventType_CLIENT){}
	ClientEvent(int counter,int dt,int flags,const Quaternion &look):ClientUpdateEvent(EventType_CLIENT,counter,dt){
		mFlags=flags;
		mLook=look;
	}

	int getFlags(){return mFlags;}
	const Quaternion &getLook(){return mLook;}

	virtual int read(DataStream *stream){
		int amount=ClientUpdateEvent::read(stream);
		amount+=stream->readBigInt32(mFlags);

		amount+=stream->readBigFloat(mLook.x);
		amount+=stream->readBigFloat(mLook.y);
		amount+=stream->readBigFloat(mLook.z);
		amount+=stream->readBigFloat(mLook.w);
		return amount;
	}

	virtual int write(DataStream *stream){
		int amount=ClientUpdateEvent::write(stream);
		amount+=stream->writeBigInt32(mFlags);

		amount+=stream->writeBigFloat(mLook.x);
		amount+=stream->writeBigFloat(mLook.y);
		amount+=stream->writeBigFloat(mLook.z);
		amount+=stream->writeBigFloat(mLook.w);
		return amount;
	}

protected:
	int mFlags;
	Quaternion mLook;
};

class ServerEvent:public ServerUpdateEvent{
public:
	TOADLET_SHARED_POINTERS(ServerEvent);

	enum{
		EventType_SERVER=203
	};

	ServerEvent():ServerUpdateEvent(EventType_SERVER){}
	ServerEvent(int lastCounter,int time):ServerUpdateEvent(EventType_SERVER,lastCounter,time){}

	void addUpdate(int id,const Vector3 &position,const Quaternion &rotation,const Vector3 &velocity){
		mIDs.add(id);
		mPositions.add(position);
		mRotations.add(rotation);
		mVelocitys.add(velocity);
	}

	int getNumUpdates(){return mIDs.size();}
	int getID(int i){return mIDs[i];}
	const Vector3 &getPosition(int i){return mPositions[i];}
	const Quaternion &getRotation(int i){return mRotations[i];}
	const Vector3 &getVelocity(int i){return mVelocitys[i];}

	virtual int read(DataStream *stream){
		int amount=ServerUpdateEvent::read(stream);
		int numUpdates=0;
		amount+=stream->readBigInt32(numUpdates);
		mIDs.resize(numUpdates);
		mPositions.resize(numUpdates);
		mRotations.resize(numUpdates);
		mVelocitys.resize(numUpdates);
		for(int i=0;i<numUpdates;++i){
			amount+=stream->readBigInt32(mIDs[i]);
			amount+=stream->readBigFloat(mPositions[i].x);
			amount+=stream->readBigFloat(mPositions[i].y);
			amount+=stream->readBigFloat(mPositions[i].z);

			amount+=stream->readBigFloat(mRotations[i].x);
			amount+=stream->readBigFloat(mRotations[i].y);
			amount+=stream->readBigFloat(mRotations[i].z);
			amount+=stream->readBigFloat(mRotations[i].w);

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

			amount+=stream->writeBigFloat(mRotations[i].x);
			amount+=stream->writeBigFloat(mRotations[i].y);
			amount+=stream->writeBigFloat(mRotations[i].z);
			amount+=stream->writeBigFloat(mRotations[i].w);

			amount+=stream->writeBigFloat(mVelocitys[i].x);
			amount+=stream->writeBigFloat(mVelocitys[i].y);
			amount+=stream->writeBigFloat(mVelocitys[i].z);
		}
		return amount;
	}

protected:
	Collection<int> mIDs;
	Collection<Vector3> mPositions;
	Collection<Quaternion> mRotations;
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
	flags(0),
	looking(false)
{
	lastClientUpdateCounter[0]=0;
	lastClientUpdateCounter[1]=0;
changeRendererPlugin(RendererPlugin_DIRECT3D9);
}

enum{
	Move_FORE=1<<0,
	Move_BACK=1<<1,
	Move_RIGHT=1<<2,
	Move_LEFT=1<<3,
	Move_JUMP=1<<4,
};

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
	scene->setGravity(Vector3(0,0,-500));
	//scene->showCollisionVolumes(true,false);
	scene->getSimulator()->setMicroCollisionThreshold(5);
	scene->setLogicDT(10);

	scene->getRootNode()->attach(getEngine()->createNodeType(LightNode::type()));

	cameraNode=getEngine()->createNodeType(CameraNode::type());
	cameraNode->setLookAt(Vector3(0,-Math::fromInt(500),Math::fromInt(250)),Math::ZERO_VECTOR3,Math::Z_UNIT_VECTOR3);
	cameraNode->setClearColor(Colors::BLUE);
	scene->getRootNode()->attach(cameraNode);

	HopEntity::ptr floor=getEngine()->createNodeType(HopEntity::type());
	floor->addShape(Shape::ptr(new Shape(AABox(-1000,-1000,-1,1000,1000,0))));
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
	block->addShape(Shape::ptr(new Shape(AABox(-8,-8,-8,8,8,8))));
	{
		Mesh::ptr mesh=getEngine()->getMeshManager()->createBox(block->getShape(0)->getAABox());
		mesh->subMeshes[0]->material->setLightEffect(Colors::RED);
		MeshNode::ptr meshNode=getEngine()->createNodeType(MeshNode::type());
		meshNode->setMesh(mesh);
		block->attach(meshNode);
	}
	block->setTranslate(0,0,32);
	scene->getRootNode()->attach(block);

	int i;
	for(i=0;i<2;++i){
		player[i]=getEngine()->createNodeType(HopEntity::type());
		player[i]->setCoefficientOfRestitution(0);
		player[i]->setCoefficientOfRestitutionOverride(true);
		player[i]->addShape(Shape::ptr(new Shape(AABox(-16,-16,-32,16,16,32))));
		{
			Mesh::ptr mesh=getEngine()->getMeshManager()->createBox(player[i]->getShape(0)->getAABox());
			mesh->subMeshes[0]->material->setLightEffect(i==0?Colors::GREEN:Colors::CYAN);
			MeshNode::ptr meshNode=getEngine()->createNodeType(MeshNode::type());
			meshNode->setMesh(mesh);
			player[i]->attach(meshNode);
		}
		player[i]->setTranslate(0,i==0?-20:20,32);
		scene->getRootNode()->attach(player[i]);
	}

	playerCollision=1<<0;
	if(server!=NULL){
		player[0]->setCollisionBits(playerCollision);
		player[0]->setRotate(0,0,Math::ONE,0);
		player[1]->setCollisionBits(playerCollision);
		player[1]->setRotate(0,0,Math::ONE,Math::PI);
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
			cameraNode->setProjectionFovY(Math::degToRad(Math::fromInt(75)),Math::div(Math::fromInt(width),Math::fromInt(height)),Math::fromMilli(100),Math::fromInt(1000));
		}
		else{
			cameraNode->setProjectionFovX(Math::degToRad(Math::fromInt(75)),Math::div(Math::fromInt(height),Math::fromInt(width)),Math::fromMilli(100),Math::fromInt(1000));
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
		ClientEvent::ptr clientEvent(new ClientEvent(currentFrame,dt,flags,player[client->getClientID()]->getRotate()));
		sentClientEvents.add(clientEvent);
		client->send(clientEvent);
	}

	if(server!=NULL && nextUpdateTime<=scene->getLogicTime()){
		nextUpdateTime=scene->getLogicTime()+random.nextInt(debugUpdateMin,debugUpdateMax);

		EventConnection::ptr eventConnection=NULL;
		int i=0;
		for(eventConnection=server->getClient(0);eventConnection!=NULL;eventConnection=server->getClient(++i)){
			ServerEvent::ptr serverEvent(new ServerEvent(lastClientUpdateCounter[i],scene->getLogicTime()));
			serverEvent->addUpdate(-1,block->getSolid()->getPosition(),block->getRotate(),block->getVelocity());
			serverEvent->addUpdate(0,player[0]->getSolid()->getPosition(),player[0]->getRotate(),player[0]->getVelocity());
			serverEvent->addUpdate(1,player[1]->getSolid()->getPosition(),player[1]->getRotate(),player[1]->getVelocity());
			eventConnection->send(serverEvent);
		}
	}
}

void updatePlayer(const ClientEvent::ptr &event,HopEntity::ptr entity){
	entity->setRotate(event->getLook());

	int flags=event->getFlags();
	Vector3 velocity;
	if((flags&Move_FORE)>0){
		Math::add(velocity,Math::Y_UNIT_VECTOR3);
	}
	if((flags&Move_BACK)>0){
		Math::add(velocity,Math::NEG_Y_UNIT_VECTOR3);
	}
	if((flags&Move_LEFT)>0){
		Math::add(velocity,Math::NEG_X_UNIT_VECTOR3);
	}
	if((flags&Move_RIGHT)>0){
		Math::add(velocity,Math::X_UNIT_VECTOR3);
	}

	Math::normalizeCarefully(velocity,0);
	Math::mul(velocity,Math::fromInt(200));
//	Math::mul(velocity,mPitch);
	Math::mul(velocity,entity->getRotate());
	scalar z=entity->getVelocity().z;
	Vector3 result;
	Math::lerp(result,entity->getVelocity(),velocity,0.5f);
	result.z=z;

	if((flags&Move_JUMP)>0 && entity->getSolid()->getTouching()!=NULL && entity->getSolid()->getTouchingNormal().z>Math::HALF){
		result.z=250;
	}
	entity->setVelocity(result);
}

void SimpleSync::logicUpdate(int dt){
	int i;

	// Skip HopScene's logicUpdate
	scene->getRootNode()->logicUpdate(dt);
Quaternion forcelook;bool force=false;
	if(client!=NULL){
		int eventStart=sentClientEvents.size()-1;

		Event::ptr event=NULL;
		while((event=client->receive())!=NULL){
			if(event->getType()==ConnectionEvent::EventType_CONNECTION){
				ConnectionEvent::ptr connectionEvent=shared_static_cast<ConnectionEvent>(event);
				client->setClientID(connectionEvent->getID());
				player[client->getClientID()]->setRotate(connectionEvent->getLook());
forcelook=connectionEvent->getLook();force=true;
				player[client->getClientID()]->setCollisionBits(playerCollision);
				player[client->getClientID()]->getSolid()->setAlwaysActive(true);

				player[client->getClientID()]->attach(cameraNode);
				cameraNode->setLookDir(Vector3(0,0,4),Math::Y_UNIT_VECTOR3,Math::Z_UNIT_VECTOR3);
			}
			else if(event->getType()==ServerEvent::EventType_SERVER){
				ServerEvent::ptr serverEvent=shared_static_cast<ServerEvent>(event);

				int u=0;
				for(u=0;u<serverEvent->getNumUpdates();++u){
					int id=serverEvent->getID(u);
					HopEntity::ptr entity=id>=0?player[id]:block;
					if(entity!=NULL){
						// Update object
						// TODO: We have to set the Solid's Position here, instead of the Entity's.  Figure out why that is.
						if(entity==player[client->getClientID()]){
							entity->getSolid()->setPositionDirect(serverEvent->getPosition(u)); // We want to retain our touching info
						}
						else{
							entity->getSolid()->setPosition(serverEvent->getPosition(u));
							entity->setRotate(serverEvent->getRotation(u));
						}
						entity->setVelocity(serverEvent->getVelocity(u));
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

				// TODO: We seem to still be simulating "farther ahead" than we should.  Look at this more and get us to simulate just as much as needed
				int updateDT=0;
				int minDT=scene->getLogicDT()!=0?scene->getLogicDT():10;
				for(updateDT=(int)Math::minVal(minDT,clientTime-serverTime);serverTime<clientTime;updateDT=(int)Math::minVal(minDT,clientTime-serverTime)){
					scene->getSimulator()->update(updateDT,~player[client->getClientID()]->getCollisionBits(),NULL);
					serverTime+=updateDT;
				}

				// Remove any old events
				int lastReceivedUpdateCounter=serverEvent->getLastClientUpdateCounter();
				while(sentClientEvents.size()>1 && sentClientEvents[0]->getCounter()<=lastReceivedUpdateCounter){
					sentClientEvents.removeAt(0);
				}

				eventStart=0;
			}
		}

		// TODO: Ideally we would somehow "ghost" the player so he wouldnt impart any forces on anything he touches
		//  We can't simply set his Mass to 0, because that affects friction calculations
		if(client->getClientID()>=0){
			for(i=eventStart;i<sentClientEvents.size();++i){
				updatePlayer(sentClientEvents[i],player[client->getClientID()]);
				scene->getSimulator()->update(sentClientEvents[i]->getDT(),0,player[client->getClientID()]->getSolid());
			}
		}

if(force){player[i]->setRotate(forcelook);}
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
					updatePlayer(clientEvent,player[i]);
					scene->getSimulator()->update(clientEvent->getDT(),0,player[i]->getSolid());
				}
			}
		}
	}

	scene->getSimulator()->update(dt,~playerCollision,NULL);
}

void SimpleSync::postLogicUpdate(int dt){
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
			flags|=Move_FORE;
		}
		else if(key=='s'){
			flags|=Move_BACK;
		}
		else if(key=='a'){
			flags|=Move_LEFT;
		}
		else if(key=='d'){
			flags|=Move_RIGHT;
		}
		else if(key==' '){
			flags|=Move_JUMP;
		}
	}
}

void SimpleSync::keyReleased(int key){
	if(key=='w'){
		flags&=~Move_FORE;
	}
	else if(key=='s'){
		flags&=~Move_BACK;
	}
	else if(key=='a'){
		flags&=~Move_LEFT;
	}
	else if(key=='d'){
		flags&=~Move_RIGHT;
	}
	else if(key==' '){
		flags&=~Move_JUMP;
	}
}

void SimpleSync::mousePressed(int x,int y,int button){
	if(button==2){
		looking=true;
		lastX=x;
		lastY=y;
	}
}

void SimpleSync::mouseMoved(int x,int y){
	if(looking){
		if(client!=NULL){
			look.y+=Math::fromMilli(lastY-y)*4;
			look.z+=Math::fromMilli(lastX-x)*4;
			Quaternion q1,q2;
			Math::setQuaternionFromAxisAngle(q1,Math::X_UNIT_VECTOR3,look.y);
			Math::setQuaternionFromAxisAngle(q2,Math::Z_UNIT_VECTOR3,look.z);
			Math::preMul(q1,q2);
			player[client->getClientID()]->setRotate(q1);
			lastX=x;
			lastY=y;
		}
	}
}

void SimpleSync::mouseReleased(int x,int y,int button){
	if(button==2){
		looking=false;
	}
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

		client->send(Event::ptr(new ConnectionEvent(i,player[i]!=NULL?player[i]->getRotate():Math::IDENTITY_QUATERNION)));
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
