package us.toadlet.particles;

import us.toadlet.pad.*;
import android.os.Bundle;

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
	
	protected native Applet createApplet(AndroidApplication app);
	protected native void destroyApplet(Applet applet);
}
