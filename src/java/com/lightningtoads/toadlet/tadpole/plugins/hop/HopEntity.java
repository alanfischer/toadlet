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

package com.lightningtoads.toadlet.tadpole.plugins.hop;

#include <com/lightningtoads/toadlet/tadpole/Types.h>

import com.lightningtoads.toadlet.egg.*;
import com.lightningtoads.toadlet.egg.Error;
import com.lightningtoads.toadlet.hop.*;
import com.lightningtoads.toadlet.tadpole.*;
import com.lightningtoads.toadlet.tadpole.entity.*;

public class HopEntity extends ParentEntity implements CollisionListener{
	// TODO: Rename this to fit with new enum standards when reworking client/server
	public static final int ENTITY_BITS_NONE=0;
	public static final int ENTITY_BITS_ALL=0xFFFFFFFF;

	public static final int ENTITY_BIT_TYPE=1<<0;
	public static final int ENTITY_BIT_SHAPE=1<<1;
	public static final int ENTITY_BIT_MASS=1<<2;
	public static final int ENTITY_BIT_POSITION=1<<3;
	public static final int ENTITY_BIT_VELOCITY=1<<4;
	public static final int ENTITY_BIT_FORCE=1<<5;
	public static final int ENTITY_BIT_GRAVITY=1<<6;
	public static final int ENTITY_BIT_CO_RESTITUTION=1<<7;
	public static final int ENTITY_BIT_CO_RESTITUTIONOVERRIDE=1<<8;
	public static final int ENTITY_BIT_CO_STATICFRICTION=1<<9;
	public static final int ENTITY_BIT_CO_DYNAMICFRICTION=1<<10;
	public static final int ENTITY_BIT_CO_EFFECTIVEDRAG=1<<11;

	public static final int ENTITY_BITS_PHYSICS_UPDATE=ENTITY_BIT_POSITION | ENTITY_BIT_VELOCITY;

	public static final int NETWORKID_NOT_NETWORKED=-1;

	public HopEntity(){super();}

	public Entity create(Engine engine){return create(engine,true);}
	public Entity create(Engine engine,boolean networked){
		super.create(engine);

		mNextThink=0;
		mSolid.reset();
		mSolid.setUserData(this);
		mScene=null;
		mListener=null;
		mHopCollision.reset();
		mLastPosition.reset();
		mActivePrevious=true;

		mNetworkID=0;
		mModifiedFields=0;

		mShadow=false;
		mShadowMatrix.reset();
		mShadowStrength=0;

		mIdentityTransform=false;
		
		if(networked==false){
			mNetworkID=NETWORKID_NOT_NETWORKED;
		}

		mScene=((HopScene)(mEngine.getScene()));
		if(mScene==null){
			Error.unknown(Categories.TOADLET_TADPOLE,
				"Invalid scene");
			return this;
		}

		mScene.registerHopEntity(this);

		return this;
	}
	
	public void destroy(){
		if(mScene!=null){
			mScene.unregisterHopEntity(this);
		}

		// We handle this here, because destroy() must be last, but we would normally need mScene when parentChanged is called
		if(mScene!=null && mScene.getSimulator()!=null){
			mScene.getSimulator().removeSolid(getSolid());
			mScene=null;
		}

		super.destroy();
	}

	public boolean isCustom(){return false;}
	
	public void setCollisionBits(int bits){mSolid.setCollisionBits(bits);}
	public int getCollisionBits(){return mSolid.getCollisionBits();}

	public void setCollideWithBits(int bits){mSolid.setCollideWithBits(bits);}
	public int getCollideWithBits(){return mSolid.getCollideWithBits();}

	public void setMass(scalar mass){
		mSolid.setMass(mass);

		mModifiedFields|=ENTITY_BIT_MASS;
	}

	public scalar getMass(){return mSolid.getMass();}

	public void setInfiniteMass(){
		mSolid.setInfiniteMass();

		mModifiedFields|=ENTITY_BIT_MASS;
	}
	
	public boolean hasInfiniteMass(){return mSolid.hasInfiniteMass();}

	public void setTranslate(Vector3 translate){
		mTranslate.set(translate);

		mSolid.setPosition(translate);

		mLastPosition.set(translate);

		mModifiedFields|=ENTITY_BIT_POSITION;

		setVisualTransformTranslate(mTranslate);
	}
	
	public void setTranslate(scalar x,scalar y,scalar z){
		mTranslate.set(x,y,z);

		setTranslate(mTranslate);
	}
	
	public void setPosition(Vector3 position){setTranslate(position);}
	public Vector3 getPosition(){return getTranslate();}

	public void setVelocity(Vector3 velocity){
		mSolid.setVelocity(velocity);

		mModifiedFields|=ENTITY_BIT_VELOCITY;
	}
	
	public void setVelocity(scalar x,scalar y,scalar z){
		Vector3 velocity=cache_setVelocity_velocity.set(x,y,z);
		setVelocity(velocity);
	}
	
	public Vector3 getVelocity(){return mSolid.getVelocity();}

	public void addForce(Vector3 force){
		mSolid.addForce(force);

		mModifiedFields|=ENTITY_BIT_FORCE;
	}

	public Vector3 getForce(){return mSolid.getForce();}

	public void clearForce(){
		mSolid.clearForce();

		mModifiedFields|=ENTITY_BIT_VELOCITY;
	}

	public void setLocalGravity(Vector3 gravity){
		mSolid.setLocalGravity(gravity);

		mModifiedFields|=ENTITY_BIT_GRAVITY;
	}

