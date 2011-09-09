package us.toadlet.randisle;

import us.toadlet.pad.*;
import android.os.Bundle;

public class RandIsle extends AndroidApplication{
	static{
		System.loadLibrary("randisle");
	}

	public RandIsle(){
		super();

		System.out.println("RandIsle");
	}

    protected void onCreate(Bundle icicle){
		System.out.println("onCreate");

//		setApplet(createApplet(this));

		super.onCreate(icicle);
    }

    protected void onDestroy(){
		System.out.println("onDestroy");

        super.onDestroy();
		
//		destroyApplet(getApplet());
    }
	
	protected native Applet createApplet(AndroidApplication app);
	protected native void destroyApplet(Applet applet);
}
