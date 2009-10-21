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

package com.lightningtoads.toadlet.peeper;

#include <com/lightningtoads/toadlet/peeper/Types.h>

public class TextureBlend{
	public enum Operation{
		UNSPECIFIED,
		REPLACE,
		ADD,
		MODULATE,
		DOTPRODUCT,
	}

	public enum Source{
		UNSPECIFIED,
		PREVIOUS,
		TEXTURE,
		PRIMARY_COLOR,
	}

	public TextureBlend(){}

	public TextureBlend(Operation op,Source src1,Source src2){
		operation=op;
		source1=src1;
		source2=src2;
	}

	public TextureBlend set(TextureBlend blend){
		operation=blend.operation;
		source1=blend.source1;
		source2=blend.source2;

		return this;
	}

	public TextureBlend set(Operation op,Source src1,Source src2){
		operation=op;
		source1=src1;
		source2=src2;

		return this;
	}

	public boolean equals(Object object){
		if(this==object){
			return true;
		}
		if((object==null) || (object.getClass()!=getClass())){
			return false;
		}
		TextureBlend blend=(TextureBlend)object;
		return operation==blend.operation && source1==blend.source1 && source2==blend.source2;
	}

	public Operation operation=Operation.UNSPECIFIED;
	public Source source1=Source.UNSPECIFIED;
	public Source source2=Source.UNSPECIFIED;
}
