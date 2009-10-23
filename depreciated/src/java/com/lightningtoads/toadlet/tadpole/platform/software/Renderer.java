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

////#define TOADLET_USE_STATISTICS

//#ifdef TOADLET_PLATFORM_BLACKBERRY
	import net.rim.device.api.ui.*;
	import net.rim.device.api.system.Bitmap;
//#else
	import javax.microedition.lcdui.*;
	import javax.microedition.lcdui.game.*;
//#endif

import com.lightningtoads.toadlet.egg.mathfixed.*;
import com.lightningtoads.toadlet.egg.mathfixed.Math;
import com.lightningtoads.toadlet.tadpole.sg.*;
//#ifdef TOADLET_USE_STATISTICS
	import com.lightningtoads.toadlet.egg.*;
//#endif

public final class Renderer{
	public Renderer(int screenWidth,int screenHeight){
		setViewportSize(screenWidth,screenHeight);
		resizeVertexCache(1024);
		resizeTriangleCache(2048);
		mOrderingTable=new Triangle[1024];
		mFaceCullEpsilon=1<<12;
	}

	public void setViewportSize(int width,int height){
		if(mBuffer==null || width>mBuffer.length){
			mBuffer=new int[width];
		}

		mViewportWidth=width;
		mViewportHeight=height;
		mFixedViewportWidth=Math.intToFixed(mViewportWidth);
		mHalfFixedViewportWidth=mFixedViewportWidth>>1;
		mFixedViewportHeight=Math.intToFixed(mViewportHeight);
		mHalfFixedViewportHeight=mFixedViewportHeight>>1;
	}

	public void setProjection(int invHX,int invHY,int near,int far){
		mInvHX=invHX;
		mInvHY=invHY;
		mNear=near;
		mFar=far;
	}

	public void setLighting(boolean enabled,Vector3 direction,int ambient){
		mLighting=enabled;
		if(enabled){
			mLightDirection.set(direction);
		}
		mAmbient=ambient;
	}

	public void setOrderingTableSize(int size){
		mOrderingTable=new Triangle[size];
	}

	public void setFaceCullEpsilon(int epsilon){
		mFaceCullEpsilon=epsilon;
	}

	public void beginScene(Graphics g){
		//#ifdef TOADLET_USE_STATISTICS
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
		//#endif

		mDX=
			////#exec fpcomp.bat "mInvHX*mHalfFixedViewportWidth"
			((mInvHX>>6)*(mHalfFixedViewportWidth>>6))>>4
		;
		mDY=-
			////#exec fpcomp.bat "mInvHY*mHalfFixedViewportHeight"
			((mInvHY>>6)*(mHalfFixedViewportHeight>>6))>>4
		;

		mGraphics=g;

		mColor=0xFFFFFFFF;
		mGraphics.setColor(mColor);
	}

	public void flush(){
		Triangle tri;
		int j;

		for(j=mOrderingTable.length-1;j>=0;--j){
			tri=mOrderingTable[j];
			mOrderingTable[j]=null;
			while(tri!=null){
				if(tri.texture!=null){
					//#ifdef TOADLET_USE_STATISTICS
						triangleTextureRendered++;
					//#endif

					drawTexturedTriangle(tri);
				}
				else{
					//#ifdef TOADLET_USE_STATISTICS
						triangleColorRendered++;
					//#endif

					if(mColor!=tri.color){
						mGraphics.setColor(tri.color);
						mColor=tri.color;
					}
					//#ifdef TOADLET_PLATFORM_BLACKBERRY
						fpx[0]=tri.vertexes[0];fpy[0]=tri.vertexes[1];
						fpx[1]=tri.vertexes[4];fpy[1]=tri.vertexes[5];
						fpx[2]=tri.vertexes[8];fpy[2]=tri.vertexes[9];
						mGraphics.drawFilledPath(fpx,fpy,null,null);
					//#else
						mGraphics.fillTriangle(tri.vertexes[0],tri.vertexes[1],tri.vertexes[4],tri.vertexes[5],tri.vertexes[8],tri.vertexes[9]);
					//#endif
				}
				tri=tri.next;
			}
		}

		mTriangleCacheCount=0;
	}
	
	public void endScene(){
		//#ifdef TOADLET_USE_STATISTICS
			Logger.getInstance().addLogString(Logger.LEVEL_ALERT,
				"Renderer data:\n"+
				"triangleIncoming:"+triangleIncoming+"\n"+
				"vertexViewTransformed:"+vertexViewTransformed+"\n"+
				"vertexViewTransformCached:"+vertexViewTransformCached+"\n"+
				"vertexScreenTransformed:"+vertexScreenTransformed+"\n"+
				"vertexScreenTransformCached:"+vertexScreenTransformCached+"\n"+
				"triangleFaceCulled:"+triangleFaceCulled+"\n"+
				"triangleNearCulled:"+triangleNearCulled+"\n"+
				"triangleFarCulled:"+triangleFarCulled+"\n"+
				"triangleTrimmed:"+triangleTrimmed+"\n"+
				"triangleSplit:"+triangleSplit+"\n"+
				"triangleScreenCulled:"+triangleScreenCulled+"\n"+
				"triangleTextureRendered:"+triangleTextureRendered+"\n"+
				"triangleColorRendered:"+triangleColorRendered+"\n"
			);
		//#endif

		mGraphics=null;
	}

