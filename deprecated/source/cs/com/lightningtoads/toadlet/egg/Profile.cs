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

/***********  c# Conversion from Java ************
 * 
 * package changed to namespace
 * Change class type from final to sealed
 * All final variables converted to readonly variables
 * Times are acquired from the System.DateTime.Now.Ticks object - THIS NEEDS TO BE TESTED
 *
 * ***********************************************/

namespace com.lightningtoads.toadlet.egg{

public sealed class Profile{
	public readonly static byte SECTION_UPDATE=1;
	public readonly static byte SECTION_PAINT=2;
	public readonly static byte SECTION_PHYSICS=3;
	public readonly static byte SECTION_RENDER=4;
	public readonly static byte SECTION_CUSTOM=5;

	private readonly static byte MAX_SECTIONS=100;

	public static void beginSection(byte section){
        mCurrentTimings[section] = System.DateTime.Now.Ticks * tick2ms; // TODO:  Check that this is working as expected -TR
	}

	public static void endSection(byte section){
        mTimingsBuffer[section] += System.DateTime.Now.Ticks*tick2ms - mCurrentTimings[section]; // TODO:  Check that this is working as expected -TR
	}

	public static long getTiming(byte section){
		return mTimings[section];
	}

	public static void copyTimingBuffer(){
		System.Array.Copy(mTimingsBuffer,0,mTimings,0,mTimings.Length);

		int i;
		for(i=0;i<mTimingsBuffer.Length;++i){
			mTimingsBuffer[i]=0;
		}
	}

    private static long[] mCurrentTimings = new long[MAX_SECTIONS];
    private static long[] mTimingsBuffer = new long[MAX_SECTIONS];
    private static long[] mTimings = new long[MAX_SECTIONS];
    private static int tick2ms = 10;  // 1 tick is 100 nanoseconds --> http://msdn.microsoft.com/en-us/library/system.datetime.aspx
}

}
