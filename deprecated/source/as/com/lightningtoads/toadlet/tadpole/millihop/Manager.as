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

package com.lightningtoads.toadlet.tadpole.millihop{

import com.lightningtoads.toadlet.egg.mathfixed.*;

public interface Manager{
	function traceLine(segment:Segment,result:Collision):void;
	function traceSolid(segment:Segment,solid:HopEntity,result:Collision):void;
	function preUpdate(dt:int,fdt:int):void;
	function postUpdate(dt:int,fdt:int):void;
	function preUpdateS(solid:HopEntity,dt:int,fdt:int):void;
	function intraUpdateS(solid:HopEntity,dt:int,fdt:int):void;
	function postUpdateS(solid:HopEntity,dt:int,fdt:int):void;
}

}
