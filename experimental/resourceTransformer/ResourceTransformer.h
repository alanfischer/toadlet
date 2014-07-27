#ifndef RESOURCETRANSFORMER_H
#define RESOURCETRANSFORMER_H

#include <toadlet/toadlet.h>
#include "mongoose.h"

class ResourceTransformer:public Object,public ResourceRequest{
public:
	TOADLET_IOBJECT(ResourceTransformer);

	ResourceTransformer();
	virtual ~ResourceTransformer();

	void update(int dt);

	int handleRequest(struct mg_connection *conn, enum mg_event ev);

	void resourceReady(Resource *resource){}
	void resourceException(const Exception &ex){}
	void resourceProgress(float progress){}

	mg_server *server;
	Engine::ptr engine;
	MemoryStream::ptr stream;
	Map<String,ResourceManager::ptr> managers;
	Map<String,String> mimetypes;
};

#endif
