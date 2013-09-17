package com.logikworks.jag;

import java.io.*;
import us.toadlet.egg.*;

public class LoggerOutputStream extends OutputStream{
	public LoggerOutputStream(Logger logger,String category,int level){
		mLogger=logger;
		mCategory=category;
		mLevel=level;
		mBuilder=new StringBuilder();
	}
	
	public void write(byte[] b){
		mBuilder.append(new String(b));
	}

	public void write(byte[] b, int off, int len){
		mBuilder.append(new String(b,off,len));
	}
	
	public void write(int b){
		mBuilder.append((char)b);
	}
	
	public void flush(){
		String string=mBuilder.toString();
		if(string.endsWith("\n")){
			string = string.substring(0,string.length()-1);
		}
		mLogger.addLogEntry(mCategory,mLevel,string);
		mBuilder.delete(0,mBuilder.length());
	}
	
	StringBuilder mBuilder;
	Logger mLogger;
	String mCategory;
	int mLevel;
}
