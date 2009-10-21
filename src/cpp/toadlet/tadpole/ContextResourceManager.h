/********** Copyright header - do not remove **********
 *
 * The Toadlet Engine
 *
 * Copyright 2009, Lightning Toads Productions, LLC
 *
 * Author(s): Alan Fischer, Andrew Fischer
 *
 * This file is part of The Toadlet Engine.
 *
 * The Toadlet Engine is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * The Toadlet Engine is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.

 * You should have received a copy of the GNU Lesser General Public License
 * along with The Toadlet Engine.  If not, see <http://www.gnu.org/licenses/>.
 *
 ********** Copyright header - do not remove **********/

#ifndef TOADLET_TADPOLE_CONTEXTRESOURCEMANAGER_H
#define TOADLET_TADPOLE_CONTEXTRESOURCEMANAGER_H

#include <toadlet/tadpole/ResourceManager.h>

namespace toadlet{
namespace tadpole{

template<typename PeerType,typename ResourceSemantics>
class ContextResourceManager:public ResourceManager{
public:
	typedef egg::Collection<typename egg::Resource::wptr> ResourceCollection;
	typedef egg::Collection<PeerType*> ResourcePeerCollection;

	ContextResourceManager(egg::io::InputStreamFactory *inputStreamFactory):ResourceManager(inputStreamFactory){}

	virtual ~ContextResourceManager(){
		int i;
		for(i=0;i<mResourcesToLoad.size();++i){
			if(mResourcesToLoad[i]!=NULL){
				mResourcesToLoad[i].setPointerQueue(NULL);
			}
		}
		mResourcesToLoad.clear();

		mResourcePeersToUnload.clear();
	}

	virtual void resourceLoaded(const egg::Resource::ptr &resource){
		if(resource!=NULL){
			mResourcesToLoad.add(resource);
		}
	}

	virtual void resourceUnloaded(egg::Resource *resource){
		PeerType *peer=ResourceSemantics::getResourcePeer(resource);
		if(peer!=NULL){
			mResourcePeersToUnload.add(peer);
		}
	}

	inline ResourceCollection &getContextResourcesToLoad(){
		return mResourcesToLoad;
	}

	inline ResourcePeerCollection &getContextResourcePeersToUnload(){
		return mResourcePeersToUnload;
	}

protected:
	ResourceCollection mResourcesToLoad;
	ResourcePeerCollection mResourcePeersToUnload;
};

}
}

#endif
