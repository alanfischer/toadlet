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

#ifndef TOADLET_EGG_TGUID_H
#define TOADLET_EGG_TGUID_H

#include <toadlet/egg/Types.h>
#include <toadlet/egg/String.h>

namespace toadlet{
namespace egg{

class TOADLET_API TGUID{
public:
	TGUID():highBits(0),lowBits(0){}
	TGUID(tbyte bytes[]){highBits=*(uint64*)(bytes+0);lowBits=*(uint64*)(bytes+8);}
	TGUID(uint64 highBits1,uint64 lowBits1):highBits(highBits1),lowBits(lowBits1){}
	TGUID(const String &string){fromString(string);}

	bool fromRandom();
	bool fromString(const String &string);
	String toString();
	
	uint64 getHighBits() const{return highBits;}
	uint64 getLowBits() const{return lowBits;}
		
	inline bool equals(const TGUID &u2) const{return highBits==u2.highBits && lowBits==u2.lowBits;}

	inline bool operator==(const TGUID &u2) const{return equals(u2);}
	inline bool operator!=(const TGUID &u2) const{return !equals(u2);}

	uint64 highBits,lowBits;
};

}
}

#endif

