#include <toadlet/tadpole/NodeManager.h>

namespace toadlet{
namespace tadpole{

NodeManager::NodeManager(Scene *scene):
	mScene(scene)
{
	mHandles.resize(1,NULL); // Handle 0 is always NULL
}

void NodeManager::destroy(){
	mHandles.clear();
}

void NodeManager::nodeCreated(Node *node){
	int handle=node->getUniqueHandle();
	if(handle<=0){
		int size=mFreeHandles.size();
		if(size>0){
			handle=mFreeHandles.at(size-1);
			mFreeHandles.removeAt(size-1);
		}
		else{
			handle=mHandles.size();
			mHandles.resize(handle+1);
		}
		mHandles[handle]=node;
		node->internal_setUniqueHandle(handle);
	}
}

void NodeManager::nodeDestroyed(Node *node){
	int handle=node->getUniqueHandle();
	if(handle>0){
		mHandles[handle]=NULL;
		mFreeHandles.add(handle);
		node->internal_setUniqueHandle(0);
	}
}

}
}
