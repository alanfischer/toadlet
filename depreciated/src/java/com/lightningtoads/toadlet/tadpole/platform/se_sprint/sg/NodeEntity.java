/********** Copyright header - do not remove **********
 *
 * The Toadlet Engine
 *
 * Copyright (C) 2006, Lightning Toads Productions, LLC
 *
 * Author(s): Alan Fischer
 *
 * All source code for the Toadlet Engine, including
 * this file, is the sole property of Lightning Toads
 * Productions, LLC. It has been developed on our own
 * time, with our own tools, on our own hardware. None
 * of this code may be observed, used or changed
 * without our explicit permission.
 *
 *  - Lightning Toads Productions, LLC
 *
 ********** Copyright header - do not remove **********/

package com.lightningtoads.toadlet.tadpole.sg;

import com.lightningtoads.toadlet.tadpole.*;

public class NodeEntity extends Entity{
	public NodeEntity(Engine engine){
		super(engine);
		mType|=TYPE_NODE;
	}

	public void destroy(){
		while(mNumEntities>0){
			mEntities[0].destroy();
		}

		super.destroy();
	}

	public void parentChanged(Entity entity,NodeEntity parent){
		super.parentChanged(entity,parent);

		int i;
		for(i=0;i<mNumEntities;++i){
			mEntities[i].parentChanged(entity,parent);
		}
	}
	
	public void attach(Entity entity){
		if(entity.mParent!=null){
			entity.mParent.remove(entity);
		}

		if(mEntities.length<mNumEntities+1){
			Entity[] entities=new Entity[mNumEntities+1];
			System.arraycopy(mEntities,0,entities,0,mEntities.length);
			mEntities=entities;
		}

		mEntities[mNumEntities]=entity;
		mNumEntities++;

		entity.parentChanged(entity,this);
	}

	public boolean remove(Entity entity){
		int i;
		for(i=0;i<mNumEntities;++i){
			if(mEntities[i]==entity)break;
		}
		if(i!=mNumEntities){
			mEntities[i]=null;
			System.arraycopy(mEntities,i+1,mEntities,i,mNumEntities-(i+1));
			mNumEntities--;
			mEntities[mNumEntities]=null;

			entity.parentChanged(entity,null);

			return true;
		}

		return false;
	}

	public final int getNumChildren(){
		return mNumEntities;
	}

	public final Entity getChild(int i){
		return mEntities[i];
	}

	Entity[] mEntities=new Entity[0];
	int mNumEntities;
}
