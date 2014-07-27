#include "ResourceTransformer.h"

static int ev_handler(struct mg_connection *conn, enum mg_event ev) {
	return ((ResourceTransformer*)(conn->server_param))->handleRequest(conn,ev);
}

ResourceTransformer::ResourceTransformer(){
	engine=new Engine();
	engine->setHasBackableFixed(true);
	engine->setHasBackableShader(true);
	engine->installHandlers();

	stream=new MemoryStream();
	
	managers["texture"]=engine->getTextureManager();
	managers["material"]=engine->getMaterialManager();
	managers["mesh"]=engine->getMeshManager();

	mimetypes["texture"]="image";
	mimetypes["mesh"]="application";
	mimetypes["material"]="application";

	server = mg_create_server(this, ev_handler);
	mg_set_option(server, "listening_port", "8080");
}

ResourceTransformer::~ResourceTransformer(){
	mg_destroy_server(&server);
	
	engine=NULL;
}

void ResourceTransformer::update(int dt){
	mg_poll_server(server, dt);
}

int ResourceTransformer::handleRequest(struct mg_connection *conn, enum mg_event ev){
	int result = MG_FALSE;

	if (ev == MG_REQUEST) {
		ResourceManager *manager=NULL;
		String mimetype;
		
		String uri=conn->uri;
		int s=uri.find('/',0);
		if(s>=0){
			String type=uri.substr(s+1,uri.length()-s-1);
			Map<String,ResourceManager::ptr>::iterator it=managers.find(type);
			if(it!=managers.end()){
				manager=it->second;
			}
			Map<String,String>::iterator it2=mimetypes.find(type);
			if(it2!=mimetypes.end()){
				mimetype=it2->second;
			}
		}

		String query=conn->query_string;
		Collection<String> paramList;
		paramList.reserve(16);
		query.split(paramList,"&");
		Map<String,String> params;
		for(int i=0;i<paramList.size();++i){
			Collection<String> param;
			param.reserve(2);
			paramList[i].split(param,"=");
			if(param.size()>1){
				params[param[0]]=param[1];
			}
		}
		
		String src=params["src"];
		String dst=params["dst"];

		mg_send_header(conn,"Access-Control-Allow-Origin","*");

		if(manager!=NULL && src.length()>0 && dst.length()>0){
			String ext=manager->findExtension(dst);

			mg_send_header(conn,"Content-Type",mimetype+"/"+ext);

			Resource::ptr resource;
			TOADLET_TRY
				resource=manager->find(src);
			TOADLET_CATCH_ANONYMOUS(){}

			if(resource!=NULL){
				ResourceStreamer::ptr streamer=manager->getStreamer(ext);
				streamer->save(stream,resource,NULL,this);
				mg_send_data(conn,stream->getOriginalDataPointer(),stream->length());
				stream->reset();
			}
		}
		result = MG_TRUE;
	}
	else if (ev == MG_AUTH) {
		result = MG_TRUE;
	}

	return result;
}

int main(){
	ResourceTransformer transformer;
	
	while(true){
		transformer.update(33);
	}
}
