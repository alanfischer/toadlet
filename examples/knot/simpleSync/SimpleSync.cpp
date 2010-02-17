#include "SimpleSync.h"
#include <string.h>
#include <toadlet/egg/Error.h>
#include <toadlet/egg/io/FileStream.h>
#include <toadlet/egg/io/ZIPArchive.h>
#include <toadlet/egg/Extents.h>

class ConnectionEvent:public BaseConnectionEvent{
public:
	TOADLET_SHARED_POINTERS(ConnectionEvent);

	enum{
		EventType_CONNECTION=204
	};

	ConnectionEvent():BaseConnectionEvent(EventType_CONNECTION){}
	ConnectionEvent(int id,const EulerAngle &look):BaseConnectionEvent(EventType_CONNECTION,id){
		mLook=look;
	}

	const EulerAngle &getLook(){return mLook;}

	virtual int read(DataStream *stream){
		int amount=BaseConnectionEvent::read(stream);
		amount+=stream->readBigFloat(mLook.x);
		amount+=stream->readBigFloat(mLook.y);
		amount+=stream->readBigFloat(mLook.z);
		return amount;
	}

	virtual int write(DataStream *stream){
		int amount=BaseConnectionEvent::write(stream);
		amount+=stream->writeBigFloat(mLook.x);
		amount+=stream->writeBigFloat(mLook.y);
		amount+=stream->writeBigFloat(mLook.z);
		return amount;
	}

protected:
	EulerAngle mLook;
};

class ClientUpdateEvent:public BaseClientUpdateEvent{
public:
	TOADLET_SHARED_POINTERS(ClientUpdateEvent);

	enum{
		EventType_CLIENTUPDATE=202
	};

	ClientUpdateEvent():BaseClientUpdateEvent(EventType_CLIENTUPDATE){}
	ClientUpdateEvent(int counter,int dt,int flags,const EulerAngle &look):BaseClientUpdateEvent(EventType_CLIENTUPDATE,counter,dt){
		mFlags=flags;
		mLook=look;
	}

	int getFlags(){return mFlags;}
	const EulerAngle &getLook(){return mLook;}

	virtual int read(DataStream *stream){
		int amount=BaseClientUpdateEvent::read(stream);
		amount+=stream->readBigInt32(mFlags);

		amount+=stream->readBigFloat(mLook.x);
		amount+=stream->readBigFloat(mLook.y);
		amount+=stream->readBigFloat(mLook.z);
		return amount;
	}

	virtual int write(DataStream *stream){
		int amount=BaseClientUpdateEvent::write(stream);
		amount+=stream->writeBigInt32(mFlags);

		amount+=stream->writeBigFloat(mLook.x);
		amount+=stream->writeBigFloat(mLook.y);
		amount+=stream->writeBigFloat(mLook.z);
		return amount;
	}

protected:
	int mFlags;
	EulerAngle mLook;
};

class ServerUpdateEvent:public BaseServerUpdateEvent{
public:
	TOADLET_SHARED_POINTERS(ServerUpdateEvent);

	enum{
		EventType_SERVERUPDATE=203
	};

	ServerUpdateEvent():BaseServerUpdateEvent(EventType_SERVERUPDATE){}
	ServerUpdateEvent(int lastCounter,int time):BaseServerUpdateEvent(EventType_SERVERUPDATE,lastCounter,time){}

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
		int amount=BaseServerUpdateEvent::read(stream);
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
		int amount=BaseServerUpdateEvent::write(stream);
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

class Player:public HopEntity{
public:
	Player():HopEntity(){}
	
	Node *create(Engine *engine){HopEntity::create(engine);setReceiveUpdates(true);return this;}
	