	public void renderSpriteEntity(Matrix4x3 transform,SpriteEntity spriteEntity){
		if(spriteEntity.mSprite==null){
			return;
		}

		int m[]=transform.data;

		int wx1,wy1,wz1;
		int vx1,vy1,vz1;
		int sx1,sy1;

		int bits=spriteEntity.getEngine().mRenderShiftBits;

		vx1=m[0+3*3];
		vy1=m[1+3*3];
		vz1=-m[2+3*3];

		if((vz1<=mNear) || (vz1>=mFar)){
			return;
		}

		sx1=
			//#exec fpcomp.bat "(vx1*mDX/vz1+mHalfFixedViewportWidth)"
			//((((((vx1>>6)*(mDX>>6))<<2)/(vz1>>6))<<4)+mHalfFixedViewportWidth)
			>>Math.BITS
		;
		sy1=
			//#exec fpcomp.bat "(vy1*mDY/vz1+mHalfFixedViewportHeight)"
			//((((((vy1>>6)*(mDY>>6))<<2)/(vz1>>6))<<4)+mHalfFixedViewportHeight)
			>>Math.BITS
		;

		int index=0;
		if(spriteEntity.mSprite.images.length>1){
			vz1=Math.mul(Math.div(vz1-mNear+(spriteEntity.mSwitchOffset>>bits),mFar-mNear)-Math.HALF,spriteEntity.mSwitchScale)+Math.HALF;
			if(vz1<0){
				vz1=0;
			}
			else if(vz1>=Math.ONE){
				vz1=Math.ONE-1; // Just a unit below 1
			}
			index=Math.fixedToInt(Math.mul(vz1,Math.intToFixed(spriteEntity.mSprite.images.length)));
		}

		//#ifdef TOADLET_PLATFORM_BLACKBERRY
			Bitmap image=spriteEntity.mSprite.images[index];
		//#else
			Image image=spriteEntity.mSprite.images[index];
		//#endif
		int imageWidth=image.getWidth()/spriteEntity.mSprite.widthFrames;
		int imageHeight=image.getHeight()/spriteEntity.mSprite.heightFrames;
		int imageXOffset=spriteEntity.mFrameX*imageWidth;
		int imageYOffset=spriteEntity.mFrameY*imageHeight;
		int x=sx1;
		int y=sy1;

		if((spriteEntity.mAlignment&Graphics.HCENTER)>0){
			x-=(imageWidth>>1);
		}
		if((spriteEntity.mAlignment&Graphics.RIGHT)>0){
			x-=imageWidth;
		}
		if((spriteEntity.mAlignment&Graphics.VCENTER)>0){
			y-=(imageHeight>>1);
		}
		if((spriteEntity.mAlignment&Graphics.BOTTOM)>0){
			y-=imageHeight;
		}

		//#ifdef TOADLET_PLATFORM_BLACKBERRY
			mGraphics.drawBitmap(x,y,imageWidth,imageHeight,image,imageXOffset,imageYOffset);
		//#else
			int origClipX=mGraphics.getClipX();
			int origClipY=mGraphics.getClipY();
			int origClipWidth=mGraphics.getClipWidth();
			int origClipHeight=mGraphics.getClipHeight();

			if(x<0){
				imageWidth+=x;
				imageXOffset-=x;
				x=0;
			}

			if(y<0){
				imageHeight+=y;
				imageYOffset-=y;
				y=0;
			}

			if(x+imageWidth>origClipX+origClipWidth){
				imageWidth-=(x+imageWidth-origClipWidth-origClipX);
			}

			if(y+imageHeight>origClipY+origClipHeight){
				imageHeight-=(y+imageHeight-origClipHeight-origClipY);
			}

			if(imageWidth>0 && imageHeight>0){
				mGraphics.clipRect(x,y,imageWidth,imageHeight);
				mGraphics.drawImage(image,x-imageXOffset,y-imageYOffset,Graphics.TOP|Graphics.LEFT);
				mGraphics.setClip(origClipX,origClipY,origClipWidth,origClipHeight);
			}
		//#endif
	}

	public void renderBeamEntity(Matrix4x3 transform,BeamEntity beamEntity){
		int m[]=transform.data;

		int wx1,wy1,wz1;
		int vx1,vy1,vz1;
		int sx1,sy1;
		int wx2,wy2,wz2;
		int vx2,vy2,vz2;
		int sx2,sy2;

		int bits=beamEntity.getEngine().mRenderShiftBits;

		wx1=(beamEntity.mStart.x>>bits)>>6;wy1=(beamEntity.mStart.y>>bits)>>6;wz1=(beamEntity.mStart.z>>bits)>>6;
		vx1=
			((((m[0+0*3]>>2)*wx1)>>8)+(((m[0+1*3]>>2)*wy1)>>8)+(((m[0+2*3]>>2)*wz1)>>8)+m[0+3*3]);
		;
		vy1=
			((((m[1+0*3]>>2)*wx1)>>8)+(((m[1+1*3]>>2)*wy1)>>8)+(((m[1+2*3]>>2)*wz1)>>8)+m[1+3*3]);
		;
		vz1=-
			((((m[2+0*3]>>2)*wx1)>>8)+(((m[2+1*3]>>2)*wy1)>>8)+(((m[2+2*3]>>2)*wz1)>>8)+m[2+3*3]);
		;
		sx1=
			//#exec fpcomp.bat "(vx1*mDX/vz1+mHalfFixedViewportWidth)"
			//((((((vx1>>6)*(mDX>>6))<<2)/(vz1>>6))<<4)+mHalfFixedViewportWidth)
			>>Math.BITS
		;
		sy1=
			//#exec fpcomp.bat "(vy1*mDY/vz1+mHalfFixedViewportHeight)"
			//((((((vy1>>6)*(mDY>>6))<<2)/(vz1>>6))<<4)+mHalfFixedViewportHeight)
			>>Math.BITS
		;

		wx2=(beamEntity.mEnd.x>>bits)>>6;wy2=(beamEntity.mEnd.y>>bits)>>6;wz2=(beamEntity.mEnd.z>>bits)>>6;
		vx2=
			((((m[0+0*3]>>2)*wx2)>>8)+(((m[0+1*3]>>2)*wy2)>>8)+(((m[0+2*3]>>2)*wz2)>>8)+m[0+3*3]);
		;
		vy2=
			((((m[1+0*3]>>2)*wx2)>>8)+(((m[1+1*3]>>2)*wy2)>>8)+(((m[1+2*3]>>2)*wz2)>>8)+m[1+3*3]);
		;
		vz2=-
			((((m[2+0*3]>>2)*wx2)>>8)+(((m[2+1*3]>>2)*wy2)>>8)+(((m[2+2*3]>>2)*wz2)>>8)+m[2+3*3]);
		;
		sx2=
			//#exec fpcomp.bat "(vx2*mDX/vz2+mHalfFixedViewportWidth)"
			//((((((vx2>>6)*(mDX>>6))<<2)/(vz2>>6))<<4)+mHalfFixedViewportWidth)
			>>Math.BITS
		;
		sy2=
			//#exec fpcomp.bat "(vy2*mDY/vz2+mHalfFixedViewportHeight)"
			//((((((vy2>>6)*(mDY>>6))<<2)/(vz2>>6))<<4)+mHalfFixedViewportHeight)
			>>Math.BITS
		;

		if(mColor!=beamEntity.mColor){
			mGraphics.setColor(beamEntity.mColor);
			mColor=beamEntity.mColor;
		}
		mGraphics.drawLine(sx1,sy1,sx2,sy2);
	}

	// ClipFlags
	// The first 4 are in screenspace
	static final byte CF_LEFT_PLANE=	1<<0;
	static final byte CF_RIGHT_PLANE=	1<<1;
	static final byte CF_BOTTOM_PLANE=	1<<2;
	static final byte CF_TOP_PLANE=		1<<3;

