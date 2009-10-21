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

import flash.utils.*;
import com.lightningtoads.toadlet.egg.mathfixed.*;
import com.lightningtoads.toadlet.egg.mathfixed.tMath;
import com.lightningtoads.toadlet.tadpole.sg.*;

/// @todo:  Add textures and sprite support
public final class Engine{
	public static var TOADLET_TADPOLE_VERSION_MAJOR:int=1;
	public static var TOADLET_TADPOLE_VERSION_MINOR:int=0;
	public static var TOADLET_TADPOLE_VERSION_BUILD:int=0;
	public static var TOADLET_TADPOLE_VERSION_REVISION:int=0;
	public static var TOADLET_TADPOLE_VERSION_STRING:String=
		TOADLET_TADPOLE_VERSION_MAJOR+"."+
		TOADLET_TADPOLE_VERSION_MINOR+"."+
		TOADLET_TADPOLE_VERSION_BUILD;

	public function Engine(width:int,height:int){
		mWidth=width;
		mHeight=height;

		mRenderer=new Renderer(width,height);
	}

	public function getVersion():String{
		return TOADLET_TADPOLE_VERSION_STRING;
	}

	public function setScreenSize(width:int,height:int):void{
		mWidth=width;
		mHeight=height;
	}

	public function getScreenWidth():int{
		return mWidth;
	}

	public function getScreenHeight():int{
		return mHeight;
	}

	public function setRenderShiftBits(b:int):void{
		mRenderShiftBits=b;
	}

	public function getRenderShiftBits():int{
		return mRenderShiftBits;
	}

	public function setSceneManager(manager:SceneManager):void{
		mSceneManager=manager;
	}

	public function getSceneManager():SceneManager{
		return mSceneManager;
	}

	public function getMesh(name:String):Mesh{
		return new Mesh();
	}

	public function getSprite(name:String):Sprite{
		return new Sprite(1,1,0,1);
	}

	// Blocks for the mmsh format
	internal static var MESH_BLOCK:int=1;
	internal static var SKELETON_BLOCK:int=2;
	internal static var ANIMATION_BLOCK:int=3;

	// VertexTypes for the mmsh format
	internal static var VT_POSITION:int=1<<0;
	internal static var VT_NORMAL:int=1<<1;
	internal static var VT_COLOR:int=1<<2;
	internal static var VT_BONE:int=1<<3;
	internal static var VT_TEXCOORD1:int=1<<8;

