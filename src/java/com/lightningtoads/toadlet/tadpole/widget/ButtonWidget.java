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

public class ButtonWidget extends ParentWidget{
	public ButtonWidget(Engine engine){super(engine);ButtonWidgetInit(engine,true);}
	public ButtonWidget(Engine engine,boolean realButton){super(engine);ButtonWidgetInit(engine,realButton);}
	private void ButtonWidgetInit(Engine engine,boolean realButton){
		mType|=Type.BUTTON;

		mLabel=new LabelWidget(mEngine);
		mLabel.setWordWrap(false);
		mLabel.setLayout(Layout.FLOW);
		attach(mLabel);

		if(realButton){
			mBorder=new BorderWidget(mEngine);
			mBorder.setLayout(Layout.FILL);
			attach(mBorder);
		}

		setColors(Colors.WHITE,Colors.GREY);
	}

	public boolean getSelectable(){return true;}

	public void setText(String text){mLabel.setText(text);}
	public String getText(){return mLabel.getText();}

	public void setFont(Font font){mLabel.setFont(font);}
	public Font getFont(){return mLabel.getFont();}

	public void setColor(Color color){
		mLightColor.set(color);
		mDarkColor.set(color);

		mLabel.setColor(mLightColor);
		if(mBorder!=null){
			mBorder.setColors(mLightColor,mDarkColor);
		}
	}
	void setColors(Color lightColor,Color darkColor){
		mLightColor.set(lightColor);
		mDarkColor.set(darkColor);

		mLabel.setColor(mLightColor);
		if(mBorder!=null){
			mBorder.setColors(mLightColor,mDarkColor);
		}
	}
	public Color getLightColor(){return mLightColor;}
	public Color getDarkColor(){return mDarkColor;}

	public void setInternalBorder(int border){mLabel.setBorder(border);}
	public int getInternalBorder(){return mLabel.getBorder();}

	public void getDesiredSize(int[] size){
		if(mLabel.mBorder==0 && mLabel.mWindow!=null){
			mLabel.setBorder(mLabel.mWindow.getDefaultInternalBorder());
		}

		super.getDesiredSize(size);
	}

	public void setFocused(boolean focused){
		super.setFocused(focused);

		if(focused==false){
			mPressedByKey=false;
			mPressedByMouse=false;
		}
	}

	public boolean mousePressed(int x,int y,int button){
		if(mBorder!=null && isPointInside(x,y)){
			mPressedByMouse=true;
			mBorder.setColors(mDarkColor,mLightColor);

			return true;
		}

		return false;
	}
	
	public boolean mouseMoved(int x,int y){
		if(mBorder!=null && isPointInside(x,y)==false && mPressedByMouse==true){
			mPressedByMouse=false;
			mBorder.setColors(mLightColor,mDarkColor);
			action(this);

			return true;
		}

		return false;
	}
	
	public boolean mouseReleased(int x,int y,int button){
		if(mBorder!=null && isPointInside(x,y) && mPressedByMouse==true){
			mPressedByMouse=false;
			mBorder.setColors(mLightColor,mDarkColor);
			action(this);

			return true;
		}

		return false;
	}

	protected boolean mPressedByKey;
	protected boolean mPressedByMouse;

	protected LabelWidget mLabel;
	protected BorderWidget mBorder;

	protected Color mLightColor=new Color();
	protected Color mDarkColor=new Color();
}
