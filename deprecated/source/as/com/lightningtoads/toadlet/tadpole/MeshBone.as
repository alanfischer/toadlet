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

package com.lightningtoads.toadlet.tadpole{

import com.lightningtoads.toadlet.egg.mathfixed.*;

public final class MeshBone{
	public function MeshBone(){}

	public var index:int=-1;
	public var parent:int=-1;
	public var children:Array;

	public var translate:Vector3=new Vector3();
	public var rotate:Quaternion=new Quaternion();

	public var worldToBoneTranslate:Vector3=new Vector3();
	public var worldToBoneRotate:Quaternion=new Quaternion();
}

}
