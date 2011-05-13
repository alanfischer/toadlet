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
#include <toadlet/egg/WeakPointer.h>

namespace toadlet{
namespace egg{

class ProfileData;

class TOADLET_API Profile{
public:
	static const int STACK_MAX_DEPTH=20;

	static Profile *getInstance();

	void beginSection(const String &name);
	void endSection(const String &name);
	void addTimings();
	void clearTimings();

	int getTimingAverage(const String &name) const;
	int getNumTimings() const;
	String getTimingName(int i) const;

	class Timing{
	public:
		TOADLET_SHARED_POINTERS(Timing);

		Timing(const String &name):
			total(0),
			current(0),
			depth(0)
		{
			this->name=name;
		}

		String name;
		uint64 total;
		uint64 current;
		int depth;
	};

protected:
	Profile();
	~Profile();

	static Profile *mTheProfile;

	ProfileData *mData;
};

class TOADLET_API ScopeProfile{
public:
	ScopeProfile(const String &name):mName(name){
		Profile::getInstance()->beginSection(mName);
	}
	
	~ScopeProfile(){
		Profile::getInstance()->endSection(mName);
	}

protected:
	String mName;
};

}
}

#if defined(TOADLET_PROFILE)
	#define TOADLET_PROFILE_BEGINSECTION(x) toadlet::egg::Profile::getInstance()->beginSection(#x)
	#define TOADLET_PROFILE_ENDSECTION(x) toadlet::egg::Profile::getInstance()->endSection(#x)
	#define TOADLET_PROFILE_SCOPE(x) toadlet::egg::ScopeProfile PROFILE##x(#x)
	#define TOADLET_PROFILE_AUTOSCOPE() toadlet::egg::ScopeProfile PROFILE##__LINE__(__FUNCTION__)
	#define TOADLET_PROFILE_ADDTIMINGS() toadlet::egg::Profile::getInstance()->addTimings()
	#define TOADLET_PROFILE_CLEARTIMINGS() toadlet::egg::Profile::getInstance()->clearTimings()
#else
	#define TOADLET_PROFILE_BEGINSECTION(x)
	#define TOADLET_PROFILE_ENDSECTION(x)
	#define TOADLET_PROFILE_SCOPE(x)
	#define TOADLET_PROFILE_AUTOSCOPE()
	#define TOADLET_PROFILE_ADDTIMINGS()
	#define TOADLET_PROFILE_CLEARTIMINGS()
#endif

#endif
