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

package org.toadlet.tadpole.widget;

#include <org/toadlet/tadpole/Types.h>

import org.toadlet.tadpole.*;
import org.toadlet.tadpole.animation.*;

public class AnimationControllerWidget extends Widget implements AnimationControllerFinishedListener{
	public AnimationControllerWidget(Engine engine){
		super(engine);
		mAnimationController=new AnimationController();
		mAnimationController.setAnimationControllerFinishedListener(this);

		setLayout(Layout.ABSOLUTE);
	}

	public void setTime(int time){setTime(time,true);}
	public void setTime(int time,boolean setagain){mAnimationController.setTime(time,setagain);}
	public int getTime(){return mAnimationController.getTime();}

	public void setInterpolation(AnimationController.Interpolation interpolation){mAnimationController.setInterpolation(interpolation);}
	public AnimationController.Interpolation getInterpolation(){return mAnimationController.getInterpolation();}

	public void setTimeScale(scalar scale){mAnimationController.setTimeScale(scale);}
	public scalar getTimeScale(){return mAnimationController.getTimeScale();}

	public void attachAnimation(Animation animation){mAnimationController.attachAnimation(animation);}
	public void removeAnimation(Animation animation){mAnimationController.removeAnimation(animation);}

	public void setDestroyOnFinish(boolean destroy){mDestroyOnFinish=destroy;}
	public boolean getDestroyOnFinish(){return mDestroyOnFinish;}

	public void start(){
		if(mAnimationController.isRunning()){
			stop();
		}

		if(mRoot!=null){
			mRoot.registerUpdateWidget(this);
		}

		mAnimationController.start();
	}
	
	public void stop(){
		if(mAnimationController.isRunning()==false){
			return;
		}

		if(mRoot!=null){
			mRoot.unregisterUpdateWidget(this);
		}

		mAnimationController.stop();
	}

	public boolean isRunning(){return mAnimationController.isRunning();}

	public void logicUpdate(int dt){mAnimationController.logicUpdate(dt);}
	public void visualUpdate(int dt){mAnimationController.visualUpdate(dt);}

	public void controllerFinished(AnimationController controller){
		if(mDestroyOnFinish){
			destroy();
		}
	}

	public void parentChanged(Widget widget,ParentWidget oldParent,ParentWidget newParent){
		if(oldParent!=null && oldParent.instanceOf(Type.WINDOW)){
			reroot(null);
		}

		if(newParent!=null && newParent.instanceOf(Type.WINDOW)){
			reroot((Window)newParent);
		}

		super.parentChanged(widget,oldParent,newParent);
	}

	public void reroot(Window root){
		if(mAnimationController.isRunning()){
			if(root==null && mRoot!=null){
				mRoot.unregisterUpdateWidget(this);
			}
			else if(root!=null){
				root.registerUpdateWidget(this);
			}
		}
		mRoot=root;
	}

	protected AnimationController mAnimationController=null;
	protected boolean mDestroyOnFinish=false;
	protected Window mRoot=null;
}
