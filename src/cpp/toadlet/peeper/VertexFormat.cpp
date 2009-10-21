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

#include <toadlet/peeper/VertexFormat.h>

namespace toadlet{
namespace peeper{

VertexFormat::VertexFormat(int numVertexElements):
	//mVertexElements,
	mVertexSize(0),
	mMaxColorIndex(0),
	mMaxTexCoordIndex(0)
{
	mVertexElements.reserve(numVertexElements);
}

const VertexElement &VertexFormat::addVertexElement(const VertexElement &element){
	short index=mVertexElements.size();

	mVertexElements.add(element);
	mVertexElements[index].offset=mVertexSize;

	mVertexSize+=element.getSize();

	if(element.type==VertexElement::Type_COLOR){
		if(element.index>=mColorElementsByIndex.size()){
			mColorElementsByIndex.resize(element.index+1,-1);
		}
		mColorElementsByIndex[element.index]=index;
	}
	else if(element.type==VertexElement::Type_TEX_COORD){
		if(element.index>=mTexCoordElementsByIndex.size()){
			mTexCoordElementsByIndex.resize(element.index+1,-1);
		}
		mTexCoordElementsByIndex[element.index]=index;
	}
	else{
		if(element.type>=mVertexElementsByType.size()){
			mVertexElementsByType.resize(element.type+1,-1);
		}
		mVertexElementsByType[element.type]=index;
	}

	return mVertexElements[index];
}

bool VertexFormat::hasVertexElementOfType(VertexElement::Type type,int index) const{
	if(type==VertexElement::Type_COLOR){
		return mColorElementsByIndex.size()>index && mColorElementsByIndex[index]!=-1;
	}
	else if(type==VertexElement::Type_TEX_COORD){
		return mTexCoordElementsByIndex.size()>index && mTexCoordElementsByIndex[index]!=-1;
	}
	else{
		return mVertexElementsByType.size()>type && mVertexElementsByType[type]!=-1;
	}
}

const VertexElement &VertexFormat::getVertexElementOfType(VertexElement::Type type,int index) const{
	TOADLET_ASSERT(hasVertexElementOfType(type,index));

	if(type==VertexElement::Type_COLOR){
		return mVertexElements[mColorElementsByIndex[index]];
	}
	else if(type==VertexElement::Type_TEX_COORD){
		return mVertexElements[mTexCoordElementsByIndex[index]];
	}
	else{
		return mVertexElements[mVertexElementsByType[type]];
	}
}

int VertexFormat::getVertexElementIndexOfType(VertexElement::Type type,int index) const{
	if(type==VertexElement::Type_COLOR){
		if(mColorElementsByIndex.size()>index){
			return mColorElementsByIndex[index];
		}
		else{
			return -1;
		}
	}
	else if(type==VertexElement::Type_TEX_COORD){
		if(mTexCoordElementsByIndex.size()>index){
			return mTexCoordElementsByIndex[index];
		}
		else{
			return -1;
		}
	}
	else{
		if(mVertexElementsByType.size()>type){
			return mVertexElementsByType[type];
		}
		else{
			return -1;
		}
	}
}

bool VertexFormat::equals(const VertexFormat &format) const{
	if(format.mVertexElements.size()!=mVertexElements.size()){
		return false;
	}
	int i;
	for(i=0;i<mVertexElements.size();++i){
		if(format.mVertexElements[i].equals(mVertexElements[i])==false){
			return false;
		}
	}
	return true;
}

}
}
