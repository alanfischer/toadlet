package us.toadlet.particles;

import us.toadlet.pad.*;
import android.os.*;

public class Particles extends AndroidApplication{
	static{
		System.loadLibrary("particles");
	}

	public Particles(){
		super();

		System.out.println("Particles");
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
