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

package com.lightningtoads.toadlet.tadpole.entity;

#include <com/lightningtoads/toadlet/tadpole/Types.h>

import com.lightningtoads.toadlet.egg.*;
import com.lightningtoads.toadlet.egg.Error;
import com.lightningtoads.toadlet.peeper.*;
import com.lightningtoads.toadlet.tadpole.*;

public class ParentEntity extends Entity{
	public ParentEntity(){super();}

	public Entity create(Engine engine){
		super.create(engine);

		mNumChildren=0;
		mNumShadowChildren=0;
		mShadowChildrenDirty=false;

		return this;
	}

	public void destroy(){
		while(mNumChildren>0){
			mChildren[0].destroy();
		}

		mNumShadowChildren=0;

		super.destroy();
	}

	public boolean isParent(){return true;}
	
	public void removeAllEntityDestroyedListeners(){
		super.removeAllEntityDestroyedListeners();

		int i;
		for(i=mNumChildren-1;i>=0;--i){
			mChildren[i].removeAllEntityDestroyedListeners();
		}
	}

	public boolean attach(Entity entity){
		if(entity.destroyed()){
			Error.unknown(Categories.TOADLET_TADPOLE,
				"cannot attach a destroyed entity");
			return false;
		}

		ParentEntity parent=entity.getParent();
		if(parent!=null){
			parent.remove(entity);
		}

		if(mChildren.length<mNumChildren+1){
			Entity[] children=new Entity[mNumChildren+1];
			System.arraycopy(mChildren,0,children,0,mChildren.length);
			mChildren=children;
		}
		mChildren[mNumChildren++]=entity;

		entity.parentChanged(this);

		mShadowChildrenDirty=true;

		return true;
	}
	
	public boolean remove(Entity entity){
		int i;
		for(i=0;i<mNumChildren;++i){
			if(mChildren[i]==entity)break;
		}
		if(i!=mNumChildren){
			System.arraycopy(mChildren,i+1,mChildren,i,mNumChildren-(i+1));
			mChildren[--mNumChildren]=null;

			entity.parentChanged(null);

			mShadowChildrenDirty=true;

			return true;
		}
		else{
			return false;
		}
	}

	public int getNumChildren(){return mNumChildren;}
	public Entity getChild(int i){return mChildren[i];}

	protected void updateShadowChildren(){
		mShadowChildrenDirty=false;
		
		if(mShadowChildren.length!=mChildren.length){
			mShadowChildren=new Entity[mChildren.length];
		}
		System.arraycopy(mChildren,0,mShadowChildren,0,mNumChildren);
		mNumShadowChildren=mNumChildren;
	}

	Entity[] mChildren=new Entity[0];
	int mNumChildren;
	Entity[] mShadowChildren=new Entity[0];
	int mNumShadowChildren;
	boolean mShadowChildrenDirty;
}
