package us.toadlet.myplanet;

import us.toadlet.pad.*;
import android.os.Bundle;

public class MyPlanet extends AndroidApplication{
	static{
		System.loadLibrary("myplanet");
	}

	public MyPlanet(){
		super();
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
