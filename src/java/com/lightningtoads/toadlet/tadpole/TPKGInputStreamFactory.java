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

package com.lightningtoads.toadlet.tadpole;

#include <com/lightningtoads/toadlet/tadpole/Types.h>

import com.lightningtoads.toadlet.egg.Categories;
import com.lightningtoads.toadlet.egg.Error;
import com.lightningtoads.toadlet.egg.Logger;
import com.lightningtoads.toadlet.egg.io.InputStreamFactory;
import java.io.ByteArrayInputStream;
import java.io.DataInputStream;
import java.io.InputStream;
import java.io.IOException;
import java.util.HashMap;

public class TPKGInputStreamFactory implements InputStreamFactory{
	public TPKGInputStreamFactory(){}

	public boolean init(InputStream inputStream) throws IOException{
		inputStream.mark(inputStream.available());
		mInputStream=new DataInputStream(inputStream);

		mDataOffset=0;

		byte[] signature=new byte[4];
		mDataOffset+=mInputStream.read(signature,0,4);
		if(signature[0]!='T' || signature[1]!='P' || signature[2]!='K' || signature[3]!='G'){
			Error.unknown(Categories.TOADLET_TADPOLE,
				"Not of TPKG format");
			mInputStream=null;
			return false;
		}

		int version=0;
		version=mInputStream.readInt();
		mDataOffset+=4;
		if(version!=1){
			Error.unknown(Categories.TOADLET_TADPOLE,
				"Not TPKG version 1");
			mInputStream=null;
			return false;
		}

		int numFiles=0;
		numFiles=mInputStream.readInt();
		mDataOffset+=4;
		int i;
		for(i=0;i<numFiles;++i){
			int nameLength=0;
			nameLength=mInputStream.readInt();
			mDataOffset+=4;
			byte[] name=new byte[nameLength]; // No need for the extra null on the end in java
			mDataOffset+=mInputStream.read(name,0,nameLength);
			
			Index index=new Index();
			index.position=mInputStream.readInt();
			mDataOffset+=4;
			index.length=mInputStream.readInt();
			mDataOffset+=4;
			mIndex.put(new String(name),index);
		}

		return true;
	}

	public InputStream makeInputStream(String name){
		Logger.log(Categories.TOADLET_TADPOLE,Logger.Level.DEBUG,
			"Creating InputStream for "+name);

		if(mInputStream==null){
			Error.unknown(Categories.TOADLET_TADPOLE,
				"TPKG not opened");
			return null;
		}

		Index it=mIndex.get(name);
		if(it==null){
			Error.unknown(Categories.TOADLET_TADPOLE,
				name+" not found in data file");
			return null;
		}

		try{
			mInputStream.reset();
			mInputStream.skip(mDataOffset+it.position);
			int length=it.length;

			byte[] data=new byte[length];
			mInputStream.read(data,0,length);
			return new ByteArrayInputStream(data,0,length);
		}
		catch(IOException ex){
			Error.unknown(Categories.TOADLET_TADPOLE,
				"Error reading data");
			return null;
		}
	}

	public class Index{
		public int position;
		public int length;
	};

	protected HashMap<String,Index> mIndex=new HashMap<String,Index>();
	protected int mDataOffset=0;
	protected DataInputStream mInputStream=null;
}
