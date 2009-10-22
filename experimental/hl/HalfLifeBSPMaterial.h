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

#ifndef HL_HALFLIFEBSPMATERIAL_H
#define HL_HALFLIFEBSPMATERIAL_H

#include <toadlet/tadpole/material/Material.h>
#include <toadlet/peeper/LightEffect.h>
#include <toadlet/peeper/Texture.h>
#include <toadlet/peeper/TextureStage.h>

namespace hl{

class TOADLET_API HalfLifeBSPMaterial:public toadlet::tadpole::material::Material{
public:
	typedef toadlet::egg::SharedPointer<HalfLifeBSPMaterial> Ptr;

	static MaterialIdentifier *getClassMaterialIdentifier();

	HalfLifeBSPMaterial();
	HalfLifeBSPMaterial(const HalfLifeBSPMaterial *material);
	virtual ~HalfLifeBSPMaterial();

	toadlet::tadpole::material::Material *clone() const;

	void setDiffuseTexture(const toadlet::peeper::Texture::Ptr &texture);
	const toadlet::peeper::Texture::Ptr &getDiffuseTexture() const;

	void setBumpmapTexture(const toadlet::peeper::Texture::Ptr &texture);
	const toadlet::peeper::Texture::Ptr &getBumpmapTexture() const;

	bool isTransparent() const;

	void compile(toadlet::tadpole::Engine *e);

protected:
	static MaterialIdentifier Identifier;

	toadlet::peeper::Texture::Ptr mDiffuseTexture;
	toadlet::peeper::Texture::Ptr mBumpmapTexture;
};

}

#endif

