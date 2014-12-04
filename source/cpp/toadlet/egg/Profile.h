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

#ifndef TOADLET_EGG_PROFILE_H
#define TOADLET_EGG_PROFILE_H

#include <toadlet/egg/String.h>
#include <toadlet/egg/SharedPointer.h>

namespace toadlet{
namespace egg{

class ProfileData;

class TOADLET_API Profile{
public:
	class ProfileNode{
	public:
		TOADLET_SPTR(ProfileNode);

		ProfileNode(const char *name);

		ProfileNode *getChild(const char *name);
		void start();
		void stop();
		void clear();

		inline const char *getName() const{return mName;}
		inline uint64 getTotal() const{return mTotal;}
		inline int getCount() const{return mCount;}
		inline ProfileNode *getParent() const{return mParent;}
		inline ProfileNode *getFirstChild() const{return mFirstChild;}
		inline ProfileNode *getNext() const{return mNext;}

	protected:
		const char *mName;
		uint64 mCurrent,mTotal;
		int mCount;
		ProfileNode *mParent;
		ProfileNode::ptr mFirstChild,mNext;
	};

	static Profile *getInstance();

	void beginSection(const char *name);
	void endSection(const char *name);

	ProfileNode *getRoot(){return mRoot;}
	void outputTimings(){outputTimings(mRoot,0,0);}
	void clearTimings(){clearTimings(mRoot);}

protected:
	Profile();
	~Profile();

	void outputTimings(ProfileNode *node,int depth,int total);
	void clearTimings(ProfileNode *node);

	static Profile *mTheProfile;

	ProfileNode::ptr mRoot;
	ProfileNode *mCurrent;
};

class TOADLET_API ScopeProfile{
public:
	ScopeProfile(const char *name):mName(name){
		Profile::getInstance()->beginSection(mName);
	}
	
	~ScopeProfile(){
		Profile::getInstance()->endSection(mName);
	}

protected:
	const char *mName;
};

}
}

#if defined(TOADLET_PROFILE)
	#define TOADLET_PROFILE_BEGINSECTION(x) toadlet::egg::Profile::getInstance()->beginSection(#x)
	#define TOADLET_PROFILE_ENDSECTION(x) toadlet::egg::Profile::getInstance()->endSection(#x)
	#define TOADLET_PROFILE_SCOPE(x) toadlet::egg::ScopeProfile PROFILE##x(#x)
	#define TOADLET_PROFILE_AUTOSCOPE() toadlet::egg::ScopeProfile PROFILE##__LINE__(__FUNCTION__)
	#define TOADLET_PROFILE_OUTPUTTIMINGS() toadlet::egg::Profile::getInstance()->outputTimings()
	#define TOADLET_PROFILE_CLEARTIMINGS() toadlet::egg::Profile::getInstance()->clearTimings()
#else
	#define TOADLET_PROFILE_BEGINSECTION(x)
	#define TOADLET_PROFILE_ENDSECTION(x)
	#define TOADLET_PROFILE_SCOPE(x)
	#define TOADLET_PROFILE_AUTOSCOPE()
	#define TOADLET_PROFILE_OUTPUTTIMINGS()
	#define TOADLET_PROFILE_CLEARTIMINGS()
#endif

#endif
