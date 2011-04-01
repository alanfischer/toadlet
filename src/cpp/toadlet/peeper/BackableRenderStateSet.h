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

#ifndef TOADLET_PEEPER_BACKABLERENDERSTATESET_H
#define TOADLET_PEEPER_BACKABLERENDERSTATESET_H

#include <toadlet/peeper/RenderStateSet.h>

namespace toadlet{
namespace peeper{

class TOADLET_API BackableRenderStateSet:public RenderStateSet{
public:
	TOADLET_SHARED_POINTERS(BackableRenderStateSet);

	BackableRenderStateSet():
		mListener(NULL),
		mBlendState(NULL),
		mDepthState(NULL),
		mRasterizerState(NULL),
		mFogState(NULL),
		mPointState(NULL),
		mMaterialState(NULL)
	{}
	virtual ~BackableRenderStateSet(){}

	virtual RenderStateSet *getRootRenderStateSet(){return mBack;}

	virtual void setRenderStateSetDestroyedListener(RenderStateSetDestroyedListener *listener){mListener=listener;}

	virtual bool create(){
		destroy();

		return true;
	}

	virtual void destroy(){
		if(mBack!=NULL){
			mBack->destroy();
			mBack=NULL;
		}

		if(mBlendState!=NULL){
			delete mBlendState;
			mBlendState=NULL;
		}
		if(mDepthState!=NULL){
			delete mDepthState;
			mDepthState=NULL;
		}
		if(mRasterizerState!=NULL){
			delete mRasterizerState;
			mRasterizerState=NULL;
		}
		if(mFogState!=NULL){
			delete mFogState;
			mFogState=NULL;
		}
		if(mPointState!=NULL){
			delete mPointState;
			mPointState=NULL;
		}
		if(mMaterialState!=NULL){
			delete mMaterialState;
			mMaterialState=NULL;
		}

		if(mListener!=NULL){
			mListener->renderStateSetDestroyed(this);
			mListener=NULL;
		}
	}

	virtual void setBlendState(const BlendState &state){
		if(mBlendState==NULL){mBlendState=new BlendState(state);}else{mBlendState->set(state);}
		if(mBack!=NULL){mBack->setBlendState(state);}
	}
	virtual bool getBlendState(BlendState &state) const{if(mBlendState==NULL){return false;}else{state.set(*mBlendState);return true;}}

	virtual void setDepthState(const DepthState &state){
		if(mDepthState==NULL){mDepthState=new DepthState(state);}else{mDepthState->set(state);}
		if(mBack!=NULL){mBack->setDepthState(state);}
	}
	virtual bool getDepthState(DepthState &state) const{if(mDepthState==NULL){return false;}else{state.set(*mDepthState);return true;}}

	virtual void setRasterizerState(const RasterizerState &state){
		if(mRasterizerState==NULL){mRasterizerState=new RasterizerState(state);}else{mRasterizerState->set(state);}
		if(mBack!=NULL){mBack->setRasterizerState(state);}
	}
	virtual bool getRasterizerState(RasterizerState &state) const{if(mRasterizerState==NULL){return false;}else{state.set(*mRasterizerState);return true;}}

	virtual void setFogState(const FogState &state){
		if(mFogState==NULL){mFogState=new FogState(state);}else{mFogState->set(state);}
		if(mBack!=NULL){mBack->setFogState(state);}
	}
	virtual bool getFogState(FogState &state) const{if(mFogState==NULL){return false;}else{state.set(*mFogState);return true;}}

	virtual void setPointState(const PointState &state){
		if(mPointState==NULL){mPointState=new PointState(state);}else{mPointState->set(state);}
		if(mBack!=NULL){mBack->setPointState(state);}
	}
	virtual bool getPointState(PointState &state) const{if(mPointState==NULL){return false;}else{state.set(*mPointState);return true;}}

	virtual void setMaterialState(const MaterialState &state){
		if(mMaterialState==NULL){mMaterialState=new MaterialState(state);}else{mMaterialState->set(state);}
		if(mBack!=NULL){mBack->setMaterialState(state);}
	}
	virtual bool getMaterialState(MaterialState &state) const{if(mMaterialState==NULL){return false;}else{state.set(*mMaterialState);return true;}}

	virtual void setBack(RenderStateSet::ptr back){
		if(back!=mBack && mBack!=NULL){
			mBack->destroy();
		}

		mBack=back;
		
		if(mBack!=NULL){
			if(mBlendState!=NULL){mBack->setBlendState(*mBlendState);}
			if(mDepthState!=NULL){mBack->setDepthState(*mDepthState);}
			if(mRasterizerState!=NULL){mBack->getRasterizerState(*mRasterizerState);}
			if(mFogState!=NULL){mBack->setFogState(*mFogState);}
			if(mPointState!=NULL){mBack->setPointState(*mPointState);}
			if(mMaterialState!=NULL){mBack->setMaterialState(*mMaterialState);}
		}
	}
	virtual RenderStateSet::ptr getBack(){return mBack;}

protected:
	RenderStateSetDestroyedListener *mListener;
	BlendState *mBlendState;
	DepthState *mDepthState;
	RasterizerState *mRasterizerState;
	FogState *mFogState;
	PointState *mPointState;
	MaterialState *mMaterialState;

	RenderStateSet::ptr mBack;
};

}
}

#endif
