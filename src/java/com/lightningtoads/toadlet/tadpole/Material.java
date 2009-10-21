/********** Copyright header - do not remove **********
 *
 * The Toadlet Engine
 *
 * Copyright 2009, Lightning Toads Productions, LLC
 *
 * Author(s): Alan Fischer, Andrew Fischer
 *
 * This file is part of The Toadlet Engine.
 *
 * The Toadlet Engine is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * The Toadlet Engine is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.

 * You should have received a copy of the GNU Lesser General Public License
 * along with The Toadlet Engine.  If not, see <http://www.gnu.org/licenses/>.
 *
 ********** Copyright header - do not remove **********/

package com.lightningtoads.toadlet.tadpole;

#include <com/lightningtoads/toadlet/tadpole/Types.h>

import com.lightningtoads.toadlet.egg.Resource;
import com.lightningtoads.toadlet.peeper.*;
import java.util.Vector;

public class Material implements Resource{
	// Min/Max Layers
	public final static int MIN_LAYER=-63;
	public final static int MAX_LAYER=63;

	public Material(){}

	public Material clone(){
		Material material=new Material();

		material.mLightEffect.set(mLightEffect);
		material.mLighting=mLighting;
		material.mFaceCulling=mFaceCulling;
		material.mAlphaTest=mAlphaTest;
		material.mAlphaTestCutoff=mAlphaTestCutoff;
		material.mBlend.set(mBlend);
		material.mDepthWrite=mDepthWrite;
		material.mDepthTest=mDepthTest;
		material.mDepthWrite=mDepthWrite;

		int i;
		for(i=0;i<mNumTextureStages;++i){
			material.setTextureStageName(i,mTextureStages[i],mTextureNames[i]);
		}

		return material;
	}

	public void setLightEffect(LightEffect lightEffect){mLightEffect.set(lightEffect);}
	public LightEffect getLightEffect(){return mLightEffect;}

	public void setLighting(boolean lighting){mLighting=lighting;}
	public boolean getLighting(){return mLighting;}

	public void setFaceCulling(Renderer.FaceCulling faceCulling){mFaceCulling=faceCulling;}
	public Renderer.FaceCulling getFaceCulling(){return mFaceCulling;}

	public void setAlphaTest(Renderer.AlphaTest alphaTest,scalar cutoff){mAlphaTest=alphaTest;mAlphaTestCutoff=cutoff;}
	public Renderer.AlphaTest getAlphaTest(){return mAlphaTest;}
	public scalar getAlphaTestCutoff(){return mAlphaTestCutoff;}

	public void setBlend(Blend blend){mBlend=blend;}
	public Blend getBlend(){return mBlend;}

	public void setDepthWrite(boolean depthWrite){mDepthWrite=depthWrite;}
	public boolean getDepthWrite(){return mDepthWrite;}

	public void setDepthTest(Renderer.DepthTest depthTest){mDepthTest=depthTest;}
	public Renderer.DepthTest getDepthTest(){return mDepthTest;}

	public void setFill(Renderer.Fill fill){mFill=fill;}
	public Renderer.Fill getFill(){return mFill;}

	public void setLayer(int layer){mLayer=layer;}
	public int getLayer(){return mLayer;}

	public boolean setTextureStage(int stage,TextureStage textureStage){
		if(stage+1>mNumTextureStages){
			mNumTextureStages=stage+1;
		}
		if(mTextureStages.length<mNumTextureStages){
			TextureStage[] textureStages=new TextureStage[mNumTextureStages];
			System.arraycopy(mTextureStages,0,textureStages,0,mTextureStages.length);
			mTextureStages=textureStages;
		}
		if(mTextureNames.length<mNumTextureStages){
			String[] textureNames=new String[mNumTextureStages];
			System.arraycopy(mTextureNames,0,textureNames,0,mTextureNames.length);
			mTextureNames=textureNames;
		}

		mTextureStages[stage]=textureStage;

		return true;
	}

	public TextureStage getTextureStage(int stage){return mTextureStages[stage];}

