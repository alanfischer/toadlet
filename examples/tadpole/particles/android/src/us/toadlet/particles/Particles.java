package us.toadlet.particles;

import us.toadlet.pad.*;

public class Particles extends AndroidApplication{
	static{
		System.loadLibrary("particles");
	}

	Particles(){
		super();
	}

    protected void onCreate(Bundle icicle){
        super.onCreate(icicle);
		
		setApplet(createApplet(this));
    }

    protected void onDestroy(){
        super.onDestroy();
		
		destroyApplet(getApplet());
    }
	
	protected static native Applet createApplet(Application app);
	protected static native void destroyApplet(Applet applet);
}
