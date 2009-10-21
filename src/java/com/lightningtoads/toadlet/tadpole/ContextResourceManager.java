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

package com.lightningtoads.toadlet.tadpole;

#include <com/lightningtoads/toadlet/tadpole/Types.h>

import com.lightningtoads.toadlet.egg.Resource;
import com.lightningtoads.toadlet.egg.io.InputStreamFactory;
import java.util.List;
import java.util.Vector;

public class ContextResourceManager<PeerType> extends ResourceManager{
	public interface ResourceSemantics{
		public Object getResourcePeer(Resource resource);
	}

	public ContextResourceManager(ResourceSemantics resourceSemantics,InputStreamFactory inputStreamFactory){
		super(inputStreamFactory);
		mResourceSemantics=resourceSemantics;
	}

	public void resourceLoaded(Resource resource){
		if(resource!=null){
			mResourcesToLoad.add(resource);
		}
	}

	public void resourceUnloaded(Resource resource){
		PeerType peer=(PeerType)(mResourceSemantics.getResourcePeer(resource));
		if(peer!=null){
			mResourcePeersToUnload.add(peer);
		}
	}

	public List<Resource> getContextResourcesToLoad(){
		return mResourcesToLoad;
	}

	public List<PeerType> getContextResourcePeersToUnload(){
		return mResourcePeersToUnload;
	}

	protected ResourceSemantics mResourceSemantics=null;
	protected List<Resource> mResourcesToLoad=new Vector<Resource>();
	protected List<PeerType> mResourcePeersToUnload=new Vector<PeerType>();
}
