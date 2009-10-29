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

public class BorderWidget extends RenderableWidget{
	public BorderWidget(Engine engine){
		super(engine);
		mType|=Type.BORDER;

		setLayout(Layout.FILL);

		VertexBuffer vertexBuffer=mEngine.loadVertexBuffer(new VertexBuffer(Buffer.UsageType.DYNAMIC,Buffer.AccessType.WRITE_ONLY,mEngine.getVertexFormats().POSITION_COLOR,12));
		mVertexData=new VertexData(vertexBuffer);

		IndexBuffer indexBuffer=mEngine.loadIndexBuffer(new IndexBuffer(Buffer.UsageType.STATIC,Buffer.AccessType.WRITE_ONLY,IndexBuffer.IndexFormat.UINT_16,24));
		mIndexData=new IndexData(IndexData.Primitive.TRIS,indexBuffer,0,indexBuffer.getSize());
		{
			IndexBufferAccessor iba=new IndexBufferAccessor();
			iba.lock(indexBuffer,Buffer.LockType.WRITE_ONLY);

			int i=0;
			iba.set(i++,0);
			iba.set(i++,1);
			iba.set(i++,2);
			iba.set(i++,2);
			iba.set(i++,1);
			iba.set(i++,3);
			iba.set(i++,2);
			iba.set(i++,3);
			iba.set(i++,4);
			iba.set(i++,4);
			iba.set(i++,3);
			iba.set(i++,5);

			iba.set(i++,6);
			iba.set(i++,7);
			iba.set(i++,8);
			iba.set(i++,7);
			iba.set(i++,9);
			iba.set(i++,8);
			iba.set(i++,8);
			iba.set(i++,9);
			iba.set(i++,10);
			iba.set(i++,10);
			iba.set(i++,9);
			iba.set(i++,11);

			iba.unlock();
		}
	}

	public void setColors(Color lightColor,Color darkColor){
		mLightColor.set(lightColor);
		mDarkColor.set(darkColor);

		rebuild();
	}
	
	public Color getLightColor(){return mLightColor;}
	public Color getDarkColor(){return mDarkColor;}
	
	public boolean getTrackColor(){return true;}
	
	public void setSize(int width,int height){
		super.setSize(width,height);

		rebuild();
	}

	public void render(Renderer renderer){
		renderer.renderPrimitive(mVertexData,mIndexData);
	}

	protected void rebuild(){
		int lightColor=mLightColor.getABGR();
		int darkColor=mDarkColor.getABGR();

		scalar left=0;
		scalar right=Math.fromInt(mWidth);
		scalar top=0;
		scalar bottom=Math.fromInt(mHeight);
		scalar thick=-Math.fromInt(2);

		vba.lock(mVertexData.getVertexBuffer(0),Buffer.LockType.WRITE_ONLY);

		vba.set3(	0,0, left,bottom,0);
		vba.setABGR(0,1, lightColor);

		vba.set3(	1,0, left+thick,bottom-thick,0);
		vba.setABGR(1,1, lightColor);

		vba.set3(	2,0, left,top,0);
		vba.setABGR(2,1, lightColor);

		vba.set3(	3,0, left+thick,top+thick,0);
		vba.setABGR(3,1, lightColor);

		vba.set3(	4,0, right,top,0);
		vba.setABGR(4,1, lightColor);

		vba.set3(	5,0, right-thick,top+thick,0);
		vba.setABGR(5,1, lightColor);

		vba.set3(	6,0, right,top,0);
		vba.setABGR(6,1, darkColor);

		vba.set3(	7,0, right-thick,top+thick,0);
		vba.setABGR(7,1, darkColor);

		vba.set3(	8,0, right,bottom,0);
		vba.setABGR(8,1, darkColor);

		vba.set3(	9,0, right-thick,bottom-thick,0);
		vba.setABGR(9,1, darkColor);

		vba.set3(	10,0, left,bottom,0);
		vba.setABGR(10,1, darkColor);

		vba.set3(	11,0, left+thick,bottom-thick,0);
		vba.setABGR(11,1, darkColor);

		vba.unlock();
	}

	protected Color mLightColor=new Color(Colors.WHITE);
	protected Color mDarkColor=new Color(Colors.GREY);
	protected VertexData mVertexData=null;
	protected IndexData mIndexData=null;

	public VertexBufferAccessor vba=new VertexBufferAccessor();
}
