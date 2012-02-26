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

#include <toadlet/egg/Logger.h>
#include <toadlet/peeper/BackablePixelBufferRenderTarget.h>

namespace toadlet{
namespace peeper{

BackablePixelBufferRenderTarget::BackablePixelBufferRenderTarget():BaseResource()
	//mBack
{}

BackablePixelBufferRenderTarget::~BackablePixelBufferRenderTarget(){
	destroy();
}

bool BackablePixelBufferRenderTarget::create(){
	if(mBack!=NULL){
		return mBack->create();
	}
	else{
		return true;
	}
}

void BackablePixelBufferRenderTarget::destroy(){
	if(mBack!=NULL){
		mBack->destroy();
		mBack=NULL;
	}

	BaseResource::destroy();
}

bool BackablePixelBufferRenderTarget::attach(PixelBuffer::ptr buffer,Attachment attachment){
	mBuffers.add(buffer);
	mBufferAttachments.add(attachment);
	
	if(mBack!=NULL){
		return mBack->attach(buffer,attachment);
	}
	else{
		return true;
	}
}

bool BackablePixelBufferRenderTarget::remove(PixelBuffer::ptr buffer){
	int i;
	for(i=0;i<mBuffers.size();++i){
		if(mBuffers[i]==buffer){
			break;
		}
	}
	if(i==mBuffers.size()){
		return false;
	}

	mBuffers.removeAt(i);
	mBufferAttachments.removeAt(i);

	if(mBack!=NULL){
		return mBack->remove(buffer);
	}
	else{
		return true;
	}
}

void BackablePixelBufferRenderTarget::setBack(PixelBufferRenderTarget::ptr back){
	int i;
	if(back!=mBack && mBack!=NULL){
		mBack->destroy();
	}

	mBack=back;
	
	if(mBack!=NULL){
		mBack->create();

		for(i=0;i<mBuffers.size();++i){
			mBack->attach(mBuffers[i],mBufferAttachments[i]);
		}
	}
}

}
}
