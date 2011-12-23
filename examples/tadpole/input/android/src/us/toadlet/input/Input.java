package us.toadlet.input;

import us.toadlet.pad.*;
import android.os.*;
import java.io.*;

public class Input extends AndroidApplication{
	static{
		System.loadLibrary("input");
	}

	public Input(){
		super();

		System.out.println("Input");
	}

    protected void onCreate(Bundle icicle){
		System.out.println("onCreate");

		super.onCreate(icicle);
    }

    protected void onDestroy(){
		System.out.println("onDestroy");

        super.onDestroy();
    }
	
	protected void onStart(){
		System.out.println("onStart");
		
		OutputStream out=null;
		try{
			File file=new File(getExternalFilesDir(null),"log.txt");
			out=new FileOutputStream(file);
		}
		catch(IOException ex){
			out=null;
		}
		if(out!=null){
			startLogging(mApplet,out);
		}
		
		super.onStart();
	}

	protected void onStop(){
		System.out.println("onStop");
		
		stopLogging(mApplet);
		
		super.onStop();
	}

	protected native Applet createApplet(AndroidApplication app);
	protected native void destroyApplet(Applet applet);
	
	protected native void startLogging(Applet applet,OutputStream out);
	protected native void stopLogging(Applet applet);
}
