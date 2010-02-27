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

#ifndef TOADLET_TADPOLE_PIXELPACKER_H
#define TOADLET_TADPOLE_PIXELPACKER_H

#include <toadlet/tadpole/Types.h>
#include <toadlet/egg/image/ImageFormatConversion.h>
#include <memory.h>

namespace toadlet{
namespace tadpole{

// Either this should be rolled into the TextureManager, or the TextureManager will make use of it, and then the idea of
//  creating a packed texture will be accessable from the TextureManager
class TOADLET_API PixelPacker{
public:
	class Node{
	public:
		Node(int x1=0,int y1=0,int width1=0,int height1=0):x(x1),y(y1),width(width1),height(height1),assigned(false){
			children[0]=NULL;
			children[1]=NULL;
		}

		Node *children[2];
		int x,y,width,height;
		bool assigned;
	};

	PixelPacker(byte *data,int format,int width,int height){
		mData=data;
		mFormat=format;
		mWidth=width;
		mHeight=height;
		
		mRootNode=new Node(0,0,mWidth,mHeight);
	}
	
	virtual ~PixelPacker(){}

	bool insert(int width,int height,byte *data,Matrix4x4 &result){
		Node *node=insert(mRootNode,width,height);
		if(node!=NULL){
			node->assigned=true;

			int pixelSize=egg::image::ImageFormatConversion::getPixelSize(mFormat);
			int y=0;
			for(y=0;y<height;++y){
				byte *src=data + (y*width*pixelSize);
				byte *dst=mData + ((node->y+y)*mWidth+node->x)*pixelSize;
				memcpy(dst,src,width*pixelSize);
			}

			Math::setMatrix4x4FromTranslate(result,Math::div(Math::fromInt(node->x),Math::fromInt(mWidth)),Math::div(Math::fromInt(node->y),Math::fromInt(mHeight)),0);
			Math::setMatrix4x4FromScale(result,Math::div(Math::fromInt(width),Math::fromInt(mWidth)),Math::div(Math::fromInt(height),Math::fromInt(mHeight)),Math::ONE);
			return true;
		}
		else{
			return false;
		}
	}
	
	Node *insert(Node *node,int width,int height){
		// Push image to children
		if(node->children[0]!=NULL && node->children[1]!=NULL){
			Node *child=insert(node->children[0],width,height);
			if(child!=NULL) return child;
			return insert(node->children[1],width,height);
		}
		// Create new children
		else{
			if(node->assigned) return NULL;

			if(node->width<width || node->height<height) return NULL;
			if(node->width==width && node->height==height) return node;

			int dw=node->width-width;
			int dh=node->height-height;
			
			if(dw>dh){
				node->children[0]=new Node(node->x,node->y,width,node->height);
				node->children[1]=new Node(node->x+width,node->y,node->width-width,node->height);
			}
			else{
				node->children[0]=new Node(node->x,node->y,node->width,height);
				node->children[1]=new Node(node->x,node->y+height,node->width,node->height-height);
			}
			
			return insert(node->children[0],width,height);
		}
	}
	
protected:
	Node *mRootNode;

	byte *mData;
	int mFormat;
	int mWidth;
	int mHeight;
};

}
}

#endif
