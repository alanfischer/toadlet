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

#ifndef TOADLET_PEEPER_BACKABLEVERTEXFORMAT_H
#define TOADLET_PEEPER_BACKABLEVERTEXFORMAT_H

#include <toadlet/peeper/VertexFormat.h>

namespace toadlet{
namespace peeper{

class TOADLET_API BackableVertexFormat:public VertexFormat{
public:
	TOADLET_SHARED_POINTERS(BackableVertexFormat);

	BackableVertexFormat();
	virtual ~BackableVertexFormat();

	virtual VertexFormat *getRootVertexFormat(){return mBack;}

	virtual void addElement(int semantic,int index,int format);
	virtual bool create();
	virtual void destroy();

	virtual int getNumElements() const{return mSemantics.size();}
	virtual int getSemantic(int i) const{return mSemantics[i];}
	virtual int getIndex(int i) const{return mIndexes[i];}
	virtual int getFormat(int i) const{return mFormats[i];}
	virtual int getOffset(int i) const{return mOffsets[i];}
	virtual int getVertexSize() const{return mVertexSize;}
	virtual int findSemantic(int semantic);

	virtual void setBack(VertexFormat::ptr back);
	virtual VertexFormat::ptr getBack(){return mBack;}
	
protected:
	VertexFormatDestroyedListener *mListener;
	egg::Collection<int> mSemantics;
	egg::Collection<int> mIndexes;
	egg::Collection<int> mFormats;
	egg::Collection<int> mOffsets;
	int mVertexSize;
	
	VertexFormat::ptr mBack;
};

}
}

#endif
