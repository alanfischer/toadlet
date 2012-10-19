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

import org.toadlet.egg.Categories;
import org.toadlet.egg.Error;
import org.toadlet.tadpole.*;

import java.util.Vector;

public class ParentWidget extends Widget{
	public ParentWidget(Engine engine){
		super(engine);
		mType|=Type.PARENT;
	}
	
	public void destroy(){
		while(mChildren.size()>0){
			mChildren.get(0).destroy();
		}

		super.destroy();
	}

	public void removeAllWidgetDestroyedListeners(){
		super.removeAllWidgetDestroyedListeners();

		int i;
		for(i=mChildren.size()-1;i>=0;--i){
			mChildren.get(i).removeAllWidgetDestroyedListeners();
		}
	}

	public void parentChanged(Widget widget,ParentWidget oldParent,ParentWidget newParent){
		super.parentChanged(widget,oldParent,newParent);

		int i;
		for(i=mChildren.size()-1;i>=0;--i){
			mChildren.get(i).parentChanged(widget,oldParent,newParent);
		}
	}

	public boolean attach(Widget widget){
		if(widget.destroyed()){
			Error.unknown(Categories.TOADLET_TADPOLE,
				"cannot attach a destroyed widget");
			return false;
		}

		if(widget.getParent()!=null){
			widget.getParent().remove(widget);
		}

		mChildren.add(widget);

		widget.parentChanged(widget,null,this);
		ParentWidget parent=this;
		while(parent.mParent!=null){
			widget.parentChanged(parent,null,parent.mParent);
			parent=parent.mParent;
		}

		layout(); /// @todo Set a layoutNeeded flag instead

		return true;
	}
	
	public boolean remove(Widget widget){
		if(mChildren.remove(widget)){
			ParentWidget parent=this;
			while(parent.mParent!=null){
				widget.parentChanged(parent,parent.mParent,null);
				parent=parent.mParent;
			}
			widget.parentChanged(widget,this,null);

			layout(); /// @todo Set a layoutNeeded flag instead

			return true;
		}
		else{
			return false;
		}
	}

	public int getNumChildren(){return mChildren.size();}
	public Widget getChild(int i){return mChildren.get(i);}

	public void setLayoutManager(boolean layoutManager){mLayoutManager=layoutManager;}
	public boolean getLayoutManager(){return mLayoutManager;}

	public void setSize(int width,int height){
		super.setSize(width,height);

		layout();
	}

	public void getDesiredSize(int[] size){
		if(mLayout==Layout.FRACTIONAL){
			size[0]=Math.toInt(Math.mul(Math.fromInt(size[0]),mFractionalWidth));
			size[1]=Math.toInt(Math.mul(Math.fromInt(size[1]),mFractionalHeight));
		}
		else{
			layout(false,size);
		}
	}

	public void layout(){
		int[] size=cache_layout_size;
		size[0]=getWidth();
		size[1]=getHeight();
		layout(true,size);
	}
	
