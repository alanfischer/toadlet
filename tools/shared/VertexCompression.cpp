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

#include "VertexCompression.h"
#include <toadlet/egg/Collection.h>
#include <toadlet/egg/math/Math.h>

using namespace toadlet::egg;
using namespace toadlet::egg::math::Math;

int VertexCompression::getSuggestedArrayBytes(int elements,int numComponents,float *array,int skipBytes,bool separateScales){
	Collection<float> minimum(elements);
	Collection<float> maximum(elements);

	int i,j,k;
	for(i=0;i<elements;++i){
		minimum[i]=array[i];
		maximum[i]=array[i];
	}

	for(i=0;i<numComponents;++i){
		float *a=(float*)(((char*)array)+skipBytes*i);

		for(j=0;j<elements;++j){
			if(a[j]<minimum[j]){
				minimum[j]=a[j];
			}
			if(a[j]>maximum[j]){
				maximum[j]=a[j];
			}
		}
	}

	Collection<float> mindiff(elements);

	for(i=0;i<elements;++i){
		mindiff[i]=maximum[i]-minimum[i];
	}

	const float epsilon=0.01;

	for(i=0;i<numComponents;++i){
		float *a1=(float*)(((char*)array)+skipBytes*i);

		for(j=i+1;j<numComponents;++j){
			float *a2=(float*)(((char*)array)+skipBytes*j);

			for(k=0;k<elements;++k){
				float diff=fabs(a1[k]-a2[k]);
				if(diff>epsilon && diff<mindiff[k]){
					mindiff[k]=diff;
				}
			}
		}
	}

	if(separateScales==false){
		float minmindiff=mindiff[0];
		for(i=1;i<elements;++i){
			if(mindiff[i]<minmindiff){
				minmindiff=mindiff[i];
			}
		}
		for(i=0;i<elements;++i){
			mindiff[i]=minmindiff;
		}
	}

	int maxbytes=1;
	for(k=0;k<elements;++k){
		if(mindiff[k]<epsilon){
			mindiff[k]=epsilon;
		}

		float r=(maximum[k]-minimum[k])/mindiff[k];

		int bytes=1;
		if(r < 256){
			bytes=1;
		}
		else if(r < 65535){
			bytes=2;
		}
		else{
			bytes=4; // Max precision
		}

		if(bytes>maxbytes){
			maxbytes=bytes;
		}
	}

	return maxbytes;
}

// If encodedMaximum==0, then we use method 2, where the encodedMaximum is NOT divided into the scaleResult,
// and the biasResult is NOT the average of min & max
void VertexCompression::calculateArrayBiasAndScale(int elements,int numComponents,float *array,int skipBytes,float *biasResult,float *scaleResult,int encodedMaximum){
	int i,j;

	Collection<float> minimum(elements);
	Collection<float> maximum(elements);

	for(i=0;i<elements;++i){
		minimum[i]=array[i];
		maximum[i]=array[i];
	}

	for(i=0;i<numComponents;++i){
		float *a=(float*)(((char*)array)+skipBytes*i);

		for(j=0;j<elements;++j){
			if(a[j]<minimum[j]){
				minimum[j]=a[j];
			}
			if(a[j]>maximum[j]){
				maximum[j]=a[j];
			}
		}
	}

	const float epsilon=0.01;

	if(encodedMaximum==0){
		for(i=0;i<elements;++i){
			biasResult[i]=minimum[i];
			scaleResult[i]=(maximum[i]-minimum[i]);
			if(scaleResult[i]==0){
				scaleResult[i]=1.0f;
			}
		}
	}
	else{
		for(i=0;i<elements;++i){
			biasResult[i]=minimum[i]*0.5f+maximum[i]*0.5f;
			scaleResult[i]=(maximum[i]-minimum[i])/(float)encodedMaximum;
			if(scaleResult[i]==0){
				scaleResult[i]=1.0f;
			}
		}
	}
}
