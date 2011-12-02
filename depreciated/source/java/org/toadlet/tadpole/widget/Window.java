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

import org.toadlet.egg.*;
import org.toadlet.tadpole.*;
import org.toadlet.peeper.*;
import java.util.Vector;

public class Window extends ParentWidget{
	public Window(Engine engine){
		super(engine);
		mType|=Type.WINDOW;
	}

	public void setDefaultFont(Font font){
		mDefaultFont=font;

		layout();
	}
	public Font getDefaultFont(){return mDefaultFont;}

	public void setDefaultInternalBorder(int border){
		mDefaultInternalBorder=border;

		layout();
	}
	public int getDefaultInternalBorder(){return mDefaultInternalBorder;}

	public void logicUpdate(int dt){
		int i;
		for(i=0;i<mUpdateWidgets.size();++i){
			Widget widget=mUpdateWidgets.get(i);
			widget.logicUpdate(dt);
			if(mUpdateWidgets.size()>i && widget!=mUpdateWidgets.get(i)){
				i--;
			}
		}
	}

	public void visualUpdate(int dt){
		int i;
		for(i=0;i<mUpdateWidgets.size();++i){
			Widget widget=mUpdateWidgets.get(i);
			widget.visualUpdate(dt);
			if(mUpdateWidgets.size()>i && widget!=mUpdateWidgets.get(i)){
				i--;
			}
		}
	}

	public void render(Renderer renderer){
		if(mWidth==0 || mHeight==0){
			Logger.warning(Categories.TOADLET_TADPOLE,
				"rendering a zero sized window");
		}

		render(renderer,this);
	}

	public void registerUpdateWidget(Widget widget){
		mUpdateWidgets.add(widget);
	}
	
	public void unregisterUpdateWidget(Widget widget){
		mUpdateWidgets.remove(widget);
	}

	protected void render(Renderer renderer,Widget widget){
		if(widget.mParent==null){
			widget.mWorldPositionX=widget.mPositionX;
			widget.mWorldPositionY=widget.mPositionY;
		}
		else{
			widget.mWorldPositionX=widget.mParent.mWorldPositionX+widget.mPositionX;
			widget.mWorldPositionY=widget.mParent.mWorldPositionY+widget.mPositionY;
		}

		if((widget.mType&Type.PARENT)>0){
			ParentWidget parent=(ParentWidget)widget;

			int numChildren=parent.getNumChildren();
			int i;
			for(i=0;i<numChildren;++i){
				render(renderer,parent.getChild(i));
			}
		}
		else if((widget.mType&Type.RENDERABLE)>0){
			RenderableWidget renderable=(RenderableWidget)widget;

			// Set up default widget states
			renderer.setBlend(Blend.Combination.ALPHA);
			renderer.setDepthTest(Renderer.DepthTest.NONE);
			renderer.setFaceCulling(Renderer.FaceCulling.NONE);
			renderer.setLighting(true);
			mLightEffect.set(renderable.getColor());
			mLightEffect.trackColor=renderable.getTrackColor();
			renderer.setLightEffect(mLightEffect);
			renderer.setTextureStage(0,null);

			Math.setMatrix4x4FromTranslate(widget.mWorldTransform,Math.fromInt(widget.mWorldPositionX),Math.fromInt(widget.mWorldPositionY),0);
			if(renderable.applyScale()){
				Math.setMatrix4x4FromScale(widget.mWorldTransform,Math.fromInt(widget.mWidth),Math.fromInt(widget.mHeight),Math.ONE);
			}
			renderer.setModelMatrix(widget.mWorldTransform);

			renderable.render(renderer);
		}
	}

	protected Font mDefaultFont=null;
	protected int mDefaultInternalBorder=0;
	
	protected Vector<Widget> mUpdateWidgets=new Vector<Widget>();

	protected LightEffect mLightEffect=new LightEffect();
}
