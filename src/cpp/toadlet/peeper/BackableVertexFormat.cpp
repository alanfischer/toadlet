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

#include <toadlet/peeper/BackableVertexFormat.h>
#include <toadlet/egg/Error.h>

using namespace toadlet::egg;

namespace toadlet{
namespace peeper{

BackableVertexFormat::BackableVertexFormat():
	mListener(NULL),
	//mSemantics,
	//mNames,
	//mIndexes,
	//mFormats,
	//mOffsets,
	mVertexSize(0)
	
	//mBack
{}

BackableVertexFormat::~BackableVertexFormat(){
	destroy();
}

bool BackableVertexFormat::create(){
	bool result=true;
	if(mBack!=NULL){
		result=mBack->create();
	}

	return result;
}

void BackableVertexFormat::destroy(){
	if(mBack!=NULL){
		mBack->destroy();
		mBack=NULL;
	}
	
	if(mListener!=NULL){
		mListener->vertexFormatDestroyed(this);
		mListener=NULL;
	}
}

bool BackableVertexFormat::addElement(int semantic,const String &name,int index,int format){
	mSemantics.add(semantic);
	mNames.add(name);
	mIndexes.add(index);
	mFormats.add(format);
	mOffsets.add(mVertexSize);

	mVertexSize+=getFormatSize(format);
	
	if(mBack!=NULL){
		mBack->addElement(semantic,index,format);
	}

	return true;
}

int BackableVertexFormat::findElement(int semantic){
	int i;
	for(i=0;i<mSemantics.size();++i){
		if(mSemantics[i]==semantic){
			return i;
		}
	}
	return -1;
}

int BackableVertexFormat::findElement(const String &name){
	int i;
	for(i=0;i<mNames.size();++i){
		if(mNames[i]==name){
			return i;
		}
	}
	return -1;
}

void BackableVertexFormat::setBack(VertexFormat::ptr back){
	if(back!=mBack && mBack!=NULL){
		mBack->destroy();
	}

	mBack=back;
	
	if(mBack!=NULL && mSemantics.size()>0){
		mBack->create();
		int i;
		for(i=0;i<mSemantics.size();++i){
			mBack->addElement(mSemantics[i],mIndexes[i],mFormats[i]);
		}
	}
}

}
}
