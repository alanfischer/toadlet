package us.toadlet.logo;

import us.toadlet.pad.*;
import android.os.*;

public class Logo extends AndroidApplication{
	static{
		System.loadLibrary("logo");
	}

	public Logo(){
		super();

		System.out.println("Logo");
	}

    protected void onCreate(Bundle icicle){
		System.out.println("onCreate");

		super.onCreate(icicle);
    }

    protected void onDestroy(){
		System.out.println("onDestroy");

        super.onDestroy();
    }
	
	protected native Applet createApplet(AndroidApplication app);
	protected native void destroyApplet(Applet applet);
}
