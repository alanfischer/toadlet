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

public final class MeshSubMesh{
	// FaceCullings
	public static var FC_NONE:int=0;
	public static var FC_CCW:int=1;
	public static var FC_CW:int=2;
	public static var FC_FRONT:int=1;
	public static var FC_BACK:int=2;

	public function MeshSubMesh(){}

	public var faceCulling:int=FC_NONE;
	public var hasTexture:Boolean=false;
	public var textureIndex:int=0;
	public var lightingEnabled:Boolean=true;
	public var flatShading:Boolean=true;
	public var color:int=0x00FFFFFF;
	public var sort:Boolean=true;
}

}
