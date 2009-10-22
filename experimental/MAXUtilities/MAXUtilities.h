#ifndef MAXUTILITIES_H
#define MAXUTILITIES_H

#include <toadlet/egg/math/Math.h>
#include <toadlet/tadpole/Engine.h>
#include <toadlet/tadpole/material/StandardMaterial.h>
#include "Max.h"

inline void setVector3FromMaxPoint3(toadlet::egg::math::Vector3 &vector3,const Point3 &maxPoint3){
	vector3.x=maxPoint3[0];
	vector3.y=maxPoint3[1];
	vector3.z=maxPoint3[2];
}

inline toadlet::egg::math::Vector3 makeVector3FromMaxPoint3(const Point3 &maxPoint3){
	toadlet::egg::math::Vector3 vector3;
	setVector3FromMaxPoint3(vector3,maxPoint3);
	return vector3;
}

inline void setVector2FromMaxPoint3(toadlet::egg::math::Vector2 &vector2,const Point3 &maxPoint3){
	vector2.x=maxPoint3[0];
	vector2.y=maxPoint3[1];
}

inline toadlet::egg::math::Vector2 makeVector2FromMaxPoint3(const Point3 &maxPoint3){
	toadlet::egg::math::Vector2 vector2;
	setVector2FromMaxPoint3(vector2,maxPoint3);
	return vector2;
}

inline void setQuaternionFromMaxQuat(toadlet::egg::math::Quaternion &quaternion,const Quat &maxQuat){
	quaternion.x=maxQuat[0];
	quaternion.y=maxQuat[1];
	quaternion.z=maxQuat[2];
	quaternion.w=-maxQuat[3];
}

inline toadlet::egg::math::Quaternion makeQuaternionFromMaxQuat(const Quat &maxQuat){
	toadlet::egg::math::Quaternion quaternion;
	setQuaternionFromMaxQuat(quaternion,maxQuat);
	return quaternion;
}

inline bool setToadletStandardMaterialMap(toadlet::tadpole::Engine *engine,StdMat *mm,TimeValue time,int id,toadlet::tadpole::material::StandardMaterial *tm,toadlet::tadpole::material::StandardMaterial::Map map){
	float amount=mm->GetTexmapAmt(id,time);
	toadlet::peeper::Texture::Ptr texture;
	toadlet::egg::String textureName;

	bool result=false;

	if(mm->MapEnabled(id) && mm->GetSubTexmap(id) && mm->SubTexmapOn(id)){
		// Check for a bitmap
		if(mm->GetSubTexmap(id)->ClassID() == Class_ID(BMTEX_CLASS_ID, 0)){
			BitmapTex *dbt=(BitmapTex*) mm->GetSubTexmap(id);

			// Split the texture filename from the path
			TSTR path,file;
			SplitPathFile((TSTR)dbt->GetMapName(), &path, &file);

			if(strlen(file)>0){
				engine->getTextureManager()->addSearchDirectory((char*)path);
				texture=engine->getTextureManager()->load((char*)file);
				textureName=file;
				result=true;
			}
		}
	}

	if(result==false){
		textureName="unknown";
	}

	tm->setMap(map,texture,amount,textureName);

	return result;
}

inline void maxMaterialToToadletMaterial(toadlet::tadpole::Engine *engine,Mtl *mm,TimeValue time,toadlet::tadpole::material::Material::Ptr &tm){
	toadlet::tadpole::material::StandardMaterial *material=new toadlet::tadpole::material::StandardMaterial();
	tm=engine->getMaterialManager()->load(material);

	// Null material conversion
	if(mm==NULL){
		return;
	}
	else if(mm->ClassID()==Class_ID(DMTL_CLASS_ID,0)){
		// Cast to standard material
		StdMat *stdMtl=(StdMat*)mm;

		material->setName(stdMtl->GetName().data());

		// Get max attributes
		Color amb=stdMtl->GetAmbient(time);
		Color diff=stdMtl->GetDiffuse(time);
		Color spec=stdMtl->GetSpecular(time);
		Color self=stdMtl->GetSelfIllumColor(time);
		float selff=stdMtl->GetSelfIllum(time);
		float opacity=stdMtl->GetOpacity(time);
		float shininess=stdMtl->GetShininess(time);
		float shinstr=stdMtl->GetShinStr(time);

		toadlet::peeper::LightEffect le;
		le.shininess=shininess*128;

		if(stdMtl->GetSelfIllumColorOn()){
			le.emissive=toadlet::egg::math::Vector4(self.r,self.g,self.b,1.0f);
		}
		else{
			le.emissive=toadlet::egg::math::Vector4(selff,selff,selff,1.0f);
		}

		if(setToadletStandardMaterialMap(engine,stdMtl,time,ID_AM,material,toadlet::tadpole::material::StandardMaterial::MAP_AMBIENT)){
			le.ambient=toadlet::peeper::COLOR_WHITE;
		}
		else{
			le.ambient=toadlet::egg::math::Vector4(amb.r,amb.g,amb.b,1.0f);
		}

		if(setToadletStandardMaterialMap(engine,stdMtl,time,ID_DI,material,toadlet::tadpole::material::StandardMaterial::MAP_DIFFUSE)){
			le.diffuse=toadlet::peeper::COLOR_WHITE;
		}
		else{
			le.diffuse=toadlet::egg::math::Vector4(diff.r,diff.g,diff.b,1.0f);
		}

		setToadletStandardMaterialMap(engine,stdMtl,time,ID_SP,material,toadlet::tadpole::material::StandardMaterial::MAP_SPECULAR);
		le.specular=toadlet::egg::math::Vector4(spec.r,spec.g,spec.b,0)*shinstr + toadlet::egg::math::Vector4(0,0,0,1.0f);

		material->setLighting(true); // Can I get a lighting parameter from MAX?

		setToadletStandardMaterialMap(engine,stdMtl,time,ID_SI,material,toadlet::tadpole::material::StandardMaterial::MAP_SELFILLUMINATION);
		setToadletStandardMaterialMap(engine,stdMtl,time,ID_OP,material,toadlet::tadpole::material::StandardMaterial::MAP_OPACITY);
		setToadletStandardMaterialMap(engine,stdMtl,time,ID_BU,material,toadlet::tadpole::material::StandardMaterial::MAP_BUMP);
		setToadletStandardMaterialMap(engine,stdMtl,time,ID_RL,material,toadlet::tadpole::material::StandardMaterial::MAP_REFLECTION);
		setToadletStandardMaterialMap(engine,stdMtl,time,ID_RR,material,toadlet::tadpole::material::StandardMaterial::MAP_REFRACTION);

		// If displacement is checked, then use parallax mapping
		if(stdMtl->MapEnabled(ID_DP)){
			material->setMethod(toadlet::tadpole::material::StandardMaterial::METHOD_PARALLAX);
		}
		else{
			material->setMethod(toadlet::tadpole::material::StandardMaterial::METHOD_BUMP);
		}

		material->setLightEffect(le);

		material->setOpacity(opacity);
	}
}

#endif
