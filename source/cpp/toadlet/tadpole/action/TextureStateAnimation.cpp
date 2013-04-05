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

#include <toadlet/tadpole/action/TextureStateAnimation.h>

namespace toadlet{
namespace tadpole{
namespace action{

TextureStateAnimation::TextureStateAnimation(RenderState *target,Shader::ShaderType type,int index,Sequence *sequence,int trackIndex):
	mTarget(NULL),
	mType(Shader::ShaderType_FRAGMENT),
	mIndex(0),
//	mSequence,
//	mTrack,
	mPositionElement(0),mRotateElement(0),mScaleElement(0),
	mValue(0)
{
	setTarget(target,type,index);
	setSequence(sequence,trackIndex);
}

void TextureStateAnimation::setTarget(RenderState *target,Shader::ShaderType type,int index){
	mTarget=target;
	mType=type;
	mIndex=index;
}

bool TextureStateAnimation::setSequence(Sequence *sequence,int trackIndex){
	mSequence=sequence;
	mTrack=mSequence->getTrack(trackIndex);
	mPositionElement=mTrack->getFormat()->findElement(VertexFormat::Semantic_POSITION);
	mRotateElement=mTrack->getFormat()->findElement(VertexFormat::Semantic_ROTATE);
	mScaleElement=mTrack->getFormat()->findElement(VertexFormat::Semantic_SCALE);
	if(mPositionElement<0){
		Error::invalidParameters(Categories::TOADLET_TADPOLE,
			"no Semantic_POSITION in Track");
		return false;
	}
	if(mRotateElement<0){
		Error::invalidParameters(Categories::TOADLET_TADPOLE,
			"no Semantic_ROTATE in Track");
		return false;
	}
	if(mScaleElement<0){
		Error::invalidParameters(Categories::TOADLET_TADPOLE,
			"no Semantic_SCALE in Track");
		return false;
	}

	if(mListener!=NULL){
		mListener->animationExtentsChanged(this);
	}

	return true;
}

void TextureStateAnimation::setValue(scalar value){
	mValue=value;
	if(mTarget!=NULL){
		int f1=-1,f2=-1;
		int hint=0;
		scalar time=mTrack->getKeyFramesAtTime(value,f1,f2,hint);

		VertexBufferAccessor &vba=mTrack->getAccessor();

		Vector3 p1,p2,position;
		Quaternion r1,r2,rotate;
		Vector3 s1,s2,scale;
		vba.get3(f1,mPositionElement,p1);
		vba.get3(f2,mPositionElement,p2);
		vba.get4(f1,mRotateElement,r1);
		vba.get4(f2,mRotateElement,r2);
		vba.get3(f1,mScaleElement,s1);
		vba.get3(f2,mScaleElement,s2);
		Math::lerp(position,p1,p2,time);
		Math::slerp(rotate,r1,r2,time);
		Math::lerp(scale,s1,s2,time);

		TextureState state;
		mTarget->getTextureState(mType,mIndex,state);
		Math::setMatrix4x4FromTranslateRotateScale(state.matrix,position,rotate,scale);
		mTarget->setTextureState(mType,mIndex,state);
	}
}

}
}
}
