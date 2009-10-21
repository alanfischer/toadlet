/********** Copyright header - do not remove **********
 *
 * The Toadlet Engine
 *
 * Copyright (C) 2006, Lightning Toads Productions, LLC
 *
 * Author(s): Alan Fischer
 *
 * All source code for the Toadlet Engine, including
 * this file, is the sole property of Lightning Toads
 * Productions, LLC. It has been developed on our own
 * time, with our own tools, on our own hardware. None
 * of this code may be observed, used or changed
 * without our explicit permission.
 *
 *  - Lightning Toads Productions, LLC
 *
 ********** Copyright header - do not remove **********/

package com.lightningtoads.toadlet.tadpole;

import com.lightningtoads.toadlet.egg.mathfixed.Math;
import com.lightningtoads.toadlet.tadpole.sg.*;

public final class LightEffect{
	public LightEffect(){
		Entity.uint32ColorToFixedColor(0xFFFFFFFF,ambient);
		Entity.uint32ColorToFixedColor(0xFFFFFFFF,diffuse);
		Entity.uint32ColorToFixedColor(0xFFFFFFFF,specular);
		shininess=Math.ONE;
		Entity.uint32ColorToFixedColor(0xFF000000,emissive);
		trackColor=false;
	}

	public LightEffect(LightEffect lightEffect){
		set(lightEffect);
	}

	public void set(LightEffect lightEffect){
		ambient[0]=lightEffect.ambient[0];ambient[1]=lightEffect.ambient[1];ambient[2]=lightEffect.ambient[2];ambient[3]=lightEffect.ambient[3];
		diffuse[0]=lightEffect.diffuse[0];diffuse[1]=lightEffect.diffuse[1];diffuse[2]=lightEffect.diffuse[2];diffuse[3]=lightEffect.diffuse[3];
		specular[0]=lightEffect.specular[0];specular[1]=lightEffect.specular[1];specular[2]=lightEffect.specular[2];specular[3]=lightEffect.specular[3];
		shininess=lightEffect.shininess;
		emissive[0]=lightEffect.emissive[0];emissive[1]=lightEffect.emissive[1];emissive[2]=lightEffect.emissive[2];emissive[3]=lightEffect.emissive[3];
		trackColor=lightEffect.trackColor;
	}
	
	public boolean equals(LightEffect lightEffect){
		return
			ambient[0]==lightEffect.ambient[0] && ambient[1]==lightEffect.ambient[1] && ambient[2]==lightEffect.ambient[2] && ambient[3]==lightEffect.ambient[3] &&
			diffuse[0]==lightEffect.diffuse[0] && diffuse[1]==lightEffect.diffuse[1] && diffuse[2]==lightEffect.diffuse[2] && diffuse[3]==lightEffect.diffuse[3] &&
			specular[0]==lightEffect.specular[0] && specular[1]==lightEffect.specular[1] && specular[2]==lightEffect.specular[2] && specular[3]==lightEffect.specular[3] &&
			shininess==lightEffect.shininess &&
			emissive[0]==lightEffect.emissive[0] && emissive[1]==lightEffect.emissive[1] && emissive[2]==lightEffect.emissive[2] && emissive[3]==lightEffect.emissive[3] &&
			trackColor==lightEffect.trackColor;
	}

	public int[] ambient=new int[4];
	public int[] diffuse=new int[4];
	public int[] specular=new int[4];
	public int shininess;
	public int[] emissive=new int[4];
	public boolean trackColor;
}
