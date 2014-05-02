package us.toadlet.egg;

import java.io.*;

public class LoggerOutputStream extends OutputStream{
	public LoggerOutputStream(Logger logger,String category,int level){
		mLogger=logger;
		mCategory=category;
		mLevel=level;
		mBuilder=new StringBuilder();
	}
	
	public void write(byte[] b){
		String string=new String(b);
		if(string.endsWith(System.getProperty("line.separator"))){
			string = string.substring(0,string.length()-System.getProperty("line.separator").length());
		}
		if(string.length()>0){
			mBuilder.append(string);
		}
	}

	public void write(byte[] b, int off, int len){
		String string=new String(b,off,len);
		if(string.endsWith(System.getProperty("line.separator"))){
			string = string.substring(0,string.length()-System.getProperty("line.separator").length());
		}
		if(string.length()>0){
			mBuilder.append(string);
		}
	}
	
	public void write(int b){
		mBuilder.append((char)b);
	}
	
	public void flush(){
		if(mBuilder.length()>0){
			mLogger.addLogEntry(mCategory,mLevel,mBuilder.toString());
			mBuilder.delete(0,mBuilder.length());
		}
	}
	
	StringBuilder mBuilder;
	Logger mLogger;
	String mCategory;
	int mLevel;
}