	// These dont necessarily need to be member variables, but its more optimal to have them here
	//#ifdef TOADLET_PLATFORM_BLACKBERRY
		int[] fpx=new int[3];
		int[] fpy=new int[3];
	//#endif

	class Triangle{
		int[] vertexes=new int[12]; // Screen Position, Tex Coord
		int[] texture;
		int color=0x00FFFFFF;

		Triangle next;
	};

	public void renderMeshEntity(Matrix4x3 transform,MeshEntity meshEntity){
		int m[]=mTransformCache;
		int j;
		// Shift the bits of the matrix that we multiply right
		for(j=0;j<9;++j){
			m[j]=transform.data[j]>>2;
		}
		m[9]=transform.data[9];
		m[10]=transform.data[10];
		m[11]=transform.data[11];

		int pos[]=null;
		int norm[]=null;
		int tex[]=meshEntity.mMesh.vertexData.texCoords;
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

		int vi1,vi2,vi3,i3,j3;
		int wx1,wy1,wz1;
		int vx1,vy1,vz1;
		int sx1,sy1;
		int wx2,wy2,wz2;
		int vx2,vy2,vz2;
		int sx2,sy2;
		int wx3,wy3,wz3;
		int vx3,vy3,vz3;
		int sx3,sy3;
		int vx4=0,vy4=0,vz4=0;
		int sx4,sy4;
		int x21,x31,y21,y31,z21,z31;
		int tcBits;
		int tableIndex;
		Triangle tri,tri2;
		byte clipFlags1,clipFlags2,clipFlags3,clipFlags4;
		boolean nearClipFlag1,nearClipFlag2,nearClipFlag3;
		int clipTime,crx1,cry1,crx2,cry2;
		boolean secondTriangle=false;
		int viewDistance=mFar-mNear;
		int lx=0,ly=0,lz=0;

		short tid[]=meshEntity.mMesh.triIndexData;
		byte tsm[]=meshEntity.mMesh.triSubMesh;
		Mesh.SubMesh[] subs=meshEntity.mMesh.subMeshes;

		int numTris=tid.length/3;

		// Size up triangle array for worst case (all triangles split)
		if(mTriangleCache.length<(mTriangleCacheCount+numTris)*2){
			resizeTriangleCache((mTriangleCacheCount+numTris)*2);
		}

		if(mLighting){
			// We should be able to use x,y,z >> 2, and with m.a00 >> 2, we would have >> 12 below.
			// However, since the m matrix is rotation & scale, we run out of precision.
			// Since our m matrix is already shifted >> 2, we just use x,y,z >> 6, and then >> 8 total

			// TODO: Not shift the m matrix >> 2, but instead >> 6, then we could use the correct mLightDirection shift

			int x=-mLightDirection.x>>6;
			int y=-mLightDirection.y>>6;
			int z=-mLightDirection.z>>6;

			// Our transform is the models space prefixed by camera space, and our light is in camera space so we transform our light by inverse(transform)
			// Then our light is in the models local space, so we do not have to transform any normals.
			// m & x,y,z are preshifted already, and the result is >> 8 total, so >> 8 & << 6 = >> 2
			lx=
				(((m[0+0*3]*x + m[1+0*3]*y + m[2+0*3]*z)>>2)/(meshEntity.mMesh.worldScale>>6))<<4
			;
			ly=
				(((m[0+1*3]*x + m[1+1*3]*y + m[2+1*3]*z)>>2)/(meshEntity.mMesh.worldScale>>6))<<4
			;
			lz=
				(((m[0+2*3]*x + m[1+2*3]*y + m[2+2*3]*z)>>2)/(meshEntity.mMesh.worldScale>>6))<<4
			;
		}

		//#ifdef TOADLET_USE_STATISTICS
			triangleIncoming+=numTris;
		//#endif
		for(j=0;j<numTris;++j){
			Mesh.SubMesh subMesh=subs[tsm[j]];

			j3=j*3; // 3 indices in each triangle

			// Vertex 1
			vi1=tid[j3+0];
			if(mRenderCount!=mVertexViewFrames[vi1]){
				//#ifdef TOADLET_USE_STATISTICS
					vertexViewTransformed++;
				//#endif

				mVertexViewFrames[vi1]=mRenderCount;
				i3=vi1*3;
				wx1=pos[i3+0]>>6;
				wy1=pos[i3+1]>>6;
				wz1=pos[i3+2]>>6;
				vz1=-
					(((m[2+0*3]*wx1)>>8)+((m[2+1*3]*wy1)>>8)+((m[2+2*3]*wz1)>>8)+m[2+3*3])
				;
				vx1=
					(((m[0+0*3]*wx1)>>8)+((m[0+1*3]*wy1)>>8)+((m[0+2*3]*wz1)>>8)+m[0+3*3])
				;
				vy1=
					(((m[1+0*3]*wx1)>>8)+((m[1+1*3]*wy1)>>8)+((m[1+2*3]*wz1)>>8)+m[1+3*3])
				;

				mVertexViewCache[i3]=vx1;
				mVertexViewCache[i3+1]=vy1;
				mVertexViewCache[i3+2]=vz1;
			}
			else{
				//#ifdef TOADLET_USE_STATISTICS
					vertexViewTransformCached++;
				//#endif

				i3=vi1*3;
				vx1=mVertexViewCache[i3];
				vy1=mVertexViewCache[i3+1];
				vz1=mVertexViewCache[i3+2];
			}

			// Vertex 2
			vi2=tid[j3+1];
			if(mRenderCount!=mVertexViewFrames[vi2]){
				//#ifdef TOADLET_USE_STATISTICS
					vertexViewTransformed++;
				//#endif

				mVertexViewFrames[vi2]=mRenderCount;
				i3=vi2*3;
				wx2=pos[i3+0]>>6;
				wy2=pos[i3+1]>>6;
				wz2=pos[i3+2]>>6;
				vz2=-
					(((m[2+0*3]*wx2)>>8)+((m[2+1*3]*wy2)>>8)+((m[2+2*3]*wz2)>>8)+m[2+3*3])
				;
				vx2=
					(((m[0+0*3]*wx2)>>8)+((m[0+1*3]*wy2)>>8)+((m[0+2*3]*wz2)>>8)+m[0+3*3])
				;
				vy2=
					(((m[1+0*3]*wx2)>>8)+((m[1+1*3]*wy2)>>8)+((m[1+2*3]*wz2)>>8)+m[1+3*3])
				;

				mVertexViewCache[i3]=vx2;
				mVertexViewCache[i3+1]=vy2;
				mVertexViewCache[i3+2]=vz2;
			}
			else{
				//#ifdef TOADLET_USE_STATISTICS
					vertexViewTransformCached++;
				//#endif

				i3=vi2*3;
				vx2=mVertexViewCache[i3];
				vy2=mVertexViewCache[i3+1];
				vz2=mVertexViewCache[i3+2];
			}

			// Vertex 3
			vi3=tid[j3+2];
			if(mRenderCount!=mVertexViewFrames[vi3]){
				//#ifdef TOADLET_USE_STATISTICS
					vertexViewTransformed++;
				//#endif

				mVertexViewFrames[vi3]=mRenderCount;
				i3=vi3*3;
				wx3=pos[i3+0]>>6;
				wy3=pos[i3+1]>>6;
				wz3=pos[i3+2]>>6;
				vz3=-
					(((m[2+0*3]*wx3)>>8)+((m[2+1*3]*wy3)>>8)+((m[2+2*3]*wz3)>>8)+m[2+3*3])
				;
				vx3=
					(((m[0+0*3]*wx3)>>8)+((m[0+1*3]*wy3)>>8)+((m[0+2*3]*wz3)>>8)+m[0+3*3])
				;
				vy3=
					(((m[1+0*3]*wx3)>>8)+((m[1+1*3]*wy3)>>8)+((m[1+2*3]*wz3)>>8)+m[1+3*3])
				;

				mVertexViewCache[i3]=vx3;
				mVertexViewCache[i3+1]=vy3;
				mVertexViewCache[i3+2]=vz3;
			}
			else{
				//#ifdef TOADLET_USE_STATISTICS
					vertexViewTransformCached++;
				//#endif

				i3=vi3*3;
				vx3=mVertexViewCache[i3];
				vy3=mVertexViewCache[i3+1];
				vz3=mVertexViewCache[i3+2];
			}

			// TODO: Compare this clip method with the previous one in screenspace, and see if we can simplify this math.
			// If we have to worry about verts behind the nearplane, perhaps we can just invert the clipping in that case?
			if(subMesh.faceCulling==Mesh.SubMesh.FC_CCW){
				x21=(vx2-vx1);
				x31=(vx3-vx1);
				y21=(vy2-vy1);
				y31=(vy3-vy1);
				z21=(vz2-vz1);
				z31=(vz3-vz1);
				if(
					//#exec fpcomp.bat "vx1*(y21*z31-z21*y31) + vy1*(z21*x31-x21*z31) + vz1*(x21*y31-y21*x31) "
					//vx1*(((y21*z31)-(z21*y31))>>4) + vy1*(((z21*x31)-(x21*z31))>>4) + vz1*(((x21*y31)-(y21*x31))>>4)
				> mFaceCullEpsilon){
					//#ifdef TOADLET_USE_STATISTICS
						triangleFaceCulled++;
					//#endif
					continue;
				}
			}
			else if(subMesh.faceCulling==Mesh.SubMesh.FC_CW){
				x21=(vx2-vx1);
				x31=(vx3-vx1);
				y21=(vy2-vy1);
				y31=(vy3-vy1);
				z21=(vz2-vz1);
				z31=(vz3-vz1);
				if(
					//#exec fpcomp.bat "vx1*(y21*z31-z21*y31) + vy1*(z21*x31-x21*z31) + vz1*(x21*y31-y21*x31) "
					//vx1*(((y21*z31)-(z21*y31))>>4) + vy1*(((z21*x31)-(x21*z31))>>4) + vz1*(((x21*y31)-(y21*x31))>>4)
				< -mFaceCullEpsilon){
					//#ifdef TOADLET_USE_STATISTICS
						triangleFaceCulled++;
					//#endif
					continue;
				}
			}

			secondTriangle=false;

			nearClipFlag1=vz1<mNear;
			nearClipFlag2=vz2<mNear;
			nearClipFlag3=vz3<mNear;

			if(nearClipFlag1 || nearClipFlag2 || nearClipFlag3){
				if(nearClipFlag1 && nearClipFlag2 && nearClipFlag3){
					//#ifdef TOADLET_USE_STATISTICS
						triangleNearCulled++;
					//#endif

					continue;
				}
				// Standard line-plane intersection is used in the clipping, but optimized for Plane((0,0,1),mNear)
				// The divisions are |1 to ensure that there is no division by zero
				// We can use limited precision on the clipTime, but not on the actual clipping
				else if(nearClipFlag1 && nearClipFlag2 && !nearClipFlag3){
					//#ifdef TOADLET_USE_STATISTICS
						triangleTrimmed++;
					//#endif

					clipTime=(((mNear-vz1)<<6)/(((vz3-vz1)>>6)|1))<<4;
					crx1=
						//#exec fpcomp.bat "clipTime*(vx3-vx1)+vx1"
						//((clipTime>>6)*((vx3-vx1)>>6))>>4+vx1
					;
					cry1=
						//#exec fpcomp.bat "clipTime*(vy3-vy1)+vy1"
						//((clipTime>>6)*((vy3-vy1)>>6))>>4+vy1
					;
					clipTime=(((mNear-vz2)<<6)/(((vz3-vz2)>>6)|1))<<4;
					crx2=
						//#exec fpcomp.bat "clipTime*(vx3-vx2)+vx2"
						//((clipTime>>6)*((vx3-vx2)>>6))>>4+vx2
					;
					cry2=
						//#exec fpcomp.bat "clipTime*(vy3-vy2)+vy2"
						//((clipTime>>6)*((vy3-vy2)>>6))>>4+vy2
					;

					vx1=crx1;vy1=cry1;vz1=mNear;
					vx2=crx2;vy2=cry2;vz2=mNear;
				}
				else if(nearClipFlag1 && !nearClipFlag2 && nearClipFlag3){
					//#ifdef TOADLET_USE_STATISTICS
						triangleTrimmed++;
					//#endif

					clipTime=(((mNear-vz1)<<6)/(((vz2-vz1)>>6)|1))<<4;
					crx1=
						//#exec fpcomp.bat "clipTime*(vx2-vx1)+vx1"
						//((clipTime>>6)*((vx2-vx1)>>6))>>4+vx1
					;
					cry1=
						//#exec fpcomp.bat "clipTime*(vy2-vy1)+vy1"
						//((clipTime>>6)*((vy2-vy1)>>6))>>4+vy1
					;
					clipTime=(((mNear-vz3)<<6)/(((vz2-vz3)>>6)|1))<<4;
					crx2=
						//#exec fpcomp.bat "clipTime*(vx2-vx3)+vx3"
						//((clipTime>>6)*((vx2-vx3)>>6))>>4+vx3
					;
					cry2=
						//#exec fpcomp.bat "clipTime*(vy2-vy3)+vy3"
						//((clipTime>>6)*((vy2-vy3)>>6))>>4+vy3
					;

					vx1=crx1;vy1=cry1;vz1=mNear;
					vx3=crx2;vy3=cry2;vz3=mNear;
				}
				else if(!nearClipFlag1 && nearClipFlag2 && nearClipFlag3){
					//#ifdef TOADLET_USE_STATISTICS
						triangleTrimmed++;
					//#endif

					clipTime=(((mNear-vz2)<<6)/(((vz1-vz2)>>6)|1))<<4;
					crx1=
						//#exec fpcomp.bat "clipTime*(vx1-vx2)+vx2"
						//((clipTime>>6)*((vx1-vx2)>>6))>>4+vx2
					;
					cry1=
						//#exec fpcomp.bat "clipTime*(vy1-vy2)+vy2"
						//((clipTime>>6)*((vy1-vy2)>>6))>>4+vy2
					;
					clipTime=(((mNear-vz3)<<6)/(((vz1-vz3)>>6)|1))<<4;
					crx2=
						//#exec fpcomp.bat "clipTime*(vx1-vx3)+vx3"
						//((clipTime>>6)*((vx1-vx3)>>6))>>4+vx3
					;
					cry2=
						//#exec fpcomp.bat "clipTime*(vy1-vy3)+vy3"
						//((clipTime>>6)*((vy1-vy3)>>6))>>4+vy3
					;

					vx2=crx1;vy2=cry1;vz2=mNear;
					vx3=crx2;vy3=cry2;vz3=mNear;
				}
				else if(nearClipFlag1 && !nearClipFlag2 && !nearClipFlag3){
					//#ifdef TOADLET_USE_STATISTICS
						triangleSplit++;
					//#endif

					clipTime=(((mNear-vz1)<<6)/(((vz2-vz1)>>6)|1))<<4;
					crx1=
						//#exec fpcomp.bat "clipTime*(vx2-vx1)+vx1"
						//((clipTime>>6)*((vx2-vx1)>>6))>>4+vx1
					;
					cry1=
						//#exec fpcomp.bat "clipTime*(vy2-vy1)+vy1"
						//((clipTime>>6)*((vy2-vy1)>>6))>>4+vy1
					;
					clipTime=(((mNear-vz1)<<6)/(((vz3-vz1)>>6)|1))<<4;
					crx2=
						//#exec fpcomp.bat "clipTime*(vx3-vx1)+vx1"
						//((clipTime>>6)*((vx3-vx1)>>6))>>4+vx1
					;
					cry2=
						//#exec fpcomp.bat "clipTime*(vy3-vy1)+vy1"
						//((clipTime>>6)*((vy3-vy1)>>6))>>4+vy1
					;

					// No need to swap anything
					vx1=crx1;vy1=cry1;vz1=mNear;
					vx4=crx2;vy4=cry2;vz4=mNear;

					secondTriangle=true;
				}
				else if(!nearClipFlag1 && nearClipFlag2 && !nearClipFlag3){
					//#ifdef TOADLET_USE_STATISTICS
						triangleSplit++;
					//#endif

					clipTime=(((mNear-vz2)<<6)/(((vz1-vz2)>>6)|1))<<4;
					crx1=
						//#exec fpcomp.bat "clipTime*(vx1-vx2)+vx2"
						//((clipTime>>6)*((vx1-vx2)>>6))>>4+vx2
					;
					cry1=
						//#exec fpcomp.bat "clipTime*(vy1-vy2)+vy2"
						//((clipTime>>6)*((vy1-vy2)>>6))>>4+vy2
					;
					clipTime=(((mNear-vz2)<<6)/(((vz3-vz2)>>6)|1))<<4;
					crx2=
						//#exec fpcomp.bat "clipTime*(vx3-vx2)+vx2"
						//((clipTime>>6)*((vx3-vx2)>>6))>>4+vx2
					;
					cry2=
						//#exec fpcomp.bat "clipTime*(vy3-vy2)+vy2"
						//((clipTime>>6)*((vy3-vy2)>>6))>>4+vy2
					;

					// Swap 2 & 1
					vx2=vx1;vy2=vy1;vz2=vz1;
					vx1=crx1;vy1=cry1;vz1=mNear;
					vx4=crx2;vy4=cry2;vz4=mNear;

					secondTriangle=true;
				}
				else{ // if(!nearClipFlag1 && !nearClipFlag2 && nearClipFlag3){
					//#ifdef TOADLET_USE_STATISTICS
						triangleSplit++;
					//#endif

					clipTime=(((mNear-vz3)<<6)/(((vz1-vz3)>>6)|1))<<4;
					crx1=
						//#exec fpcomp.bat "clipTime*(vx1-vx3)+vx3"
						//((clipTime>>6)*((vx1-vx3)>>6))>>4+vx3
					;
					cry1=
						//#exec fpcomp.bat "clipTime*(vy1-vy3)+vy3"
						//((clipTime>>6)*((vy1-vy3)>>6))>>4+vy3
					;
					clipTime=(((mNear-vz3)<<6)/(((vz2-vz3)>>6)|1))<<4;
					crx2=
						//#exec fpcomp.bat "clipTime*(vx2-vx3)+vx3"
						//((clipTime>>6)*((vx2-vx3)>>6))>>4+vx3
					;
					cry2=
						//#exec fpcomp.bat "clipTime*(vy2-vy3)+vy3"
						//((clipTime>>6)*((vy2-vy3)>>6))>>4+vy3
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
				//#ifdef TOADLET_USE_STATISTICS
					triangleFarCulled++;
				//#endif

				continue;
			}

			// Screen Vertex 1
			if(mRenderCount!=mVertexScreenFrames[vi1] || nearClipFlag1 || secondTriangle){
				//#ifdef TOADLET_USE_STATISTICS
					vertexScreenTransformed++;
				//#endif

				sx1=
					//#exec fpcomp.bat "(vx1/vz1*mDX+mHalfFixedViewportWidth)"
					//((((((vx1<<6)/(vz1>>6))>>2)*(mDX>>6))>>4)+mHalfFixedViewportWidth)
					>>Math.BITS
				;
				sy1=
					//#exec fpcomp.bat "(vy1/vz1*mDY+mHalfFixedViewportHeight)"
					//((((((vy1<<6)/(vz1>>6))>>2)*(mDY>>6))>>4)+mHalfFixedViewportHeight)
					>>Math.BITS
				;

				if(!(nearClipFlag1 || secondTriangle)){
					mVertexScreenFrames[vi1]=mRenderCount;
					i3=vi1<<1;
					mVertexScreenCache[i3]=sx1;
					mVertexScreenCache[i3+1]=sy1;
				}
			}
			else{
				//#ifdef TOADLET_USE_STATISTICS
					vertexScreenTransformCached++;
				//#endif

				i3=vi1<<1;
				sx1=mVertexScreenCache[i3];
				sy1=mVertexScreenCache[i3+1];
			}

			// Screen Vertex 2
			if(mRenderCount!=mVertexScreenFrames[vi2] || nearClipFlag2 || secondTriangle){
				//#ifdef TOADLET_USE_STATISTICS
					vertexScreenTransformed++;
				//#endif

				sx2=
					//#exec fpcomp.bat "(vx2/vz2*mDX+mHalfFixedViewportWidth)"
					//((((((vx2<<6)/(vz2>>6))>>2)*(mDX>>6))>>4)+mHalfFixedViewportWidth)
					>>Math.BITS
				;
				sy2=
					//#exec fpcomp.bat "(vy2/vz2*mDY+mHalfFixedViewportHeight)"
					//((((((vy2<<6)/(vz2>>6))>>2)*(mDY>>6))>>4)+mHalfFixedViewportHeight)
					>>Math.BITS
				;

				if(!(nearClipFlag2 || secondTriangle)){
					mVertexScreenFrames[vi2]=mRenderCount;
					i3=vi2<<1;
					mVertexScreenCache[i3]=sx2;
					mVertexScreenCache[i3+1]=sy2;
				}
			}
			else{
				//#ifdef TOADLET_USE_STATISTICS
					vertexScreenTransformCached++;
				//#endif

				i3=vi2<<1;
				sx2=mVertexScreenCache[i3];
				sy2=mVertexScreenCache[i3+1];
			}

			// Screen Vertex 3
			if(mRenderCount!=mVertexScreenFrames[vi3] || nearClipFlag3 || secondTriangle){
				//#ifdef TOADLET_USE_STATISTICS
					vertexScreenTransformed++;
				//#endif

				sx3=
					//#exec fpcomp.bat "(vx3/vz3*mDX+mHalfFixedViewportWidth)"
					//((((((vx3<<6)/(vz3>>6))>>2)*(mDX>>6))>>4)+mHalfFixedViewportWidth)
					>>Math.BITS
				;
				sy3=
					//#exec fpcomp.bat "(vy3/vz3*mDY+mHalfFixedViewportHeight)"
					//((((((vy3<<6)/(vz3>>6))>>2)*(mDY>>6))>>4)+mHalfFixedViewportHeight)
					>>Math.BITS
				;

				if(!(nearClipFlag3 || secondTriangle)){
					mVertexScreenFrames[vi3]=mRenderCount;
					i3=vi3<<1;
					mVertexScreenCache[i3]=sx3;
					mVertexScreenCache[i3+1]=sy3;
				}
			}
			else{
				//#ifdef TOADLET_USE_STATISTICS
					vertexScreenTransformCached++;
				//#endif

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
				//#ifdef TOADLET_USE_STATISTICS
					triangleScreenCulled++;
				//#endif

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
				tri.vertexes[0]=sx1;
				tri.vertexes[1]=sy1;
				tri.vertexes[4]=sx2;
				tri.vertexes[5]=sy2;
				tri.vertexes[8]=sx3;
				tri.vertexes[9]=sy3;

				if(subMesh.lightingEnabled && mLighting){
					i3=vi1*3;
					tri.color =(((norm[i3]>>6)*(lx>>2))+((norm[i3+1]>>6)*(ly>>2))+((norm[i3+2]>>6)*(lz>>2)))>>8;
					i3=vi2*3;
					tri.color+=(((norm[i3]>>6)*(lx>>2))+((norm[i3+1]>>6)*(ly>>2))+((norm[i3+2]>>6)*(lz>>2)))>>8;
					i3=vi3*3;
					tri.color+=(((norm[i3]>>6)*(lx>>2))+((norm[i3+1]>>6)*(ly>>2))+((norm[i3+2]>>6)*(lz>>2)))>>8;
					tri.color=(tri.color/6)+Math.HALF+mAmbient; // Divide by 3 to average, then divide by 2 and add a half to renormalize, then add in ambient
					if(tri.color>0x0000FFFF){
						tri.color=0x0000FFFF;
					}

					tri.color>>=8; // Only use the upper 8 bits
					tri.color=(tri.color<<16) | (tri.color<<8) | tri.color;
				}
				else{
					tri.color=0xFFFFFFFF;
				}

				if(subMesh.hasTexture){
					tri.texture=meshEntity.mTextures[subMesh.textureIndex][1];
					tcBits=meshEntity.mTexCoordShiftBits[subMesh.textureIndex]+2;
					tri.vertexes[2]=(tex[(vi1<<1)+0])>>tcBits;
					tri.vertexes[3]=(tex[(vi1<<1)+1])>>tcBits;
					tri.vertexes[6]=(tex[(vi2<<1)+0])>>tcBits;
					tri.vertexes[7]=(tex[(vi2<<1)+1])>>tcBits;
					tri.vertexes[10]=(tex[(vi3<<1)+0])>>tcBits;
					tri.vertexes[11]=(tex[(vi3<<1)+1])>>tcBits;
				}
				else{
					tri.texture=null;
					tri.color&=subMesh.color;
				}
			}

			if(secondTriangle){
				sx4=
					//#exec fpcomp.bat "(vx4/vz4*mDX+mHalfFixedViewportWidth)"
					//((((((vx4<<6)/(vz4>>6))>>2)*(mDX>>6))>>4)+mHalfFixedViewportWidth)
					>>Math.BITS
				;
				sy4=
					//#exec fpcomp.bat "(vy4/vz4*mDY+mHalfFixedViewportHeight)"
					//((((((vy4<<6)/(vz4>>6))>>2)*(mDY>>6))>>4)+mHalfFixedViewportHeight)
					>>Math.BITS
				;

				clipFlags4=sx4<=0?CF_LEFT_PLANE:0;
				clipFlags4|=sx4>=mViewportWidth?CF_RIGHT_PLANE:0;
				clipFlags4|=sy4<=0?CF_BOTTOM_PLANE:0;
				clipFlags4|=sy4>=mViewportHeight?CF_TOP_PLANE:0;
				
				if((clipFlags1 & clipFlags3 & clipFlags4)>0){
					//#ifdef TOADLET_USE_STATISTICS
						triangleScreenCulled++;
					//#endif

					continue;
				}

				tri2=mTriangleCache[mTriangleCacheCount++];
				tri2.next=tri;
				mOrderingTable[tableIndex]=tri2;
				{
					tri2.vertexes[0]=sx1;
					tri2.vertexes[1]=sy1;
					tri2.vertexes[4]=sx3;
					tri2.vertexes[5]=sy3;
					tri2.vertexes[8]=sx4;
					tri2.vertexes[9]=sy4;

					tri2.color=tri.color;
					tri2.texture=tri.texture;

					if(subMesh.hasTexture){
						tri2.vertexes[2]=tri.vertexes[2];
						tri2.vertexes[3]=tri.vertexes[3];
						tri2.vertexes[6]=tri.vertexes[6];
						tri2.vertexes[7]=tri.vertexes[7];
						tri2.vertexes[10]=tri.vertexes[10];
						tri2.vertexes[11]=tri.vertexes[11];
					}
				}
			}
		}
	}

	int left_array[]=new int[3],right_array[]=new int[3];
	int left_section, right_section;
	int left_section_height, right_section_height;
	int dudx, dvdx;
	int left_u, delta_left_u, left_v, delta_left_v;
	int left_x, delta_left_x, right_x, delta_right_x;
	int[] triVerts;

	int rightSection(){
	    int v1 = right_array[ right_section ];
	    int v2 = right_array[ right_section-1 ];

	    int height = triVerts[v2+1] - triVerts[v1+1];
	    if(height == 0)
	        return 0;

	    // Calculate the deltas along this section

	    delta_right_x = ((triVerts[v2]- triVerts[v1]) << 16) / height;
	    right_x = triVerts[v1] << 16;

	    right_section_height = height;
	    return height;                  // return the height of this section
	}

	int leftSection(){
		int v1 = left_array[ left_section ];
	    int v2 = left_array[ left_section-1 ];

	    int height = triVerts[v2+1] - triVerts[v1+1];
	    if(height == 0)
	        return 0;

	    // Calculate the deltas along this section

	    delta_left_x = ((triVerts[v2] - triVerts[v1]) << 16) / height;
	    left_x = triVerts[v1] << 16;
	    delta_left_u = ((triVerts[v2+2] - triVerts[v1+2]) << 10) / height;
	    left_u = triVerts[v1+2] << 10;
	    delta_left_v = ((triVerts[v2+3] - triVerts[v1+3]) << 10) / height;
	    left_v = triVerts[v1+3] << 10;

	    left_section_height = height;
	    return height;                  // return the height of this section
	}

	void drawTexturedTriangle(Triangle triangle){
		triVerts=triangle.vertexes;
		int color=triangle.color;
		int[] texture=triangle.texture;

		int v1=0;
		int v2=4;
		int v3=8;

	    // Sort the triangle so that v1 points to the topmost, v2 to the
	    // middle and v3 to the bottom vertex.
	    if(triVerts[v1+1] > triVerts[v2+1]) { int t=v1;v1=v2;v2=t; }
	    if(triVerts[v1+1] > triVerts[v3+1]) { int t=v1;v1=v3;v3=t; }
	    if(triVerts[v2+1] > triVerts[v3+1]) { int t=v2;v2=v3;v3=t; }

	    // We start out by calculating the length of the longest scanline.
	    int height = triVerts[v3+1] - triVerts[v1+1];
	    if(height == 0)
	        return;
	    int temp = ((triVerts[v2+1] - triVerts[v1+1]) << 16) / height;
	    int longest = temp * (triVerts[v3] - triVerts[v1]) + ((triVerts[v1] - triVerts[v2]) << 16);
	    if(longest == 0)
	        return;

		// Now that we have the length of the longest scanline we can use that 
	    // to tell us which is left and which is the right side of the triangle.

	    if(longest < 0){
	        // If longest is neg. we have the middle vertex on the right side.
	        // Store the pointers for the right and left edge of the triangle.
	        right_array[0] = v3;
	        right_array[1] = v2;
	        right_array[2] = v1;
	        right_section  = 2;
	        left_array[0]  = v3;
	        left_array[1]  = v1;
	        left_section   = 1;

	        // Calculate initial left and right parameters
	        if(leftSection() <= 0)
	            return;
	        if(rightSection() <= 0){
	            // The first right section had zero height. Use the next section. 
	            right_section--;
	            if(rightSection() <= 0)
	                return;
	        }

	        // Ugly compensation so that the dudx,dvdx divides won't overflow
	        // if the longest scanline is very short.
	        if(longest > -0x1000)
	            longest = -0x1000;     
	    }
	    else{
	        // If longest is pos. we have the middle vertex on the left side.
	        // Store the pointers for the left and right edge of the triangle.
	        left_array[0]  = v3;
	        left_array[1]  = v2;
	        left_array[2]  = v1;
	        left_section   = 2;
	        right_array[0] = v3;
	        right_array[1] = v1;
	        right_section  = 1;

	        // Calculate initial right and left parameters
	        if(rightSection() <= 0)
	            return;
	        if(leftSection() <= 0){
	            // The first left section had zero height. Use the next section.
	            left_section--;
	            if(leftSection() <= 0)
	                return;
	        }

	        // Ugly compensation so that the dudx,dvdx divides won't overflow
	        // if the longest scanline is very short.
	        if(longest < 0x1000)
	            longest = 0x1000;     
	    }

	    // Now we calculate the constant deltas for u and v (dudx, dvdx)
		int tv31=(triVerts[v3+2] - triVerts[v1+2])<<10;int tv12=(triVerts [v1+2] - triVerts[v2+2])<<10;
		int dudx=
			//Math.div(Math.mul(temp,(triVerts[v3+2] - triVerts[v1+2])<<10)+((triVerts [v1+2] - triVerts[v2+2])<<10),longest)
			//#exec fpcomp.bat "(temp*tv31+tv12)/longest"
		;
		tv31=(triVerts[v3+3] - triVerts[v1+3])<<10;tv12=(triVerts [v1+3] - triVerts[v2+3])<<10;
		int dvdx=
			//Math.div(Math.mul(temp,(triVerts[v3+3] - triVerts[v1+3])<<10)+((triVerts [v1+3] - triVerts[v2+3])<<10),longest)
			//#exec fpcomp.bat "(temp*tv31+tv12)/longest"
		;
		int h=triVerts[v1+1];

	    // If you are using a table lookup inner loop you should setup the
	    // lookup table here.

	    // Here starts the outer loop (for each scanline)
		if(color==0xFFFFFFFF){
		    while(true){
		        int x1 = left_x >> 16;
				int x2 = right_x >> 16;

				if(x1<mViewportWidth && x2>=0 && h<mViewportHeight && h>=0){
					if(x2>=mViewportWidth){
						x2=mViewportWidth-1;
					}

		            int u  = left_u >> 8;
		            int v  = left_v >> 8;
		            int du = dudx   >> 8;
		            int dv = dvdx   >> 8;

					if(x1<0){
						u+=du*(-x1);
				        v+=dv*(-x1);
						x1=0;
					}

					int width=x2-x1;
					int x=x1;
					int w=width;

					if((w&1)==0){
						while((w-=2)>=0){
							mBuffer[x++] = texture[ (v & 0xff00) + ((u & 0xff00) >> 8) ];
							u += du;
							v += dv;
							mBuffer[x++] = texture[ (v & 0xff00) + ((u & 0xff00) >> 8) ];
							u += du;
							v += dv;
						}
					}						
					else{
						w--;
						while((w-=2)>=0){
							mBuffer[x++] = texture[ (v & 0xff00) + ((u & 0xff00) >> 8) ];
							u += du;
							v += dv;
							mBuffer[x++] = texture[ (v & 0xff00) + ((u & 0xff00) >> 8) ];
							u += du;
							v += dv;
						}
						mBuffer[x++] = texture[ (v & 0xff00) + ((u & 0xff00) >> 8) ];
						u += du;
						v += dv;
					}

					//#ifdef TOADLET_PLATFORM_BLACKBERRY
						mGraphics.drawRGB(mBuffer,x1,width,x1,h,width,1);
					//#else
						mGraphics.drawRGB(mBuffer,x1,width,x1,h,width,1,false);
					//#endif
				}

				h++;

		        // Interpolate along the left edge of the triangle
		        if(--left_section_height <= 0){  // At the bottom of this section?
		            if(--left_section <= 0)     // All sections done?
		                return;
		            if(leftSection() <= 0)      // Nope, do the last section
		                return;
		        }
		        else{
		            left_x += delta_left_x;
		            left_u += delta_left_u;
		            left_v += delta_left_v;
		        }

		        // Interpolate along the right edge of the triangle
		        if(--right_section_height <= 0){ // At the bottom of this section?
		            if(--right_section <= 0)    // All sections done?
		                return;
		            if(rightSection() <= 0)     // Nope, do the last section
		                return;
		        }
		        else{
		            right_x += delta_right_x;
		        }
			}
		}
		else{
		    while(true){
		        int x1 = left_x >> 16;
				int x2 = right_x >> 16;

				if(x1<mViewportWidth && x2>=0 && h<mViewportHeight && h>=0){
					if(x2>=mViewportWidth){
						x2=mViewportWidth-1;
					}

		            int u  = left_u >> 8;
		            int v  = left_v >> 8;
		            int du = dudx   >> 8;
		            int dv = dvdx   >> 8;

					if(x1<0){
						u+=du*(-x1);
				        v+=dv*(-x1);
						x1=0;
					}

					int width=x2-x1;
					int x=x1;
					int w=width;

					if((w&1)==0){
						while((w-=2)>=0){
							mBuffer[x++] = texture[ (v & 0xff00) + ((u & 0xff00) >> 8) ] & color;
							u += du;
							v += dv;
							mBuffer[x++] = texture[ (v & 0xff00) + ((u & 0xff00) >> 8) ] & color;
							u += du;
							v += dv;
						}
					}						
					else{
						w--;
						while((w-=2)>=0){
							mBuffer[x++] = texture[ (v & 0xff00) + ((u & 0xff00) >> 8) ] & color;
							u += du;
							v += dv;
							mBuffer[x++] = texture[ (v & 0xff00) + ((u & 0xff00) >> 8) ] & color;
							u += du;
							v += dv;
						}
						mBuffer[x++] = texture[ (v & 0xff00) + ((u & 0xff00) >> 8) ] & color;
						u += du;
						v += dv;
					}

					//#ifdef TOADLET_PLATFORM_BLACKBERRY
						mGraphics.drawRGB(mBuffer,x1,width,x1,h,width,1);
					//#else
						mGraphics.drawRGB(mBuffer,x1,width,x1,h,width,1,false);
					//#endif
				}

				h++;

		        // Interpolate along the left edge of the triangle
		        if(--left_section_height <= 0){  // At the bottom of this section?
		            if(--left_section <= 0)     // All sections done?
		                return;
		            if(leftSection() <= 0)      // Nope, do the last section
		                return;
		        }
		        else{
		            left_x += delta_left_x;
		            left_u += delta_left_u;
		            left_v += delta_left_v;
		        }

		        // Interpolate along the right edge of the triangle
		        if(--right_section_height <= 0){ // At the bottom of this section?
		            if(--right_section <= 0)    // All sections done?
		                return;
		            if(rightSection() <= 0)     // Nope, do the last section
		                return;
		        }
		        else{
		            right_x += delta_right_x;
		        }
		    }
		}
	}

	private void resizeVertexCache(int size){
		mVertexViewCache=new int[size*3];
		mVertexScreenCache=new int[size*2];
		mVertexViewFrames=new short[size];
		mVertexScreenFrames=new short[size];
	}

	private void resizeTriangleCache(int size){
		mTriangleCache=new Triangle[size];
		int i;
		for(i=0;i<size;++i){
			mTriangleCache[i]=new Triangle();
		}
	}
	
	boolean mLighting;
	Vector3 mLightDirection=new Vector3();
	int mAmbient;
	int mFaceCullEpsilon;

	int mViewportWidth,mViewportHeight;
	int mFixedViewportWidth,mFixedViewportHeight;
	int mHalfFixedViewportWidth,mHalfFixedViewportHeight;
	int mInvHX,mInvHY;
	int mNear,mFar;
	int mDX,mDY;

	short mRenderCount;
	short[] mVertexViewFrames;
	short[] mVertexScreenFrames;
	int[] mVertexViewCache;
	int[] mVertexScreenCache;
	Triangle[] mTriangleCache;
	int mTriangleCacheCount;
	Triangle[] mOrderingTable;
	int[] mTransformCache=new int[12];
	int mColor;
	int[] mBuffer;

	Graphics mGraphics;

	//#ifdef TOADLET_USE_STATISTICS
		int vertexViewTransformed;
		int vertexViewTransformCached;
		int vertexScreenTransformed;
		int vertexScreenTransformCached;
		int triangleIncoming;
		int triangleFaceCulled;
		int triangleNearCulled;
		int triangleFarCulled;
		int triangleTrimmed;
		int triangleSplit;
		int triangleScreenCulled;
		int triangleTextureRendered;
		int triangleColorRendered;
	//#endif
}