	public void layout(boolean doLayout,int[] size){
		if(mLayoutManager==false){
			return;
		}

		int maxWidth=size[0];
		int maxHeight=size[1];

		int maxWidgetWidth=0;
		int y=0;
		int numFlowWidgets=0;
		int flowWidget=0;
		int i;

		for(i=0;i<mChildren.size();++i){
			Widget widget=mChildren.get(i);
			if(widget.getLayout()==Layout.FLOW){
				numFlowWidgets++;
			}
		}

		for(i=0;i<mChildren.size();++i){
			Widget widget=mChildren.get(i);
			int border=widget.getBorder();

			if(widget.getLayout()==Layout.ABSOLUTE){
				// Just reset the size, as a notification that it has been layed out
				// No other work is necessary
				widget.setSize(widget.getWidth(),widget.getHeight());
			}
			else if(widget.getLayout()==Layout.FRACTIONAL){
				int[] wsize=cache_layout_wsize;
				wsize[0]=maxWidth-border*2;
				wsize[1]=maxHeight-border*2;

				if(doLayout){
					int[] wsize2=cache_layout_wsize2;wsize2[0]=wsize[0];wsize2[1]=wsize[1];
					widget.getDesiredSize(wsize2);
					widget.setSize(wsize2[0],wsize2[1]);

					if(widget.getRightJustified()){
						widget.setPosition(
							Math.toInt(Math.mul(widget.getFractionalPositionX(),Math.fromInt(wsize[0])))-wsize2[0],
							Math.toInt(Math.mul(widget.getFractionalPositionY(),Math.fromInt(wsize[1]))));
					}
					else{
						widget.setPosition(
							Math.toInt(Math.mul(widget.getFractionalPositionX(),Math.fromInt(wsize[0]))),
							Math.toInt(Math.mul(widget.getFractionalPositionY(),Math.fromInt(wsize[1]))));
					}
				}
			}
			else if(widget.getLayout()==Layout.FILL){
				int width=getWidth()-border*2;
				int height=getHeight()-border*2;
				if(width<0) width=0;
				if(height<0) height=0;

				if(doLayout){
					widget.setPosition(border,border);
					widget.setSize(width,height);
				}
			}
			else if(widget.getLayout()==Layout.FLOW){
				int[] wsize=cache_layout_wsize;
				wsize[0]=maxWidth-border*2;
				wsize[1]=maxHeight-border*2;
				widget.getDesiredSize(wsize);
				int width=wsize[0];
				int height=wsize[1];
	//			if(width+border*2>maxWidth) width=maxWidth-border*2;
	//			if(height+border*2>maxHeight-y) height=maxHeight-y-border*2;

				if(doLayout){
				// Flowing
	//				widget.setPosition(
	//					(getWidth()-width)/2,
	//					y+border);

				// Centering
					widget.setPosition(
						(getWidth()-width)/2,
						flowWidget*getHeight()/(numFlowWidgets+1) + getHeight()/(numFlowWidgets+1) - height/2);

					widget.setSize(width,height);
				}

				if(width+border*2>maxWidgetWidth){
					maxWidgetWidth=width+border*2;
				}
				y+=height+border*2;

				flowWidget++;
			}
		}

		if(size!=null){
			size[0]=maxWidgetWidth;
			size[1]=y;
		}
	}

	public boolean mousePressed(int x,int y,int button){
		boolean handled=false;

		if(isPointInside(x,y)){
			x-=mPositionX;
			y-=mPositionY;

			int i;
			for(i=0;i<mChildren.size();++i){
				Widget widget=mChildren.get(i);
				if(widget.isPointInside(x,y)){
					handled|=widget.mousePressed(x,y,button);
				}
			}

			x+=mPositionX;
			y+=mPositionY;
		}

		return handled;
	}
	
	public boolean mouseMoved(int x,int y){
		boolean handled=false;

		x-=mPositionX;
		y-=mPositionY;

		int i;
		for(i=0;i<mChildren.size();++i){
			Widget widget=mChildren.get(i);
			handled|=widget.mouseMoved(x,y);
		}

		x+=mPositionX;
		y+=mPositionY;

		return handled;
	}
	
	public boolean mouseReleased(int x,int y,int button){
		boolean handled=false;

		if(isPointInside(x,y)){
			x-=mPositionX;
			y-=mPositionY;

			int i;
			for(i=0;i<mChildren.size();++i){
				Widget widget=mChildren.get(i);
				if(widget.isPointInside(x,y)){
					handled|=widget.mouseReleased(x,y,button);
				}
			}

			x+=mPositionX;
			y+=mPositionY;
		}

		return handled;
	}

	protected Vector<Widget> mChildren=new Vector<Widget>();
	
	boolean mLayoutManager=true;
	
	int[] cache_layout_size=new int[2];
	int[] cache_layout_wsize=new int[2];
	int[] cache_layout_wsize2=new int[2];
}
