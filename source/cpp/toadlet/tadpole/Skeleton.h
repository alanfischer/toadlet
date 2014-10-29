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

#ifndef TOADLET_TADPOLE_SKELETON_H
#define TOADLET_TADPOLE_SKELETON_H

#include <toadlet/egg/BaseResource.h>
#include <toadlet/tadpole/Sequence.h>

namespace toadlet{
namespace tadpole{

class TOADLET_API Skeleton:public BaseResource{
public:
	TOADLET_RESOURCE(Skeleton,Skeleton);

	class TOADLET_API Bone:public Object{
	public:
		TOADLET_OBJECT(Bone);

		Bone():
			index(-1),
			parentIndex(-1),
			scale(Math::ONE,Math::ONE,Math::ONE){}

		int index;
		int parentIndex;

		Vector3 translate;
		Quaternion rotate;
		Vector3 scale; /// @todo: Implement bone scaling

		Vector3 worldToBoneTranslate;
		Quaternion worldToBoneRotate;

		AABox bound;

		String name;
	};

	Skeleton();

	void destroy();
	void compile();

	int getNumBones() const{return mBones.size();}
	void addBone(Bone *bone){mBones.push_back(bone);}
	void removeBone(int i){mBones.erase(mBones.begin()+i);}
	Bone *getBone(int i) const{return mBones[i];}
	Bone *getBone(const String &name) const;

	int getNumSequences() const{return mSequences.size();}
	void addSequence(Sequence *sequence){mSequences.push_back(sequence);}
	void removeSequence(int i){mSequences.erase(mSequences.begin()+i);}
	Sequence *getSequence(int i){return mSequences[i];}

protected:
	Collection<Bone::ptr> mBones;
	Collection<Sequence::ptr> mSequences;
};

}
}

#endif
