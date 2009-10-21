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

public class LabelWidget extends RenderableWidget{
	public LabelWidget(Engine engine){
		super(engine);
		mType|=Type.LABEL;

		mTextureStage=new TextureStage();
	}

	public void setText(String text){
		mText=text;

		if(mParent!=null){
			mParent.layout();
		}

		rebuild();
	}

	public String getText(){return mText;}

	public void setFont(Font font){
		mFont=font;

		if(mParent!=null){
			mParent.layout();
		}

		rebuild();
	}

	public Font getFont(){return mFont;}

	public void setColor(Color color){mColor.set(color);}
	public Color getColor(){return mColor;}

	public void setWordWrap(boolean wordWrap){
		mWordWrap=wordWrap;

		if(mParent!=null){
	//		mParent.layout();
		}
	}

	public boolean getWordWrap(){return mWordWrap;}

	public void parentChanged(Widget widget,ParentWidget oldParent,ParentWidget newParent){
		if(oldParent==mWindow){
			mWindow=null;
		}

		if(newParent!=null && newParent.instanceOf(Type.WINDOW)){
			mWindow=(Window)newParent;
		}

		super.parentChanged(widget,oldParent,newParent);
	}

	public void setSize(int width,int height){
		super.setSize(width,height);

		rebuild();
	}

	public void getDesiredSize(int[] size){
		Font font=mFont;
		if(font==null && mWindow!=null){
			font=mWindow.getDefaultFont();
		}
		if(font==null || mText==null){
			size[0]=0;
			size[1]=0;
			return;
		}

		int width=0;
		int height=0;
		String text=mText;
		if(mWordWrap){
			text=wordWrap(font,size[0],text);
		}
		int start=0;
		int i;
		for(i=0;i<text.length();++i){
			if(text.charAt(i)==(char)10){
				int w=font.getStringWidth(text.subSequence(start,i));
				if(w>width) width=w;
				height+=font.getHeight();
				start=i+1;
			}
		}

		if(height==0){
			width=font.getStringWidth(text);
		}
		height+=font.getHeight();

		size[0]=width;
		size[1]=height;
	}

	public void render(Renderer renderer){
		if(mVertexData==null || mIndexData==null){
			return;
		}

		renderer.setBlend(mBlend);
		renderer.setTextureStage(0,mTextureStage);

		renderer.renderPrimitive(mVertexData,mIndexData);
	}

	protected String wordWrap(Font font,int width,String text){	
		// Word wrapping algorithm
		int spaceWidth=font.getStringWidth(" ");
		int spaceLeft=width;
		int start=0;
		int end=text.indexOf(' ');
		while(end!=-1){
			String word=text.substring(start,end);
			int wordWidth=font.getStringWidth(word);
			if(wordWidth>spaceLeft){
				text=text.substring(0,start)+(char)10+text.substring(start,text.length());
				spaceLeft=width-wordWidth;
			}
			else{
				spaceLeft=spaceLeft-(wordWidth + spaceWidth);
			}

			if(end>=text.length()){
				break;
			}
			else{
				start=end+1;
				end=text.indexOf(' ',start);
				if(end==-1){
					end=text.length();
				}
			}
		}

		return text;
	}

	public void rebuild(){
		Font font=mFont;
		if(font==null && mWindow!=null){
			font=mWindow.getDefaultFont();
		}
		if(font==null || mText==null){
			return;
		}

		String text=mText;
		if(mWordWrap){
			text=wordWrap(font,mWidth,text);
		}

		int length=text.length();

		if(mVertexData==null || mVertexData.getVertexBuffer(0).getSize()/4<length){
			int i,ix;

			VertexBuffer vertexBuffer=mEngine.loadVertexBuffer(new VertexBuffer(Buffer.UsageType.DYNAMIC,Buffer.AccessType.WRITE_ONLY,mEngine.getVertexFormats().POSITION_TEX_COORD,length*4));
			// Any time our contents would be lost, should be followed up with a resize so they should get restored
			// realistically I should be able to ensure this in Window somehow, but for now we'll just trust its the truth
			// TODO: Why if I turn it off in BorderWidget does it not work?  Maybe i'm incorrect?
			vertexBuffer.setRememberContents(false);
			mVertexData=new VertexData(vertexBuffer);

			IndexBuffer indexBuffer=mEngine.loadIndexBuffer(new IndexBuffer(Buffer.UsageType.STATIC,Buffer.AccessType.WRITE_ONLY,IndexBuffer.IndexFormat.UINT_16,length*6));
			mIndexData=new IndexData(IndexData.Primitive.TRIS,indexBuffer,0,length*6);

			{
				IndexBufferAccessor iba=new IndexBufferAccessor();
				iba.lock(indexBuffer,Buffer.LockType.WRITE_ONLY);
				for(i=0;i<length;i++){
					ix=i*6;
					iba.set(ix+0,i*4+0);
					iba.set(ix+1,i*4+1);
					iba.set(ix+2,i*4+3);
					iba.set(ix+3,i*4+3);
					iba.set(ix+4,i*4+1);
					iba.set(ix+5,i*4+2);
				}
				iba.unlock();
			}
		}

		font.updateVertexBufferForString(mVertexData.getVertexBuffer(0),text,Colors.WHITE,Font.Alignment.BIT_LEFT|Font.Alignment.BIT_TOP);
		mIndexData.setCount(length*6);

		Texture texture=font.getTexture();
		mTextureStage.setTexture(texture);
		if((texture.getFormat()&Texture.Format.BIT_A)>0){
			mBlend=Blend.Combination.ALPHA;
		}
		else{
			mBlend=Blend.Combination.COLOR;
		}
	}

	protected String mText;
	protected Font mFont=null;
	protected Color mColor=new Color(Colors.WHITE);
	protected boolean mWordWrap=false;

	protected VertexData mVertexData=null;
	protected IndexData mIndexData=null;
	protected TextureStage mTextureStage=null;
	protected Blend mBlend=new Blend();

	protected Window mWindow=null;
}
