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

#ifndef TOADLET_EGG_IO_STREAM_IOSTREAM_H
#define TOADLET_EGG_IO_STREAM_IOSTREAM_H

#include <toadlet/egg/io/Stream.h>
#include <iostream>

namespace toadlet{
namespace egg{
namespace io{

class Stream_streambuf:public std::streambuf{
public:
	typedef std::char_traits<char> traits_type;

	Stream_streambuf(Stream::ptr parent):mParent(parent){}
	
	std::streampos seekoff(std::streamoff off,std::ios_base::seekdir way,std::ios_base::openmode which){
		if(way == std::ios_base::beg){
			mParent->seek(off);
		}
		else if(way == std::ios_base::cur){
			mParent->seek(off + mParent->position());
		}
		else if(way == std::ios_base::end){
			mParent->seek(off + mParent->length());
		}
		return mParent->position();
	}
		
	pos_type seekpos(pos_type pos,std::ios_base::openmode which){
		mParent->seek(pos);
		return mParent->position();
	}

	std::streambuf::int_type underflow(){
		tbyte value=0;
		if( mParent->read(&value, 1) < 0 ){
			value = EOF;
		}
		return std::streambuf::int_type(value);
	}

	std::streambuf::int_type overflow(std::streambuf::int_type value){
		if( mParent->write((tbyte*)&value, 1) < 0 ){
			value = EOF;
		}
		else if ( value == EOF){
			value = 0;
		}
		return value;
	}

protected:
	Stream::ptr mParent;
};

class Stream_istream: public std::istream{
public:
    Stream_istream(Stream::ptr parent) :
        std::istream(new Stream_streambuf(parent)){}

    virtual ~Stream_istream(){
        delete rdbuf();
    }
};

class Stream_ostream: public std::ostream{
public:
    Stream_ostream(Stream::ptr parent) :
        std::ostream(new Stream_streambuf(parent)){}

    virtual ~Stream_ostream(){
        delete rdbuf();
    }
};

}
}
}

#endif
