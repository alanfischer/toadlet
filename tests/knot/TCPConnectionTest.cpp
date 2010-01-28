#include <toadlet/egg/Logger.h>
#include <toadlet/egg/System.h>
#include <toadlet/knot/TCPConnection.h>
#include "../quicktest.h"

using namespace toadlet;
using namespace toadlet::egg;
using namespace toadlet::egg::io;
using namespace toadlet::egg::net;
using namespace toadlet::knot;

#define mlen(x) strlen((char*)x)
#define mcmp(x,y) strcmp((char*)x,(char*)y)

class Server:public Thread{
public:
	TOADLET_SHARED_POINTERS(Server);

	Server(int localPort):Thread(){
		this->localPort=localPort;
		connection=TCPConnection::ptr(new TCPConnection());
	}

	void run(){
		byte *smsg=(byte*)"message"; int samt=0;
		byte rmsg[1024]; int ramt=0;
		connection->accept(localPort);

		time1=System::mtime();
		samt=connection->send(smsg,mlen(smsg)+1);

		ramt=connection->receive(rmsg,sizeof(rmsg));
		time2=System::mtime();

		TOADLET_ASSERT(samt==ramt && mcmp(smsg,rmsg)==0);

		time3=System::mtime();
		samt=connection->send(smsg,mlen(smsg)+1);

		ramt=connection->receive(rmsg,sizeof(rmsg));
		time4=System::mtime();

		TOADLET_ASSERT(samt==ramt && mcmp(smsg,rmsg)==0);

		connection->debugSetPacketDelayTime(1000,1000);

		ramt=connection->receive(rmsg,sizeof(rmsg));
		time5=System::mtime();

		time6=System::mtime();
		samt=connection->send(smsg,mlen(smsg)+1);

		TOADLET_ASSERT(samt==ramt && mcmp(smsg,rmsg)==0);

		connection->debugSetPacketDelayTime(0,0);

		// One final send/receive just for kicks to let the debugSetPacketDelayTime's effect wear off so we don't get the recv error
		connection->send(smsg,mlen(smsg)+1);
		connection->receive(rmsg,sizeof(rmsg));

		Logger::alert("Server done");
	}

	int localPort;
	TCPConnection::ptr connection;
	int time1,time2,time3,time4,time5,time6;
};

class Client:public Thread{
public:
	TOADLET_SHARED_POINTERS(Client);

	Client(uint32 remoteHost,int remotePort):Thread(){
		this->remoteHost=remoteHost;
		this->remotePort=remotePort;
		connection=TCPConnection::ptr(new TCPConnection());
	}

	void run(){
		byte *smsg=(byte*)"message"; int samt=0;
		byte rmsg[1024]; int ramt=0;
		connection->connect(remoteHost,remotePort);

		ramt=connection->receive(rmsg,sizeof(rmsg));
		time1=System::mtime();

		time2=System::mtime();
		samt=connection->send(smsg,mlen(smsg)+1);

		TOADLET_ASSERT(samt==ramt && mcmp(smsg,rmsg)==0);

		connection->debugSetPacketDelayTime(1000,1000);

		ramt=connection->receive(rmsg,sizeof(rmsg));
		time3=System::mtime();

		time4=System::mtime();
		samt=connection->send(smsg,mlen(smsg)+1);

		TOADLET_ASSERT(samt==ramt && mcmp(smsg,rmsg)==0);

		connection->debugSetPacketDelayTime(0,0);

		time5=System::mtime();
		samt=connection->send(smsg,mlen(smsg)+1);

		ramt=connection->receive(rmsg,sizeof(rmsg));
		time6=System::mtime();

		TOADLET_ASSERT(samt==ramt && mcmp(smsg,rmsg)==0);

		// One final receive/send just for kicks to let the debugSetPacketDelayTime's effect wear off so we don't get the recv error
		connection->receive(rmsg,sizeof(rmsg));
		connection->send(smsg,mlen(smsg)+1);

		Logger::alert("Client done");
	}

	uint32 remoteHost;
	int remotePort;
	TCPConnection::ptr connection;
	int time1,time2,time3,time4,time5,time6;
};

QT_TEST(TCPConnectionTest){
	Server::ptr server(new Server(52525));
	server->start();

	System::msleep(1000);

	Client::ptr client(new Client(Socket::stringToIP("127.0.0.1"),52525));
	client->start();

	while(server->isAlive() || client->isAlive()){
		System::msleep(100);
	}

	Logger::alert(String("Server times:")+server->time1+","+server->time2+","+server->time3+","+server->time4+","+server->time5+","+server->time6);

	Logger::alert(String("Client times:")+client->time1+","+client->time2+","+client->time3+","+client->time4+","+client->time5+","+client->time6);

	// First check no latency times
	QT_CHECK(server->time1<=client->time1 && abs(server->time1-client->time1)<50);

	QT_CHECK(server->time2>=client->time2 && abs(server->time2-client->time2)<50);

	// Then check client fake latency times
	QT_CHECK(server->time3<client->time3 && abs(server->time3-client->time3)>=1000);

	QT_CHECK(server->time4>=client->time4 && abs(server->time4-client->time4)<50);

	// Then check server fake latency times
	QT_CHECK(server->time5>client->time5 && abs(server->time5-client->time5)>=1000);

	QT_CHECK(server->time6<=client->time6 && abs(server->time6-client->time6)<50);
}
