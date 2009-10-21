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
#include "string.h"

using namespace toadlet;
using namespace toadlet::egg;
using namespace toadlet::peeper;
using namespace toadlet::tadpole;

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
	if(vertexFormat->hasVertexElementOfType(VertexElement::Type_NORMAL)==false){
		return; // Cannot be lit
	}

	VertexFormat::ptr newVertexFormat(new VertexFormat());
	for(i=0;i<vertexFormat->getNumVertexElements();++i){
		if(vertexFormat->getVertexElement(i).type!=VertexElement::Type_NORMAL || mKeepNormals){
			newVertexFormat->addVertexElement(vertexFormat->getVertexElement(i));
		}
	}
	if(vertexFormat->hasVertexElementOfType(VertexElement::Type_COLOR)==false){
		newVertexFormat->addVertexElement(VertexElement(VertexElement::Type_COLOR,VertexElement::Format_COLOR_RGBA,0));
	}

	VertexBuffer::ptr newVertexBuffer=VertexBuffer::ptr(vertexBuffer->cloneWithNewParameters(Buffer::UsageType_STATIC,Buffer::AccessType_READ_WRITE,newVertexFormat,vertexBuffer->getSize()));
	mesh->staticVertexData=VertexData::ptr(new VertexData(newVertexBuffer));

	int pi=newVertexFormat->getVertexElementIndexOfType(VertexElement::Type_POSITION);
	int ni=newVertexFormat->getVertexElementIndexOfType(VertexElement::Type_NORMAL);
	int ci=newVertexFormat->getVertexElementIndexOfType(VertexElement::Type_TEX_COORD);

	VertexBufferAccessor vba(vertexBuffer,Buffer::LockType_READ_WRITE);
	VertexBufferAccessor nvba(newVertexBuffer,Buffer::LockType_READ_WRITE);

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
		Color ambient(Colors::BLACK);
		Color diffuse(Colors::WHITE);

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
		IndexBufferAccessor iba(indexBuffer,Buffer::LockType_READ_ONLY);

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
				Color color(
					Math::mul(d,Math::mul(mDiffuseColor.r,diffuse.r))+mAmbientColor.r+ambient.r,
					Math::mul(d,Math::mul(mDiffuseColor.g,diffuse.g))+mAmbientColor.g+ambient.g,
					Math::mul(d,Math::mul(mDiffuseColor.b,diffuse.b))+mAmbientColor.b+ambient.b,
					Math::ONE
				);
				if(color.r>Math::ONE){
					color.r=Math::ONE;
				}
				if(color.g>Math::ONE){
					color.g=Math::ONE;
				}
				if(color.b>Math::ONE){
					color.b=Math::ONE;
				}
				if(color.a>Math::ONE){
					color.a=Math::ONE;
				}

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

void VertexLighter::setLightDiffuseColor(const Color &diffuseColor){
	mDiffuseColor=diffuseColor;
}

void VertexLighter::setLightAmbientColor(const Color &ambientColor){
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

