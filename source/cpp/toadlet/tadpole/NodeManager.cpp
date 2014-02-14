#include <toadlet/tadpole/NodeManager.h>

namespace toadlet{
namespace tadpole{

NodeManager::NodeManager(Scene *scene):
	mScene(scene)
{
	mNodes.resize(1,NULL); // Handle 0 is always NULL
}

void NodeManager::destroy(){
	#if defined(TOADLET_THREADSAFE)
		mMutex.lock();
	#endif

	mNodes.clear();

	#if defined(TOADLET_THREADSAFE)
		mMutex.unlock();
	#endif
}

void NodeManager::nodeCreated(Node *node){
	int handle=node->getUniqueHandle();
	if(handle<=0){
		#if defined(TOADLET_THREADSAFE)
			mMutex.lock();
		#endif

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

		#if defined(TOADLET_THREADSAFE)
			mMutex.unlock();
		#endif

		node->internal_setUniqueHandle(handle);
	}
}

void NodeManager::nodeDestroyed(Node *node){
	int handle=node->getUniqueHandle();

	#if defined(TOADLET_THREADSAFE)
		mMutex.lock();
	#endif

	if(handle>0 && mNodes[handle]==node){
		mNodes[handle]=NULL;
		mFreeHandles.add(handle);
	}

	#if defined(TOADLET_THREADSAFE)
		mMutex.unlock();
	#endif
}

Node *NodeManager::getNodeByHandle(int handle) const{
	#if defined(TOADLET_THREADSAFE)
		mMutex.lock();
	#endif

	Node *node=(handle>=0 && handle<mNodes.size())?mNodes[handle]:NULL;

	#if defined(TOADLET_THREADSAFE)
		mMutex.unlock();
	#endif

	return node;
}

}
}
