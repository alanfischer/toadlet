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

package org.toadlet.tadpole.animation;

#include <org/toadlet/tadpole/Types.h>

public class AnimationController{
	public enum Interpolation{
		LINEAR,
		COS,
	}

	public AnimationController(){}

	public void setTime(int time){setTime(time,true);}
	public void setTime(int time,boolean setagain){
		mLogicTime=time;
		mVisualTime=time;

		if(setagain){
			set(Math.fromMilli(mVisualTime));
		}
	}

	public int getTime(){return mLogicTime;}

	public void setLoop(boolean loop){mLoop=loop;}
	public boolean getLoop(){return mLoop;}

	public void setInterpolation(Interpolation interpolation){mInterpolation=interpolation;}
	public Interpolation getInterpolation(){return mInterpolation;}

	public void setTimeScale(scalar scale){mTimeScale=scale;}
	public scalar getTimeScale(){return mTimeScale;}

	public void setAnimationControllerFinishedListener(AnimationControllerFinishedListener listener){mFinishedListener=listener;}
	public AnimationControllerFinishedListener getAnimationControllerFinishedListener(){return mFinishedListener;}

	public void setMinMaxValue(scalar minValue,scalar maxValue){mMinValue=minValue;mMaxValue=maxValue;}
	public scalar getMinValue(){return mMinValue;}
	public scalar getMaxValue(){return mMaxValue;}

	public void start(){
		mRunning=true;

		mLogicTime=0;
		mVisualTime=0;
	}

	public void stop(){
		mRunning=false;

		mLogicTime=0;
		mVisualTime=0;

		set(0);
	}

	public boolean isRunning(){return mRunning;}

	public void logicUpdate(int dt){
		if(mRunning==false){
			return;
		}

		if(mMaxTime>0 && mLogicTime>=mMaxTime){
			return;
		}

		mLogicTime+=Math.mul(dt,mTimeScale);

		if(mMaxTime!=0 && mLogicTime>=mMaxTime){
			mLogicTime=mMaxTime;
			mVisualTime=mMaxTime;

			set(Math.fromMilli(mVisualTime));

			if(mLoop){
				mLogicTime=0;
				mVisualTime=0;
			}
			else if(mFinishedListener!=null){
				mRunning=false;
				mFinishedListener.controllerFinished(this); // Must be last since it may delete this
			}
		}
	}
	
	public void visualUpdate(int dt){
		if(mRunning==false){
			return;
		}

		if(mMaxTime>0 && mVisualTime>=mMaxTime){
			return;
		}

		mVisualTime+=Math.mul(dt,mTimeScale);

		if(mMaxTime>0 && mVisualTime>=mMaxTime){
			mVisualTime=mMaxTime;
		}

		set(Math.fromMilli(mVisualTime));
	}

	public void set(scalar value){
		if(mInterpolation==Interpolation.COS && mMaxValue!=0){
			value=Math.mul(mMaxValue,Math.div(Math.ONE-Math.cos(Math.mul(Math.div(value,mMaxValue),Math.PI)),Math.TWO));
		}

		int i;
		for(i=0;i<mNumAnimations;++i){
			Animation animation=mAnimations[i];
			animation.set(value);
		}
	}

	public void extentsChanged(){
		mMinValue=0;
		mMaxValue=0;

		int i;
		for(i=0;i<mNumAnimations;++i){
			Animation animation=mAnimations[i];
			
			scalar minValue=animation.getMin();
			if(mMinValue<minValue){
				mMinValue=minValue;
			}

			scalar maxValue=animation.getMax();
			if(mMaxValue<maxValue){
				mMaxValue=maxValue;
			}
		}

		mMinTime=Math.toMilli(mMinValue);
		mMaxTime=Math.toMilli(mMaxValue);
	}

	public void attachAnimation(Animation animation){
		if(mAnimations.length<mNumAnimations+1){
			Animation[] animations=new Animation[mNumAnimations+1];
			System.arraycopy(mAnimations,0,animations,0,mAnimations.length);
			mAnimations=animations;
		}
		mAnimations[mNumAnimations++]=animation;

		animation.attached(this);

		extentsChanged();
	}
	
	public void removeAnimation(Animation animation){
		int i;
		for(i=0;i<mNumAnimations;++i){
			if(mAnimations[i]==animation)break;
		}
		if(i!=mNumAnimations){
			System.arraycopy(mAnimations,i+1,mAnimations,i,mNumAnimations-(i+1));
			mAnimations[--mNumAnimations]=null;
		}

		animation.removed(this);

		extentsChanged();
	}

	Animation[] mAnimations=new Animation[0];
	int mNumAnimations=0;

	int mLogicTime=0;
	int mVisualTime=0;
	scalar mMinValue=0;
	scalar mMaxValue=0;
	int mMinTime=0;
	int mMaxTime=0;
	boolean mLoop=false;
	Interpolation mInterpolation=Interpolation.LINEAR;
	scalar mTimeScale=Math.ONE;
	boolean mRunning=false;
	AnimationControllerFinishedListener mFinishedListener=null;
}
