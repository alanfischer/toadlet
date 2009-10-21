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

#ifndef BSP_COMPILER_H
#define BSP_COMPILER_H

#include "InputData.h"
#include "OutputData.h"

namespace bsp{

class TOADLET_API Compiler{
public:
	Compiler();
	~Compiler();

	void setCleanDoubleVertexes(bool b){mCleanDoubleVertexes=b;}
	bool getCleanDoubleVertexes() const{return mCleanDoubleVertexes;}

	void setCleanZeroAreaPolygons(bool b){mCleanZeroAreaPolygons=b;}
	bool getCleanZeroAreaPolygons() const{return mCleanZeroAreaPolygons;}

	void setOptimizeSplits(bool b){mOptimizeSplits=b;}
	bool getOptimizeSplits() const{return mOptimizeSplits;}

	void setBalanceTree(bool b){mBalanceTree=b;}
	bool getBalanceTree() const{return mBalanceTree;}

	void setEpsilon(float f){mEpsilon=f;}
	float getEpsilon() const{return mEpsilon;}

	void compile(InputData *in,OutputData *out);

	void validate();
	void createBSP();
	void createNode(int node,toadlet::egg::List<Polygon> &polygons);
	int createLeaf(toadlet::egg::List<Polygon> &list);

	void pushPolygon(toadlet::egg::List<Polygon> &polygons,toadlet::egg::Collection<toadlet::egg::math::Vector3> &vertexes,const Polygon &polygon,int node);

	void outputTree(int node,int depth);

protected:
	InputData *mIn;
	OutputData *mOut;

	bool mCleanDoubleVertexes;
	bool mCleanZeroAreaPolygons;
	bool mOptimizeSplits;
	bool mBalanceTree;
	float mEpsilon;

};

}

#endif