	void logicUpdate(int dt){}
};

SimpleSync::SimpleSync():Application(),
	nextUpdateTime(0),
	clientServerTimeDifference(0),
	debugUpdateMin(0),
	debugUpdateMax(0),
	packetDelay(0),
	packetDelayVariance(0),
	flags(0)
{
	lastClientUpdateCounter[0]=0;
	lastClientUpdateCounter[1]=0;
//changeRendererPlugin(RendererPlugin_DIRECT3D9);
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
	debugUpdateMin=50;
	debugUpdateMax=100;
}

void SimpleSync::connect(int remoteHost,int remotePort){
	TCPConnector::ptr connector(new TCPConnector());
	if(connector->connect(remoteHost,remotePort)==false){
		Error::unknown("error connecting to server");
		return;
	}

	connector->addConnectionListener(this,false);
	client=SimplePredictedClient::ptr(new SimplePredictedClient(this,connector));
//	shared_static_cast<TCPConnection>(client->getConnection())->debugSetPacketDelayTime(100,130);
}

void SimpleSync::create(){
	Application::create();

	scene=HopScene::ptr(new HopScene(mEngine->createNodeType(SceneNode::type())));
	getEngine()->setScene(scene);
	scene->setUpdateListener(this);
	scene->setGravity(Vector3(0,0,-500));
	//scene->showCollisionVolumes(true,false);
	scene->getSimulator()->setMicroCollisionThreshold(250);
	if(client!=NULL){
		scene->setLogicDT(20);
	}
	else{
		scene->setLogicDT(20);
	}

	scene->getRootNode()->attach(getEngine()->createNodeType(LightNode::type()));

	cameraNode=getEngine()->createNodeType(CameraNode::type());
	cameraNode->setLookAt(Vector3(0,-Math::fromInt(500),Math::fromInt(250)),Math::ZERO_VECTOR3,Math::Z_UNIT_VECTOR3);
	cameraNode->setClearColor(Colors::BLUE);
	cameraNode->setName("camera");
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
		player[i]=(HopEntity*)(new Player())->create(getEngine());//getEngine()->createNodeType(HopEntity::type());
		player[i]->setCoefficientOfRestitution(0);
		player[i]->setCoefficientOfRestitutionOverride(true);
		player[i]->addShape(Shape::ptr(new Shape(AABox(-16,-16,0,16,16,64))));
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

	playerScope=1<<0;
	if(server!=NULL){
		player[0]->setRotate(0,0,Math::ONE,0);
		player[1]->setRotate(0,0,Math::ONE,Math::PI);
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
			cameraNode->setProjectionFovY(Math::degToRad(Math::fromInt(75)),Math::div(Math::fromInt(width),Math::fromInt(height)),Math::fromMilli(100),Math::fromInt(1000));
		}
		else{
			cameraNode->setProjectionFovX(Math::degToRad(Math::fromInt(75)),Math::div(Math::fromInt(height),Math::fromInt(width)),Math::fromMilli(100),Math::fromInt(1000));
		}
		cameraNode->setViewport(Viewport(0,0,width,height));
	}
}

void SimpleSync::render(Renderer *renderer){
	renderer->beginScene();
		scene->render(renderer,cameraNode,NULL);
	renderer->endScene();
	renderer->swap();
}

void SimpleSync::updatePlayerView(const EulerAngle &look,HopEntity::ptr entity){
	Quaternion playerRotate,cameraRotate;
	EulerAngle angle;
	Math::setQuaternionFromEulerAngleXYZ(playerRotate,angle.set(0,look.y,0));
	Math::setQuaternionFromEulerAngleXYZ(cameraRotate,angle.set(0,0,look.z+Math::HALF_PI));
	entity->setRotate(playerRotate);
	CameraNode *camera=(CameraNode*)entity->findNodeByName("camera");
	if(camera!=NULL){
		camera->setRotate(cameraRotate);
	}
}

void SimpleSync::updatePlayer(Event::ptr event,HopEntity::ptr entity){
	ClientUpdateEvent::ptr clientEvent=shared_static_cast<ClientUpdateEvent>(event);
	updatePlayerView(clientEvent->getLook(),entity);

	int flags=clientEvent->getFlags();
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
	Math::mul(velocity,entity->getRotate());
	scalar z=entity->getVelocity().z;
	Vector3 result;
	Math::lerp(result,entity->getVelocity(),velocity,0.5f);
	result.z=z;

	// TODO: Fix the hop touching when manually running the simulation so we can use the "touching" information here if desired
	Collision c;
	Segment path; path.setStartEnd(entity->getSolid()->getPosition(),entity->getSolid()->getPosition()+Vector3(0,0,-1));
	entity->getScene()->getSimulator()->traceSolid(c,entity->getSolid(),path);
	if((flags&Move_JUMP)>0 && c.time>=0){
		result.z=250;
	}
	entity->setVelocity(result);
}

void SimpleSync::update(int dt){
	mutex.lock();
	mutex.unlock();

	scene->update(dt);
}

