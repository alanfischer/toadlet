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

#ifndef VERTEXLIGHTER_H
#define VERTEXLIGHTER_H

#include <toadlet/tadpole/Engine.h>

class VertexLighter{
public:
	VertexLighter(toadlet::tadpole::Engine *engine);
	virtual ~VertexLighter();

	void lightMesh(toadlet::tadpole::Mesh *mesh);

	void setLightDirection(const toadlet::tadpole::Vector3 &direction);
	void setLightDiffuseColor(const toadlet::peeper::Color &diffuseColor);
	void setLightAmbientColor(const toadlet::peeper::Color &ambientColor);
	void setLightFalloffFactor(toadlet::scalar falloff);
	void setKeepNormals(bool keepNormals);
	void setLightEdges(bool edges,toadlet::scalar edgeEpsilon);

protected:
	toadlet::tadpole::Engine *mEngine;
	toadlet::tadpole::Vector3 mDirection;
	toadlet::peeper::Color mDiffuseColor;
	toadlet::peeper::Color mAmbientColor;
	toadlet::scalar mFalloffFactor;
	bool mKeepNormals;
	bool mLightEdges;
	toadlet::scalar mEdgeEpsilon;
};

#endif