	public Vector3 getLocalGravity(){return mSolid.getLocalGravity();}

	public void setWorldGravity(){
		mSolid.setWorldGravity();

		mModifiedFields|=ENTITY_BIT_GRAVITY;
	}

	public boolean hasLocalGravity(){return mSolid.hasLocalGravity();}

	public void setCoefficientOfRestitution(scalar coeff){
		mSolid.setCoefficientOfRestitution(coeff);

		mModifiedFields|=ENTITY_BIT_CO_RESTITUTION;
	}

	public scalar getCoefficientOfRestitution(){return mSolid.getCoefficientOfRestitution();}

	public void setCoefficientOfRestitutionOverride(boolean over){
		mSolid.setCoefficientOfRestitutionOverride(over);

		mModifiedFields|=ENTITY_BIT_CO_RESTITUTIONOVERRIDE;
	}
	
	public boolean getCoefficientOfRestitutionOverride(){return mSolid.getCoefficientOfRestitutionOverride();}

	public void setCoefficientOfStaticFriction(scalar coeff){
		mSolid.setCoefficientOfStaticFriction(coeff);

		mModifiedFields|=ENTITY_BIT_CO_STATICFRICTION;
	}
	
	public scalar getCoefficientOfStaticFriction(){return mSolid.getCoefficientOfStaticFriction();}

	public void setCoefficientOfDynamicFriction(scalar coeff){
		mSolid.setCoefficientOfDynamicFriction(coeff);

		mModifiedFields|=ENTITY_BIT_CO_DYNAMICFRICTION;
	}
	
	public scalar getCoefficientOfDynamicFriction(){return mSolid.getCoefficientOfDynamicFriction();}

	public void setCoefficientOfEffectiveDrag(scalar coeff){
		mSolid.setCoefficientOfEffectiveDrag(coeff);

		mModifiedFields|=ENTITY_BIT_CO_EFFECTIVEDRAG;
	}

	public scalar getCoefficientOfEffectiveDrag(){return mSolid.getCoefficientOfEffectiveDrag();}

	public HopScene getScene(){return mScene;}
	public Solid getSolid(){return mSolid;}
	public HopEntity getTouching(){
		HopEntity ent=null;
		Solid solid=mSolid.getTouching();
		if(solid!=null){
			ent=((HopEntity)(solid.getUserData()));
		}
		return ent;
	}

	public void setCollisionListener(HopCollisionListener listener){
		if(listener!=null){
			mSolid.setCollisionListener(this);
		}
		else{
			mSolid.setCollisionListener(null);
		}

		mListener=listener;
	}

	public void think(){}

	public void setNextThink(int nextThink){mNextThink=nextThink;}
	public int getNextThink(){return mNextThink;}

	public int getNetworkID(){return mNetworkID;}
	public boolean isNetworked(){return mNetworkID!=NETWORKID_NOT_NETWORKED;}

	public void resetModifiedFields(){mModifiedFields=0;}
	public void addModifiedFields(int fields){mModifiedFields|=fields;}
	public int getModifiedFields(){return mModifiedFields;}

	public void setCastsShadow(boolean shadow){mShadow=shadow;}
	public boolean getCastsShadow(){return mShadow;}

	public void parentChanged(ParentEntity newParent){
		if(mScene!=null){
			if(newParent==mScene){
				mScene.getSimulator().addSolid(getSolid());
			}
			else{
				mScene.getSimulator().removeSolid(getSolid());
			}
		}

		super.parentChanged(newParent);
	}

	public void collision(Collision c){
		if(mListener!=null){
			mHopCollision.time=c.time;
			mHopCollision.point.set(c.point);
			mHopCollision.normal.set(c.normal);
			mHopCollision.velocity.set(c.velocity);
			if(c.collider!=null){
				mHopCollision.collider=((HopEntity)(c.collider.getUserData()));
			}
			else{
				mHopCollision.collider=null;
			}
			if(c.collidee!=null){
				mHopCollision.collidee=((HopEntity)(c.collidee.getUserData()));
			}
			else{
				mHopCollision.collidee=null;
			}
			mListener.collision(mHopCollision);
		}
	}

	protected void preLogicUpdateLoop(int dt){
		mLastPosition.set(mSolid.getPosition());
		mActivePrevious=mSolid.getActive();
	}

	protected void postLogicUpdate(int dt){
		if(mSolid.getActive()){
			mTranslate.set(mSolid.getPosition());
		}

		if(mNextThink>0){
			mNextThink-=dt;
			if(mNextThink<=0){
				mNextThink=0;
				think();
			}
		}
	}
	
	protected void interpolatePhysicalParameters(scalar f){
		Vector3 interpolate=cache_interpolatePhysicalParameters_interpolate;
		Math.lerp(interpolate,mLastPosition,mSolid.getPosition(),f);
		setVisualTransformTranslate(interpolate);
	}
	
	protected int mNextThink=0;
	protected Solid mSolid=new Solid();
	protected HopScene mScene=null;
	protected HopCollisionListener mListener=null;
	protected HopCollision mHopCollision=new HopCollision();
	protected Vector3 mLastPosition=new Vector3();
	protected boolean mActivePrevious=true;

	protected int mNetworkID=0;
	protected int mModifiedFields=0;

	protected boolean mShadow=false;
	protected Matrix4x4 mShadowMatrix=new Matrix4x4();
	protected scalar mShadowStrength=0;

	protected Vector3 cache_setVelocity_velocity=new Vector3();
	protected Vector3 cache_interpolatePhysicalParameters_interpolate=new Vector3();
}
