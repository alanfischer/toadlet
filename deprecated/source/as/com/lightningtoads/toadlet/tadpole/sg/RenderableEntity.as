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

public class RenderableEntity extends Entity{
	public static var MAX_LAYERS:int=4;
	public static var MAX_RENDERABLES_IN_LAYER:int=32;

	public function RenderableEntity(engine:Engine){
		super(engine);
		mType|=TYPE_RENDERABLE;
	}

	public function setVisible(visible:Boolean):void{
		mVisible=visible;
	}

	public function getVisible():Boolean{
		return mVisible;
	}

	public function setLayer(layer:int):void{
		mLayer=layer;
	}

	public function getLayer():int{
		return mLayer;
	}

	public var mLayer:int=1;
	public var mVisible:Boolean=true;
}

}
