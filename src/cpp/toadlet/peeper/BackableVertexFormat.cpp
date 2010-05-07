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
	//mSemantics,
	//mIndexes,
	//mFormats,
	//mOffsets,
	mVertexSize(0)
	
	//mBack
{
}

BackableVertexFormat::~BackableVertexFormat(){
	destroy();
}

void BackableVertexFormat::addElement(int semantic,int index,int format){
	mSemantics.add(semantic);
	mIndexes.add(index);
	mFormats.add(format);
	mOffsets.add(mVertexSize);

	mVertexSize+=getFormatSize(format);
	
	if(mBack!=NULL){
		mBack->addElement(semantic,index,format);
	}
}

bool BackableVertexFormat::create(){
	if(mBack!=NULL){
		mBack->create();
	}

	return true;
}

void BackableVertexFormat::destroy(){
	if(mBack!=NULL){
		mBack->destroy();
		mBack=NULL;
	}
}

int BackableVertexFormat::findSemantic(int semantic){
	int i;
	for(i=0;i<mSemantics.size();++i){
		if(mSemantics[i]==semantic){
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
		int i;
		for(i=0;i<mSemantics.size();++i){
			mBack->addElement(mSemantics[i],mIndexes[i],mFormats[i]);
		}
		mBack->create();
	}
}

}
}
