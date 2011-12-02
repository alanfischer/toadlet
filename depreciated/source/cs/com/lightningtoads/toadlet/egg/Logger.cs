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
 * Thread synch is done by adding the [MethodImpl(MethodImplOptions.Synchronized)] attr
 *      There is also a "lock" function which may work
 * addLogString output goes to System.console rather than System.out
 * 
 * ***********************************************/

using System.Runtime.CompilerServices;
using System.Text;

namespace com.lightningtoads.toadlet.egg{

public sealed class Logger{
	public readonly static byte LEVEL_DISABLED=0;

    public readonly static byte LEVEL_ERROR=1;
	public readonly static byte LEVEL_WARNING=2;
	public readonly static byte LEVEL_ALERT=3;
	public readonly static byte LEVEL_DEBUG=4;
	public readonly static byte LEVEL_EXCESSIVE=5;

	public readonly static byte LEVEL_MAX=100;

	Logger(){}

    [MethodImpl(MethodImplOptions.Synchronized)] 
	public static Logger getInstance(){
		if(mTheLogger==null){
			mTheLogger=new Logger();
		}

		return mTheLogger;
	}

    [MethodImpl(MethodImplOptions.Synchronized)] 
	public void setMasterReportingLevel(byte level){
		mReportingLevel=level;

		addLogString(LEVEL_DISABLED,"Master Reporting Level is "+(int)level);
	}
    [MethodImpl(MethodImplOptions.Synchronized)]
	public byte getMasterReportingLevel(){
		return mReportingLevel;
	}

    [MethodImpl(MethodImplOptions.Synchronized)]
	public void addLogString(int level,string text){
		if(mReportingLevel>=level){
			string prefix;

			if(level==LEVEL_ALERT){
				prefix="ALERT:   ";
			}
			else if(level==LEVEL_WARNING){
				prefix="WARNING: ";
			}
			else if(level==LEVEL_ERROR){
				prefix="ERROR:   ";
			}
			else if(level==LEVEL_DEBUG){
				prefix="DEBUG:   ";
			}
			else{
				prefix="";
			}

			string line=prefix+text+"\n";

            System.Console.WriteLine(line);

            mStringBuffer.Append(line);
		}
	}

    [MethodImpl(MethodImplOptions.Synchronized)]
	public string getLogString(){
		return mStringBuffer.ToString();
	}

	private static Logger mTheLogger;
	private byte mReportingLevel=LEVEL_MAX;
	private StringBuilder mStringBuffer=new StringBuilder();
}

}
