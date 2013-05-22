#include <toadlet/tadpole/NodeManager.h>

namespace toadlet{
namespace tadpole{

NodeManager::NodeManager(Scene *scene):
	mScene(scene)
{
	mNodes.resize(1,NULL); // Handle 0 is always NULL
}

void NodeManager::destroy(){
	mNodes.clear();
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
			handle=mNodes.size();
			mNodes.resize(handle+1);
		}
		mNodes[handle]=node;
		node->internal_setUniqueHandle(handle);
	}
}

void NodeManager::nodeDestroyed(Node *node){
	int handle=node->getUniqueHandle();
	if(handle>0 && mNodes[handle]==node){
		mNodes[handle]=NULL;
		mFreeHandles.add(handle);
	}
}

}
}
