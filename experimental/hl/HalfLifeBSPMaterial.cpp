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

#include "HalfLifeBSPMaterial.h"
#include <toadlet/tadpole/SceneStates.h>
#include <toadlet/peeper/Renderer.h>
#include <toadlet/peeper/BlendFunction.h>
#include <toadlet/peeper/Light.h>
#include <toadlet/tadpole/Engine.h>
#include <toadlet/egg/image/ImageUtilities.h>

using namespace toadlet::egg;
using namespace toadlet::egg::image;
using namespace toadlet::egg::math;
using namespace toadlet::egg::math::Math;
using namespace toadlet::peeper;
using namespace toadlet::tadpole;
using namespace toadlet::tadpole::material;

namespace hl{

Material::MaterialIdentifier *HalfLifeBSPMaterial::getClassMaterialIdentifier(){return &Identifier;}

Material::MaterialIdentifier HalfLifeBSPMaterial::Identifier;

HalfLifeBSPMaterial::HalfLifeBSPMaterial(){
	mIdentifier=getClassMaterialIdentifier();
}

HalfLifeBSPMaterial::HalfLifeBSPMaterial(const HalfLifeBSPMaterial *material){
	mIdentifier=material->mIdentifier;
	mDiffuseTexture=material->mDiffuseTexture;
}

HalfLifeBSPMaterial::~HalfLifeBSPMaterial(){
}

Material *HalfLifeBSPMaterial::clone() const{
	return new HalfLifeBSPMaterial(this);
}

void HalfLifeBSPMaterial::setDiffuseTexture(const Texture::Ptr &texture){
	mDiffuseTexture=texture;
}

const Texture::Ptr &HalfLifeBSPMaterial::getDiffuseTexture() const{
	return mDiffuseTexture;
}

void HalfLifeBSPMaterial::setBumpmapTexture(const Texture::Ptr &texture){
	mBumpmapTexture=texture;
}

const Texture::Ptr &HalfLifeBSPMaterial::getBumpmapTexture() const{
	return mBumpmapTexture;
}

bool HalfLifeBSPMaterial::isTransparent() const{
	return false;
}

void HalfLifeBSPMaterial::compile(Engine *e){
}

}
