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

#include "VertexLighter.h"
#include <string.h> // memset

VertexLighter::VertexLighter(Engine *engine){
	mEngine=engine;
	mDirection=Vector3(0,0,Math::ONE);
	mDiffuseColor=Colors::WHITE;
	mAmbientColor=Colors::BLACK;
	mFalloffFactor=Math::ONE;
	mKeepNormals=false;
	mLightEdges=true;
	mEdgeEpsilon=Math::fromMilli(50);
}

VertexLighter::~VertexLighter(){
}

void VertexLighter::lightMesh(Mesh *mesh){
	int i;

	if(mesh->staticVertexData->getNumVertexBuffers()==0){
		return;
	}

	VertexBuffer::ptr vertexBuffer=mesh->staticVertexData->getVertexBuffer(0);
	VertexFormat::ptr vertexFormat=vertexBuffer->getVertexFormat();
	if(vertexFormat->findSemantic(VertexFormat::Semantic_NORMAL)<0){
		return; // Cannot be lit
	}

	VertexFormat::ptr newVertexFormat=mEngine->getBufferManager()->createVertexFormat();
	for(i=0;i<vertexFormat->getNumElements();++i){
		if(vertexFormat->getSemantic(i)!=VertexFormat::Semantic_NORMAL || mKeepNormals){
			newVertexFormat->addElement(vertexFormat->getSemantic(i),vertexFormat->getIndex(i),vertexFormat->getFormat(i));
		}
	}
	if(vertexFormat->findSemantic(VertexFormat::Semantic_COLOR)<0){
		newVertexFormat->addElement(VertexFormat::Semantic_COLOR,0,VertexFormat::Format_COLOR_RGBA);
	}

	VertexBuffer::ptr newVertexBuffer=mEngine->getBufferManager()->cloneVertexBuffer(vertexBuffer,Buffer::Usage_BIT_STATIC,Buffer::Access_READ_WRITE,newVertexFormat,vertexBuffer->getSize());
	mesh->staticVertexData=VertexData::ptr(new VertexData(newVertexBuffer));

	int pi=newVertexFormat->findSemantic(VertexFormat::Semantic_POSITION);
	int ni=newVertexFormat->findSemantic(VertexFormat::Semantic_NORMAL);
	int ci=newVertexFormat->findSemantic(VertexFormat::Semantic_TEX_COORD);

	VertexBufferAccessor vba;(vertexBuffer);
	VertexBufferAccessor nvba(newVertexBuffer);

	Collection<char> touchedVertexes(vertexBuffer->getSize());
	memset(&touchedVertexes[0],0,touchedVertexes.size());

	Vector3 worldMins; vba.get3(0,pi,worldMins);
	Vector3 worldMaxs; vba.get3(0,pi,worldMaxs);

	for(i=0;i<vba.getSize();++i){
		Vector3 pos; vba.get3(i,pi,pos);

		if(worldMins.x>pos.x){
			worldMins.x=pos.x;
		}
		if(worldMins.y>pos.y){
			worldMins.y=pos.y;
		}
		if(worldMins.z>pos.z){
			worldMins.z=pos.z;
		}

		if(worldMaxs.x<pos.x){
			worldMaxs.x=pos.x;
		}
		if(worldMaxs.y<pos.y){
			worldMaxs.y=pos.y;
		}
		if(worldMaxs.z<pos.z){
			worldMaxs.z=pos.z;
		}
	}

	for(i=0;i<mesh->subMeshes.size();++i){
		Vector4 ambient(Colors::BLACK);
		Vector4 diffuse(Colors::WHITE);

		Material::ptr material=mesh->subMeshes[i]->material;

		LightEffect le=material->getLightEffect();
		le.trackColor=true;
		material->setLightEffect(le);
		material->setLighting(false);
		if(le.diffuse!=Colors::BLACK){
			diffuse=le.diffuse;
		}
		if(le.ambient!=Colors::BLACK){
			ambient=le.ambient;
		}

		IndexBuffer::ptr indexBuffer(mesh->subMeshes[i]->indexData->getIndexBuffer());
		IndexBufferAccessor iba(indexBuffer);

		int j;
		for(j=0;j<iba.getSize();++j){
			int index=iba.get(j);
			if(touchedVertexes[index]==0){
				touchedVertexes[index]=1;

				Vector3 normal; vba.get3(index,ni,normal);
				scalar d=Math::dot(normal,mDirection);
				#if defined(TOADLET_FIXED_POINT)
					d=Math::fromFloat(powf(Math::toFloat(d),Math::toFloat(mFalloffFactor)));
				#else
					d=powf(d,mFalloffFactor);
				#endif
				Vector4 color;
				Math::mul(color,mDiffuseColor,diffuse);
				Math::mul(color,d);
				Math::add(color,mAmbientColor);
				Math::add(color,ambient);
				if(color.x>Math::ONE) color.x=Math::ONE;
				if(color.y>Math::ONE) color.y=Math::ONE;
				if(color.z>Math::ONE) color.z=Math::ONE;
				if(color.w>Math::ONE) color.w=Math::ONE;

				if(mLightEdges==false){
					Vector3 position; vba.get3(index,pi,position);
					if(	Math::abs(position.x-worldMins.x)<mEdgeEpsilon ||
						Math::abs(position.y-worldMins.y)<mEdgeEpsilon ||
						Math::abs(position.x-worldMaxs.x)<mEdgeEpsilon ||
						Math::abs(position.y-worldMaxs.y)<mEdgeEpsilon){
						color=Colors::WHITE;
					}
				}

				nvba.setRGBA(index,ci,color.getRGBA());
			}
		}
	}
}

void VertexLighter::setLightDirection(const Vector3 &direction){
	mDirection=direction;
	Math::normalize(mDirection);
}

void VertexLighter::setLightDiffuseColor(const Vector4 &diffuseColor){
	mDiffuseColor=diffuseColor;
}

void VertexLighter::setLightAmbientColor(const Vector4 &ambientColor){
	mAmbientColor=ambientColor;
}

void VertexLighter::setLightFalloffFactor(scalar falloff){
	mFalloffFactor=falloff;
}

void VertexLighter::setKeepNormals(bool keepNormals){
	mKeepNormals=keepNormals;
}

void VertexLighter::setLightEdges(bool edges,scalar edgeEpsilon){
	mLightEdges=edges;
	mEdgeEpsilon=edgeEpsilon;
}

