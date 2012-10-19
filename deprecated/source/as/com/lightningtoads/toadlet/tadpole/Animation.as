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

public final class Animation{
	public function Animation(){}

	public function compile():void{
		var maxBone:int=0;
		var i:int;
		for(i=0;i<tracks.length;++i){
			if(tracks[i].bone>maxBone){
				maxBone=tracks[i].bone;
			}
		}

		mBoneToTrackLookup=new Array();

		for(i=0;i<tracks.length;++i){
			mBoneToTrackLookup[tracks[i].bone]=tracks[i];
		}
	}

	public function getTrackFromBone(b:int):AnimationTrack{
		if(b<mBoneToTrackLookup.length){
			return mBoneToTrackLookup[b];
		}
		return null;
	}

	public var tracks:Array;
	public var length:int;

	protected var mBoneToTrackLookup:Array;
}

}
