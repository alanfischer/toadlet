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

#ifndef TOADLET_TADPOLE_SEQUENCE_H
#define TOADLET_TADPOLE_SEQUENCE_H

#include <toadlet/egg/BaseResource.h>
#include <toadlet/tadpole/Track.h>

namespace toadlet{
namespace tadpole{

class TOADLET_API Sequence:public BaseResource{
public:
	TOADLET_RESOURCE(Sequence,Sequence);

	Sequence();

	void destroy();

	int getNumTracks() const{return mTracks.size();}
	void addTrack(Track *track);
	void removeTrack(int i);
	Track *getTrack(int i) const{return mTracks[i];}

	scalar getLength() const{return mLength;}
	void setLength(scalar length){mLength=length;}

	void compile();

protected:
	Collection<Track::ptr> mTracks;
	scalar mLength;
};

}
}

#endif
