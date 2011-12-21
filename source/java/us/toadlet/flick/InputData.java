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

package us.toadlet.flick;

public class InputData{
	public static final int Semantic_JOY_BUTTON_PRESSED=0;
	public static final int Semantic_JOY_BUTTON_RELEASED=1;
	public static final int Semantic_JOY_DIRECTION=2;
	public static final int Semantic_JOY_ROTATION=3;
	public static final int Semantic_MAX_JOY=4;
	
	public static final int Semantic_MOTION_ACCELERATION=0;
	public static final int Semantic_MOTION_VELOCITY=1;
	public static final int Semantic_MAX_MOTION=2;

	public static final int Semantic_LIGHT=0;
	public static final int Semantic_MAX_LIGHT=1;

	public InputData(int type1,int time1,int size){
		type=type1;
		time=time1;
		resize(size);
	}
	
	public void set(InputData data){
		type=data.type;
		time=data.time;
		valid=data.valid;
		resize(data.values.length);
		int i;
		for(i=0;i<values.length;++i){
			System.arraycopy(data.values[i],0,values[i],0,4);
		}
	}

	public void resize(int size){
		values=new float[size][];
		int i;
		for(i=0;i<size;++i){
			values[i]=new float[4];
		}
	}

	public int type;
	public long time;
	public int valid;
	public float[][] values;
};