	public function loadMicroMeshFromDataInput(din:IDataInput):Mesh{
		var signature:String=din.readMultiByte(4,"utf-8");
		if(signature!="TMMH"){
			throw new Error("Not of TMMH format");
		}

		var version:int=0;
		version=din.readByte();
		if(version!=3){
			throw new Error("Not TMMH version 3");
		}

		var bits:int=mRenderShiftBits;

		var mesh:Mesh=new Mesh();

		var i:int,j:int;

		var i8:int=0;
		var i16:int=0;

		var bytes:int=0;
		var bias:Array=new Array(4);
		var scale:Number;

		var translateBytes:int;
		var translateBias:Array=new Array(4);
		var translateScale:Number;

		while(din.bytesAvailable>0){
			var block:int=din.readByte();
			if(block==MESH_BLOCK){
				mesh.worldScale=din.readInt();

				var numVertexes:int=din.readShort();
				var vertexType:int=din.readShort();

				mesh.vertexData=new VertexData();
				if((vertexType&VT_POSITION)>0){
					mesh.vertexData.positions=new Array(numVertexes*3);
				}
				if((vertexType&VT_NORMAL)>0){
					mesh.vertexData.normals=new Array(numVertexes*3);
				}
				if((vertexType&VT_COLOR)>0){
					mesh.vertexData.colors=new Array(numVertexes);
				}
				if((vertexType&VT_TEXCOORD1)>0){
					mesh.vertexData.texCoords=new Array(numVertexes*2);
				}

				if((vertexType&VT_BONE)>0){
					mesh.boneControllingVertexes=new Array(numVertexes);
				}

				if((vertexType&VT_POSITION)>0){
					bytes=din.readByte();

					bias[0]=tMath.fixedToFloat(din.readInt());
					bias[1]=tMath.fixedToFloat(din.readInt());
					bias[2]=tMath.fixedToFloat(din.readInt());

					scale=tMath.fixedToFloat(din.readInt());

					if(bytes==1){
						for(i=0;i<numVertexes;++i){
							i8=din.readByte();
							mesh.vertexData.positions[i*3+0]=tMath.floatToFixed(i8*scale+bias[0])>>bits;
							i8=din.readByte();
							mesh.vertexData.positions[i*3+1]=tMath.floatToFixed(i8*scale+bias[1])>>bits;
							i8=din.readByte();
							mesh.vertexData.positions[i*3+2]=tMath.floatToFixed(i8*scale+bias[2])>>bits;
						}
					}
					else if(bytes==2){
						for(i=0;i<numVertexes;++i){
							i16=din.readShort();
							mesh.vertexData.positions[i*3+0]=tMath.floatToFixed(i16*scale+bias[0])>>bits;
							i16=din.readShort();
							mesh.vertexData.positions[i*3+1]=tMath.floatToFixed(i16*scale+bias[1])>>bits;
							i16=din.readShort();
							mesh.vertexData.positions[i*3+2]=tMath.floatToFixed(i16*scale+bias[2])>>bits;
						}
					}
					else{
						throw new Error("Invalid byte count");
					}
				}

				if((vertexType&VT_NORMAL)>0){
					var normal:Vector3=new Vector3();

					for(i=0;i<numVertexes;i++){
						i8=din.readByte();
						normal.x=i8<<9;
						i8=din.readByte();
						normal.y=i8<<9;
						i8=din.readByte();
						normal.z=i8<<9;

						tMath.normalizeCarefullyV(normal,0);

						mesh.vertexData.normals[i*3+0]=normal.x;
						mesh.vertexData.normals[i*3+1]=normal.y;
						mesh.vertexData.normals[i*3+2]=normal.z;
					}
				}

				if((vertexType&VT_COLOR)>0){
					for(i=0;i<numVertexes;i++){
						var r:int=din.readByte();
						var g:int=din.readByte();
						var b:int=din.readByte();

						mesh.vertexData.colors[i]=0xFF000000 | (b<<16) | (g<<8) | (r<<0);
					}
				}

				if((vertexType&VT_BONE)>0){
					for(i=0;i<numVertexes;i++){
						mesh.boneControllingVertexes[i]=din.readByte();
					}
				}

				if((vertexType&VT_TEXCOORD1)>0){
					bytes=din.readByte();

					bias[0]=tMath.fixedToFloat(din.readInt());
					bias[1]=tMath.fixedToFloat(din.readInt());

					scale=din.readInt();

					if(bytes==1){
						for(i=0;i<numVertexes;++i){
							i8=din.readByte();
							mesh.vertexData.texCoords[i*2+0]=tMath.floatToFixed(i8*scale+bias[0]);
							i8=din.readByte();
							mesh.vertexData.texCoords[i*2+1]=tMath.floatToFixed(i8*scale+bias[1]);
						}
					}
					else if(bytes==2){
						for(i=0;i<numVertexes;++i){
							i16=din.readShort();
							mesh.vertexData.texCoords[i*2+0]=tMath.floatToFixed(i16*scale+bias[0]);
							i16=din.readShort();
							mesh.vertexData.texCoords[i*2+1]=tMath.floatToFixed(i16*scale+bias[1]);
						}
					}
					else{
						throw new Error("Invalid byte count");
					}
				}

				var numSubMeshes:int=din.readByte();
				mesh.subMeshes=new Array(numSubMeshes);

				var triIndexData:Array=new Array(numSubMeshes);
				var totalIndexes:int=0;

				var textureCount:int=0;
				for(i=0;i<numSubMeshes;++i){
					var sub:MeshSubMesh=new MeshSubMesh();
					mesh.subMeshes[i]=sub;

					// Material
					{
						sub.faceCulling=din.readByte();
						sub.hasTexture=din.readByte()>0;
						if(sub.hasTexture){
							sub.textureIndex=textureCount++;
						}
						sub.lightingEnabled=din.readByte()>0;
						if(sub.lightingEnabled){
							var ambientColor:int=din.readInt();
							var diffuseColor:int=din.readInt();
							sub.color=diffuseColor;
							var specularColor:int=din.readInt();
							var shininess:int=din.readInt();
							var emissiveColor:int=din.readInt();
						}
					}

					var numIndexes:int=din.readShort();
					var indexes:Array=new Array(numIndexes);

					for(j=0;j<numIndexes;++j){
						indexes[j]=din.readShort();
					}

					var numStrips:int=din.readShort();

					var stripLengths:Array=new Array(numStrips);

					var newNumIndexes:int=0;
					for(j=0;j<numStrips;++j){
						stripLengths[j]=din.readShort();
						newNumIndexes+=(stripLengths[j]-2)*3;
					}

					var indexBufferSize:int=newNumIndexes;
					triIndexData[i]=new Array(indexBufferSize);
					totalIndexes+=indexBufferSize;
					{
						var ipo:int=0;
						var ipn:int=0;
						for(j=0;j<numStrips;++j){
							var k:int;
							for(k=0;k<stripLengths[j]-2;++k){
								if(k%2==0){
									triIndexData[i][ipn++]=indexes[ipo+k+0];
									triIndexData[i][ipn++]=indexes[ipo+k+1];
									triIndexData[i][ipn++]=indexes[ipo+k+2];
								}
								else{
									triIndexData[i][ipn++]=indexes[ipo+k+2];
									triIndexData[i][ipn++]=indexes[ipo+k+1];
									triIndexData[i][ipn++]=indexes[ipo+k+0];
								}
							}
							ipo+=stripLengths[j];
						}
					}
				}

				mesh.triIndexData=new Array(totalIndexes);
				mesh.triSubMesh=new Array(totalIndexes/3);

				totalIndexes=0;
				var totalTris:int=0;
				for(i=0;i<numSubMeshes;++i){
					for(j=0;j<triIndexData[i].length/3;++j){
						mesh.triIndexData[totalIndexes++]=triIndexData[i][j*3];
						mesh.triIndexData[totalIndexes++]=triIndexData[i][j*3+1];
						mesh.triIndexData[totalIndexes++]=triIndexData[i][j*3+2];
						mesh.triSubMesh[totalTris++]=i;
					}
				}
			}
			else if(block==SKELETON_BLOCK){
				var numBones:int=din.readByte();

				mesh.bones=new Array(numBones);

				translateBytes=din.readByte();

				translateBias[0]=tMath.fixedToFloat(din.readInt());
				translateBias[1]=tMath.fixedToFloat(din.readInt());
				translateBias[2]=tMath.fixedToFloat(din.readInt());

				translateScale=tMath.fixedToFloat(din.readInt());

				for(i=0;i<numBones;++i){
					var bone:MeshBone=new MeshBone();
					mesh.bones[i]=bone;
					bone.index=i;

					if(translateBytes==1){
						i8=din.readByte();
						bone.translate.x=tMath.floatToFixed(i8*translateScale+translateBias[0])>>bits;
						i8=din.readByte();
						bone.translate.y=tMath.floatToFixed(i8*translateScale+translateBias[1])>>bits;
						i8=din.readByte();
						bone.translate.z=tMath.floatToFixed(i8*translateScale+translateBias[2])>>bits;
					}
					else{
						i16=din.readShort();
						bone.translate.x=tMath.floatToFixed(i16*translateScale+translateBias[0])>>bits;
						i16=din.readShort();
						bone.translate.y=tMath.floatToFixed(i16*translateScale+translateBias[1])>>bits;
						i16=din.readShort();
						bone.translate.z=tMath.floatToFixed(i16*translateScale+translateBias[2])>>bits;
					}

					i8=din.readByte();
					bone.rotate.x=i8<<9;
					i8=din.readByte();
					bone.rotate.y=i8<<9;
					i8=din.readByte();
					bone.rotate.z=i8<<9;
					i8=din.readByte();
					bone.rotate.w=i8<<9;
					tMath.normalizeCarefullyQ(bone.rotate,0);

					if(translateBytes==1){
						i8=din.readByte();
						bone.worldToBoneTranslate.x=tMath.floatToFixed(i8*translateScale+translateBias[0])>>bits;
						i8=din.readByte();
						bone.worldToBoneTranslate.y=tMath.floatToFixed(i8*translateScale+translateBias[1])>>bits;
						i8=din.readByte();
						bone.worldToBoneTranslate.z=tMath.floatToFixed(i8*translateScale+translateBias[2])>>bits;
					}
					else{
						i16=din.readShort();
						bone.worldToBoneTranslate.x=tMath.floatToFixed(i16*translateScale+translateBias[0])>>bits;
						i16=din.readShort();
						bone.worldToBoneTranslate.y=tMath.floatToFixed(i16*translateScale+translateBias[1])>>bits;
						i16=din.readShort();
						bone.worldToBoneTranslate.z=tMath.floatToFixed(i16*translateScale+translateBias[2])>>bits;
					}

					i8=din.readByte();
					bone.worldToBoneRotate.x=i8<<9;
					i8=din.readByte();
					bone.worldToBoneRotate.y=i8<<9;
					i8=din.readByte();
					bone.worldToBoneRotate.z=i8<<9;
					i8=din.readByte();
					bone.worldToBoneRotate.w=i8<<9;
					tMath.normalizeCarefullyQ(bone.worldToBoneRotate,0);

					bone.parent=din.readByte();

					if(bone.parent!=-1){
						if(mesh.bones[bone.parent].children==null){
							mesh.bones[bone.parent].children=new Array();
						}
						mesh.bones[bone.parent].children[mesh.bones[bone.parent].children.length]=i;
					}
				}
			}
			else if(block==ANIMATION_BLOCK){
				var animation:Animation=new Animation();

				animation.length=din.readInt();

				translateBytes=din.readByte();

				translateBias[0]=tMath.fixedToFloat(din.readInt());
				translateBias[1]=tMath.fixedToFloat(din.readInt());
				translateBias[2]=tMath.fixedToFloat(din.readInt());

				translateScale=tMath.fixedToFloat(din.readInt());

				var numTracks:int=din.readByte();
				animation.tracks=new Array(numTracks);

				for(i=0;i<numTracks;++i){
					var track:AnimationTrack=new AnimationTrack(animation);
					animation.tracks[i]=track;

					track.bone=din.readByte();

					var numKeyFrames:int=din.readShort();
					track.keyFrames=new Array(numKeyFrames);

					for(j=0;j<numKeyFrames;++j){
						var keyFrame:AnimationKeyFrame=new AnimationKeyFrame();
						track.keyFrames[j]=keyFrame;

						keyFrame.time=din.readInt();

						if(translateBytes==1){
							i8=din.readByte();
							keyFrame.translate.x=tMath.floatToFixed(i8*translateScale+translateBias[0])>>bits;
							i8=din.readByte();
							keyFrame.translate.y=tMath.floatToFixed(i8*translateScale+translateBias[1])>>bits;
							i8=din.readByte();
							keyFrame.translate.z=tMath.floatToFixed(i8*translateScale+translateBias[2])>>bits;
						}
						else{
							i16=din.readShort();
							keyFrame.translate.x=tMath.floatToFixed(i16*translateScale+translateBias[0])>>bits;
							i16=din.readShort();
							keyFrame.translate.y=tMath.floatToFixed(i16*translateScale+translateBias[1])>>bits;
							i16=din.readShort();
							keyFrame.translate.z=tMath.floatToFixed(i16*translateScale+translateBias[2])>>bits;
						}

						i8=din.readByte();
						keyFrame.rotate.x=i8<<9;
						i8=din.readByte();
						keyFrame.rotate.y=i8<<9;
						i8=din.readByte();
						keyFrame.rotate.z=i8<<9;
						i8=din.readByte();
						keyFrame.rotate.w=i8<<9;
						tMath.normalizeCarefullyQ(keyFrame.rotate,0);
					}
				}

				animation.compile();
				if(mesh.animations==null){
					mesh.animations=new Array();
				}
				mesh.animations[mesh.animations.length]=animation;
			}
			else{
				throw new Error("Unknown block type in mesh file");
			}
		}

		return mesh;
	}

	public function getRenderer():Renderer{
		return mRenderer;
	}

	protected var mSceneManager:SceneManager=null;
	public var mWidth:int=0;
	public var mHeight:int=0;
	public var mRenderShiftBits:int=0;
	protected var mRenderer:Renderer;
}

}
