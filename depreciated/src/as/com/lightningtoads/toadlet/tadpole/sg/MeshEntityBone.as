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

import com.lightningtoads.toadlet.egg.mathfixed.*;
import com.lightningtoads.toadlet.tadpole.*;

public final class MeshEntityBone{
	public function MeshEntityBone(){}

	public var boneIndex:int;

	public var parent:MeshEntityBone;
	public var children:Array;

	public var localTranslate:Vector3=new Vector3();
	public var localRotate:Quaternion=new Quaternion();

	public var worldTranslate:Vector3=new Vector3();
	public var worldRotate:Quaternion=new Quaternion();
	public var worldRotateMatrix:Matrix3x3=new Matrix3x3();

	public var boneSpaceTranslate:Vector3=new Vector3();
	public var boneSpaceRotate:Matrix3x3=new Matrix3x3();
}

}
