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

package com.lightningtoads.toadlet.tadpole;

import com.lightningtoads.toadlet.egg.mathfixed.*;
import com.lightningtoads.toadlet.egg.mathfixed.Math;

public final class Animation{
	public final static class KeyFrame{
		public KeyFrame(){}

		public int time;
		public Vector3 translate=new Vector3();
		public Quaternion rotate=new Quaternion();
	}

	public final static class Track{
		public Track(Animation parent){
			mParent=parent;
		}

		public int getKeyFramesAtTime(int time,KeyFrame[] f1,KeyFrame[] f2,int trackHint){
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

			int it=trackHint;
			int it2;
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
				time=Math.div(time-f1[0].time,f2[0].time-f1[0].time);
			}
			else if(f1[0].time>f2[0].time){
				time=Math.div(time-f1[0].time,mParent.length-f1[0].time);
			}
			else{
				time=0;
			}

			return time;
		}

		public byte bone;
		public KeyFrame[] keyFrames;

		protected Animation mParent;
	}

	public Animation(){}

	public void compile(){
		int maxBone=0;
		int i;
		for(i=0;i<tracks.length;++i){
			if(tracks[i].bone>maxBone){
				maxBone=tracks[i].bone;
			}
		}

		mBoneToTrackLookup=new Track[maxBone+1];

		for(i=0;i<tracks.length;++i){
			mBoneToTrackLookup[tracks[i].bone]=tracks[i];
		}
	}

	public Track getTrackFromBone(int b){
		if(b<mBoneToTrackLookup.length){
			return mBoneToTrackLookup[b];
		}
		return null;
	}

	public Track[] tracks;
	public int length;

	protected Track[] mBoneToTrackLookup;
}
