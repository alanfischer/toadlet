package us.toadlet.logo;

import us.toadlet.pad.*;
import android.os.*;

public class Logo extends AndroidApplication{
	static{
		System.loadLibrary("toadlet_egg");
		System.loadLibrary("toadlet_hop");
		System.loadLibrary("toadlet_flick");
		System.loadLibrary("toadlet_flick_jinputdevice");
		System.loadLibrary("toadlet_knot");
		System.loadLibrary("toadlet_peeper");
		System.loadLibrary("toadlet_peeper_gles1renderdevice");
		System.loadLibrary("toadlet_peeper_gles2renderdevice");
		System.loadLibrary("toadlet_ribbit");
		System.loadLibrary("toadlet_ribbit_jaudiodevice");
		System.loadLibrary("toadlet_tadpole");
		System.loadLibrary("toadlet_tadpole_hop");
		System.loadLibrary("toadlet_pad");
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
	
	protected void onStart(){
		System.out.println("onStart");
		
		super.onStart();
	}

	protected void onStop(){
		System.out.println("onStop");
		
		super.onStop();
	}
	
	protected native Applet createApplet(AndroidApplication app);
	protected native void destroyApplet(Applet applet);
}
