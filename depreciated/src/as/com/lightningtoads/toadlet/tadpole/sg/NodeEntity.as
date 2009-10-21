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

package com.lightningtoads.toadlet.tadpole.sg{

import com.lightningtoads.toadlet.tadpole.*;

public class NodeEntity extends Entity{
	public function NodeEntity(engine:Engine){
		super(engine);
		mType|=TYPE_NODE;
	}

	public override function destroy():void{
		while(mEntities.length>0){
			mEntities[0].destroy();
		}
		super.destroy();
	}

	public override function parentChanged(entity:Entity,parent:NodeEntity):void{
		super.parentChanged(entity,parent);

		var i:int;
		for(i=mEntities.length-1;i>=0;--i){
			mEntities[i].parentChanged(entity,parent);
		}
	}

	public function attach(entity:Entity):void{
		if(entity.getParent()!=null){
			entity.getParent().remove(entity);
		}

		mEntities[mEntities.length]=entity;

		entity.parentChanged(entity,this);
	}

	public function remove(entity:Entity):Boolean{
		var i:int;
		for(i=0;i<mEntities.length;++i){
			if(mEntities[i]==entity)break;
		}
		if(i!=mEntities.length){
			mEntities.splice(i,1);

			entity.parentChanged(entity,null);

			return true;
		}

		return false;
	}

	public function getNumChildren():int{
		return mEntities.length;
	}

	public function getChild(i:int):Entity{
		return mEntities[i];
	}

	public var mEntities:Array=new Array;
}

}