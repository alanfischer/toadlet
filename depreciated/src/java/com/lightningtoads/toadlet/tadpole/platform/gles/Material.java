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

public final class Material{
	// FaceCullings
	public static final byte FC_NONE=0;
	public static final byte FC_CCW=1;
	public static final byte FC_CW=2;
	public static final byte FC_FRONT=1;
	public static final byte FC_BACK=2;

	// AlphaTests
	public static final byte AT_NONE=0;
	public static final byte AT_LESS=1;
	public static final byte AT_EQUAL=2;
	public static final byte AT_LEQUAL=3;
	public static final byte AT_GREATER=4;
	public static final byte AT_NOTEQUAL=5;
	public static final byte AT_GEQUAL=6;
	public static final byte AT_ALWAYS=7;

	// Blends
	public static final byte DISABLED_BLEND=0;
	public static final byte COMBINATION_COLOR=1;
	public static final byte COMBINATION_COLOR_ADDITIVE=2;
	public static final byte COMBINATION_ALPHA=3;
	public static final byte COMBINATION_ALPHA_ADDITIVE=4;

	public Material(){}

	public Material clone(){
		Material material=new Material();

		material.mLightEffect.set(mLightEffect);
		material.mLighting=mLighting;
		material.mFaceCulling=mFaceCulling;
		material.mAlphaTest=mAlphaTest;
		material.mAlphaTestCutoff=mAlphaTestCutoff;;
		material.mBlend=mBlend;
		material.mDepthWrite=mDepthWrite;
		material.mTexture=mTexture;
		material.mTextureName=mTextureName;

		return material;
	}
	
	public void setLightEffect(LightEffect lightEffect){mLightEffect.set(lightEffect);}
	public LightEffect getLightEffect(){return mLightEffect;}

	public void setLighting(boolean lighting){mLighting=lighting;}
	public boolean getLighting(){return mLighting;}

	public void setFaceCulling(byte faceCulling){mFaceCulling=faceCulling;}
	public byte getFaceCulling(){return mFaceCulling;}

	public void setAlphaTest(byte alphaTest,int cutoff){
		mAlphaTest=alphaTest;
		mAlphaTestCutoff=cutoff;
	}

	public byte getAlphaTest(){return mAlphaTest;}
	public int getAlphaTestCutoff(){return mAlphaTestCutoff;}

	public void setBlend(byte blend){mBlend=blend;}
	public byte getBlend(){return mBlend;}

	public void setDepthWrite(boolean depthWrite){mDepthWrite=depthWrite;}
	public boolean getDepthWrite(){return mDepthWrite;}

	public void setTexture(Texture texture){setTexture(texture,null);}
	public void setTexture(Texture texture,String name){
		mTexture=texture;
		mTextureName=name;
	}

	public Texture getTexture(){return mTexture;}
	public String getTextureName(){return mTextureName;}

	public void setName(String name){mName=name;}
	public String getName(){return mName;}

	public LightEffect mLightEffect=new LightEffect();
	public boolean mLighting=false;
	public byte mFaceCulling=FC_BACK;
	public byte mAlphaTest=AT_NONE;
	public int mAlphaTestCutoff=0;
	public byte mBlend=DISABLED_BLEND;
	public boolean mDepthWrite=true;
	public Texture mTexture;
	public String mTextureName;

	String mName;
}
