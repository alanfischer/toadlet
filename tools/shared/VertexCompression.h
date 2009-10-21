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

#ifndef VERTEXCOMPRESSION_H
#define VERTEXCOMPRESSION_H

class VertexCompression{
public:
	static int getSuggestedArrayBytes(int elements,int numComponents,float *array,int skipBytes,bool separateScales=false);

	// If encodedMaximum==0, then we use method 2, where the encodedMaximum is NOT divided into the scaleResult,
	// and the biasResult is NOT the average of min & max
	static void calculateArrayBiasAndScale(int elements,int numComponents,float *array,int skipBytes,float *biasResult,float *scaleResult,int encodedMaximum);
};

#endif
