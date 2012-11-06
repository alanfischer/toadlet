package us.toadlet.randisle;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;

public class RandIsle extends Activity{
	static{
		System.loadLibrary("toadlet_egg");
		System.loadLibrary("toadlet_hop");
		System.loadLibrary("toadlet_flick");
		System.loadLibrary("toadlet_flick_jinputdevice");
		System.loadLibrary("toadlet_flick_androidsensordevice");
		System.loadLibrary("toadlet_peeper");
		System.loadLibrary("toadlet_peeper_gles1renderdevice");
		System.loadLibrary("toadlet_peeper_gles2renderdevice");
		System.loadLibrary("toadlet_ribbit");
		System.loadLibrary("toadlet_ribbit_jaudiodevice");
		System.loadLibrary("toadlet_tadpole");
		System.loadLibrary("toadlet_pad");
		System.loadLibrary("randisle");
	}

	protected void onCreate(Bundle savedInstanceState) {       
		super.onCreate(savedInstanceState);

		Intent intent = new Intent(RandIsle.this, android.app.NativeActivity.class);
		RandIsle.this.startActivity(intent);
	}
}
