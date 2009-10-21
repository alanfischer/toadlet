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

#include "CollisionTreeHandler.h"
#include <toadlet/egg/io/DataInputStream.h>
#include <toadlet/egg/io/DataOutputStream.h>
#include <toadlet/egg/EndianConversion.h>

#include <limits.h>
#include <stdexcept>

using namespace toadlet::egg;
using namespace toadlet::egg::io;
using namespace toadlet::egg::math;

namespace bsp{

Node *CollisionTreeHandler::loadCollisionTree(InputStream *in){
	short version=0;

	char buffer[4];
	in->read(buffer,4);
	if(buffer[0]!='T' || buffer[1]!='C' || buffer[2]!='L' || buffer[3]!='T'){
		throw std::runtime_error("Not of TCLT format");
	}

	DataInputStream *dataIn=new DataInputStream(in);

	short numNodes=0;
	dataIn->readBigInt16(numNodes);

	Node *nodes=new Node[numNodes];

	short c;
	for(c=0;c<numNodes;++c){
		Node &node=nodes[c];

		int i=0;

		dataIn->readBigInt32(i);
		node.plane.normal.x=fixedToFloat(i);
		dataIn->readBigInt32(i);
		node.plane.normal.y=fixedToFloat(i);
		dataIn->readBigInt32(i);
		node.plane.normal.z=fixedToFloat(i);
		dataIn->readBigInt32(i);
		node.plane.d=fixedToFloat(i);

		short s=0;

		dataIn->readBigInt16(s);
		node.child[0]=s;
		dataIn->readBigInt16(s);
		node.child[1]=s;
		dataIn->readBigInt16(s);
		node.parent=s;
		dataIn->readBigInt16(s);
		node.leaf=s;
	}

	delete dataIn;

	return nodes;
}

void CollisionTreeHandler::saveCollisionTree(OutputData *data,OutputStream *out){
	out->write("TCLT",4);

	if(data->nodes.size()>SHRT_MAX){
		throw std::runtime_error("TCLT Format only supports short trees!");
	}

	DataOutputStream *dataOut=new DataOutputStream(out);

	short numNodes=data->nodes.size();
	dataOut->writeBigInt16(numNodes);

	short c;
	for(c=0;c<numNodes;++c){
		Node &node=data->nodes[c];

		dataOut->writeBigInt32(floatToFixed(node.plane.normal.x));
		dataOut->writeBigInt32(floatToFixed(node.plane.normal.y));
		dataOut->writeBigInt32(floatToFixed(node.plane.normal.z));
		dataOut->writeBigInt32(floatToFixed(node.plane.d));

		// Write rest of data
		dataOut->writeBigInt16(node.child[0]);
		dataOut->writeBigInt16(node.child[1]);
		dataOut->writeBigInt16(node.parent);
		dataOut->writeBigInt16(node.leaf);
	}

	delete dataOut;
}

int CollisionTreeHandler::floatToFixed(float f){
	return (int)(f * (float)(1<<16));
}

float CollisionTreeHandler::fixedToFloat(int f){
	return ((float)f) / (float)(1<<16);
}

}
