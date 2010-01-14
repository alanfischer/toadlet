class EventClient{
	void serverConnected(Connection connection);
	void serverDisconnected(Connection connection);

	void sendEvent(Event event);
	void sendEventToClient(Event event,int clientID);
	Event::ptr receiveEvent();
	
	int getClientID(); // -1 = not connected
	
};