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

package com.lightningtoads.toadlet.tadpole.widget;

#include <com/lightningtoads/toadlet/tadpole/Types.h>

import com.lightningtoads.toadlet.tadpole.*;
import com.lightningtoads.toadlet.peeper.*;

public class Widget{
	public static class Type{
		public final static int PARENT=		1<<0;
		public final static int WINDOW=		1<<1;
		public final static int RENDERABLE=	1<<2;
		public final static int LABEL=		1<<3;
		public final static int BORDER=		1<<4;
		public final static int BUTTON=		1<<5;
		public final static int SPRITE=		1<<6;
	}

	public enum Layout{
		ABSOLUTE,
		FRACTIONAL,
		FILL,
		FLOW,
	}

	public Widget(Engine engine){
		mEngine=engine;
	}
	
	public void destroy(){
		if(mDestroyed){
			return;
		}

		mDestroyed=true;

		if(mParent!=null){
			mParent.remove(this);
		}

		if(mWidgetDestroyedListener!=null){
			mWidgetDestroyedListener.widgetDestroyed(this);
		}
	}

	public boolean instanceOf(long type){return (mType&type)>0;}
	public long getType(){return mType;}
	public boolean destroyed(){return mDestroyed;}

	public void setWidgetDestroyedListener(WidgetDestroyedListener listener){mWidgetDestroyedListener=listener;}
	public WidgetDestroyedListener getWidgetDestroyedListener(){return mWidgetDestroyedListener;}
	public void removeAllWidgetDestroyedListeners(){setWidgetDestroyedListener(null);}

	public void parentChanged(Widget widget,ParentWidget oldParent,ParentWidget newParent){
		if(widget==this){
			mParent=newParent;
		}
	}
	public ParentWidget getParent(){return mParent;}

	public void setPosition(int x,int y){
		mPositionX=x;
		mPositionY=y;
	}
	public int getPositionX(){return mPositionX;}
	public int getPositionY(){return mPositionY;}

	public void setFractionalPosition(scalar x,scalar y){
		mFractionalPositionX=x;
		mFractionalPositionY=y;
	}
	public scalar getFractionalPositionX(){return mFractionalPositionX;}
	public scalar getFractionalPositionY(){return mFractionalPositionY;}

	public void setSize(int width,int height){
		mWidth=width;
		mHeight=height;
	}
	public int getWidth(){return mWidth;}
	public int getHeight(){return mHeight;}

	public void setFractionalSize(scalar width,scalar height){setFractionalSize(width,height,true,true);}
	public void setFractionalSize(scalar width,scalar height,boolean respectToWidth,boolean respectToHeight){
		mFractionalWidth=width;
		mFractionalHeight=height;
		mRespectToWidth=respectToWidth;
		mRespectToHeight=respectToHeight;
	}
	public scalar getFractionalWidth(){return mFractionalWidth;}
	public scalar getFractionalHeight(){return mFractionalHeight;}

	public void setRightJustified(boolean right){mRightJustified=right;}
	public boolean getRightJustified(){return mRightJustified;}

	public boolean isPointInside(int x,int y){return x>=mPositionX && x<mPositionX+mWidth && y>=mPositionY && y<mPositionY+mHeight;}

	public void setLayout(Layout layout){mLayout=layout;}
	public Layout getLayout(){return mLayout;}

	/// Takes in a maximum possible size, and writes out its desired size
	public void getDesiredSize(int[] size){ /// @todo  If we add in some sort of integer Vector2, return that instead
		if(mLayout==Layout.ABSOLUTE){
			size[0]=mWidth;
			size[1]=mHeight;
		}
		else if(mLayout==Layout.FRACTIONAL){
			if(mRespectToWidth && !mRespectToHeight){
				size[0]=Math.toInt(Math.mul(mFractionalWidth,Math.fromInt(size[0])));
				size[1]=Math.toInt(Math.mul(mFractionalHeight,Math.fromInt(size[0])));
			}
			else if(!mRespectToWidth && mRespectToHeight){
				size[0]=Math.toInt(Math.mul(mFractionalWidth,Math.fromInt(size[1])));
				size[1]=Math.toInt(Math.mul(mFractionalHeight,Math.fromInt(size[1])));
			}
			else{
				size[0]=Math.toInt(Math.mul(mFractionalWidth,Math.fromInt(size[0])));
				size[1]=Math.toInt(Math.mul(mFractionalHeight,Math.fromInt(size[1])));
			}
		}
		else{
			size[0]=0;
			size[1]=0;
		}
	}

	public void setBorder(int border){mBorder=border;}
	public int getBorder(){return mBorder;}

	public void setFocused(boolean focused){}

	/// Only called if the Widget registers itself with the Window in registerUpdateWidget.
	/// Dont forget to call unregisterUpdateWidget in its destroy.
	public void logicUpdate(int dt){}
	public void visualUpdate(int dt){}

	public boolean mousePressed(int x,int y,int button){return false;}
	public boolean mouseMoved(int x,int y){return false;}
	public boolean mouseReleased(int x,int y,int button){return false;}

	public boolean action(Widget widget){
		if(mActionListener!=null){
			mActionListener.action(widget);
		}
		else if(mParent!=null){
			mParent.action(widget);
		}
		return true;
	}

	public void setActionListener(ActionListener listener){mActionListener=listener;}
	public ActionListener getActionListener(){return mActionListener;}

	public Engine getEngine(){return mEngine;}

	protected boolean mDestroyed=false;
	protected Engine mEngine=null;
	protected long mType=0;

	protected WidgetDestroyedListener mWidgetDestroyedListener=null;

	protected ParentWidget mParent=null;

	protected int mPositionX=0,mPositionY=0;
	protected scalar mFractionalPositionX=0,mFractionalPositionY=0;
	protected boolean mRightJustified=false;
	protected int mWidth=0,mHeight=0;
	protected scalar mFractionalWidth=0,mFractionalHeight=0;
	protected boolean mRespectToWidth=false,mRespectToHeight=false;
	protected Layout mLayout=Layout.FLOW;
	protected int mBorder=0;

	protected ActionListener mActionListener=null;

	protected int mWorldPositionX=0,mWorldPositionY=0;
	
	protected Matrix4x4 mWorldTransform=new Matrix4x4();
}
