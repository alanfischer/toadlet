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

namespace toadlet{
namespace tadpole{

// Either this should be rolled into the TextureManager, or the TextureManager will make use of it, and then the idea of
//  creating a packed texture will be accessable from the TextureManager
class TOADLET_API PixelPacker{
public:
	class Node{
	public:
		Node(int x1=0,int y1=0,int width1=0,int height1=0):x(x1),y(y1),width(width1),height(height1),id(0){
			children[0]=NULL;
			children[1]=NULL;
		}

		Node *children[2];
		int x,int y,int width,int height;
		int id;
	};

	PixelPacker();
	virtual ~PixelPacker();

	int insert(Image::ptr image){
		Node *node=insert(mRootNode,image);
		if(node!=NULL){
			copyImagePortion(image->getFormat(),image->getData(),0,0,image->getWidth(),image->getHeight(),mData,node->x,node->y,node->width,node->height);
			return node->id;
		}
		else{
			return -1;
		}
	}
	
	Node *insert(Node *node,Image::ptr image){
		// Push image to children
		if(node->children[0]!=NULL && node->children[1]!=NULL){
			Node *node=insert(children[0],image);
			
			if(node!=NULL) return node;
			return insert(children[1],image);
		}
		// Create new children
		else{
			if(id==0) return NULL;

			int imageWidth=image->getWidth();
			int imageHeight=image->getHeight();
			
			if(width<imageWidth || height<imageHeight) return;
			if(width==imageWidth && height==imageHeight) return this;

			int dw=width-imageWidth;
			int dh=height-imageHeight;
			
			if(dw>dh){
				children[0]=new Node(x,y,imageWidth,height);
				children[1]=new Node(x+imageWidth,y,width-imageWidth,height);
			}
			else{
				children[0]=new Node(x,y,width,imageHeight);
				children[1]=new Node(x,y+imageHeight,width,height-imageHeight);
			}
			
			return insert(children[0],image);
		}
	}
	
protected:
	Node *mRootNode;
};

}
}

#endif
