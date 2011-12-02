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

#ifndef TOADLET_TADPOLE_MATERIAL_MATERIAL_H
#define TOADLET_TADPOLE_MATERIAL_MATERIAL_H

#include <toadlet/egg/BaseResource.h>
#include <toadlet/tadpole/material/RenderPath.h>

namespace toadlet{
namespace tadpole{
namespace material{

class TOADLET_API Material:public BaseResource{
public:
	TOADLET_SHARED_POINTERS(Material);

	enum SortType{
		SortType_AUTO,
		SortType_MATERIAL,
		SortType_DEPTH,
	};

	enum Scope{
		Scope_MATERIAL	=1<<0,
		Scope_RENDERABLE=1<<1,
	};

	Material(MaterialManager *manager,Material *source=NULL,bool clone=false);
	virtual ~Material();

	void destroy();

	RenderPath::ptr addPath();
	inline int getNumPaths() const{return mPaths.size();}
	inline RenderPath::ptr getPath(int i) const{return mPaths[i];}
	inline void setBestPath(RenderPath::ptr path){mBestPath=path;}
	RenderPath::ptr getBestPath(){return mBestPath;}
	RenderPass::ptr getPass(int path=-1,int pass=-1);

	/// @todo: Move SortType to the RenderPath at least, so some paths may be sorted, and others not
	inline void setSort(SortType sort){mSort=sort;}
	inline SortType getSort() const{return mSort;}

	inline void setLayer(int layer){mLayer=layer;}
	inline int getLayer() const{return mLayer;}

	bool isDepthSorted() const;

	RenderPath::ptr findFixedPath();

	void shareStates(Material *source);

	bool compile();

protected:
	MaterialManager *mManager;
	Collection<RenderPath::ptr> mPaths;
	RenderPath::ptr mBestPath;
	SortType mSort;
	int mLayer;
};

}
}
}

#endif