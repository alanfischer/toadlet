/********** Copyright header - do not remove **********
 *
 * The Toadlet Engine
 *
 * Copyright (C) 2006, Lightning Toads Productions, LLC
 *
 * Author(s): Alan Fischer
 *
 * All source code for the Toadlet Engine, including
 * this file, is the sole property of Lightning Toads
 * Productions, LLC. It has been developed on our own
 * time, with our own tools, on our own hardware. None
 * of this code may be observed, used or changed
 * without our explicit permission.
 *
 *  - Lightning Toads Productions, LLC
 *
 ********** Copyright header - do not remove **********/

package com.lightningtoads.toadlet.egg{

public class Logger{
	public static var LEVEL_DISABLED:int=0;

	public static var LEVEL_ERROR:int=1;
	public static var LEVEL_WARNING:int=2;
	public static var LEVEL_ALERT:int=3;
	public static var LEVEL_DEBUG:int=4;
	public static var LEVEL_EXCESSIVE:int=5;

	public static var LEVEL_MAX:int=100;

	public static function getInstance():Logger{
		if(mTheLogger==null){
			mTheLogger=new Logger();
		}

		return mTheLogger;
	}

	public function setMasterReportingLevel(level:int):void{
		mReportingLevel=level;

		addLogString(LEVEL_DISABLED,"Master Reporting Level is "+level);
	}

	public function getMasterReportingLevel():int{
		return mReportingLevel;
	}

	public function addLogString(level:int,text:String):void{
		if(mReportingLevel>=level){
			var prefix:String;

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

			// The extra backslash here is because gpp strips out one
			var line:String=prefix+text+"\\n";

			trace(line);
		}
	}

	protected static var mTheLogger:Logger;
	protected var mReportingLevel:int=LEVEL_MAX;
}

}
