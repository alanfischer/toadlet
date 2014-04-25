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

#include <toadlet/tadpole/terrain/TextureDataSource.h>
#include <toadlet/peeper/TextureFormatConversion.h>

namespace toadlet{
namespace tadpole{
namespace terrain{

TextureDataSource::TextureDataSource(Engine *engine,const Vector3 &scale,scalar offset,const String &name):
	mEngine(NULL),
	mPatchSize(0),
	mPatchScale(scale),
	mOffset(offset),
	//mTextureFormat
	//mFormat
	mTextureData(NULL),
	mData(NULL)
{
	mEngine=engine;
	setTexture(name,0,0);
}

TextureDataSource::TextureDataSource(Engine *engine,const Vector3 &scale,scalar offset,Texture *texture):
	mEngine(NULL),
	mPatchSize(0),
	mPatchScale(scale),
	mOffset(offset),
	//mTextureFormat
	//mFormat
	mTextureData(NULL),
	mData(NULL)
{
	mEngine=engine;
	if(texture!=NULL){
		setTexture(texture,0,0);
	}
}

TextureDataSource::~TextureDataSource(){
	delete[] mTextureData;
	delete[] mData;
}

bool TextureDataSource::setTexture(const String &name,int px,int py){
	return mEngine->getTextureManager()->find(name,new TextureRequest(this,px,py),new TextureData(Texture::Usage_BIT_STAGING));
}

bool TextureDataSource::setTexture(Texture *texture,int px,int py){
	TextureFormat::ptr format=texture->getFormat();
	if(format->getWidth()!=format->getHeight() || (mPatchSize!=0 && mPatchSize!=format->getWidth())){
		Error::unknown("incorrect texture size");
		return false;
	}

	if(mPatchSize==0){
		mPatchSize=format->getWidth();
		mTextureFormat=texture->getFormat();
		mTextureData=new tbyte[mTextureFormat->getDataSize()];
		mFormat=new TextureFormat(TextureFormat::Dimension_D2,TextureFormat::Format_L_8,mPatchSize,mPatchSize,1,0);
		mData=new tbyte[mFormat->getDataSize()];
		memset(mData,0,mFormat->getDataSize());
	}

	mTexture[px][py]=texture;

	return true;
}

bool TextureDataSource::getPatchHeightData(scalar *data,int px,int py){
	Log::debug("TextureDataSource::getPatchHeightData");

	if(mTexture[px][py]==NULL){
		int i,j;
		for(i=0;i<mPatchSize;++i){
			for(j=0;j<mPatchSize;++j){
				data[i*mPatchSize+j]=mOffset/mPatchScale.z;
			}
		}
		return true;
	}

	bool result=mTexture[px][py]->read(mTextureFormat,mTextureData);
	if(result){
		result=TextureFormatConversion::convert(mTextureData,mTextureFormat,mData,mFormat);
	}
	if(result){
		int i,j;
		for(i=0;i<mPatchSize;++i){
			for(j=0;j<mPatchSize;++j){
				data[j*mPatchSize+i]=((float)mData[j*mPatchSize+i])/255.0 + mOffset/mPatchScale.z;
			}
		}
	}
	return result;
}

bool TextureDataSource::getPatchLayerData(tbyte *data,int px,int py){
	memset(data,0,mPatchSize*mPatchSize);

	return true;
}

}
}
}
