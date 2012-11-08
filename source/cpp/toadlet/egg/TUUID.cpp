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

#include <toadlet/egg/TUUID.h>
#include <toadlet/egg/Random.h>
#include <toadlet/egg/System.h>
#include <stdio.h>

namespace toadlet{
namespace egg{

bool TUUID::fromRandom(){
	tbyte *uu=(tbyte*)&highBits;

	Random random(System::utime());
	int i;
	for(i=0;i<16;++i){
		uu[i]=random.nextInt()%0xFF;
	}

	uu[6]=(uu[6] & 0x0F) | 0x40;
	uu[8]=(uu[8] & 0x3F) | 0x80;
	
	return true;
}

bool TUUID::fromString(const String &string){
	tbyte *uu=(tbyte*)&highBits;
	int n=0;
	sscanf(string.c_str(),	"%2hhx%2hhx%2hhx%2hhx-"
							"%2hhx%2hhx-"
							"%2hhx%2hhx-"
							"%2hhx%2hhx-"
							"%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%n",
							&uu[0], &uu[1], &uu[2], &uu[3],
							&uu[4], &uu[5],
							&uu[6], &uu[7],
							&uu[8], &uu[9],
							&uu[10], &uu[11], &uu[12], &uu[13], &uu[14], &uu[15], &n);
	return n==36 && string[n]=='\0';
}

String TUUID::toString() const{
	tbyte *uu=(tbyte*)&highBits;
	char string[128];
	sprintf(string,	"%02x%02x%02x%02x-"
					"%02x%02x-"
					"%02x%02x-"
					"%02x%02x-"
					"%02x%02x%02x%02x%02x%02x",
					uu[0], uu[1], uu[2], uu[3],
					uu[4], uu[5],
					uu[6], uu[7],
					uu[8], uu[9],
					uu[10], uu[11], uu[12], uu[13], uu[14], uu[15]);
	return string;
}

}
}

