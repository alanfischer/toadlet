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

#include <toadlet/toadlet.h>

using namespace toadlet;

class VertexLighter{
public:
	VertexLighter(Engine *engine);
	virtual ~VertexLighter();

	void lightMesh(Mesh *mesh);

	void setLightDirection(const Vector3 &direction);
	void setLightDiffuseColor(const Vector4 &diffuseColor);
	void setLightAmbientColor(const Vector4 &ambientColor);
	void setLightFalloffFactor(scalar falloff);
	void setKeepNormals(bool keepNormals);
	void setLightEdges(bool edges,scalar edgeEpsilon);

protected:
	Engine *mEngine;
	Vector3 mDirection;
	Vector4 mDiffuseColor;
	Vector4 mAmbientColor;
	scalar mFalloffFactor;
	bool mKeepNormals;
	bool mLightEdges;
	scalar mEdgeEpsilon;
};

#endif

