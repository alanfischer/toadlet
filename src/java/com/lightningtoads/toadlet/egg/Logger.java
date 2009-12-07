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

package com.lightningtoads.toadlet.egg;

#include <com/lightningtoads/toadlet/Types.h>

#define TOADLET_MAKE_LOGGER_FUNCTION(name,level) \
	public static void name(String description){name(null,description);} \
	public static void name(String categoryName,const String &description){ \
		Logger instance=getInstance(); \
		if(level>Level.MAX) ; \
		else if(level<=instance.getMasterReportingLevel() && level<=instance.getCategoryReportingLevel(categoryName)){ \
			instance.addLogString(categoryName,level,description); \
		} \
	}

import java.util.Vector;
import java.util.HashMap;

public final class Logger{
	public static class Level{
		public final static byte DISABLED=0;
		public final static byte ERROR=1;
		public final static byte WARNING=2;
		public final static byte ALERT=3;
		public final static byte DEBUG=4;
		public final static byte EXCESS=5;
		public final static byte MAX=6;
	}

	public class Category{
		public Category(String name){
			this.name=name;
		}

		public String name;
		public byte reportingLevel=Level.MAX;
	};

	Logger(){
		mLoggedMessage=false;
		mReportingLevel=Level.MAX;
		mOutputLogString=true;
		mStoreLogString=true;

		addCategory(Categories.TOADLET_EGG_LOGGER);
		addCategory(Categories.TOADLET_EGG);
		addCategory(Categories.TOADLET_FLICK);
		addCategory(Categories.TOADLET_HOP);
		addCategory(Categories.TOADLET_KNOT);
		addCategory(Categories.TOADLET_PEEPER);
		addCategory(Categories.TOADLET_RIBBIT);
		addCategory(Categories.TOADLET_TADPOLE);
		addCategory(Categories.TOADLET_PAD);
	}

	public synchronized static Logger getInstance(){
		if(mTheLogger==null){
			mTheLogger=new Logger();
		}

		return mTheLogger;
	}

	public synchronized static void destroy(){
		if(mTheLogger!=null){
			mTheLogger=null;
		}
	}

	TOADLET_MAKE_LOGGER_FUNCTION(error,Level.ERROR);
	TOADLET_MAKE_LOGGER_FUNCTION(warning,Level.WARNING);
	TOADLET_MAKE_LOGGER_FUNCTION(alert,Level.ALERT);
	TOADLET_MAKE_LOGGER_FUNCTION(debug,Level.DEBUG);
	TOADLET_MAKE_LOGGER_FUNCTION(excess,Level.EXCESS);

	public synchronized void setMasterReportingLevel(byte level){
		mReportingLevel=level;

		addLogString(Categories.TOADLET_EGG_LOGGER,Level.DISABLED,"Master Reporting Level is "+(int)level);
	}

	public synchronized byte getMasterReportingLevel(){
		return mReportingLevel;
	}

	public synchronized void setCategoryReportingLevel(String categoryName,byte level){
		Category category=getCategory(categoryName);

		if(category!=null){
			category.reportingLevel=level;
		}
	}

	public synchronized byte getCategoryReportingLevel(String categoryName){
		byte reportingLevel=Level.MAX;

		Category category=getCategory(categoryName);
		if(category!=null){
			reportingLevel=category.reportingLevel;
		}

		return reportingLevel;
	}

	public synchronized byte getMasterCategoryReportingLevel(String categoryName){
		byte level=getCategoryReportingLevel(categoryName);
		if(level>mReportingLevel){
			level=mReportingLevel;
		}
		return level;
	}

	public synchronized void addLoggerListener(LoggerListener listener){
		mLoggerListeners.addElement(listener);
	}

	public synchronized void removeLoggerListener(LoggerListener listener){
		mLoggerListeners.removeElement(listener);
	}

	public synchronized void setOutputLogString(boolean outputLogString){
		mOutputLogString=outputLogString;
	}

	public synchronized boolean getOutputLogString(){
		return mOutputLogString;
	}

	public synchronized void setStoreLogString(boolean storeLogString){
		mStoreLogString=storeLogString;
	}

	public synchronized boolean getStoreLogString(){
		return mStoreLogString;
	}

	public synchronized void addLogString(String categoryName,byte level,String data){
		Category category=getCategory(categoryName);
		if((category==null || category.reportingLevel>=level) && mReportingLevel>=level){
			if(mLoggedMessage==false){
				mLoggedMessage=true;

				String line="Started toadlet::egg::Logger";

				byte loggerLevel=getCategoryReportingLevel(Categories.TOADLET_EGG_LOGGER);
				if(loggerLevel>Level.DISABLED){
					addCompleteLogString(null,Level.DISABLED,line);
				}
			}

			addCompleteLogString(category,level,data);
		}
	}

	public synchronized void addLogString(byte level,String string){
		addLogString(null,level,string);
	}

	public synchronized String getLogString(){
		return mLogString.toString();
	}

	protected synchronized void addCompleteLogString(Category category,byte level,String data){
		int i;
		for(i=mLoggerListeners.size()-1;i>=0;--i){
			mLoggerListeners.elementAt(i).addLogString(category,level,data);
		}

		if(mOutputLogString || mStoreLogString){
			String prefix;

			switch(level){
				case Level.DISABLED:
					prefix="LOGGER:  ";
				break;
				case Level.ERROR:
					prefix="ERROR:   ";
				break;
				case Level.WARNING:
					prefix="WARNING: ";
				break;
				case Level.ALERT:
					prefix="ALERT:   ";
				break;
				case Level.DEBUG:
					prefix="DEBUG:   ";
				break;
				case Level.EXCESS:
					prefix="EXCESS:  ";
				break;
				default:
					prefix="UNKNOWN: ";
				break;
			}

			String line=prefix+data;

			if(mOutputLogString){
				#if defined(TOADLET_PLATFORM_ANDROID)
					int priority=android.util.Log.ERROR-level+Level.ERROR; // Convert from Level to a Priority
					android.util.Log.println(priority,category!=null?category.name:"toadlet",data);
				#else
					System.out.println(line);
				#endif
			}

			if(mStoreLogString){
				mLogString.append(line);
				mLogString.append((char)10);
			}
		}
	}

	protected synchronized Category addCategory(String categoryName){
		Category category=new Category(categoryName);
		mCategoryNameCategoryMap.put(categoryName,category);
		return category;
	}

	protected synchronized Category getCategory(String categoryName){
		Category category=mCategoryNameCategoryMap.get(categoryName);
		return category;
	}

	protected static Logger mTheLogger;

	protected Vector<LoggerListener> mLoggerListeners=new Vector<LoggerListener>();
	protected boolean mOutputLogString;
	protected boolean mStoreLogString;
	protected byte mReportingLevel;
	protected boolean mLoggedMessage;
	protected HashMap<String,Category> mCategoryNameCategoryMap=new HashMap<String,Category>();
	protected StringBuilder mLogString=new StringBuilder();
}
