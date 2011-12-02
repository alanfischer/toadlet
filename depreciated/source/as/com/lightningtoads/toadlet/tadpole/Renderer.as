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

package com.lightningtoads.toadlet.tadpole{

#if defined(TOADLET_DEBUG)
	#define TOADLET_USE_STATISTICS
#endif

import flash.display.Graphics;
import com.lightningtoads.toadlet.egg.mathfixed.*;
import com.lightningtoads.toadlet.egg.mathfixed.tMath;
import com.lightningtoads.toadlet.tadpole.sg.*;
#ifdef TOADLET_USE_STATISTICS
	import com.lightningtoads.toadlet.egg.*;
#endif

public class Renderer{
	public function Renderer(screenWidth:int,screenHeight:int):void{
		setViewportSize(screenWidth,screenHeight);
		resizeVertexCache(1024);
		resizeTriangleCache(2048);
		mOrderingTable=new Array(1024);
		mFaceCullEpsilon=1<<12;
	}

	public function setViewportSize(width:int,height:int):void{
		mViewportWidth=width;
		mViewportHeight=height;
		mFixedViewportWidth=tMath.intToFixed(mViewportWidth);
		mHalfFixedViewportWidth=mFixedViewportWidth>>1;
		mFixedViewportHeight=tMath.intToFixed(mViewportHeight);
		mHalfFixedViewportHeight=mFixedViewportHeight>>1;
	}

	public function setProjection(invHX:int,invHY:int,near:int,far:int):void{
		mInvHX=invHX;
		mInvHY=invHY;
		mNear=near;
		mFar=far;
	}

	public function setLighting(enabled:Boolean,direction:Vector3,ambient:int):void{
		mLighting=enabled;
		if(enabled){
			mLightDirection.setV(direction);
		}
		mAmbient=ambient;
	}

	public function setOrderingTableSize(size:int):void{
		mOrderingTable=new Array(size);
	}
	
	public function setFaceCullEpsilon(epsilon:int):void{
		mFaceCullEpsilon=epsilon;
	}
	
	public function beginScene(g:Graphics):void{
		#ifdef TOADLET_USE_STATISTICS
			vertexViewTransformed=0;
			vertexViewTransformCached=0;
			vertexScreenTransformed=0;
			vertexScreenTransformCached=0;
			triangleIncoming=0;
			triangleFaceCulled=0;
			triangleNearCulled=0;
			triangleFarCulled=0;
			triangleTrimmed=0;
			triangleSplit=0;
			triangleScreenCulled=0;
			triangleTextureRendered=0;
			triangleColorRendered=0;
		#endif

		mDX=
			// mInvHX*mHalfFixedViewportWidth
			((mInvHX>>6)*(mHalfFixedViewportWidth>>6))>>4
		;
		mDY=-
			// mInvHY*mHalfFixedViewportHeight
			((mInvHY>>6)*(mHalfFixedViewportHeight>>6))>>4
		;

		mGraphics=g;

		mGraphics.clear();
		mGraphics.lineStyle(undefined);
	}

	public function flush():void{
		var tri:Triangle;
		var j:int;

		for(j=mOrderingTable.length-1;j>=0;--j){
			tri=mOrderingTable[j];
			mOrderingTable[j]=null;
			while(tri!=null){
				if(tri.texture!=null){
					#ifdef TOADLET_USE_STATISTICS
						triangleTextureRendered++;
					#endif

					mGraphics.beginFill(0x00FF0000);
					mGraphics.moveTo(tri.sx1,tri.sy1);
					mGraphics.lineTo(tri.sx2,tri.sy2);
					mGraphics.lineTo(tri.sx3,tri.sy3);
					mGraphics.lineTo(tri.sx1,tri.sy1);
					mGraphics.endFill();
				}
				else{
					#ifdef TOADLET_USE_STATISTICS
						triangleColorRendered++;
					#endif

					mGraphics.beginFill(tri.color1);
					mGraphics.moveTo(tri.sx1,tri.sy1);
					mGraphics.lineTo(tri.sx2,tri.sy2);
					mGraphics.lineTo(tri.sx3,tri.sy3);
					mGraphics.lineTo(tri.sx1,tri.sy1);
					mGraphics.endFill();
				}
				tri=tri.next;
			}
		}

		mTriangleCacheCount=0;
	}
	
	public function endScene():void{
		#ifdef TOADLET_USE_STATISTICS
			// The extra backslash here is because gpp strips out one
			Logger.getInstance().addLogString(Logger.LEVEL_ALERT,
				"Renderer data:"+"\\n"+
				"triangleIncoming:"+triangleIncoming+"\\n"+
				"vertexViewTransformed:"+vertexViewTransformed+"\\n"+
				"vertexViewTransformCached:"+vertexViewTransformCached+"\\n"+
				"vertexScreenTransformed:"+vertexScreenTransformed+"\\n"+
				"vertexScreenTransformCached:"+vertexScreenTransformCached+"\\n"+
				"triangleFaceCulled:"+triangleFaceCulled+"\\n"+
				"triangleNearCulled:"+triangleNearCulled+"\\n"+
				"triangleFarCulled:"+triangleFarCulled+"\\n"+
				"triangleTrimmed:"+triangleTrimmed+"\\n"+
				"triangleSplit:"+triangleSplit+"\\n"+
				"triangleScreenCulled:"+triangleScreenCulled+"\\n"+
				"triangleTextureRendered:"+triangleTextureRendered+"\\n"+
				"triangleColorRendered:"+triangleColorRendered+"\\n"
			);
		#endif

		mGraphics=null;
	}

	public function renderSpriteEntity(transform:Matrix4x3,spriteEntity:SpriteEntity):void{
		if(spriteEntity.mSprite==null){
			return;
		}

		var vx1:int,vy1:int,vz1:int;
		var sx1:int,sy1:int;
		var vx2:int,vy2:int,vz2:int;
		var sx2:int,sy2:int;

		var bits:int=spriteEntity.getEngine().mRenderShiftBits;

		vx1=(-spriteEntity.mWidth>>bits) + transform.a03;
		vy1=(-spriteEntity.mHeight>>bits) + transform.a13;
		vz1=-transform.a23;
		if(spriteEntity.mScaled){
			sx1=(((((vx1>>6)*(mDX>>6)<<2)/(vz1>>6))<<4)+mHalfFixedViewportWidth)
				>>tMath.BITS
			;
			sy1=(((((vy1>>6)*(mDY>>6)<<2)/(vz1>>6))<<4)+mHalfFixedViewportHeight)
				>>tMath.BITS
			;
		}
		else{
			sx1=(((vx1>>6)*(mDX>>6)>>4)+mHalfFixedViewportWidth)
				>>tMath.BITS
			;
			sy1=(((vy1>>6)*(mDY>>6)>>4)+mHalfFixedViewportHeight)
				>>tMath.BITS
			;
		}

		if((vz1<=mNear) || (vz1>=mFar)){
			return;
		}

		vx2=(spriteEntity.mWidth>>bits) + transform.a03;
		vy2=(spriteEntity.mHeight>>bits) + transform.a13;
		vz2=-transform.a23;
		if(spriteEntity.mScaled){
			sx2=(((((vx2>>6)*(mDX>>6)<<2)/(vz2>>6))<<4)+mHalfFixedViewportWidth)
				>>tMath.BITS
			;
			sy2=(((((vy2>>6)*(mDY>>6)<<2)/(vz2>>6))<<4)+mHalfFixedViewportHeight)
				>>tMath.BITS
			;
		}
		else{
			sx2=(((vx2>>6)*(mDX>>6)>>4)+mHalfFixedViewportWidth)
				>>tMath.BITS
			;
			sy2=(((vy2>>6)*(mDY>>6)>>4)+mHalfFixedViewportHeight)
				>>tMath.BITS
			;
		}

		mGraphics.beginFill(0x00FF0000);
		mGraphics.drawRect(sx1,sy1,sx2-sx1,sy2-sy1);
		mGraphics.endFill();
	}

	public function renderBeamEntity(transform:Matrix4x3,beamEntity:BeamEntity):void{
		var wx1:int,wy1:int,wz1:int;
		var vx1:int,vy1:int,vz1:int;
		var sx1:int,sy1:int;
		var wx2:int,wy2:int,wz2:int;
		var vx2:int,vy2:int,vz2:int;
		var sx2:int,sy2:int;

		var bits:int=beamEntity.getEngine().mRenderShiftBits;

		wx1=(beamEntity.mStart.x>>bits)>>6;wy1=(beamEntity.mStart.y>>bits)>>6;wz1=(beamEntity.mStart.z>>bits)>>6;
		vx1=
			((((transform.a00>>2)*wx1)>>8) + (((transform.a01>>2)*wy1)>>8) + (((transform.a02>>2)*wz1)>>8) + transform.a03);
		vy1=
			((((transform.a10>>2)*wx1)>>8) + (((transform.a11>>2)*wy1)>>8) + (((transform.a12>>2)*wz1)>>8) + transform.a13);
		vz1=-
			((((transform.a20>>2)*wx1)>>8) + (((transform.a21>>2)*wy1)>>8) + (((transform.a22>>2)*wz1)>>8) + transform.a23);
		sx1=(((((vx1>>6)*(mDX>>6)<<2)/(vz1>>6))<<4)+mHalfFixedViewportWidth)
			>>tMath.BITS
		;
		sy1=(((((vy1>>6)*(mDY>>6)<<2)/(vz1>>6))<<4)+mHalfFixedViewportHeight)
			>>tMath.BITS
		;

		wx2=(beamEntity.mEnd.x>>bits)>>6;wy2=(beamEntity.mEnd.y>>bits)>>6;wz2=(beamEntity.mEnd.z>>bits)>>6;
		vx2=
			((((transform.a00>>2)*wx2)>>8) + (((transform.a01>>2)*wy2)>>8) + (((transform.a02>>2)*wz2)>>8) + transform.a03);
		vy2=
			((((transform.a10>>2)*wx2)>>8) + (((transform.a11>>2)*wy2)>>8) + (((transform.a12>>2)*wz2)>>8) + transform.a13);
		vz2=-
			((((transform.a20>>2)*wx2)>>8) + (((transform.a21>>2)*wy2)>>8) + (((transform.a22>>2)*wz2)>>8) + transform.a23);
		sx2=(((((vx2>>6)*(mDX>>6)<<2)/(vz2>>6))<<4)+mHalfFixedViewportWidth)
			>>tMath.BITS
		;
		sy2=(((((vy2>>6)*(mDY>>6)<<2)/(vz2>>6))<<4)+mHalfFixedViewportHeight)
			>>tMath.BITS
		;

		mGraphics.lineStyle(5,beamEntity.mColor);
		mGraphics.moveTo(sx1,sy1);
		mGraphics.lineTo(sx2,sy2);
		mGraphics.lineStyle(undefined);
	}

	// ClipFlags
	// The first 4 are in screenspace
	internal static var CF_LEFT_PLANE:int=	1<<0;
	internal static var CF_RIGHT_PLANE:int=	1<<1;
	internal static var CF_BOTTOM_PLANE:int=1<<2;
	internal static var CF_TOP_PLANE:int=	1<<3;

	public function renderMeshEntity(transform:Matrix4x3,meshEntity:MeshEntity):void{
		var m:Matrix4x3=mTransformCache;
		var j:int;
		// Shift the bits of the matrix that we multiply right
		m.a00=transform.a00>>2;
		m.a10=transform.a10>>2;
		m.a20=transform.a20>>2;
		m.a01=transform.a01>>2;
		m.a11=transform.a11>>2;
		m.a21=transform.a21>>2;
		m.a02=transform.a02>>2;
		m.a12=transform.a12>>2;
		m.a22=transform.a22>>2;
		m.a03=transform.a03;
		m.a13=transform.a13;
		m.a23=transform.a23;

		var pos:Array;
		var norm:Array;
		var tex:Array=meshEntity.mMesh.vertexData.texCoords;
		if(meshEntity.mDynamicVertexData!=null){
			pos=meshEntity.mDynamicVertexData.positions;
			norm=meshEntity.mDynamicVertexData.normals;
		}
		else{
			pos=meshEntity.mMesh.vertexData.positions;
			norm=meshEntity.mMesh.vertexData.normals;
		}

		if(pos==null){
			return;
		}

		// Size up vertex cache array
		if(mVertexViewCache.length<pos.length){
			resizeVertexCache(pos.length/3);
		}

		// Update our render count for correct caching
		mRenderCount++;
		if(mRenderCount<0){
			mRenderCount=1;
			for(j=0;j<mVertexViewFrames.length;++j){
				mVertexViewFrames[j]=0;
				mVertexScreenFrames[j]=0;
			}
		}

		var vi1:int,vi2:int,vi3:int,i3:int,j3:int;
		var wx1:int,wy1:int,wz1:int;
		var vx1:int,vy1:int,vz1:int;
		var sx1:int,sy1:int;
		var wx2:int,wy2:int,wz2:int;
		var vx2:int,vy2:int,vz2:int;
		var sx2:int,sy2:int;
		var wx3:int,wy3:int,wz3:int;
		var vx3:int,vy3:int,vz3:int;
		var sx3:int,sy3:int;
		var vx4:int=0,vy4:int=0,vz4:int=0;
		var sx4:int,sy4:int;
		var x21:int,x31:int,y21:int,y31:int,z21:int,z31:int;
		var tableIndex:int;
		var tri:Triangle,tri2:Triangle;
		var clipFlags1:int,clipFlags2:int,clipFlags3:int,clipFlags4:int;
		var nearClipFlag1:Boolean,nearClipFlag2:Boolean,nearClipFlag3:Boolean;
		var clipTime:int,crx1:int,cry1:int,crx2:int,cry2:int;
		var secondTriangle:Boolean=false;
		var viewDistance:int=mFar-mNear;
		var lx:int=0,ly:int=0,lz:int=0;

		var tid:Array=meshEntity.mMesh.triIndexData;
		var tsm:Array=meshEntity.mMesh.triSubMesh;
		var subs:Array=meshEntity.mMesh.subMeshes;

		var numTris:int=tid.length/3;

		// Size up triangle array for worst case (all triangles split)
		if(mTriangleCache.length<(mTriangleCacheCount+numTris)*2){
			resizeTriangleCache((mTriangleCacheCount+numTris)*2);
		}

		if(mLighting){
			// We should be able to use x,y,z >> 2, and with m.a00 >> 2, we would have >> 12 below.
			// However, since the m matrix is rotation & scale, we run out of precision.
			// Since our m matrix is already shifted >> 2, we just use x,y,z >> 6, and then >> 8 total

			// TODO: Not shift the m matrix >> 2, but instead >> 6, then we could use the correct mLightDirection shift

			var x:int=-mLightDirection.x>>6;
			var y:int=-mLightDirection.y>>6;
			var z:int=-mLightDirection.z>>6;

			// Our transform is the models space prefixed by camera space, and our light is in camera space so we transform our light by inverse(transform)
			// Then our light is in the models local space, so we do not have to transform any normals.
			// m & x,y,z are preshifted already, and the result is >> 8 total, so >> 8 & << 6 = >> 2
			lx=
				(((m.a00*x + m.a10*y + m.a20*z)>>2)/(meshEntity.mMesh.worldScale>>6))<<4
			;
			ly=
				(((m.a01*x + m.a11*y + m.a21*z)>>2)/(meshEntity.mMesh.worldScale>>6))<<4
			;
			lz=
				(((m.a02*x + m.a12*y + m.a22*z)>>2)/(meshEntity.mMesh.worldScale>>6))<<4
			;
		}

		#ifdef TOADLET_USE_STATISTICS
			triangleIncoming+=numTris;
		#endif
		for(j=0;j<numTris;++j){
			var subMesh:MeshSubMesh=subs[tsm[j]];

			j3=j*3; // 3 indices in each triangle

			vi1=tid[j3+0];
			vi2=tid[j3+1];
			vi3=tid[j3+2];

			// Vertex 1
			if(mRenderCount!=mVertexViewFrames[vi1]){
				#ifdef TOADLET_USE_STATISTICS
					vertexViewTransformed++;
				#endif

				mVertexViewFrames[vi1]=mRenderCount;
				i3=vi1*3;
				wx1=pos[i3+0]>>6;
				wy1=pos[i3+1]>>6;
				wz1=pos[i3+2]>>6;
				vz1=-
					(((m.a20*wx1)>>8)+((m.a21*wy1)>>8)+((m.a22*wz1)>>8)+m.a23)
				;
				vx1=
					(((m.a00*wx1)>>8)+((m.a01*wy1)>>8)+((m.a02*wz1)>>8)+m.a03)
				;
				vy1=
					(((m.a10*wx1)>>8)+((m.a11*wy1)>>8)+((m.a12*wz1)>>8)+m.a13)
				;

				mVertexViewCache[i3]=vx1;
				mVertexViewCache[i3+1]=vy1;
				mVertexViewCache[i3+2]=vz1;
			}
			else{
				#ifdef TOADLET_USE_STATISTICS
					vertexViewTransformCached++;
				#endif

				i3=vi1*3;
				vx1=mVertexViewCache[i3];
				vy1=mVertexViewCache[i3+1];
				vz1=mVertexViewCache[i3+2];
			}

			// Vertex 2
			if(mRenderCount!=mVertexViewFrames[vi2]){
				#ifdef TOADLET_USE_STATISTICS
					vertexViewTransformed++;
				#endif

				mVertexViewFrames[vi2]=mRenderCount;
				i3=vi2*3;
				wx2=pos[i3+0]>>6;
				wy2=pos[i3+1]>>6;
				wz2=pos[i3+2]>>6;
				vz2=-
					(((m.a20*wx2)>>8)+((m.a21*wy2)>>8)+((m.a22*wz2)>>8)+m.a23)
				;
				vx2=
					(((m.a00*wx2)>>8)+((m.a01*wy2)>>8)+((m.a02*wz2)>>8)+m.a03)
				;
				vy2=
					(((m.a10*wx2)>>8)+((m.a11*wy2)>>8)+((m.a12*wz2)>>8)+m.a13)
				;

				mVertexViewCache[i3]=vx2;
				mVertexViewCache[i3+1]=vy2;
				mVertexViewCache[i3+2]=vz2;
			}
			else{
				#ifdef TOADLET_USE_STATISTICS
					vertexViewTransformCached++;
				#endif

				i3=vi2*3;
				vx2=mVertexViewCache[i3];
				vy2=mVertexViewCache[i3+1];
				vz2=mVertexViewCache[i3+2];
			}

			// Vertex 3
			if(mRenderCount!=mVertexViewFrames[vi3]){
				#ifdef TOADLET_USE_STATISTICS
					vertexViewTransformed++;
				#endif

				mVertexViewFrames[vi3]=mRenderCount;
				i3=vi3*3;
				wx3=pos[i3+0]>>6;
				wy3=pos[i3+1]>>6;
				wz3=pos[i3+2]>>6;
				vz3=-
					(((m.a20*wx3)>>8)+((m.a21*wy3)>>8)+((m.a22*wz3)>>8)+m.a23)
				;
				vx3=
					(((m.a00*wx3)>>8)+((m.a01*wy3)>>8)+((m.a02*wz3)>>8)+m.a03)
				;
				vy3=
					(((m.a10*wx3)>>8)+((m.a11*wy3)>>8)+((m.a12*wz3)>>8)+m.a13)
				;

				mVertexViewCache[i3]=vx3;
				mVertexViewCache[i3+1]=vy3;
				mVertexViewCache[i3+2]=vz3;
			}
			else{
				#ifdef TOADLET_USE_STATISTICS
					vertexViewTransformCached++;
				#endif

				i3=vi3*3;
				vx3=mVertexViewCache[i3];
				vy3=mVertexViewCache[i3+1];
				vz3=mVertexViewCache[i3+2];
			}

			// TODO: Compare this clip method with the previous one in screenspace, and see if we can simplify this math.
			// If we have to worry about verts behind the nearplane, perhaps we can just invert the clipping in that case?
			if(subMesh.faceCulling==MeshSubMesh.FC_CCW){
				x21=(vx2-vx1)>>6;
				x31=(vx3-vx1)>>6;
				y21=(vy2-vy1)>>6;
				y31=(vy3-vy1)>>6;
				z21=(vz2-vz1)>>6;
				z31=(vz3-vz1)>>6;
				if(
					// vx1*(y21*z31-z21*y31) + vy1*(z21*x31-x21*z31) + vz1*(x21*y31-y21*x31)
					vx1*(((y21*z31)-(z21*y31))>>4) + vy1*(((z21*x31)-(x21*z31))>>4) + vz1*(((x21*y31)-(y21*x31))>>4)
				> mFaceCullEpsilon){
					#ifdef TOADLET_USE_STATISTICS
						triangleFaceCulled++;
					#endif
					continue;
				}
			}
			else if(subMesh.faceCulling==MeshSubMesh.FC_CW){
				x21=(vx2-vx1)>>6;
				x31=(vx3-vx1)>>6;
				y21=(vy2-vy1)>>6;
				y31=(vy3-vy1)>>6;
				z21=(vz2-vz1)>>6;
				z31=(vz3-vz1)>>6;
				if(
					// vx1*(y21*z31-z21*y31) + vy1*(z21*x31-x21*z31) + vz1*(x21*y31-y21*x31)
					vx1*(((y21*z31)-(z21*y31))>>4) + vy1*(((z21*x31)-(x21*z31))>>4) + vz1*(((x21*y31)-(y21*x31))>>4)
				< -mFaceCullEpsilon){
					#ifdef TOADLET_USE_STATISTICS
						triangleFaceCulled++;
					#endif
					continue;
				}
			}

			secondTriangle=false;

			nearClipFlag1=vz1<mNear;
			nearClipFlag2=vz2<mNear;
			nearClipFlag3=vz3<mNear;

			if(nearClipFlag1 || nearClipFlag2 || nearClipFlag3){
				if(nearClipFlag1 && nearClipFlag2 && nearClipFlag3){
					#ifdef TOADLET_USE_STATISTICS
						triangleNearCulled++;
					#endif

					continue;
				}
				// Standard line-plane intersection is used in the clipping, but optimized for Plane((0,0,1),mNear)
				// The divisions are |1 to ensure that there is no division by zero
				// We can use limited precision on the clipTime, but not on the actual clipping
				else if(nearClipFlag1 && nearClipFlag2 && !nearClipFlag3){
					#ifdef TOADLET_USE_STATISTICS
						triangleTrimmed++;
					#endif

					clipTime=(((mNear-vz1)<<6)/(((vz3-vz1)>>6)|1))<<4;
					crx1=
						// clipTime*(vx3-vx1)+vx1
						//((clipTime>>2)*((vx3-vx1)>>6))>>8+vx1
						int((Number(clipTime)*Number(vx3-vx1))/Number(tMath.ONE)) + vx1
					;
					cry1=
						// clipTime*(vy3-vy1)+vy1
						//((clipTime>>2)*((vy3-vy1)>>6))>>8+vy1
						int((Number(clipTime)*Number(vy3-vy1))/Number(tMath.ONE)) + vx1
					;
					clipTime=(((mNear-vz2)<<6)/(((vz3-vz2)>>6)|1))<<4;
					crx2=
						// clipTime*(vx3-vx2)+vx2
						//((clipTime>>2)*((vx3-vx2)>>6))>>8+vx2
						int((Number(clipTime)*Number(vx3-vx2))/Number(tMath.ONE)) + vx2
					;
					cry2=
						// clipTime*(vy3-vy2)+vy2
						//((clipTime>>2)*((vy3-vy2)>>6))>>8+vy2
						int((Number(clipTime)*Number(vy3-vy2))/Number(tMath.ONE)) + vy2
					;

					vx1=crx1;vy1=cry1;vz1=mNear;
					vx2=crx2;vy2=cry2;vz2=mNear;
				}
				else if(nearClipFlag1 && !nearClipFlag2 && nearClipFlag3){
					#ifdef TOADLET_USE_STATISTICS
						triangleTrimmed++;
					#endif

					clipTime=(((mNear-vz1)<<6)/(((vz2-vz1)>>6)|1))<<4;
					crx1=
						// clipTime*(vx2-vx1)+vx1
						//((clipTime>>2)*((vx2-vx1)>>6))>>8+vx1
						int((Number(clipTime)*Number(vx2-vx1))/Number(tMath.ONE)) + vx1
					;
					cry1=
						// clipTime*(vy2-vy1)+vy1
						//((clipTime>>2)*((vy2-vy1)>>6))>>8+vy1
						int((Number(clipTime)*Number(vy2-vy1))/Number(tMath.ONE)) + vy1
					;
					clipTime=(((mNear-vz3)<<6)/(((vz2-vz3)>>6)|1))<<4;
					crx2=
						// clipTime*(vx2-vx3)+vx3
						//((clipTime>>2)*((vx2-vx3)>>6))>>8+vx3
						int((Number(clipTime)*Number(vx2-vx3))/Number(tMath.ONE)) + vx3
					;
					cry2=
						// clipTime*(vy2-vy3)+vy3
						//((clipTime>>2)*((vy2-vy3)>>6))>>8+vy3
						int((Number(clipTime)*Number(vy2-vy3))/Number(tMath.ONE)) + vy3
					;

					vx1=crx1;vy1=cry1;vz1=mNear;
					vx3=crx2;vy3=cry2;vz3=mNear;
				}
				else if(!nearClipFlag1 && nearClipFlag2 && nearClipFlag3){
					#ifdef TOADLET_USE_STATISTICS
						triangleTrimmed++;
					#endif

					clipTime=(((mNear-vz2)<<6)/(((vz1-vz2)>>6)|1))<<4;
					crx1=
						// clipTime*(vx1-vx2)+vx2
						//((clipTime>>2)*((vx1-vx2)>>6))>>8+vx2
						int((Number(clipTime)*Number(vx1-vx2))/Number(tMath.ONE)) + vx2
					;
					cry1=
						// clipTime*(vy1-vy2)+vy2
						//((clipTime>>2)*((vy1-vy2)>>6))>>8+vy2
						int((Number(clipTime)*Number(vy1-vy2))/Number(tMath.ONE)) + vy2
					;
					clipTime=(((mNear-vz3)<<6)/(((vz1-vz3)>>6)|1))<<4;
					crx2=
						// clipTime*(vx1-vx3)+vx3
						//((clipTime>>2)*((vx1-vx3)>>6))>>8+vx3
						int((Number(clipTime)*Number(vx1-vx3))/Number(tMath.ONE)) + vx3
					;
					cry2=
						// clipTime*(vy1-vy3)+vy3
						//((clipTime>>2)*((vy1-vy3)>>6))>>8+vy3
						int((Number(clipTime)*Number(vy1-vy3))/Number(tMath.ONE)) + vy3
					;

					vx2=crx1;vy2=cry1;vz2=mNear;
					vx3=crx2;vy3=cry2;vz3=mNear;
				}
				else if(nearClipFlag1 && !nearClipFlag2 && !nearClipFlag3){
					#ifdef TOADLET_USE_STATISTICS
						triangleSplit++;
					#endif

					clipTime=(((mNear-vz1)<<6)/(((vz2-vz1)>>6)|1))<<4;
					crx1=
						// clipTime*(vx2-vx1)+vx1
						//((clipTime>>2)*((vx2-vx1)>>6))>>8+vx1
						int((Number(clipTime)*Number(vx2-vx1))/Number(tMath.ONE)) + vx1
					;
					cry1=
						// clipTime*(vy2-vy1)+vy1
						//((clipTime>>2)*((vy2-vy1)>>6))>>8+vy1
						int((Number(clipTime)*Number(vy2-vy1))/Number(tMath.ONE)) + vy1
					;
					clipTime=(((mNear-vz1)<<6)/(((vz3-vz1)>>6)|1))<<4;
					crx2=
						// clipTime*(vx3-vx1)+vx1
						//((clipTime>>2)*((vx3-vx1)>>6))>>8+vx1
						int((Number(clipTime)*Number(vx3-vx1))/Number(tMath.ONE)) + vx1
					;
					cry2=
						// clipTime*(vy3-vy1)+vy1
						//((clipTime>>2)*((vy3-vy1)>>6))>>8+vy1
						int((Number(clipTime)*Number(vy3-vy1))/Number(tMath.ONE)) + vy1
					;

					// No need to swap anything
					vx1=crx1;vy1=cry1;vz1=mNear;
					vx4=crx2;vy4=cry2;vz4=mNear;

					secondTriangle=true;
				}
				else if(!nearClipFlag1 && nearClipFlag2 && !nearClipFlag3){
					#ifdef TOADLET_USE_STATISTICS
						triangleSplit++;
					#endif

					clipTime=(((mNear-vz2)<<6)/(((vz1-vz2)>>6)|1))<<4;
					crx1=
						// clipTime*(vx1-vx2)+vx2
						//((clipTime>>2)*((vx1-vx2)>>6))>>8+vx2
						int((Number(clipTime)*Number(vx1-vx2))/Number(tMath.ONE)) + vy2
					;
					cry1=
						// clipTime*(vy1-vy2)+vy2
						//((clipTime>>2)*((vy1-vy2)>>6))>>8+vy2
						int((Number(clipTime)*Number(vy1-vy2))/Number(tMath.ONE)) + vy2
					;
					clipTime=(((mNear-vz2)<<6)/(((vz3-vz2)>>6)|1))<<4;
					crx2=
						// clipTime*(vx3-vx2)+vx2
						//((clipTime>>2)*((vx3-vx2)>>6))>>8+vx2
						int((Number(clipTime)*Number(vx3-vx2))/Number(tMath.ONE)) + vx2
					;
					cry2=
						// clipTime*(vy3-vy2)+vy2
						//((clipTime>>2)*((vy3-vy2)>>6))>>8+vy2
						int((Number(clipTime)*Number(vy3-vy2))/Number(tMath.ONE)) + vy2
					;

					// Swap 2 & 1
					vx2=vx1;vy2=vy1;vz2=vz1;
					vx1=crx1;vy1=cry1;vz1=mNear;
					vx4=crx2;vy4=cry2;vz4=mNear;

					secondTriangle=true;
				}
				else{ // if(!nearClipFlag1 && !nearClipFlag2 && nearClipFlag3){
					#ifdef TOADLET_USE_STATISTICS
						triangleSplit++;
					#endif

					clipTime=(((mNear-vz3)<<6)/(((vz1-vz3)>>6)|1))<<4;
					crx1=
						// clipTime*(vx1-vx3)+vx3
						//((clipTime>>2)*((vx1-vx3)>>6))>>8+vx3
						int((Number(clipTime)*Number(vx1-vx3))/Number(tMath.ONE)) + vx3
					;
					cry1=
						// clipTime*(vy1-vy3)+vy3
						//((clipTime>>2)*((vy1-vy3)>>6))>>8+vy3
						int((Number(clipTime)*Number(vy1-vy3))/Number(tMath.ONE)) + vy3
					;
					clipTime=(((mNear-vz3)<<6)/(((vz2-vz3)>>6)|1))<<4;
					crx2=
						// clipTime*(vx2-vx3)+vx3
						//((clipTime>>2)*((vx2-vx3)>>6))>>8+vx3
						int((Number(clipTime)*Number(vx2-vx3))/Number(tMath.ONE)) + vx3
					;
					cry2=
						// clipTime*(vy2-vy3)+vy3
						//((clipTime>>2)*((vy2-vy3)>>6))>>8+vy3
						int((Number(clipTime)*Number(vy2-vy3))/Number(tMath.ONE)) + vy3
					;

					// Swap 1 & 2, using 3 as a temporary
					vx3=vx1;vy3=vy1;vz3=vz1;
					vx1=vx2;vy1=vy2;vz1=vz2;
					vx2=vx3;vy2=vy3;vz2=vz3;
					vx3=crx1;vy3=cry1;vz3=mNear;
					vx4=crx2;vy4=cry2;vz4=mNear;

					secondTriangle=true;
				}
			}
			else if(vz1>mFar && vz2>mFar && vz3>mFar){
				#ifdef TOADLET_USE_STATISTICS
					triangleFarCulled++;
				#endif

				continue;
			}

			// Screen Vertex 1
			if(mRenderCount!=mVertexScreenFrames[vi1] || nearClipFlag1 || secondTriangle){
				#ifdef TOADLET_USE_STATISTICS
					vertexScreenTransformed++;
				#endif

				sx1=
					// (vx1/vz1*mDX+mHalfFixedViewportWidth)
					((((((vx1<<6)/(vz1>>6))>>2)*(mDX>>6))>>4)+mHalfFixedViewportWidth)
					>>tMath.BITS
				;
				sy1=
					// (vy1/vz1*mDY+mHalfFixedViewportHeight)
					((((((vy1<<6)/(vz1>>6))>>2)*(mDY>>6))>>4)+mHalfFixedViewportHeight)
					>>tMath.BITS
				;

				if(!(nearClipFlag1 || secondTriangle)){
					mVertexScreenFrames[vi1]=mRenderCount;
					i3=vi1<<1;
					mVertexScreenCache[i3]=sx1;
					mVertexScreenCache[i3+1]=sy1;
				}
			}
			else{
				#ifdef TOADLET_USE_STATISTICS
					vertexScreenTransformCached++;
				#endif

				i3=vi1<<1;
				sx1=mVertexScreenCache[i3];
				sy1=mVertexScreenCache[i3+1];
			}

			// Screen Vertex 2
			if(mRenderCount!=mVertexScreenFrames[vi2] || nearClipFlag2 || secondTriangle){
				#ifdef TOADLET_USE_STATISTICS
					vertexScreenTransformed++;
				#endif

				sx2=
					// (vx2/vz2*mDX+mHalfFixedViewportWidth)
					((((((vx2<<6)/(vz2>>6))>>2)*(mDX>>6))>>4)+mHalfFixedViewportWidth)
					>>tMath.BITS
				;
				sy2=
					// (vy2/vz2*mDY+mHalfFixedViewportHeight)
					((((((vy2<<6)/(vz2>>6))>>2)*(mDY>>6))>>4)+mHalfFixedViewportHeight)
					>>tMath.BITS
				;

				if(!(nearClipFlag2 || secondTriangle)){
					mVertexScreenFrames[vi2]=mRenderCount;
					i3=vi2<<1;
					mVertexScreenCache[i3]=sx2;
					mVertexScreenCache[i3+1]=sy2;
				}
			}
			else{
				#ifdef TOADLET_USE_STATISTICS
					vertexScreenTransformCached++;
				#endif

				i3=vi2<<1;
				sx2=mVertexScreenCache[i3];
				sy2=mVertexScreenCache[i3+1];
			}

			// Screen Vertex 3
			if(mRenderCount!=mVertexScreenFrames[vi3] || nearClipFlag3 || secondTriangle){
				#ifdef TOADLET_USE_STATISTICS
					vertexScreenTransformed++;
				#endif

				sx3=
					// (vx3/vz3*mDX+mHalfFixedViewportWidth)
					((((((vx3<<6)/(vz3>>6))>>2)*(mDX>>6))>>4)+mHalfFixedViewportWidth)
					>>tMath.BITS
				;
				sy3=
					// (vy3/vz3*mDY+mHalfFixedViewportHeight)
					((((((vy3<<6)/(vz3>>6))>>2)*(mDY>>6))>>4)+mHalfFixedViewportHeight)
					>>tMath.BITS
				;

				if(!(nearClipFlag3 || secondTriangle)){
					mVertexScreenFrames[vi3]=mRenderCount;
					i3=vi3<<1;
					mVertexScreenCache[i3]=sx3;
					mVertexScreenCache[i3+1]=sy3;
				}
			}
			else{
				#ifdef TOADLET_USE_STATISTICS
					vertexScreenTransformCached++;
				#endif

				i3=vi3<<1;
				sx3=mVertexScreenCache[i3];
				sy3=mVertexScreenCache[i3+1];
			}

			clipFlags1=sx1<=0?CF_LEFT_PLANE:0;
			clipFlags1|=sx1>=mViewportWidth?CF_RIGHT_PLANE:0;
			clipFlags1|=sy1<=0?CF_BOTTOM_PLANE:0;
			clipFlags1|=sy1>=mViewportHeight?CF_TOP_PLANE:0;

			clipFlags2=sx2<=0?CF_LEFT_PLANE:0;
			clipFlags2|=sx2>=mViewportWidth?CF_RIGHT_PLANE:0;
			clipFlags2|=sy2<=0?CF_BOTTOM_PLANE:0;
			clipFlags2|=sy2>=mViewportHeight?CF_TOP_PLANE:0;

			clipFlags3=sx3<=0?CF_LEFT_PLANE:0;
			clipFlags3|=sx3>=mViewportWidth?CF_RIGHT_PLANE:0;
			clipFlags3|=sy3<=0?CF_BOTTOM_PLANE:0;
			clipFlags3|=sy3>=mViewportHeight?CF_TOP_PLANE:0;

			if((clipFlags1 & clipFlags2 & clipFlags3)>0){
				#ifdef TOADLET_USE_STATISTICS
					triangleScreenCulled++;
				#endif

				continue;
			}

			if(subMesh.sort){
				tableIndex=(((vz1+vz2+vz3)/3)-mNear)*mOrderingTable.length/viewDistance;
				// This tableIndex capping can be an if >=, or a bitwise AND with 1023
				// The bitwise AND wont properly sort polygons that fall outside of it however
				if(tableIndex>=mOrderingTable.length){
					tableIndex=mOrderingTable.length-1;
				}
			}
			else{
				tableIndex=0;
			}

			tri=mTriangleCache[mTriangleCacheCount++];
			tri.next=mOrderingTable[tableIndex];
			mOrderingTable[tableIndex]=tri;
			{
				tri.sx1=sx1;
				tri.sy1=sy1;
				tri.sx2=sx2;
				tri.sy2=sy2;
				tri.sx3=sx3;
				tri.sy3=sy3;

				if(subMesh.lightingEnabled && mLighting){
					i3=vi1*3;
					tri.color1=(((norm[i3]>>6)*(lx>>2))+((norm[i3+1]>>6)*(ly>>2))+((norm[i3+2]>>6)*(lz>>2)))>>8;
					i3=vi2*3;
					tri.color2=(((norm[i3]>>6)*(lx>>2))+((norm[i3+1]>>6)*(ly>>2))+((norm[i3+2]>>6)*(lz>>2)))>>8;
					i3=vi3*3;
					tri.color3=(((norm[i3]>>6)*(lx>>2))+((norm[i3+1]>>6)*(ly>>2))+((norm[i3+2]>>6)*(lz>>2)))>>8;
					if(mLowResLighting || subMesh.flatShading){
						tri.color1=((tri.color1+tri.color2+tri.color3)/6+tMath.HALF+mAmbient);
						if(tri.color1>0x0000FFFF){
							tri.color1=0x0000FFFF;
						}
						tri.color1>>=8;
						tri.color3=tri.color2=tri.color1=(tri.color1<<16)|(tri.color1<<8)|tri.color1;
					}
					else{
						tri.color1=tri.color1>>1+tMath.HALF+mAmbient;
						if(tri.color1>0x0000FFFF){
							tri.color1=0x0000FFFF;
						}
						tri.color1>>=8;
						tri.color1=(tri.color1<<16)|(tri.color1<<8)|tri.color1;

						tri.color2=tri.color2>>1+tMath.HALF+mAmbient;
						if(tri.color2>0x0000FFFF){
							tri.color2=0x0000FFFF;
						}
						tri.color2>>=8;
						tri.color2=(tri.color2<<16)|(tri.color2<<8)|tri.color2;

						tri.color3=tri.color3>>1+tMath.HALF+mAmbient;
						if(tri.color3>0x0000FFFF){
							tri.color3=0x0000FFFF;
						}
						tri.color3>>=8;
						tri.color3=(tri.color3<<16)|(tri.color3<<8)|tri.color3;
					}
				}
				else{
					tri.color1=0xFFFFFFFF;
					tri.color2=0xFFFFFFFF;
					tri.color3=0xFFFFFFFF;
				}

				if(subMesh.hasTexture){
					tri.texture=meshEntity.mTextures[subMesh.textureIndex];
					tri.tu1=(tex[(vi1<<1)+0]);
					tri.tv1=(tex[(vi1<<1)+1]);
					tri.tu2=(tex[(vi2<<1)+0]);
					tri.tv2=(tex[(vi2<<1)+1]);
					tri.tu3=(tex[(vi3<<1)+0]);
					tri.tv3=(tex[(vi3<<1)+1]);
				}
				else{
					tri.texture=null;
					if(mLowResLighting){
						tri.color3=tri.color2=tri.color1&=subMesh.color;
					}
					else{
						tri.color1=
							(((((tri.color1>>16)&0x000000FF)*((subMesh.color>>16)&0x000000FF))<<8)&0x00FF0000) |
							(((((tri.color1>>8)&0x000000FF)*((subMesh.color>>8)&0x000000FF))>>0)&0x0000FF00) |
							(((((tri.color1>>0)&0x000000FF)*((subMesh.color>>0)&0x000000FF))>>8)&0x000000FF);

						tri.color2=
							(((((tri.color2>>16)&0x000000FF)*((subMesh.color>>16)&0x000000FF))<<8)&0x00FF0000) |
							(((((tri.color2>>8)&0x000000FF)*((subMesh.color>>8)&0x000000FF))>>0)&0x0000FF00) |
							(((((tri.color2>>0)&0x000000FF)*((subMesh.color>>0)&0x000000FF))>>8)&0x000000FF);

						tri.color3=
							(((((tri.color3>>16)&0x000000FF)*((subMesh.color>>16)&0x000000FF))<<8)&0x00FF0000) |
							(((((tri.color3>>8)&0x000000FF)*((subMesh.color>>8)&0x000000FF))>>0)&0x0000FF00) |
							(((((tri.color3>>0)&0x000000FF)*((subMesh.color>>0)&0x000000FF))>>8)&0x000000FF);
					}
				}
			}

			if(secondTriangle){
				sx4=
					// (vx4/vz4*mDX+mHalfFixedViewportWidth)
					((((((vx4<<6)/(vz4>>6))>>2)*(mDX>>6))>>4)+mHalfFixedViewportWidth)
					>>tMath.BITS
				;
				sy4=
					// (vy4/vz4*mDY+mHalfFixedViewportHeight)
					((((((vy4<<6)/(vz4>>6))>>2)*(mDY>>6))>>4)+mHalfFixedViewportHeight)
					>>tMath.BITS
				;

				clipFlags4=sx4<=0?CF_LEFT_PLANE:0;
				clipFlags4|=sx4>=mViewportWidth?CF_RIGHT_PLANE:0;
				clipFlags4|=sy4<=0?CF_BOTTOM_PLANE:0;
				clipFlags4|=sy4>=mViewportHeight?CF_TOP_PLANE:0;
				
				if((clipFlags1 & clipFlags3 & clipFlags4)>0){
					#ifdef TOADLET_USE_STATISTICS
						triangleScreenCulled++;
					#endif

					continue;
				}

				tri2=mTriangleCache[mTriangleCacheCount++];
				tri2.next=tri;
				mOrderingTable[tableIndex]=tri2;
				{
					tri2.sx1=sx1;
					tri2.sy1=sy1;
					tri2.sx2=sx3;
					tri2.sy2=sy3;
					tri2.sx3=sx4;
					tri2.sy3=sy4;

					tri2.color1=tri.color1;
					tri2.color2=tri.color2;
					tri2.color3=tri.color3;
					tri2.texture=tri.texture;

					if(subMesh.hasTexture){
						tri2.tu1=tri.tu1;
						tri2.tv1=tri.tv1;
						tri2.tu2=tri.tu2;
						tri2.tv2=tri.tv2;
						tri2.tu3=tri.tu3;
						tri2.tv3=tri.tv3;
					}
				}
			}
		}
	}

	private function resizeVertexCache(size:int):void{
		mVertexViewCache=new Array(size*3);
		mVertexScreenCache=new Array(size*2);
		mVertexViewFrames=new Array(size);
		mVertexScreenFrames=new Array(size);
	}

	private function resizeTriangleCache(size:int):void{
		mTriangleCache=new Array(size);
		var i:int;
		for(i=0;i<size;++i){
			mTriangleCache[i]=new Triangle();
		}
	}

	internal var mLighting:Boolean;
	internal var mLightDirection:Vector3=new Vector3();
	internal var mAmbient:int;
	internal var mFaceCullEpsilon:int;
	internal var mLowResLighting:Boolean=false;

	internal var mViewportWidth:int,mViewportHeight:int;
	internal var mFixedViewportWidth:int,mFixedViewportHeight:int;
	internal var mHalfFixedViewportWidth:int,mHalfFixedViewportHeight:int;
	internal var mInvHX:int,mInvHY:int;
	internal var mNear:int,mFar:int;
	internal var mDX:int,mDY:int;

	internal var mRenderCount:int;
	internal var mVertexViewFrames:Array;
	internal var mVertexScreenFrames:Array;
	internal var mVertexViewCache:Array;
	internal var mVertexScreenCache:Array;
	internal var mTriangleCache:Array;
	internal var mTriangleCacheCount:int;
	internal var mOrderingTable:Array;
	internal var mTransformCache:Matrix4x3=new Matrix4x3();

	internal var mGraphics:Graphics;

	#ifdef TOADLET_USE_STATISTICS
		internal var vertexViewTransformed:int;
		internal var vertexViewTransformCached:int;
		internal var vertexScreenTransformed:int;
		internal var vertexScreenTransformCached:int;
		internal var triangleIncoming:int;
		internal var triangleFaceCulled:int;
		internal var triangleNearCulled:int;
		internal var triangleFarCulled:int;
		internal var triangleTrimmed:int;
		internal var triangleSplit:int;
		internal var triangleScreenCulled:int;
		internal var triangleTextureRendered:int;
		internal var triangleColorRendered:int;
	#endif
}

}
