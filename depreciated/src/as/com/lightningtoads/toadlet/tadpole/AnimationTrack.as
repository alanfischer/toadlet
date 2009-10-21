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
import com.lightningtoads.toadlet.egg.mathfixed.tMath;

public final class AnimationTrack{
	public function AnimationTrack(parent:Animation){
		mParent=parent;
	}

	public function getKeyFramesAtTime(time:int,f1:Array,f2:Array,trackHint:int):int{
		if(keyFrames.length<2){
			if(keyFrames.length==1){
				f1[0]=keyFrames[0];
				f2[0]=keyFrames[0];
			}
			return 0;
		}

		if(trackHint<0 || trackHint>=keyFrames.length){
			trackHint=0;
		}

		var it:int=trackHint;
		var it2:int;
		if(keyFrames[it].time>=time){
			// Search backwards
			while(it!=0 && keyFrames[it].time>time){
				--it;
			}
		}
		else{
			// Search forwards
			while(it!=keyFrames.length && keyFrames[it].time<time){
				++it;
			}

			--it;
		}

		trackHint=it;

		if(it+1==keyFrames.length){
			it2=keyFrames.length-1;
		}
		else{
			it2=it+1;
		}

		f1[0]=keyFrames[it];
		f2[0]=keyFrames[it2];

		if(f1[0].time<f2[0].time){
			time=tMath.div(time-f1[0].time,f2[0].time-f1[0].time);
		}
		else if(f1[0].time>f2[0].time){
			time=tMath.div(time-f1[0].time,mParent.length-f1[0].time);
		}
		else{
			time=0;
		}

		return time;
	}

	public var bone:int;
	public var keyFrames:Array;

	protected var mParent:Animation;
}

}
