class EventServer{

	clientConnected(Connection connection);
	clientDisconnected(Connection connection);
	
	void sendEvent(Event event,Client client);
	void sendEvent(Event event,int clientID);
	void broadcastEvent(Event event);
	Event::ptr receiveEvent();

};