/********** Copyright header - do not remove **********
 *
 * The Toadlet Engine 
 *
 * Copyright (C) 2005, Lightning Toads Productions, LLC
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

#include "M3GConverter.h"
#include "../shared/Adler32.h"
#include "../shared/VertexCompression.h"
#include <iostream>
#include <sstream>

class M3GProxy{
public:
	static void writeBool(DataStream *out,bool b){
		out->writeBool(b);
	}

	static void writeInt8(DataStream *out,int8 i){
		out->writeInt8(i);
	}

	static void writeUInt8(DataStream *out,uint8 i){
		out->writeUInt8(i);
	}

	static void writeInt16(DataStream *out,int16 i){
		out->writeLInt16(i);
	}

	static void writeUInt16(DataStream *out,uint16 i){
		out->writeLUInt16(i);
	}

	static void writeInt32(DataStream *out,int32 i){
		out->writeLInt32(i);
	}

	static void writeUInt32(DataStream *out,uint32 i){
		out->writeLUInt32(i);
	}

	static void writeFloat(DataStream *out,float f){
		out->writeLFloat(f);
	}

	static void writeVector3(DataStream *out,const Vector3 &v){
		int i;
		for(i=0;i<3;++i){
			out->writeLFloat(v[i]);
		}
	}

	static void writeMatrix4x4(DataStream *out,const Matrix4x4 &m){
		int i;
		for(i=0;i<16;++i){
			out->writeLFloat(m.data[i]);
		}
	}

	static void writeString(DataStream *out,const String &s){
		out->writeNullTerminatedString(s);
	}

	static void writeObjectIndex(DataStream *out,M3GProxy *object){
		if(object==NULL){
			writeUInt32(out,0);
		}
		else{
			writeUInt32(out,object->id);
		}
	}

	static void writeColorRGB(DataStream *out,int color){
		writeUInt8(out,(color&0x00FF0000)>>16);
		writeUInt8(out,(color&0x0000FF00)>>8);
		writeUInt8(out,(color&0x000000FF)>>0);
	}

	static void writeColorRGBA(DataStream *out,int color){
		writeUInt8(out,(color&0x00FF0000)>>16);
		writeUInt8(out,(color&0x0000FF00)>>8);
		writeUInt8(out,(color&0x000000FF)>>0);
		writeUInt8(out,(color&0xFF000000)>>24);
	}
	
	M3GProxy(){
		id=0;
		objectType=0;
	}

	virtual void getData(DataStream *out)=0;
	virtual int getDataLength()=0;

	String name;
	uint32 id;
	uint8 objectType;
};

class M3GHeaderObject:public M3GProxy{
public:
	M3GHeaderObject():M3GProxy(){
		objectType=0;
		id=1;
		versionNumber[0]=1;
		versionNumber[1]=0;
		hasExternalReferences=false;
		totalFileSize=0;
		approximateContentSize=0;
		authoringField="(C) Lightning Toads Productions, LLC";
	}

	virtual void getData(DataStream *out){
		writeUInt8(out,versionNumber[0]);
		writeUInt8(out,versionNumber[1]);
		writeBool(out,hasExternalReferences);
		writeUInt32(out,totalFileSize);
		writeUInt32(out,approximateContentSize);
		writeString(out,authoringField);
	}

	virtual int getDataLength(){
		return 1+1+1+4+4+authoringField.length()+1;
	}

	uint8 versionNumber[2];
	bool hasExternalReferences;
	uint32 totalFileSize;
	uint32 approximateContentSize;
	String authoringField;
};

class M3GExternalReference:public M3GProxy{
public:
	M3GExternalReference():M3GProxy(){
		objectType=0xFF;
	}

	virtual void getData(DataStream *out){
		writeString(out,uri);
	}

	virtual int getDataLength(){
		return uri.length()+1;
	}

	String uri;
};

class M3GObject3D:public M3GProxy{
public:
	class UserParameter{
	public:
		UserParameter(){
			id=0;
		}

		uint32 id;
		Collection<uint8> value;
	};

	M3GObject3D():M3GProxy(){
		userID=0;
	}

	virtual void getData(DataStream *out){
		writeUInt32(out,userID);
		writeUInt32(out,animationTracks.size());
		int i;
		for(i=0;i<animationTracks.size();++i){
			writeObjectIndex(out,animationTracks[i]);
		}
		writeUInt32(out,userParameters.size());
		for(i=0;i<userParameters.size();++i){
			writeUInt32(out,userParameters[i].id);
			writeUInt32(out,userParameters[i].value.size());
			int j;
			for(j=0;j<userParameters[i].value.size();++j){
				writeUInt8(out,userParameters[i].value[j]);
			}
		}
	}

	virtual int getDataLength(){
		int value=4 + 4  + animationTracks.size()*4 + 4;
		int i;
		for(i=0;i<userParameters.size();++i){
			value+=4 + 4 + userParameters[i].value.size()*1;
		}
		return value;
	}

	virtual void gatherObjects(Collection<M3GProxy*> &objects){
		int i;
		for(i=0;i<animationTracks.size();++i){
			if(animationTracks[i]!=NULL){
				animationTracks[i]->gatherObjects(objects);
			}
		}
		objects.add(this);
	}

	uint32 userID;
	Collection<M3GObject3D*> animationTracks;
	Collection<UserParameter> userParameters;
};

class M3GTransformable:public M3GObject3D{
public:
	M3GTransformable():M3GObject3D(){
		hasComponentTransform=false;
		translation=Math::ZERO_VECTOR3;
		scale=Math::ONE_VECTOR3;
		orientationAngle=0;
		orientationAxis=Math::Z_UNIT_VECTOR3;
		hasGeneralTransform=false;
		transform=Math::IDENTITY_MATRIX4X4;
	}

	virtual void getData(DataStream *out){
		M3GObject3D::getData(out);
		writeBool(out,hasComponentTransform);
		if(hasComponentTransform){
			writeVector3(out,translation);
			writeVector3(out,scale);
			writeFloat(out,orientationAngle);
			writeVector3(out,orientationAxis);
		}
		writeBool(out,hasGeneralTransform);
		if(hasGeneralTransform){
			writeMatrix4x4(out,transform);
		}
	}

	virtual int getDataLength(){
		int value=M3GObject3D::getDataLength();
		value+=1;
		if(hasComponentTransform){
			value+=4*3 + 4*3 + 4 + 4*3;
		}
		value+=1;
		if(hasGeneralTransform){
			value+=16*4;
		}
		return value;
	}

	virtual void gatherObjects(Collection<M3GProxy*> &objects){
		M3GObject3D::gatherObjects(objects);
	}

	bool hasComponentTransform;
	Vector3 translation;
	Vector3 scale;
	float orientationAngle;
	Vector3 orientationAxis;
	bool hasGeneralTransform;
	Matrix4x4 transform;
};

class M3GNode:public M3GTransformable{
public:
	M3GNode():M3GTransformable(){
		enableRendering=true;
		enablePicking=true;
		alphaFactor=255;
		scope=4294967295UL; // -1 by default
		hasAlignment=false;
		zTarget=0;
		yTarget=0;
		zReference=NULL;
		yReference=NULL;
	};

	virtual void getData(DataStream *out){
		M3GTransformable::getData(out);
		writeBool(out,enableRendering);
		writeBool(out,enablePicking);
		writeUInt8(out,alphaFactor);
		writeUInt32(out,scope);
		writeBool(out,hasAlignment);
		if(hasAlignment){
			writeUInt8(out,zTarget);
			writeUInt8(out,yTarget);
			writeObjectIndex(out,zReference);
			writeObjectIndex(out,yReference);
		}
	}

	virtual int getDataLength(){
		int value=M3GTransformable::getDataLength();
		value+=1 + 1 + 1 + 4 + 1;
		if(hasAlignment){
			value+=1 + 1 + 4 + 4;
		}
		return value;
	}

	virtual void gatherObjects(Collection<M3GProxy*> &objects){
		// No need to gather reference nodes, are in scene elsewhere
		M3GTransformable::gatherObjects(objects);
	}

	bool enableRendering;
	bool enablePicking;
	uint8 alphaFactor;
	uint32 scope;
	bool hasAlignment;
	uint8 zTarget;
	uint8 yTarget;
	M3GNode *zReference;
	M3GNode *yReference;
};

class M3GGroup:public M3GNode{
public:
	M3GGroup():M3GNode(){
		objectType=9;
	}

	virtual void getData(DataStream *out){
		M3GNode::getData(out);
		writeUInt32(out,children.size());
		int i;
		for(i=0;i<children.size();++i){
			writeObjectIndex(out,children[i]);
		}
	}

	virtual int getDataLength(){
		return M3GNode::getDataLength() + 4 + children.size()*4;
	}

	virtual void gatherObjects(Collection<M3GProxy*> &objects){
		int i;
		for(i=0;i<children.size();++i){
			if(children[i]!=NULL){
				children[i]->gatherObjects(objects);
			}
		}
		M3GNode::gatherObjects(objects);
	}

	Collection<M3GNode*> children;
};

class M3GCamera:public M3GNode{
public:
	enum{
		GENERIC=48,
		PARALLEL=49,
		PERSPECTIVE=50,
	};

	M3GCamera(){
		objectType=5;
		projectionType=PARALLEL;
		fovy=0.0f;
		aspectRatio=0.0f;
		nearDist=0.0f;
		farDist=0.0f;
	}

	virtual void getData(DataStream *out){
		M3GNode::getData(out);
		writeUInt8(out,projectionType);
		if(projectionType==GENERIC){
			writeMatrix4x4(out,projectionMatrix);
		}
		else{
			writeFloat(out,fovy);
			writeFloat(out,aspectRatio);
			writeFloat(out,nearDist);
			writeFloat(out,farDist);
		}
	}

	virtual int getDataLength(){
		int value=M3GNode::getDataLength();
		value+=1;
		if(projectionType==GENERIC){
			value+=16*4;
		}
		else{
			value+=4*4;
		}
		return value;
	}

	uint8 projectionType;
	Matrix4x4 projectionMatrix;
	float fovy;
	float aspectRatio;
	float nearDist;
	float farDist;
};

class M3GBackground:public M3GObject3D{
public:
	enum{
		BORDER=32,
		REPEAT=33,
	};

	M3GBackground():M3GObject3D(){
		objectType=4;
		backgroundColor=0x00000000;
		backgroundImage=NULL;
		backgroundImageModeX=BORDER;
		backgroundImageModeY=BORDER;
		cropX=0;
		cropY=0;
		cropWidth=0;
		cropHeight=0;
		depthClearEnabled=true;
		colorClearEnabled=true;
	}

	virtual void getData(DataStream *out){
		M3GObject3D::getData(out);
		writeColorRGBA(out,backgroundColor);
		writeObjectIndex(out,backgroundImage);
		writeUInt8(out,backgroundImageModeX);
		writeUInt8(out,backgroundImageModeY);
		writeInt32(out,cropX);
		writeInt32(out,cropY);
		writeInt32(out,cropWidth);
		writeInt32(out,cropHeight);
		writeBool(out,depthClearEnabled);
		writeBool(out,colorClearEnabled);
	}

	virtual int getDataLength(){
		return M3GObject3D::getDataLength() + 4 + 4 + 1 + 1 + 4 + 4 + 4 + 4 + 1 + 1;
	}

	uint32 backgroundColor;
	M3GObject3D *backgroundImage;
	uint8 backgroundImageModeX;
	uint8 backgroundImageModeY;
	int32 cropX;
	int32 cropY;
	int32 cropWidth;
	int32 cropHeight;
	bool depthClearEnabled;
	bool colorClearEnabled;
};

class M3GWorld:public M3GGroup{
public:
	M3GWorld():M3GGroup(){
		objectType=22;
		activeCamera=NULL;
		background=NULL;
	}

	virtual void getData(DataStream *out){
		M3GGroup::getData(out);
		writeObjectIndex(out,activeCamera);
		writeObjectIndex(out,background);
	}

	virtual int getDataLength(){
		return M3GGroup::getDataLength() + 4 + 4;
	}

	virtual void gatherObjects(Collection<M3GProxy*> &objects){
		// Don't gather activeCamera, it will be attached elsewhere
		if(background!=NULL){
			background->gatherObjects(objects);
		}
		M3GGroup::gatherObjects(objects);
	}

	M3GNode *activeCamera;
	M3GObject3D *background;
};

class M3GLight:public M3GNode{
public:
	enum{
		AMBIENT=128,
		DIRECTIONAL=129,
		OMNI=130,
		SPOT=131,
	};

	M3GLight():M3GNode(){
		objectType=12;
		attenuationConstant=1.0f;
		attenuationLinear=1.0f;
		attenuationQuadratic=1.0f;
		color=0x00FFFFFF;
		mode=AMBIENT;
		intensity=1.0f;
		spotAngle=0.0f;
		spotExponent=0.0f;
	}

	virtual void getData(DataStream *out){
		M3GNode::getData(out);
		writeFloat(out,attenuationConstant);
		writeFloat(out,attenuationLinear);
		writeFloat(out,attenuationQuadratic);
		writeColorRGB(out,color);
		writeUInt8(out,mode);
		writeFloat(out,intensity);
		writeFloat(out,spotAngle);
		writeFloat(out,spotExponent);
	}

	virtual int getDataLength(){
		return M3GNode::getDataLength() + 4 + 4 + 4 + 3 + 1 + 4 + 4 + 4;
	}

	float attenuationConstant;
	float attenuationLinear;
	float attenuationQuadratic;
	uint32 color;
	uint8 mode;
	float intensity;
	float spotAngle;
	float spotExponent;
};

class M3GVertexArray:public M3GObject3D{
public:
	enum{
		ENCODING_COMPONENT=0,
		ENCODING_COMPONENT_DELTA=1,
	};

	M3GVertexArray():M3GObject3D(){
		objectType=20;
		componentSize=1; // Number of bytes per component, must be [1,2]
		componentCount=2; // Number of components per vertex, must be [2,4]
		encoding=ENCODING_COMPONENT; // 0 for components, 1 for componentDeltas

		scale=1.0f;
	}

	virtual void getData(DataStream *out){
		M3GObject3D::getData(out);
		writeUInt8(out,componentSize);
		writeUInt8(out,componentCount);
		writeUInt8(out,encoding);
		if(componentSize==1){
			writeUInt16(out,byteComponents.size()/componentCount);
			int i;
			for(i=0;i<byteComponents.size();++i){
				writeInt8(out,byteComponents[i]);
			}
		}
		else if(componentSize==2){
			writeUInt16(out,shortComponents.size()/componentCount);
			int i;
			for(i=0;i<shortComponents.size();++i){
				writeInt16(out,shortComponents[i]);
			}
		}
		else{
			Error::unknown("Invalid component size");
		}
	}

	virtual int getDataLength(){
		int value=M3GObject3D::getDataLength();
		value+=1 + 1 + 1;
		if(componentSize==1){
			value+=2 + byteComponents.size()*1;
		}
		else if(componentSize==2){
			value+=2 + shortComponents.size()*2;
		}
		else{
			Error::unknown("Invalid component size");
		}
		return value;
	}

	uint8 componentSize;
	uint8 componentCount;
	uint8 encoding;
	Collection<int8> byteComponents;
	Collection<int16> shortComponents;

	Vector3 bias;
	float scale;
};

class M3GVertexBuffer:public M3GObject3D{
public:
	M3GVertexBuffer():M3GObject3D(){
		objectType=21;
		defaultColor=0xFFFFFFFF;
		positions=NULL;
		normals=NULL;
		colors=NULL;
	}

	virtual void getData(DataStream *out){
		M3GObject3D::getData(out);
		writeColorRGBA(out,defaultColor);
		writeObjectIndex(out,positions);
		writeVector3(out,positions!=NULL?positions->bias:Math::ZERO_VECTOR3);
		writeFloat(out,positions!=NULL?positions->scale:1.0f);
		writeObjectIndex(out,normals);
		writeObjectIndex(out,colors);
		writeUInt32(out,texCoordArrays.size());
		int i;
		for(i=0;i<texCoordArrays.size();++i){
			writeObjectIndex(out,texCoordArrays[i]);
			writeVector3(out,texCoordArrays[i]!=NULL?texCoordArrays[i]->bias:Math::ZERO_VECTOR3);
			writeFloat(out,texCoordArrays[i]!=NULL?texCoordArrays[i]->scale:1.0f);
		}
	}

	virtual int getDataLength(){
		int value=M3GObject3D::getDataLength();
		value+=4 + 4 + 4*3 + 4 + 4 + 4 + 4 + texCoordArrays.size()*(4 + 4*3 + 4);
		return  value;
	}

	virtual void gatherObjects(Collection<M3GProxy*> &objects){
		if(positions!=NULL){
			positions->gatherObjects(objects);
		}
		if(normals!=NULL){
			normals->gatherObjects(objects);
		}
		if(colors!=NULL){
			colors->gatherObjects(objects);
		}
		int i;
		for(i=0;i<texCoordArrays.size();++i){
			if(texCoordArrays[i]!=NULL){
				texCoordArrays[i]->gatherObjects(objects);
			}
		}
		M3GObject3D::gatherObjects(objects);
	}

	uint32 defaultColor;
	M3GVertexArray *positions;
	M3GVertexArray *normals;
	M3GVertexArray *colors;
	Collection<M3GVertexArray*> texCoordArrays;
};

class M3GIndexBuffer:public M3GObject3D{
public:
	M3GIndexBuffer():M3GObject3D(){}

	virtual void getData(DataStream *out){
		M3GObject3D::getData(out);
	}

	virtual int getDataLength(){
		return M3GObject3D::getDataLength();
	}
};

class M3GTriangleStripArray:public M3GIndexBuffer{
public:
	enum{
		IMPLICIT_UINT32=0,
		IMPLICIT_UINT8=1,
		IMPLICIT_UINT16=2,
		EXPLICIT_UINT32=128,
		EXPLICIT_UINT8=129,
		EXPLICIT_UINT16=130,
	};

	M3GTriangleStripArray():M3GIndexBuffer(){
		objectType=11;
		encoding=EXPLICIT_UINT16;
	}

	virtual void getData(DataStream *out){
		M3GIndexBuffer::getData(out);
		writeUInt8(out,encoding);
		int i;
		if(encoding==IMPLICIT_UINT32){
			writeUInt32(out,intIndexes[0]);
		}
		else if(encoding==IMPLICIT_UINT8){
			writeUInt8(out,byteIndexes[0]);
		}
		else if(encoding==IMPLICIT_UINT16){
			writeUInt16(out,shortIndexes[0]);
		}
		else if(encoding==EXPLICIT_UINT32){
			writeUInt32(out,intIndexes.size());
			for(i=0;i<intIndexes.size();++i){
				writeUInt32(out,intIndexes[i]);
			}
		}
		else if(encoding==EXPLICIT_UINT8){
			writeUInt32(out,byteIndexes.size());
			for(i=0;i<byteIndexes.size();++i){
				writeUInt8(out,byteIndexes[i]);
			}
		}
		else if(encoding==EXPLICIT_UINT16){
			writeUInt32(out,shortIndexes.size());
			for(i=0;i<shortIndexes.size();++i){
				writeUInt16(out,shortIndexes[i]);
			}
		}
		else{
			Error::unknown("Invalid encoding");
		}
		writeUInt32(out,stripLengths.size());
		for(i=0;i<stripLengths.size();++i){
			writeUInt32(out,stripLengths[i]);
		}
	}

	virtual int getDataLength(){
		int value=M3GIndexBuffer::getDataLength();
		value+=1;
		if(encoding==IMPLICIT_UINT32){
			value+=4;
		}
		else if(encoding==IMPLICIT_UINT8){
			value+=1;
		}
		else if(encoding==IMPLICIT_UINT16){
			value+=2;
		}
		else if(encoding==EXPLICIT_UINT32){
			value+=4 + intIndexes.size()*4;
		}
		else if(encoding==EXPLICIT_UINT8){
			value+=4 + byteIndexes.size()*1;
		}
		else if(encoding==EXPLICIT_UINT16){
			value+=4 + shortIndexes.size()*2;
		}
		value+=4 + stripLengths.size()*4;
		return value;
	}

	uint8 encoding;
	Collection<uint32> intIndexes;
	Collection<uint8> byteIndexes;
	Collection<uint16> shortIndexes;
	Collection<uint32> stripLengths;
};

class M3GCompositingMode:public M3GObject3D{
public:
	enum{
		ALPHA=64,
		ALPHA_ADD=65,
		MODULATE=66,
		MODULATE_X2=67,
		REPLACE=68,
	};

	M3GCompositingMode():M3GObject3D(){
		objectType=6;
		depthTestEnabled=true;
		depthWriteEnabled=true;
		colorWriteEnabled=true;
		alphaWriteEnabled=true;
		blending=REPLACE;
		alphaThreshold=0x00;
		depthOffsetFactor=0;
		depthOffsetUnits=0;
	}

	virtual void getData(DataStream *out){
		M3GObject3D::getData(out);
		writeBool(out,depthTestEnabled);
		writeBool(out,depthWriteEnabled);
		writeBool(out,colorWriteEnabled);
		writeBool(out,alphaWriteEnabled);
		writeUInt8(out,blending);
		writeUInt8(out,alphaThreshold);
		writeFloat(out,depthOffsetFactor);
		writeFloat(out,depthOffsetUnits);
	}

	virtual int getDataLength(){
		return M3GObject3D::getDataLength() + 1 + 1 + 1 + 1 + 1 + 1 + 4 + 4;
	}

	bool depthTestEnabled;
	bool depthWriteEnabled;
	bool colorWriteEnabled;
	bool alphaWriteEnabled;
	uint8 blending;
	uint8 alphaThreshold;
	float depthOffsetFactor;
	float depthOffsetUnits;
};

class M3GPolygonMode:public M3GObject3D{
public:
	enum{
		CULL_BACK=160,
		CULL_NONE=162,
		SHADE_FLAT=164,
		SHADE_SMOOTH=165,
		WINDING_CCW=168,
		WINDING_CW=169,
	};
    
	M3GPolygonMode():M3GObject3D(){
		objectType=8;
		culling=CULL_BACK;
		shading=SHADE_SMOOTH;
		winding=WINDING_CCW;
		twoSidedLightingEnabled=false;
		localCameraLightingEnabled=false;
		perspectiveCorrectionEnabled=true;
	}

	virtual void getData(DataStream *out){
		M3GObject3D::getData(out);
		writeUInt8(out,culling);
		writeUInt8(out,shading);
		writeUInt8(out,winding);
		writeUInt8(out,twoSidedLightingEnabled);
		writeUInt8(out,localCameraLightingEnabled);
		writeUInt8(out,perspectiveCorrectionEnabled);
	}

	virtual int getDataLength(){
		return M3GObject3D::getDataLength() + 1 + 1 + 1 + 1 + 1 + 1;
	}

	uint8 culling;
	uint8 shading;
	uint8 winding;
	uint8 twoSidedLightingEnabled;
	uint8 localCameraLightingEnabled;
	uint8 perspectiveCorrectionEnabled;
};        

class M3GMaterial:public M3GObject3D{
public:
	M3GMaterial():M3GObject3D(){
		objectType=13;
		ambientColor=0x007F7F7F;
		diffuseColor=0xFF7F7F7F;
		emissiveColor=0x00000000;
		specularColor=0x00000000;
		shininess=0;
		vertexColorTrackingEnabled=false;
	}

	void getData(DataStream *out){
		M3GObject3D::getData(out);
		writeColorRGB(out,ambientColor);
		writeColorRGBA(out,diffuseColor);
		writeColorRGB(out,emissiveColor);
		writeColorRGB(out,specularColor);
		writeFloat(out,shininess);
		writeBool(out,vertexColorTrackingEnabled);
	}

	int getDataLength(){
		return M3GObject3D::getDataLength() + 3 + 4 + 3 + 3 + 4 + 1;
	}

	uint32 ambientColor;
	uint32 diffuseColor;
	uint32 emissiveColor;
	uint32 specularColor;
	float shininess;
	bool vertexColorTrackingEnabled;
};

class M3GAppearance:public M3GObject3D{
public:
	M3GAppearance():M3GObject3D(){
		objectType=3;
		layer=0;
		compositingMode=NULL;
		fog=NULL;
		polygonMode=NULL;
		material=NULL;
	}

	virtual void getData(DataStream *out){
		M3GObject3D::getData(out);
		writeUInt8(out,layer);
		writeObjectIndex(out,compositingMode);
		writeObjectIndex(out,fog);
		writeObjectIndex(out,polygonMode);
		writeObjectIndex(out,material);
		writeUInt32(out,textures.size());
		int i;
		for(i=0;i<textures.size();++i){
			writeObjectIndex(out,textures[i]);
		}
	}

	virtual int getDataLength(){
		return M3GObject3D::getDataLength() + 1 + 4 + 4 + 4 + 4 + 4 + textures.size()*4;
	}

	virtual void gatherObjects(Collection<M3GProxy*> &objects){
		if(compositingMode!=NULL){
			compositingMode->gatherObjects(objects);
		}
		if(fog!=NULL){
			fog->gatherObjects(objects);
		}
		if(polygonMode!=NULL){
			polygonMode->gatherObjects(objects);
		}
		if(material!=NULL){
			material->gatherObjects(objects);
		}
		int i;
		for(i=0;i<textures.size();++i){
			if(textures[i]!=NULL){
				textures[i]->gatherObjects(objects);
			}
		}
		M3GObject3D::gatherObjects(objects);
	}

	uint8 layer;
	M3GCompositingMode *compositingMode;
	M3GObject3D *fog;
	M3GPolygonMode *polygonMode;
	M3GMaterial *material;
	Collection<M3GObject3D*> textures;
};

class M3GMesh:public M3GNode{
public:
	class Submesh{
	public:
		Submesh(){
			indexBuffer=NULL;
			appearance=NULL;
		}

		M3GIndexBuffer *indexBuffer;
		M3GAppearance *appearance;
	};

	M3GMesh():M3GNode(){
		objectType=14;
		vertexBuffer=NULL;
	}

	virtual void getData(DataStream *out){
		M3GNode::getData(out);
		writeObjectIndex(out,vertexBuffer);
		writeUInt32(out,submeshes.size());
		int i;
		for(i=0;i<submeshes.size();++i){
			writeObjectIndex(out,submeshes[i].indexBuffer);
			writeObjectIndex(out,submeshes[i].appearance);
		}
	}

	virtual int getDataLength(){
		int value=M3GNode::getDataLength();
		value+=4 + 4 + submeshes.size()*(4 + 4);
		return value;
	}

	virtual void gatherObjects(Collection<M3GProxy*> &objects){
		if(vertexBuffer!=NULL){
			vertexBuffer->gatherObjects(objects);
		}
		int i;
		for(i=0;i<submeshes.size();++i){
			if(submeshes[i].indexBuffer!=NULL){
				submeshes[i].indexBuffer->gatherObjects(objects);
			}
			if(submeshes[i].appearance!=NULL){
				submeshes[i].appearance->gatherObjects(objects);
			}
		}
		M3GNode::gatherObjects(objects);
	}

	M3GVertexBuffer *vertexBuffer;
	Collection<Submesh> submeshes;
};

class M3GSkinnedMesh:public M3GMesh{
public:
	class Bone{
	public:
		Bone(){
			transformNode=NULL;
			firstVertex=0;
			vertexCount=0;
			weight=0;
		}

		M3GNode *transformNode;
		uint32 firstVertex;
		uint32 vertexCount;
		int32 weight;
	};

	M3GSkinnedMesh():M3GMesh(){
		objectType=16;
		skeleton=NULL;
	}

	virtual void getData(DataStream *out){
		M3GMesh::getData(out);
		writeObjectIndex(out,skeleton);
		writeUInt32(out,bones.size());
		int i;
		for(i=0;i<bones.size();++i){
			writeObjectIndex(out,bones[i].transformNode);
			writeUInt32(out,bones[i].firstVertex);
			writeUInt32(out,bones[i].vertexCount);
			writeInt32(out,bones[i].weight);
		}
	}

	virtual int getDataLength(){
		int value=M3GMesh::getDataLength();
		value+=4 + 4 + bones.size()*(4 + 4 + 4 + 4);
		return value;
	}

	virtual void gatherObjects(Collection<M3GProxy*> &objects){
		if(skeleton!=NULL){
			skeleton->gatherObjects(objects);
		}
		// Don't gather the bones, they are attached to the skeleton
		M3GMesh::gatherObjects(objects);
	}

	M3GGroup *skeleton;
	Collection<Bone> bones;
};

class M3GKeyframeSequence:public M3GObject3D{
public:
	enum{
		CONSTANT=192,
		LOOP=193,
	};

	enum{
		LINEAR=176,
		SLERP=177,
		SPLINE=178,
		SQUAD=179,
		STEP=180,
	};

	enum{
		ENCODING_FULL=0,
		ENCODING_BYTE=1,
		ENCODING_SHORT=2,
	};

	class FullKeyframe{
	public:
		FullKeyframe(){
			time=0;
			vectorValue[0]=0.0f;
			vectorValue[1]=0.0f;
			vectorValue[2]=0.0f;
			vectorValue[3]=0.0f;
		}

		uint32 time;
		float vectorValue[4];
	};

	class ByteKeyframe{
	public:
		ByteKeyframe(){
			time=0;
			vectorValue[0]=0;
			vectorValue[1]=0;
			vectorValue[2]=0;
			vectorValue[3]=0;
		}

		uint32 time;
		uint8 vectorValue[4];
	};

	class ShortKeyframe{
	public:
		ShortKeyframe(){
			time=0;
			vectorValue[0]=0;
			vectorValue[1]=0;
			vectorValue[2]=0;
			vectorValue[3]=0;
		}

		uint32 time;
		uint16 vectorValue[4];
	};

	M3GKeyframeSequence():M3GObject3D(){
        objectType=19;
		interpolation=LINEAR;
		repeatMode=CONSTANT;
		encoding=ENCODING_FULL;
		duration=0;
		validRangeFirst=0;
		validRangeLast=0;
		componentCount=3;

		vectorScale=Vector4(1,1,1,1);
	}

	virtual void prepareData(){
		// Remove negative zeros, M3G can't handle them
		int i,j;
		for(i=0;i<componentCount;++i){
			if(fabs(vectorBias[i])<0.00001){
				vectorBias[i]=0;
			}
			if(fabs(vectorScale[i])<0.00001){
				vectorScale[i]=0;
			}
		}

		if(encoding==ENCODING_FULL){
			for(i=0;i<fullKeyframes.size();++i){
				for(j=0;j<componentCount;++j){
					if(fabs(fullKeyframes[i].vectorValue[j])<0.00001){
						fullKeyframes[i].vectorValue[j]=0.0f;
					}
				}
			}
		}
	}

	virtual void getData(DataStream *out){
		prepareData();

		M3GObject3D::getData(out);
		writeUInt8(out,interpolation);
		writeUInt8(out,repeatMode);
		writeUInt8(out,encoding);
		writeUInt32(out,duration);
		writeUInt32(out,validRangeFirst);
		writeUInt32(out,validRangeLast);
		writeUInt32(out,componentCount);
		int i,j;
		if(encoding==ENCODING_FULL){
			writeUInt32(out,fullKeyframes.size());
			for(i=0;i<fullKeyframes.size();++i){
				writeUInt32(out,fullKeyframes[i].time);
				for(j=0;j<componentCount;++j){
					writeFloat(out,fullKeyframes[i].vectorValue[j]);
				}
			}
		}
		else if(encoding==ENCODING_BYTE){
			writeUInt32(out,byteKeyframes.size());
			for(i=0;i<componentCount;++i){
				writeFloat(out,vectorBias[i]);
			}
			for(i=0;i<componentCount;++i){
				writeFloat(out,vectorScale[i]);
			}
			for(i=0;i<byteKeyframes.size();++i){
				writeUInt32(out,byteKeyframes[i].time);
				for(j=0;j<componentCount;++j){
					writeUInt8(out,byteKeyframes[i].vectorValue[j]);
				}
			}
		}
		else if(encoding==ENCODING_SHORT){
			writeUInt32(out,shortKeyframes.size());
			for(i=0;i<componentCount;++i){
				writeFloat(out,vectorBias[i]);
			}
			for(i=0;i<componentCount;++i){
				writeFloat(out,vectorScale[i]);
			}
			for(i=0;i<shortKeyframes.size();++i){
				writeUInt32(out,shortKeyframes[i].time);
				for(j=0;j<componentCount;++j){
					writeUInt16(out,shortKeyframes[i].vectorValue[j]);
				}
			}
		}
	}

	virtual int getDataLength(){
		int value=M3GObject3D::getDataLength();
		value+=1 + 1 + 1 + 4 + 4 + 4 + 4;
		if(encoding==ENCODING_FULL){
			value+=4 + fullKeyframes.size()*(4 + componentCount*4);
		}
		else if(encoding==ENCODING_BYTE){
			value+=4 + componentCount*(4+4) + byteKeyframes.size()*(4 + componentCount*1);
		}
		else if(encoding==ENCODING_SHORT){
			value+=4 + componentCount*(4+4) + shortKeyframes.size()*(4 + componentCount*2);
		}
		return value;
	}

	uint8 interpolation;
	uint8 repeatMode;
	uint8 encoding;
	uint32 duration;
	uint32 validRangeFirst;
	uint32 validRangeLast;
	uint32 componentCount;
	Collection<FullKeyframe> fullKeyframes;

	Vector4 vectorBias;
	Vector4 vectorScale;
	Collection<ByteKeyframe> byteKeyframes;
	Collection<ShortKeyframe> shortKeyframes;
};

class M3GAnimationController:public M3GObject3D{
public:
	M3GAnimationController():M3GObject3D(){
		objectType=1;
		speed=1.0f;
		weight=0.0f;
		activeIntervalStart=0;
		activeIntervalEnd=0;
		referenceSequenceTime=0.0f;
		referenceWorldTime=0;
	}

	virtual void getData(DataStream *out){
		M3GObject3D::getData(out);
		writeFloat(out,speed);
		writeFloat(out,weight);
		writeInt32(out,activeIntervalStart);
		writeInt32(out,activeIntervalEnd);
		writeFloat(out,referenceSequenceTime);
		writeInt32(out,referenceWorldTime);
	}

	virtual int getDataLength(){
		return M3GObject3D::getDataLength() + 4 + 4 + 4 + 4 + 4 + 4;
	}

	float speed;
	float weight;
	int32 activeIntervalStart;
	int32 activeIntervalEnd;
	float referenceSequenceTime;
	int32 referenceWorldTime;
};

class M3GAnimationTrack:public M3GObject3D{
public:
	enum{
		ALPHA=256,
		AMBIENT_COLOR=257,
		COLOR=258,
		CROP=259,
		DENSITY=260,
		DIFFUSE_COLOR=261,
		EMISSIVE_COLOR=262,
		FAR_DISTANCE=263,
		FIELD_OF_VIEW=264,
		INTENSITY=265,
		MORPH_WEIGHTS=266,
		NEAR_DISTANCE=267,
		ORIENTATION=268,
		PICKABILITY=269,
		SCALE=270,
		SHININESS=271,
		SPECULAR_COLOR=272,
		SPOT_ANGLE=273,
		SPOT_EXPONENT=274,
		TRANSLATION=275,
		VISIBILITY=276,
	};

	M3GAnimationTrack():M3GObject3D(){
		objectType=2;
		keyframeSequence=NULL;
		animationController=NULL;
		propertyID=TRANSLATION;
	}

	virtual void getData(DataStream *out){
		M3GObject3D::getData(out);
		writeObjectIndex(out,keyframeSequence);
		writeObjectIndex(out,animationController);
		writeInt32(out,propertyID);
	}

	virtual int getDataLength(){
		return M3GObject3D::getDataLength() + 4 + 4 + 4;
	}

	virtual void gatherObjects(Collection<M3GProxy*> &objects){
		if(keyframeSequence!=NULL){
			keyframeSequence->gatherObjects(objects);
		}
		if(animationController!=NULL){
			animationController->gatherObjects(objects);
		}
		M3GObject3D::gatherObjects(objects);
	}

	M3GKeyframeSequence *keyframeSequence;
	M3GAnimationController *animationController;
	uint32 propertyID;
};

class M3GSectionObject{
public:
	M3GSectionObject(){
		mObject=NULL;
	}

	M3GSectionObject(M3GProxy *object){
		mObject=object;
	}

	virtual void getData(DataStream *out){
		M3GProxy::writeUInt8(out,mObject->objectType);
		M3GProxy::writeUInt32(out,mObject->getDataLength());
		mObject->getData(out);
	}

	virtual int getDataLength(){
		return 1 + 4 + mObject->getDataLength();
	}

protected:
	M3GProxy *mObject;
};

class M3GSection{
public:
	M3GSection(const Collection<M3GProxy*> objectList){
		compressionScheme=0;
		totalSectionLength=0;
		uncompressedLength=0;
		mSectionObjects.resize(objectList.size());
		int i;
		for(i=0;i<objectList.size();++i){
			mSectionObjects[i]=M3GSectionObject(objectList[i]);
			uncompressedLength+=mSectionObjects[i].getDataLength();
		}
		totalSectionLength=1 + 4 + 4 + uncompressedLength + 4;
	}

    virtual void getData(DataStream *out){
		MemoryStream mout;

		DataStream dout(&mout);
		M3GProxy::writeUInt8(&dout,compressionScheme);
		M3GProxy::writeUInt32(&dout,totalSectionLength);
		M3GProxy::writeUInt32(&dout,uncompressedLength);

		int i;
		for(i=0;i<mSectionObjects.size();++i){
			mSectionObjects[i].getData(&dout);
		}

		Adler32 adler;
		adler.update((char*)mout.getOriginalDataPointer(),mout.length());

		out->write(mout.getOriginalDataPointer(),mout.length());
		M3GProxy::writeUInt32(out,adler.getValue());
	}

	virtual int getLength(){
		return totalSectionLength;
	}

	uint8 compressionScheme;
	uint32 totalSectionLength;
	uint32 uncompressedLength;

protected:
	Collection<M3GSectionObject> mSectionObjects;
};

const tbyte M3GFileIdentifierData[12]={0xAB, 0x4A, 0x53, 0x52, 0x31, 0x38, 0x34, 0xBB, 0x0D, 0x0A, 0x1A, 0x0A};
class M3GFileIdentifier{
public:
	M3GFileIdentifier(){
	}

	virtual void getData(DataStream *out){
		out->write(M3GFileIdentifierData,12);
	}

	virtual int getLength(){
		return 12;
	}
};

M3GConverter::M3GConverter(){
#ifdef USE_ACTC
	mTC=actcNew();
	actcParami(mTC,ACTC_OUT_MIN_FAN_VERTS,INT_MAX);
	actcParami(mTC,ACTC_OUT_HONOR_WINDING,ACTC_TRUE);
#endif
}

M3GConverter::~M3GConverter(){
#ifdef USE_ACTC
	actcDelete(mTC);
#endif
}

bool M3GConverter::convertMesh(Mesh *mesh,Stream *outStream,float scale,int forceBytes,bool invertYTexCoord,bool viewable,int animation){
	int i,j;

	if(scale==0){
		Logger::alert("Using a scale of 0!");
	}

	bool result=false;

	DataStream::ptr out(new DataStream(outStream));

	Collection<M3GProxy*> objects;
	M3GHeaderObject headerObject;
	objects.clear();
	objects.add(&headerObject);
	M3GSection section0(objects);

	objects.clear();
	M3GObject3D *object=buildSceneGraph(mesh,scale,forceBytes,invertYTexCoord,viewable,animation);
	if(object!=NULL){
		object->gatherObjects(objects);

		for(i=0;i<objects.size();++i){
			if(objects[i]==NULL){
				objects.erase(objects.begin()+i);
				i--;
			}
			else for(j=i+1;j<objects.size();++j){
				if(objects[j]==objects[i]){
					objects.erase(objects.begin()+j);
					j--;
				}
			}
		}

		for(i=0;i<objects.size();++i){
			objects[i]->id=i+2;
		}

		M3GSection section1(objects);

		M3GFileIdentifier fileIdentifier;
		int length=fileIdentifier.getLength();
		length+=section0.getLength();
		length+=section1.getLength();

		headerObject.totalFileSize=length;
		headerObject.approximateContentSize=length;

		fileIdentifier.getData(out);
		section0.getData(out);
		section1.getData(out);

		result=true;
	}

	for(i=0;i<objects.size();++i){
		delete objects[i];
	}
	objects.clear();

	return result;
}

uint32 toM3GColor(const Vector4 &color){
	return ((int)(color.x*255.0f))<<24 | ((int)(color.y*255.0f))<<16 | ((int)(color.z*255.0f))<<8 | ((int)(color.w*255.0f));
}

M3GObject3D *M3GConverter::buildSceneGraph(Mesh *toadletMesh,float scale,int forceBytes,bool invertYTexCoord,bool viewable,int activeAnimation){
	int i,j,k;

	M3GMesh *mesh=NULL;

	VertexBuffer::ptr vertexBuffer=toadletMesh->getStaticVertexData()->getVertexBuffer(0);
	Collection<int> reorder;
	reorder.resize(vertexBuffer->getSize());
	for(i=0;i<reorder.size();++i){
		reorder[i]=i;
	}

	if(toadletMesh->getSkeleton()!=NULL && toadletMesh->getVertexBoneAssignments()>0){
		Skeleton::ptr toadletSkeleton=toadletMesh->getSkeleton();
		const Collection<Mesh::VertexBoneAssignmentList> &vbas=toadletMesh->getVertexBoneAssignments();

		Collection<Collection<int> > boneverts;
		boneverts.resize(toadletSkeleton->bones.size());

		// Here we calculate a vertexBuffer reordering so that bones can reference their vertexes sequentially
		int count=0;
		for(j=0;j<vbas.size();++j){
			const Mesh::VertexBoneAssignmentList &assignments=vbas[j];
			if(assignments.size()==0){
				reorder[j]=count;
				count++;
			}
		}
		for(i=0;i<toadletSkeleton->bones.size();++i){
			for(j=0;j<vbas.size();++j){
				const Mesh::VertexBoneAssignmentList &assignments=vbas[j];
				if(assignments.size()>0 && assignments[0].bone==i){
					boneverts[i].add(count);
					reorder[j]=count;
					count++;
				}
			}
		}

		M3GSkinnedMesh *skinnedMesh=new M3GSkinnedMesh();

		Collection<M3GGroup*> bones;
		for(i=0;i<toadletSkeleton->bones.size();++i){
			Skeleton::Bone *toadletBone=toadletSkeleton->bones[i];
			M3GGroup *bone=new M3GGroup();
			bones.add(bone);

			if(toadletBone->parentIndex==-1){
				if(skinnedMesh->skeleton==NULL){
					skinnedMesh->skeleton=bone;
				}
				else{
					for(i=0;i<bones.size();++i){
						delete bones[i];
					}
					bones.clear();
					delete skinnedMesh;

					Error::unknown("Multiple base-bones unsupported, if we want support add an extra node at top");

					return NULL;
				}
			}
			else{
				bones[toadletBone->parentIndex]->children.add(bone);
			}

			// Assign the vertexes to the bone
			if(boneverts[i].size()>0){
				M3GSkinnedMesh::Bone meshBone;
				meshBone.transformNode=bone;
				meshBone.weight=1;
				meshBone.firstVertex=boneverts[i][0];
				meshBone.vertexCount=boneverts[i].size();
				skinnedMesh->bones.add(meshBone);
			}

			// Setup bone transform
			bone->hasComponentTransform=true;
			bone->orientationAngle=Math::setAxisAngleFromQuaternion(bone->orientationAxis,toadletBone->rotate,0.0001);
			bone->translation=toadletBone->translate*scale;
			bone->scale=toadletBone->scale;
		}

		skinnedMesh->userParameters.resize(toadletSkeleton->sequences.size());

		for(i=0;i<toadletSkeleton->sequences.size();++i){
			Sequence *sequence=toadletSkeleton->sequences[i];

			M3GAnimationController *animationController=new M3GAnimationController();
			animationController->speed=1.0f;
			if(i==activeAnimation){
				animationController->weight=1.0f;
			}
			else{
				animationController->weight=0.0f;
			}
			animationController->referenceSequenceTime=0.0f;
			animationController->referenceWorldTime=0;
			// Here we are storing the length of the animation in milliseconds in the active interval
			// We don't use it, and it will be ignored if start==end

			animationController->activeIntervalStart=sequence->getLength();
			animationController->activeIntervalEnd=sequence->getLength();

			animationController->userID=i+1;

			skinnedMesh->userParameters[i].id=i+1;
			skinnedMesh->userParameters[i].value.add(i+1);

			for(j=0;j<sequence->getNumTracks();++j){
				Track *track=sequence->getTrack(j);
				VertexBufferAccessor &vba=track->getAccessor();

				M3GKeyframeSequence *translationSequence=new M3GKeyframeSequence();
				translationSequence->repeatMode=M3GKeyframeSequence::CONSTANT;
				translationSequence->interpolation=M3GKeyframeSequence::LINEAR;
				translationSequence->duration=sequence->getLength();
				translationSequence->validRangeFirst=0;
				translationSequence->validRangeLast=track->getNumKeyFrames()-1;
				translationSequence->componentCount=3;

				bool keepTranslationSequence=true;
				Collection<Vector3> translation(track->getNumKeyFrames());
				for(k=0;k<track->getNumKeyFrames();++k){
					vba.get3(k,0,translation[k]);
					translation[k]*=scale;
				}
				int translationBytes=forceBytes;
				if(forceBytes==0){
					translationBytes=VertexCompression::getSuggestedArrayBytes(3,track->getNumKeyFrames(),(float*)&translation[0],sizeof(Vector3));
					Logger::alert(String("Animation Translation bytes:")+translationBytes);
				}
				if(translationBytes==1){
					VertexCompression::calculateArrayBiasAndScale(3,track->getNumKeyFrames(),
						(float*)&translation[0],sizeof(Vector3),
						(float*)&translationSequence->vectorBias,(float*)&translationSequence->vectorScale,0);

					translationSequence->encoding=M3GKeyframeSequence::ENCODING_BYTE;
					translationSequence->byteKeyframes.resize(track->getNumKeyFrames());
				}
				else if(translationBytes==2){
					VertexCompression::calculateArrayBiasAndScale(3,track->getNumKeyFrames(),
						(float*)&translation[0],sizeof(Vector3),
						(float*)&translationSequence->vectorBias,(float*)&translationSequence->vectorScale,0);

					translationSequence->encoding=M3GKeyframeSequence::ENCODING_SHORT;
					translationSequence->shortKeyframes.resize(track->getNumKeyFrames());
				}
				else{
					// If it's telling us to use full precision, that means there is enough data that we don't want to cut the keyframe
					translationSequence->encoding=M3GKeyframeSequence::ENCODING_FULL;
					translationSequence->fullKeyframes.resize(track->getNumKeyFrames());
				}

				M3GKeyframeSequence *rotationSequence=new M3GKeyframeSequence();
				rotationSequence->repeatMode=M3GKeyframeSequence::CONSTANT;
				rotationSequence->interpolation=M3GKeyframeSequence::SLERP;
				rotationSequence->duration=sequence->getLength();
				rotationSequence->validRangeFirst=0;
				rotationSequence->validRangeLast=track->getNumKeyFrames()-1;
				rotationSequence->componentCount=4;

				bool keepRotationSequence=true;
				Collection<Quaternion> quaternions(track->getNumKeyFrames());
				for(k=0;k<track->getNumKeyFrames();++k){
					vba.get4(k,1,quaternions[k]);
				}
				int rotationBytes=forceBytes;
				if(forceBytes==0){
					rotationBytes=VertexCompression::getSuggestedArrayBytes(4,track->getNumKeyFrames(),(float*)&quaternions[0],sizeof(Quaternion));
					Logger::alert(String("Animation Rotation bytes:")+rotationBytes);
				}
				if(rotationBytes==1){
					VertexCompression::calculateArrayBiasAndScale(4,track->getNumKeyFrames(),
						(float*)&quaternions[0],sizeof(Quaternion),
						(float*)&rotationSequence->vectorBias,(float*)&rotationSequence->vectorScale,0);

					rotationSequence->encoding=M3GKeyframeSequence::ENCODING_BYTE;
					rotationSequence->byteKeyframes.resize(track->getNumKeyFrames());
				}
				else if(rotationBytes==2){
					VertexCompression::calculateArrayBiasAndScale(4,track->getNumKeyFrames(),
						(float*)&quaternions[0],sizeof(Quaternion),
						(float*)&rotationSequence->vectorBias,(float*)&rotationSequence->vectorScale,0);

					rotationSequence->encoding=M3GKeyframeSequence::ENCODING_SHORT;
					rotationSequence->shortKeyframes.resize(track->getNumKeyFrames());
				}
				else{
					// If it's telling us to use full precision, that means there is enough data that we don't want to cut the keyframe
					rotationSequence->encoding=M3GKeyframeSequence::ENCODING_FULL;
					rotationSequence->fullKeyframes.resize(track->getNumKeyFrames());
				}

				// Check to see if we can just cut out the translation sequence
				if(	fabs(translationSequence->vectorScale.x)<0.00001 &&
					fabs(translationSequence->vectorScale.y)<0.00001 &&
					fabs(translationSequence->vectorScale.z)<0.00001 &&
					fabs(translationSequence->vectorBias.x - bones[j]->translation.x)<0.00001 &&
					fabs(translationSequence->vectorBias.y - bones[j]->translation.y)<0.00001 &&
					fabs(translationSequence->vectorBias.z - bones[j]->translation.z)<0.00001){
					keepTranslationSequence=false;
				}


				// Check to see if we can just cut out the rotation sequence
				if(	fabs(rotationSequence->vectorScale.x)<0.00001 &&
					fabs(rotationSequence->vectorScale.y)<0.00001 &&
					fabs(rotationSequence->vectorScale.z)<0.00001 &&
					fabs(rotationSequence->vectorScale.w)<0.00001 &&
					fabs(rotationSequence->vectorBias.x - bones[j]->orientationAxis.x)<0.00001 &&
					fabs(rotationSequence->vectorBias.y - bones[j]->orientationAxis.y)<0.00001 &&
					fabs(rotationSequence->vectorBias.z - bones[j]->orientationAxis.z)<0.00001 &&
					fabs(rotationSequence->vectorBias.w - bones[j]->orientationAngle)<0.00001){
					keepRotationSequence=false;
				}

				for(k=0;k<track->getNumKeyFrames();++k){
					scalar time=track->getTime(k);

					if(translationBytes==1){
						translationSequence->byteKeyframes[k].time=(unsigned int)(time*1000);

						int l;
						for(l=0;l<3;++l){
							uint8 value=(uint8)(((translation[k][l]-translationSequence->vectorBias[l])/translationSequence->vectorScale[l])*255);
							translationSequence->byteKeyframes[k].vectorValue[l]=value;
						}
					}
					else if(translationBytes==2){
						translationSequence->shortKeyframes[k].time=(unsigned int)(time*1000);

						int l;
						for(l=0;l<3;++l){
							uint16 value=(uint16)(((translation[k][l]-translationSequence->vectorBias[l])/translationSequence->vectorScale[l])*65535);
							translationSequence->shortKeyframes[k].vectorValue[l]=value;
						}
					}
					else{
						translationSequence->fullKeyframes[k].time=(unsigned int)(time*1000);

						int l;
						for(l=0;l<3;++l){
							translationSequence->fullKeyframes[k].vectorValue[l]=translation[k][l];
						}
					}

					if(rotationBytes==1){
						rotationSequence->byteKeyframes[k].time=(unsigned int)(time*1000);

						int l;
						for(l=0;l<4;++l){
							uint8 value=(uint8)(((quaternions[k][l]-rotationSequence->vectorBias[l])/rotationSequence->vectorScale[l])*255);
							rotationSequence->byteKeyframes[k].vectorValue[l]=value;
						}
					}
					else if(rotationBytes==2){
						rotationSequence->shortKeyframes[k].time=(unsigned int)(time*1000);

						int l;
						for(l=0;l<4;++l){
							uint16 value=(uint16)(((quaternions[k][l]-rotationSequence->vectorBias[l])/rotationSequence->vectorScale[l])*65535);
							rotationSequence->shortKeyframes[k].vectorValue[l]=value;
						}
					}
					else{
						rotationSequence->fullKeyframes[k].time=(unsigned int)(time*1000);

						int l;
						for(l=0;l<4;++l){
							rotationSequence->fullKeyframes[k].vectorValue[l]=quaternions[k][l];
						}
					}
				}

				if(keepTranslationSequence){
					M3GAnimationTrack *translationTrack=new M3GAnimationTrack();
					translationTrack->keyframeSequence=translationSequence;
					translationTrack->animationController=animationController;
					translationTrack->propertyID=M3GAnimationTrack::TRANSLATION;
					bones[j]->animationTracks.add(translationTrack);
				}

				if(keepRotationSequence){
					M3GAnimationTrack *rotationTrack=new M3GAnimationTrack();
					rotationTrack->keyframeSequence=rotationSequence;
					rotationTrack->animationController=animationController;
					rotationTrack->propertyID=M3GAnimationTrack::ORIENTATION;
					bones[j]->animationTracks.add(rotationTrack);
				}
			}
		}

		mesh=skinnedMesh;
	}
	else{
		mesh=new M3GMesh();
	}

	M3GVertexBuffer *vertexes=new M3GVertexBuffer();
	{
		VertexBuffer::ptr vertexBuffer=toadletMesh->getStaticVertexData()->getVertexBuffer(0);
		VertexFormat::ptr vertexFormat=vertexBuffer->getVertexFormat();
		int positionIndex=vertexFormat->findElement(VertexFormat::Semantic_POSITION);
		int positionOffset=positionIndex>=0?vertexFormat->getElementOffset(positionIndex):0;
		int normalIndex=vertexFormat->findElement(VertexFormat::Semantic_NORMAL);
		int colorIndex=vertexFormat->findElement(VertexFormat::Semantic_COLOR);
		int texCoordIndex=vertexFormat->findElement(VertexFormat::Semantic_TEXCOORD);
		int texCoordOffset=texCoordIndex>=0?vertexFormat->getElementOffset(texCoordIndex):0;

		int numVertexes=vertexBuffer->getSize();
		VertexBufferAccessor vba(vertexBuffer);
		if(positionIndex>=0){
			M3GVertexArray *positions=new M3GVertexArray();

			positions->componentCount=3;

			Vector3 position;
			int bytes=forceBytes;
			if(forceBytes==0){
				bytes=VertexCompression::getSuggestedArrayBytes(3,numVertexes,
					(float*)(vba.getData()+positionOffset),vertexFormat->getVertexSize());
				Logger::alert(String("Position bytes:")+bytes);
			}
			if(bytes==1){
				positions->componentSize=1;
				positions->byteComponents.resize(numVertexes*3);

				Vector3 bias,scale;

				VertexCompression::calculateArrayBiasAndScale(3,numVertexes,
					(float*)(vba.getData()+positionOffset),vertexFormat->getVertexSize(),
					(float*)&bias,(float*)&scale,255);

				positions->bias=bias;
				positions->scale=Math::maxVal(Math::maxVal(scale[0],scale[1]),scale[2]);

				for(i=0;i<numVertexes;++i){
					vba.get3(i,positionIndex,position);
					for(j=0;j<3;++j){
						int8 value=(int8)((position[j]-positions->bias[j])/positions->scale);
						positions->byteComponents[reorder[i]*3+j]=value;
					}
				}
			}
			else{ // Otherwise we just use shorts
				positions->componentSize=2;
				positions->shortComponents.resize(numVertexes*3);

				Vector3 bias,scale;

				VertexCompression::calculateArrayBiasAndScale(3,numVertexes,
					(float*)(vba.getData()+positionOffset),vertexFormat->getVertexSize(),
					(float*)&bias,(float*)&scale,65535);

				positions->bias=bias;
				positions->scale=Math::maxVal(Math::maxVal(scale[0],scale[1]),scale[2]);

				for(i=0;i<numVertexes;++i){
					vba.get3(i,positionIndex,position);
					for(j=0;j<3;++j){
						int16 value=(int16)((position[j]-positions->bias[j])/positions->scale);
						positions->shortComponents[reorder[i]*3+j]=value;
					}
				}
			}

			positions->scale*=scale;
			positions->bias*=scale;
			vertexes->positions=positions;
		}

		if(normalIndex>=0){
			M3GVertexArray *normals=new M3GVertexArray();

			normals->componentCount=3;
			normals->componentSize=1;
			normals->byteComponents.resize(numVertexes*3);

			Vector3 normal;
			for(i=0;i<numVertexes;++i){
				vba.get3(i,normalIndex,normal);
				for(j=0;j<3;++j){
					int8 value=(int8)(normal[j]*127);
					normals->byteComponents[reorder[i]*3+j]=value;
				}
			}

			vertexes->normals=normals;
		}

		if(colorIndex>=0){
			M3GVertexArray *colors=new M3GVertexArray();

			colors->componentCount=3;
			colors->componentSize=1;
			colors->byteComponents.resize(numVertexes*3);
			Vector4 color;

			for(i=0;i<numVertexes;++i){
				int c=vba.getRGBA(i,colorIndex);
				color.setRGBA(c);
				for(j=0;j<3;++j){
					uint8 value=(uint8)(color[j]*255);
					colors->byteComponents[reorder[i]*3+j]=value;
				}
			}

			vertexes->colors=colors;
		}

		if(texCoordIndex>=0){
			M3GVertexArray *texCoords=new M3GVertexArray();

			texCoords->componentCount=2;

			Vector2 texCoord;
			vba.get2(0,texCoordIndex,texCoord);

			if(invertYTexCoord){
				for(i=0;i<numVertexes;++i){
					vba.get2(i,texCoordIndex,texCoord);
					texCoord.y=1.0f-texCoord.y;
					vba.set2(i,texCoordIndex,texCoord);
				}
			}

			int bytes=forceBytes;
			if(forceBytes==0){
				bytes=VertexCompression::getSuggestedArrayBytes(2,numVertexes,
					(float*)(vba.getData()+texCoordOffset),vertexFormat->getVertexSize());
				Logger::alert(String("TexCoord bytes:")+bytes);
			}
			if(bytes==1){
				texCoords->componentSize=1;
				texCoords->byteComponents.resize(numVertexes*2);

				Vector3 bias,scale;

				VertexCompression::calculateArrayBiasAndScale(2,numVertexes,
					(float*)(vba.getData()+texCoordOffset),vertexFormat->getVertexSize(),
					(float*)&bias,(float*)&scale,255);

				texCoords->bias=bias;
				texCoords->scale=Math::maxVal(scale[0],scale[1]);

				for(i=0;i<numVertexes;++i){
					vba.get2(i,texCoordIndex,texCoord);

					int8 x=(int8)((texCoord.x-texCoords->bias[0])/texCoords->scale);
					texCoords->byteComponents[reorder[i]*2+0]=x;

					int8 y=(int8)((texCoord.y-texCoords->bias[1])/texCoords->scale);
					texCoords->byteComponents[reorder[i]*2+1]=y;
				}
			}
			else{ // Otherwise we just use shorts
				texCoords->componentSize=2;
				texCoords->shortComponents.resize(numVertexes*2);

				Vector3 bias,scale;

				VertexCompression::calculateArrayBiasAndScale(2,numVertexes,
					(float*)(vba.getData()+texCoordOffset),vertexFormat->getVertexSize(),
					(float*)&bias,(float*)&scale,65535);

				texCoords->bias=bias;
				texCoords->scale=Math::maxVal(scale[0],scale[1]);

				for(i=0;i<numVertexes;++i){
					vba.get2(i,texCoordIndex,texCoord);

					int16 x=(int16)((texCoord.x-texCoords->bias[0])/texCoords->scale);
					texCoords->shortComponents[reorder[i]*2+0]=x;

					int16 y=(int16)((texCoord.y-texCoords->bias[1])/texCoords->scale);
					texCoords->shortComponents[reorder[i]*2+1]=y;
				}
			}

			vertexes->texCoordArrays.add(texCoords);
		}
		vba.unlock();
	}
	mesh->vertexBuffer=vertexes;

	mesh->submeshes.resize(toadletMesh->getNumSubMeshes());

	for(i=0;i<toadletMesh->getNumSubMeshes();++i){
		Mesh::SubMesh *sub=toadletMesh->getSubMesh(i);
		
		IndexBuffer::ptr indexBuffer=sub->indexData->getIndexBuffer();
		int numIndexes=indexBuffer->getSize();
		IndexBufferAccessor iba(indexBuffer);

#ifdef USE_ACTC
		{
			actcMakeEmpty(mTC);

			actcBeginInput(mTC);
			for(j=0;j<numIndexes/3;++j){
				actcAddTriangle(mTC,reorder[iba.get(j*3+0)],reorder[iba.get(j*3+1)],reorder[iba.get(j*3+2)]);
			}
			actcEndInput(mTC);

			int prim=0;
			unsigned int v1=0,v2=0,v3=0;
			Collection<Collection<int> > strips;
			int numIndexes=0;

			actcBeginOutput(mTC);
			while((prim=actcStartNextPrim(mTC,&v1,&v2))!=ACTC_DATABASE_EMPTY){
				strips.add(Collection<int>());
				Collection<int> &strip=strips[strips.size()-1];

				strip.add(v1);
				strip.add(v2);
				numIndexes+=2;

				while(actcGetNextVert(mTC,&v3)!=ACTC_PRIM_COMPLETE){
					strip.add(v3);
					numIndexes++;
				}
			}
			actcEndOutput(mTC);

			M3GTriangleStripArray *triStrip=new M3GTriangleStripArray();
			triStrip->encoding=M3GTriangleStripArray::EXPLICIT_UINT16;
			triStrip->shortIndexes.resize(numIndexes);
			numIndexes=0;
			for(j=0;j<strips.size();++j){
				Collection<int> &strip=strips[j];
				for(k=0;k<strip.size();++k){
					triStrip->shortIndexes[numIndexes]=strip[k];
					numIndexes++;
				}
			}

			Logger::alert(String("Number of strips:")+strips.size());

			triStrip->stripLengths.resize(strips.size());

			std::stringstream os;
			os << "Strip lengths:";
			for(j=0;j<strips.size();++j){
				os << strips[j].size() << ",";
				triStrip->stripLengths[j]=strips[j].size();
			}

			Logger::alert(os.str().c_str());

			mesh->submeshes[i].indexBuffer=triStrip;
		}
#endif

#ifdef USE_NVTRISTRIP
		{
			unsigned short *indices=new unsigned short[numIndexes];
			for(j=0;j<numIndexes;++j){
				indices[j]=iba.get(j);
			}

			PrimitiveGroup *primitiveGroups;
			unsigned short numGroups;
			GenerateStrips(indices,indexBuffer->getNumIndexes(),&primitiveGroups,&numGroups);

			int numIndexes=0;
			for(j=0;j<numGroups;++j){
				numIndexes+=primitiveGroups[j].numIndices;
			}

			M3GTriangleStripArray *triStrip=new M3GTriangleStripArray();
			triStrip->encoding=M3GTriangleStripArray::EXPLICIT_UINT16;
			triStrip->shortIndexes.resize(numIndexes);
			numIndexes=0;
			for(j=0;j<numGroups;++j){
				for(k=0;k<primitiveGroups[j].numIndices;++k){
					triStrip->shortIndexes[numIndexes]=primitiveGroups[j].indices[k];
					numIndexes++;
				}
			}

			Logger::alert(String("Number of strips:")+numGroups);

			triStrip->stripLengths.resize(numGroups);

			std::stringstream os;
			os << "Strip lengths:";
			for(j=0;j<numGroups;++j){
				os << primitiveGroups[j].numIndices << ",";
				triStrip->stripLengths[j]=primitiveGroups[j].numIndices;
			}

			delete[] primitiveGroups;

			Logger::alert(os.str().c_str());

			mesh->submeshes[i].indexBuffer=triStrip;
		}
#endif

		M3GAppearance *appearance=new M3GAppearance();

		Material::ptr material=sub->material;
		if(material!=NULL){
			RasterizerState rasterizerState;
			MaterialState materialState;

			if(material->getPass()->getRasterizerState(rasterizerState)){
				M3GPolygonMode *polygonMode=new M3GPolygonMode();

				if(rasterizerState.cull==RasterizerState::CullType_NONE) polygonMode->culling=M3GPolygonMode::CULL_NONE;

				appearance->polygonMode=polygonMode;
			}

			if(material->getPass()->getMaterialState(materialState) && materialState.light){
				M3GMaterial *m3gMaterial=new M3GMaterial();

				m3gMaterial->ambientColor=toM3GColor(materialState.ambient);
				m3gMaterial->diffuseColor=toM3GColor(materialState.diffuse);
				m3gMaterial->emissiveColor=toM3GColor(materialState.emissive);
				m3gMaterial->specularColor=toM3GColor(materialState.specular);
				m3gMaterial->shininess=materialState.shininess;
				m3gMaterial->vertexColorTrackingEnabled=materialState.trackColor;

				appearance->material=m3gMaterial;
			}

			// ID 0 of the userParameters on an appearance specifies if it should have a texture or not on it
			appearance->userParameters.resize(1);
			if(material->getPass()->getNumTextures()>0){
				appearance->userParameters[0].id=0;
				appearance->userParameters[0].value.add(1);
			}
			else{
				appearance->userParameters[0].id=0;
				appearance->userParameters[0].value.add(0);
			}
		}
		else{
			Logger::alert(String("Invalid material on submesh:")+i);

			appearance->polygonMode=new M3GPolygonMode();
			appearance->userParameters.resize(1);
			appearance->userParameters[0].id=0;
			appearance->userParameters[0].value.add(0);
		}
		iba.unlock();

		mesh->submeshes[i].appearance=appearance;
	}

	if(Error::getInstance()->getError()!=Error::Type_NONE){
		// WARNING: Memory leakage here, should somehow clean up allocated objects
		return NULL;
	}

	if(viewable){
		M3GWorld *world=new M3GWorld();

		M3GCamera *camera=new M3GCamera();
		camera->hasComponentTransform=true;
		camera->translation.y-=40;
		camera->orientationAngle=90;
		camera->orientationAxis=Vector3(1,0,0);
		camera->projectionType=M3GCamera::PERSPECTIVE;
		camera->fovy=60;
		camera->aspectRatio=1;
		camera->nearDist=1;
		camera->farDist=1000;
		world->children.add(camera);
		world->activeCamera=camera;

		M3GBackground *background=new M3GBackground();
		background->backgroundColor=0x00000000;
		world->background=background;

		M3GLight *light=new M3GLight();
		light->attenuationConstant=1.0f;
		light->attenuationLinear=1.0f;
		light->attenuationQuadratic=1.0f;
		light->color=0x00FFFFFF;
		light->mode=M3GLight::DIRECTIONAL;
		light->intensity=1.0f;
		light->spotAngle=0.0f;
		light->spotExponent=0.0f;
		world->children.add(light);

		light=new M3GLight();
		light->attenuationConstant=1.0f;
		light->attenuationLinear=1.0f;
		light->attenuationQuadratic=1.0f;
		light->color=0x007F7F7F;
		light->mode=M3GLight::AMBIENT;
		light->intensity=1.0f;
		light->spotAngle=0.0f;
		light->spotExponent=0.0f;
		world->children.add(light);

		world->children.add(mesh);

		return world;
	}
	else{
		return mesh;
	}
}