void SimpleSync::intraUpdate(int dt){
	int i;

	if(client!=NULL){
		Event::ptr event=NULL;
		while((event=client->receive())!=NULL){
			if(event->getType()==ConnectionEvent::EventType_CONNECTION){
				ConnectionEvent::ptr connectionEvent=shared_static_cast<ConnectionEvent>(event);

				client->handleConnectionEvent(connectionEvent);

				player[client->getClientID()]->setCollisionBits(playerScope);
				player[client->getClientID()]->getSolid()->setStayActive(true);

				// TODO: The initial setting of the Look angles doesn't work yet
				angles=connectionEvent->getLook();
				player[client->getClientID()]->attach(cameraNode);
				cameraNode->setLookDir(Vector3(0,0,64),Math::Y_UNIT_VECTOR3,Math::Z_UNIT_VECTOR3);
			}
			else if(event->getType()==ServerUpdateEvent::EventType_SERVERUPDATE){
				ServerUpdateEvent::ptr serverEvent=shared_static_cast<ServerUpdateEvent>(event);

				client->handleServerUpdateEvent(serverEvent);

				int u=0;
				for(u=0;u<serverEvent->getNumUpdates();++u){
					int id=serverEvent->getID(u);
					HopEntity::ptr entity=id>=0?player[id]:block;
					if(entity!=NULL){
						// Update object's physical properties
						if(entity==player[client->getClientID()]){
							entity->getSolid()->setPositionDirect(serverEvent->getPosition(u)); // We want to retain our touching info
						}
						else{
							entity->getSolid()->setPosition(serverEvent->getPosition(u));
							entity->setRotate(serverEvent->getRotation(u));
						}
						entity->getSolid()->setVelocity(serverEvent->getVelocity(u));
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
			}
		}

		client->update();

		// TODO: Ideally we would somehow "ghost" the player so he wouldnt impart any forces on anything he touches
		//  We can't simply set his Mass to 0, because that affects friction calculations
		// Now we apply all new ClientEvents.
		Collection<BaseClientUpdateEvent::ptr> clientEvents=client->enumerateClientUpdateEvents();
		for(i=0;i<clientEvents.size();++i){
			ClientUpdateEvent::ptr clientEvent=shared_static_cast<ClientUpdateEvent>(clientEvents[i]);
			updatePlayer(clientEvent,player[client->getClientID()]);
			scene->getSimulator()->update(clientEvent->getDT(),0,player[client->getClientID()]->getSolid());
		}

		// Update the player's view again, in the case that the client events are slower than our frame rate, we want the view to be smooth
		updatePlayerView(angles,player[client->getClientID()]);
	}

	// START: Move to PredictedServer/Client classes, basically this will all be the same, except you'll call a setLastClientUpdate()
	if(server!=NULL){
		mutex.lock();
			for(i=0;i<playersConnected.size();++i){
				int id=playersConnected[i];
				EventConnection::ptr client=server->getClient(id);
				if(client!=NULL){
					player[id]->setScope(playerScope);
					player[id]->setCollisionBits(playerScope);
					EulerAngle eulerAngles;
					Math::setEulerAngleXYZFromQuaternion(eulerAngles,player[id]!=NULL?player[id]->getRotate():Math::IDENTITY_QUATERNION,0.001);
					client->send(Event::ptr(new ConnectionEvent(id,eulerAngles)));
				}
			}
			playersConnected.clear();
		
			for(i=0;i<playersDisconnected.size();++i){
				int id=playersDisconnected[i];
				player[id]->setScope(-1);
				player[id]->setCollisionBits(-1);
			}
			playersDisconnected.clear();
		mutex.unlock();

		server->update();

		EventConnection::ptr eventConnection=NULL;
		for(i=0,eventConnection=server->getClient(0);eventConnection!=NULL;eventConnection=server->getClient(++i)){
			Event::ptr event;
			while((event=eventConnection->receive())!=NULL){
				if(event->getType()==ClientUpdateEvent::EventType_CLIENTUPDATE){
					ClientUpdateEvent::ptr clientEvent=shared_static_cast<ClientUpdateEvent>(event);

					lastClientUpdateCounter[i]=clientEvent->getCounter();
					updatePlayer(clientEvent,player[i]);
					scene->getRootNode()->logicUpdate(player[i],clientEvent->getDT(),playerScope);
					scene->getSimulator()->update(clientEvent->getDT(),0,player[i]->getSolid());
				}
			}
		}
	}
	// END
}

void SimpleSync::preLogicUpdate(int dt){
	scene->preLogicUpdate(dt);

	if(client!=NULL){
		int currentFrame=scene->getLogicFrame()+1;
		client->sendClientUpdateEvent(ClientUpdateEvent::ptr(new ClientUpdateEvent(currentFrame,dt,flags,angles)));
	}

	// START: Move to PredictedServer/Client classes, for the most part this will be the same, except you'll call a getLastClientUpdate() to get that counter
	if(server!=NULL && nextUpdateTime<=scene->getLogicTime()){
		nextUpdateTime=scene->getLogicTime()+random.nextInt(debugUpdateMin,debugUpdateMax);

		EventConnection::ptr eventConnection=NULL;
		int i=0;
		for(eventConnection=server->getClient(0);eventConnection!=NULL;eventConnection=server->getClient(++i)){
			ServerUpdateEvent::ptr serverEvent(new ServerUpdateEvent(lastClientUpdateCounter[i],scene->getLogicTime()));
			serverEvent->addUpdate(-1,block->getSolid()->getPosition(),block->getRotate(),block->getVelocity());
			serverEvent->addUpdate(0,player[0]->getSolid()->getPosition(),player[0]->getRotate(),player[0]->getVelocity());
			serverEvent->addUpdate(1,player[1]->getSolid()->getPosition(),player[1]->getRotate(),player[1]->getVelocity());
			eventConnection->send(serverEvent);
		}
	}
	// END
}


void SimpleSync::logicUpdate(int dt){
	// Skip HopScene's logicUpdate
	scene->getRootNode()->logicUpdate(dt,~playerScope);

	// Update everything but the players
	scene->getSimulator()->update(dt,~playerScope,NULL);
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
	else if(server!=NULL){
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
		if(key=='r'){
			block->setPosition(Vector3(0,0,10));
			block->setVelocity(Vector3(0,0,0));
			block->clearForce();
		}
		else if(key=='w'){
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
	if(client!=NULL && button==2){
		setMouseLocked(true);
	}
}

void SimpleSync::mouseMoved(int x,int y){
	int hwidth=getWidth()/2;
	int hheight=getHeight()/2;
	if(client!=NULL && getMouseLocked() && (hwidth!=x || hheight!=y)){
		int dx=x-hwidth;
		int dy=y-hheight;

		angles.y-=((float)dx)/500.0f*Math::PI;
		if(angles.y<0) angles.y+=Math::TWO_PI;
		if(angles.y>=Math::TWO_PI) angles.y-=Math::TWO_PI;

		angles.z-=((float)dy)/500.0f*Math::PI;
		if(angles.z<-Math::HALF_PI) angles.z=-Math::HALF_PI;
		if(angles.z>Math::HALF_PI) angles.z=Math::HALF_PI;
	}
}

void SimpleSync::mouseReleased(int x,int y,int button){
	if(client!=NULL && button==2){
		setMouseLocked(false);
	}
}

Event::ptr SimpleSync::createEventType(int type){
	if(type==ConnectionEvent::EventType_CONNECTION){
		return Event::ptr(new ConnectionEvent());
	}
	else if(type==ClientUpdateEvent::EventType_CLIENTUPDATE){
		return Event::ptr(new ClientUpdateEvent());
	}
	else if(type==ServerUpdateEvent::EventType_SERVERUPDATE){
		return Event::ptr(new ServerUpdateEvent());
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

		mutex.lock();
		playersConnected.add(i);
		mutex.unlock();
	}
}

void SimpleSync::disconnected(Connection::ptr connection){
	if(server!=NULL){
		Logger::alert("Client disconnected");

		EventConnection::ptr client=server->getClient(connection);
		int i=0;
		for(EventConnection::ptr eventConnection=server->getClient(0);client!=eventConnection;eventConnection=server->getClient(++i));

		mutex.lock();
		playersDisconnected.add(i);
		mutex.unlock();
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
					Collection<uint32> hosts;
					Socket::getHostAdaptorsByName(hosts,c);
					if(hosts.size()>0){
						remoteHost=hosts[0];
					}
					else{
						remoteHost=Socket::stringToIP(c);
					}
					remotePort=defaultPort;
				}
				else{
					*p=0;
					p++;

					if(strlen(c)==0){
						remoteHost=Socket::stringToIP("127.0.0.1");
					}
					else{
						Collection<uint32> hosts;
						Socket::getHostAdaptorsByName(hosts,c);
						if(hosts.size()>0){
							remoteHost=hosts[0];
						}
						else{
							remoteHost=Socket::stringToIP(c);
						}
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
			server->setPosition(0,0);
			server->setSize(640,480);
			server->accept(localPort);
			serverThread=Thread::ptr(new Thread(server));
			serverThread->start();
		}

		// HACK: Lets try to wait for the system to be initialized before we start accepting
		System::msleep(2000);

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
	TOADLET_CATCH(const Exception &){}

	return 0;

}
#endif
