#include "Brush.h"
#include "../bsp/Polygon.h"

using namespace toadlet::egg;
using namespace toadlet::egg::math;
using namespace toadlet::egg::math::Math;
using namespace bsp;

namespace csg{

Brush::Brush(){
}

Brush::~Brush(){
	int i;
	for(i=0;i<mFaces.size();++i){
		delete mFaces[i];
	}
	mFaces.clear();
}

void Brush::addFace(BrushFace *face){
	mFaces.push_back(face);
}

int Brush::getNumFaces(){
	return mFaces.size();
}

BrushFace *Brush::getFace(int i){
	return mFaces[i];
}

void Brush::deleteFace(int i){
	delete mFaces[i];
	mFaces.erase(mFaces.begin()+i);
}

#if 1
Vector3 makeOrigin(const Vector3 vectors[],int numVectors){
	Vector3 origin;
	int i;
	for(i=0;i<numVectors;++i){
		origin+=vectors[i];
	}

	return origin/(float)numVectors;
}

void flipVectors(Vector3 vectors[],int numVectors){
	Vector3 temp;
	int i;
	for(i=0;i<numVectors/2;++i){
		temp=vectors[i];
		vectors[i]=vectors[numVectors-1-i];
		vectors[numVectors-1-i]=temp;
	}
}

void arrangeCCW(const Vector3 &origin,Vector3 vectors[],int numVectors){
	if(numVectors<3){
		return;
	}

	Vector3 m=makeOrigin(vectors,3);

	Vector3 ma=m-vectors[0];
	Vector3 mb=m-vectors[1];
	Vector3 mz=m-vectors[2];

	Vector3 maxmb=cross(ma,mb);
	Vector3 maxmz=cross(ma,mz);

	normalize(maxmb);
	normalize(maxmz);

	maxmb+=m;
	maxmz+=m;

	mb=origin-maxmb;
	mz=origin-maxmz;

	if(length(mb)>length(mz)){
		flipVectors(vectors,numVectors);
	}
}

void sortVectors(Vector3 vectors[],int numVectors,const Vector3 &normal){
	int smallestAngleIndex;
	float angle,smallestAngle;
	Vector3 temp,ma,mb;
	Vector3 m=makeOrigin(vectors,numVectors);

	int i;
	for(i=0;i<numVectors-2;++i){
		smallestAngleIndex=-1;
		smallestAngle=-1.0f;

		ma=vectors[i]-m;
		normalize(ma);

		int j;
		for(j=i+1;j<numVectors;++j){
			mb=vectors[j]-m;
			normalize(mb);

			// 3=360
			// 2=270
			// 1=180
			// 0=90
			// -1=0

			angle=dot(ma,mb);

			if(dot(cross(ma,mb),normal)<0.0f){
				angle+=2.0f;
			}

			if(angle>smallestAngle){
				smallestAngle=angle;
				smallestAngleIndex=j;
			}
		}

		if(smallestAngleIndex!=i+1 && smallestAngleIndex!=-1){
			temp=vectors[i+1];
			vectors[i+1]=vectors[smallestAngleIndex];
			vectors[smallestAngleIndex]=temp;
		}
	}
}

bool pointIsValid(const Vector3 &point){
	return point.x>=-4096 && point.x<=4096 && point.y>=-4096 && point.y<=4096 && point.z>=-4096 && point.z<=4096;
}

bool pointInArray(const Vector3 points[],int numPoints,const Vector3 &point,float epsilon){
	int i;
	for(i=0;i<numPoints;++i){
		if(length(points[i],point)<epsilon){
			return true;
		}
	}

	return false;
}

Brush::CompileResult Brush::compile(){
	int i,j,k,l;
	float epsilon=0.001f;
	CompileResult result=CR_SUCCESS;
	float minThickness=0.5f;
	Vector3 intersection;

	if(mFaces.size()<4){
		return CR_ERROR_INSUFFICIENT_FACES;
	}

	for(i=0;i<mFaces.size();++i){
		if(length(mFaces[i]->mPlane.normal)<epsilon){
			return CR_ERROR_INVALID_NORMAL;
		}

		for(j=0;j<mFaces.size();++j){
			if(i!=j){
				if(	length(mFaces[i]->getPlane().normal,mFaces[j]->getPlane().normal)<epsilon &&
					fabs(mFaces[i]->getPlane().d-mFaces[j]->getPlane().d)<epsilon){
					result=CR_WARNING_DUPLICATE_PLANE;
				
					deleteFace(j);
					j--;
					continue;
				}

				Vector3 pi=mFaces[i]->getPlane().normal*mFaces[i]->getPlane().d;
				Vector3 pj=mFaces[j]->getPlane().normal*mFaces[j]->getPlane().d;
				if(length(pi,pj)<minThickness){
					return CR_ERROR_NO_THICKNESS;
				}
			}
		}
	}

	for(i=0;i<mFaces.size()-2;++i){
		for(j=0;j<mFaces.size()-1;++j){
			for(k=0;k<mFaces.size();++k){
				if(i!=j && i!=k && j!=k){
					if(getIntersectionOfThreePlanes(intersection,mFaces[i]->mPlane,mFaces[j]->mPlane,mFaces[k]->mPlane,0.001f) && pointIsValid(intersection)){
						bool legal=true;

						for(l=0;l<mFaces.size();++l){
							if(l!=i && l!=j && l!=k){
								const Plane &plane=mFaces[l]->mPlane;
								if((dot(plane.normal,intersection)-plane.d) < epsilon){
									legal=false;
									break;
								}
							}
						}

						if(legal){
							if(pointInArray(mFaces[i]->mPolygonPoints.begin(),mFaces[i]->mPolygonPoints.size(),intersection,epsilon)==false){
								mFaces[i]->mPolygonPoints.push_back(intersection);
							}
							if(pointInArray(mFaces[j]->mPolygonPoints.begin(),mFaces[j]->mPolygonPoints.size(),intersection,epsilon)==false){
								mFaces[j]->mPolygonPoints.push_back(intersection);
							}
							if(pointInArray(mFaces[k]->mPolygonPoints.begin(),mFaces[k]->mPolygonPoints.size(),intersection,epsilon)==false){
								mFaces[k]->mPolygonPoints.push_back(intersection);
							}
						}
					}
				}
			}
		}
	}

	int count=0;
	Vector3 origin;

	for(i=0;i<mFaces.size();++i){
		for(j=0;j<mFaces[i]->mPolygonPoints.size();++j){
			count++;
			origin+=mFaces[i]->mPolygonPoints[j];
		}
	}

	origin/=(float)count;

	Vector3 newVertex;
	for(i=0;i<mFaces.size();++i){
		if(mFaces[i]->mPolygonPoints.size()<3){
			return CR_ERROR_INVALID_POLYGON;
		}

		sortVectors(mFaces[i]->mPolygonPoints.begin(),mFaces[i]->mPolygonPoints.size(),mFaces[i]->mPlane.normal);
		arrangeCCW(origin,mFaces[i]->mPolygonPoints.begin(),mFaces[i]->mPolygonPoints.size());
	}

	return CR_SUCCESS;
}
#else
Brush::CompileResult Brush::compile(){
	int i,j;
	Collection<Vector3> vertexes;
	float epsilon=0.0001f;
	float minThickness=0.25f;
	CompileResult result=CR_SUCCESS;

	for(i=0;i<mFaces.size();++i){
		for(j=0;j<mFaces.size();++j){
			if(i!=j){
				if(	length(mFaces[i]->getPlane().normal,mFaces[j]->getPlane().normal)<epsilon &&
					fabs(mFaces[i]->getPlane().d-mFaces[j]->getPlane().d)<epsilon){
					result=CR_WARNING_DUPLICATE_PLANE;
				
					deleteFace(j);
					j--;
					continue;
				}

				Vector3 pi=mFaces[i]->getPlane().normal*mFaces[i]->getPlane().d;
				Vector3 pj=mFaces[j]->getPlane().normal*mFaces[j]->getPlane().d;
				if(length(pi,pj)<minThickness){
					return CR_ERROR_NO_THICKNESS;
				}
			}
		}
	}

	for(i=0;i<mFaces.size();++i){
		BrushFace *face=mFaces[i];

		const Vector3 &normal=face->getPlane().normal;

		if(length(normal)<epsilon){
			return CR_ERROR_INVALID_NORMAL;
		}

		Vector3 anormal(fabs(normal.x),fabs(normal.y),fabs(normal.z));

		Vector3 points[4];
		float m=8192;
		float m2=m/2;

		if(anormal.x>=anormal.y && anormal.x>=anormal.z){
			if(normal.x>0){
				points[3]=Vector3(0,-m,-m);
				points[2]=Vector3(0,m,-m);
				points[1]=Vector3(0,m,m);
				points[0]=Vector3(0,-m,m);
			}
			else{
				points[0]=Vector3(0,-m,-m);
				points[1]=Vector3(0,m,-m);
				points[2]=Vector3(0,m,m);
				points[3]=Vector3(0,-m,m);
			}
		}
		else if(anormal.y>=anormal.z){
			if(normal.y>0){
				points[3]=Vector3(-m,0,-m);
				points[2]=Vector3(-m,0,m);
				points[1]=Vector3(m,0,m);
				points[0]=Vector3(m,0,-m);
			}
			else{
				points[0]=Vector3(-m,0,-m);
				points[1]=Vector3(-m,0,m);
				points[2]=Vector3(m,0,m);
				points[3]=Vector3(m,0,-m);
			}
		}
		else{
			if(normal.z>0){
				points[3]=Vector3(-m,-m,0);
				points[2]=Vector3(m,-m,0);
				points[1]=Vector3(m,m,0);
				points[0]=Vector3(-m,m,0);
			}
			else{
				points[0]=Vector3(-m,-m,0);
				points[1]=Vector3(m,-m,0);
				points[2]=Vector3(m,m,0);
				points[3]=Vector3(-m,m,0);
			}
		}

		for(j=0;j<4;++j){
			Vector3 temp;
			project(face->getPlane(),temp,points[j]);
			points[j]=temp;
		}

		vertexes.push_back(points[0]);
		vertexes.push_back(points[1]);
		vertexes.push_back(points[2]);
		vertexes.push_back(points[3]);

		Polygon polygon;
		polygon.indexes.push_back(vertexes.size()-4);
		polygon.indexes.push_back(vertexes.size()-3);
		polygon.indexes.push_back(vertexes.size()-2);
		polygon.indexes.push_back(vertexes.size()-1);
		polygon.updatePlane(vertexes);

		for(j=0;j<mFaces.size();++j){
			if(i!=j){
				Polygon back;
				Polygon front;
				polygon.splitPolygon(vertexes,mFaces[j]->getPlane(),front,back,epsilon);
				polygon=back;
				polygon.cleanDoubleVerts(vertexes);
			}
		}

		if(polygon.indexes.size()<3){
			return CR_ERROR_INVALID_POLYGON;
		}

		for(j=0;j<polygon.indexes.size();++j){
			const Vector3 &vert=vertexes[polygon.indexes[j]];
			if(vert.x<-m2 || vert.x>m2 || vert.y<-m2 || vert.y>m2 || vert.z<-m2 || vert.z>m2){
				return CR_ERROR_INFINITE_POLYGON;
			}
		}

		face->mPolygonPoints.resize(polygon.indexes.size());
		for(j=0;j<polygon.indexes.size();++j){
			face->mPolygonPoints[j]=vertexes[polygon.indexes[j]];
		}
	}

	return CR_SUCCESS;
}
#endif

}