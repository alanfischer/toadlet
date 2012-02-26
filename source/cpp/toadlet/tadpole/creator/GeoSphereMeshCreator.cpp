/********** Copyright header - do not remove **********
 *
 * The Toadlet Engine
 *
 * Copyright 2009, Lightning Toads Productions, LLC
 *
 * Author(s): Alan Fischer, Andrew Fischer
 *
 * This file is part of The Toadlet Engine.
 *
 * The Toadlet Engine is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * The Toadlet Engine is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.

 * You should have received a copy of the GNU Lesser General Public License
 * along with The Toadlet Engine.  If not, see <http://www.gnu.org/licenses/>.
 *
 ********** Copyright header - do not remove **********/

#include <toadlet/tadpole/Engine.h>
#include <toadlet/tadpole/creator/GeoSphereMeshCreator.h>

namespace toadlet{
namespace tadpole{
namespace creator{

Mesh::ptr GeoSphereMeshCreator::createGeoSphereMesh(VertexBuffer::ptr vertexBuffer,IndexBuffer::ptr indexBuffer,const Sphere &sphere,int depth,bool icosahedron){
	currentSphere=sphere;
	vba.lock(vertexBuffer,Buffer::Access_BIT_WRITE);

	int vertexIndex=0;
	int i,j;

	Collection<IndexTri> old;

	if(icosahedron){
		int indices[]={
			0, 1, 2, 0, 2, 3, 0, 3, 4, 0, 4, 5, 0, 5, 1, 1, 10, 6, 2, 6,
			7, 3, 7, 8, 4, 8, 9, 5, 9, 10, 6, 2, 1, 7, 3, 2, 8, 4, 3, 9,
			5, 4, 10, 1, 5, 11, 7, 6, 11, 8, 7, 11, 9, 8, 11, 10, 9, 11,
			6, 10
		};
		int indicesLength=sizeof(indices)/sizeof(int);
		scalar y = Math::fromMilli(447);
		scalar a = Math::fromMilli(894);
		scalar b = Math::fromMilli(276);
		scalar c = Math::fromMilli(723);
		scalar d = Math::fromMilli(850);
		scalar e = Math::fromMilli(525);
		geoSet(vertexIndex++,Vector3(0, Math::ONE, 0));
		geoSet(vertexIndex++,Vector3(a, y, 0));
		geoSet(vertexIndex++,Vector3(b, y, -d));
		geoSet(vertexIndex++,Vector3(-c, y, -e));
		geoSet(vertexIndex++,Vector3(-c, y, e));
		geoSet(vertexIndex++,Vector3(b, y, d));
		geoSet(vertexIndex++,Vector3(c, -y, -e));
		geoSet(vertexIndex++,Vector3(-b, -y, -d));
		geoSet(vertexIndex++,Vector3(-a, -y, 0));
		geoSet(vertexIndex++,Vector3(-b, -y, d));
		geoSet(vertexIndex++,Vector3(c, -y, e));
		geoSet(vertexIndex++,Vector3(0, -Math::ONE, 0));

		old.resize(indicesLength/3);
		for(i=0;i<old.size();++i){
			old[i].set(indices[i*3+0],indices[i*3+1],indices[i*3+2]);
		}
	}
	else{
		/* Six equidistant points lying on the unit sphere */
		int xplus = vertexIndex;
		geoSet(vertexIndex++,Math::X_UNIT_VECTOR3);
		int xmin = vertexIndex;
		geoSet(vertexIndex++,Math::NEG_X_UNIT_VECTOR3);
		int yplus = vertexIndex;
		geoSet(vertexIndex++,Math::Y_UNIT_VECTOR3);
		int ymin = vertexIndex;
		geoSet(vertexIndex++,Math::NEG_Y_UNIT_VECTOR3);
		int zplus = vertexIndex;
		geoSet(vertexIndex++,Math::Z_UNIT_VECTOR3);
		int zmin = vertexIndex;
		geoSet(vertexIndex++,Math::NEG_Z_UNIT_VECTOR3);

		old.resize(8);
		old[0].set(yplus,zplus,xplus);
		old[1].set(xmin, zplus,yplus);
		old[2].set(ymin, zplus,xmin);
		old[3].set(xplus,zplus,ymin);
		old[4].set(zmin, yplus,xplus);
		old[5].set(zmin, xmin, yplus);
		old[6].set(zmin, ymin, xmin);
		old[7].set(zmin, xplus,ymin);
	}

	Vector3 pt0,pt1,pt2;

	/* Subdivide each starting triangle (maxlevels - 1) times */
	int level;
	for(level=1;level<=depth;++level){
		Collection<IndexTri> next(old.size()*4);

		/*
		 * Subdivide each polygon in the old approximation and normalize the
		 * next points thus generated to lie on the surface of the unit
		 * sphere. Each input triangle with vertBuf labelled [0,1,2] as
		 * shown below will be turned into four next triangles:
		 * 
		 * Make next points
		 *   a = (0+2)/2
		 *   b = (0+1)/2
		 *   c = (1+2)/2
		 *   
		 * 1   /\   Normalize a, b, c
		 *    /  \
		 * b /____\ c
		 * 
		 * Construct next triangles
		 * 
		 *    /\    /\   [0,b,a] 
		 *   /  \  /  \  [b,1,c]
		 *  /____\/____\ [a,b,c]
		 *  0 a 2 [a,c,2]
		 */
		for(i=0;i<old.size();++i){
			int newi=i*4;
			IndexTri oldt=old[i];

			vba.get3(oldt.index[0],0,pt0);
			vba.get3(oldt.index[1],0,pt1);
			vba.get3(oldt.index[2],0,pt2);
			Vector3 av;Math::lerp(av,pt0,pt2,Math::HALF);Math::normalize(av);
			Vector3 bv;Math::lerp(bv,pt0,pt1,Math::HALF);Math::normalize(bv);
			Vector3 cv;Math::lerp(cv,pt1,pt2,Math::HALF);Math::normalize(cv);
			int a=vertexIndex;
			geoSet(vertexIndex++,av);
			int b=vertexIndex;
			geoSet(vertexIndex++,bv);
			int c=vertexIndex;
			geoSet(vertexIndex++,cv);

			next[newi++].set(oldt.index[0],b,a);
			next[newi++].set(b,oldt.index[1],c);
			next[newi++].set(a,b,c);
			next[newi++].set(a,c,oldt.index[2]);
		}

		/* Continue subdividing next triangles */
		old=next;
	}

	vba.unlock();

	iba.lock(indexBuffer,Buffer::Access_BIT_WRITE);
	for(i=0;i<old.size();++i){
		for(j=0;j<3;++j){
			iba.set(i*3+j,old[i].index[j]);
		}
	}
	iba.unlock();

	Mesh::SubMesh::ptr subMesh(new Mesh::SubMesh());
	subMesh->indexData=IndexData::ptr(new IndexData(IndexData::Primitive_TRIS,indexBuffer));

	Mesh::ptr mesh(new Mesh());
	mesh->addSubMesh(subMesh);
	mesh->setStaticVertexData(VertexData::ptr(new VertexData(vertexBuffer)));
	mesh->setBound(Bound(sphere));

	return mesh;
}

Mesh::ptr GeoSphereMeshCreator::createGeoSphereMesh(const Sphere &sphere,int depth,bool icosahedron,Material::ptr material){
	VertexBuffer::ptr vertexBuffer=mEngine->getBufferManager()->createVertexBuffer(Buffer::Usage_BIT_STATIC,Buffer::Access_BIT_WRITE,mEngine->getVertexFormats().POSITION_NORMAL_TEX_COORD,getGeoSphereVertexCount(depth,icosahedron));
	IndexBuffer::ptr indexBuffer=mEngine->getBufferManager()->createIndexBuffer(Buffer::Usage_BIT_STATIC,Buffer::Access_BIT_WRITE,IndexBuffer::IndexFormat_UINT16,getGeoSphereIndexCount(depth,icosahedron));

	Mesh::ptr mesh=createGeoSphereMesh(vertexBuffer,indexBuffer,sphere,depth,icosahedron);
	if(material==NULL){
		material=mEngine->getMaterialManager()->createDiffuseMaterial(NULL);
	}
	mesh->getSubMesh(0)->material=material;

	return mesh;
}

void GeoSphereMeshCreator::geoSet(int vertexIndex,const Vector3 &vec){
	vba.set3(vertexIndex,0,
		Math::mul(vec.x,currentSphere.radius)+currentSphere.origin.x,
		Math::mul(vec.y,currentSphere.radius)+currentSphere.origin.y,
		Math::mul(vec.z,currentSphere.radius)+currentSphere.origin.z);

	scalar l=Math::length(vec);
	scalar nx=Math::div(vec.x,l),ny=Math::div(vec.y,l),nz=Math::div(vec.z,l);
	vba.set3(vertexIndex,1,nx,ny,nz);

	// This algorithm generates a distortion around the .5 texture coordinate, since it quickly goes from .5 to 1.5
	//  so you see it flip the whole texture around, this should be fixed
	scalar tx=Math::div(Math::atan2(ny,nx),Math::TWO_PI)+Math::ONE;
	scalar ty=nz/2+Math::HALF;
	vba.set2(vertexIndex,2,tx,ty);
}

}
}
}
