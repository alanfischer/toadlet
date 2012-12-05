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

		super.onCreate(icicle);
    }

    protected void onDestroy(){
		System.out.println("onDestroy");

        super.onDestroy();
    }
	
	protected native Applet createApplet(AndroidApplication app);
}