	public boolean setTextureName(int stage,String textureName){
		if(stage>mNumTextureStages){ 
			mNumTextureStages=stage;
		}
		if(mTextureStages.length<mNumTextureStages+1){
			TextureStage[] textureStages=new TextureStage[mNumTextureStages+1];
			System.arraycopy(mTextureStages,0,textureStages,0,mTextureStages.length);
			mTextureStages=textureStages;
		}
		if(mTextureNames.length<mNumTextureStages+1){
			String[] textureNames=new String[mNumTextureStages+1];
			System.arraycopy(mTextureNames,0,textureNames,0,mTextureNames.length);
			mTextureNames=textureNames;
		}

		mTextureNames[stage]=textureName;

		return true;
	}

	public String getTextureName(int stage){return mTextureNames[stage];}

	public boolean setTextureStageName(int stage,TextureStage textureStage,String textureName){
		boolean result=true;

		result=result && setTextureStage(stage,textureStage);
		result=result && setTextureName(stage,textureName);

		return true;
	}

	public int getNumTextureStages(){return mNumTextureStages;}

	public void setName(String name){mName=name;}
	public String getName(){return mName;}

	public void setupRenderer(Renderer renderer){setupRenderer(renderer,null);}
	public void setupRenderer(Renderer renderer,Material previousMaterial){
		if(previousMaterial==null){
			renderer.setFaceCulling(mFaceCulling);

			renderer.setLighting(mLighting);

			if(mLighting){
				renderer.setLightEffect(mLightEffect);
			}

			renderer.setAlphaTest(mAlphaTest,mAlphaTestCutoff);

			renderer.setBlend(mBlend);

			renderer.setDepthWrite(mDepthWrite);

			renderer.setDepthTest(mDepthTest);

			if(mFill!=null){
				renderer.setFill(mFill);
			}

			int i;
			for(i=0;i<mNumTextureStages;++i){
				renderer.setTextureStage(i,mTextureStages[i]);
			}
			/// Since we dont know the state of the texture stages if there is no previous material, we just make sure
			///  to set all the stages.
			/// @todo Since we are not setting all render states above, should this instead be done by a setDefaultStates
			///  in the Scene before we start rendering?
			for(;i<renderer.getCapabilitySet().maxTextureStages;++i){
				renderer.setTextureStage(i,null);
			}
		}
		else{
			if(previousMaterial.mFaceCulling!=mFaceCulling){
				renderer.setFaceCulling(mFaceCulling);
			}

			if(previousMaterial.mLighting!=mLighting){
				renderer.setLighting(mLighting);
			}

			if(mLighting){
				renderer.setLightEffect(mLightEffect);
			}

			if(previousMaterial.mAlphaTest!=mAlphaTest || previousMaterial.mAlphaTestCutoff!=mAlphaTestCutoff){
				renderer.setAlphaTest(mAlphaTest,mAlphaTestCutoff);
			}

			if(previousMaterial.mBlend.equals(mBlend)==false){
				renderer.setBlend(mBlend);
			}

			if(previousMaterial.mDepthWrite!=mDepthWrite){
				renderer.setDepthWrite(mDepthWrite);
			}

			if(previousMaterial.mDepthTest!=mDepthTest){
				renderer.setDepthTest(mDepthTest);
			}

			if(previousMaterial.mFill!=mFill){
				renderer.setFill(mFill);
			}

			int numTextureStages=mNumTextureStages;
			int numPreviousTextureStages=previousMaterial.mNumTextureStages;
			int i;
			for(i=0;i<numTextureStages;++i){
				if(i>=numPreviousTextureStages || previousMaterial.mTextureStages[i]!=mTextureStages[i]){
					renderer.setTextureStage(i,mTextureStages[i]);
				}
			}
			for(;i<numPreviousTextureStages;++i){
				renderer.setTextureStage(i,null);
			}
		}
	}

	protected LightEffect mLightEffect=new LightEffect();
	boolean mLighting=false;
	Renderer.FaceCulling mFaceCulling=Renderer.FaceCulling.BACK;
	Renderer.AlphaTest mAlphaTest=Renderer.AlphaTest.NONE;
	scalar mAlphaTestCutoff=0;
	Blend mBlend=new Blend();
	boolean mDepthWrite=true;
	Renderer.DepthTest mDepthTest=Renderer.DepthTest.LEQUAL;
	Renderer.Fill mFill=null; // Disable this initially, since its not supported on all platforms
	int mLayer=0;
	TextureStage[] mTextureStages=new TextureStage[1];
	String[] mTextureNames=new String[1];
	int mNumTextureStages=0;

	String mName;
}
