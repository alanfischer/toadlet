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

#include <toadlet/egg/Collection.h>
#include <toadlet/peeper/VertexFormat.h>

namespace toadlet{
namespace peeper{

class TOADLET_API BackableVertexFormat:public VertexFormat{
public:
	TOADLET_SHARED_POINTERS(BackableVertexFormat);

	BackableVertexFormat();
	virtual ~BackableVertexFormat();

	virtual VertexFormat *getRootVertexFormat(){return mBack;}

	virtual void setVertexFormatDestroyedListener(VertexFormatDestroyedListener *listener){mListener=listener;}

	virtual bool create();
	virtual void destroy();

	virtual bool addElement(int semantic,int index,int format){return addElement(semantic,(char*)NULL,index,format);}
	virtual bool addElement(const String &name,int index,int format){return addElement(Semantic_UNKNOWN,name,index,format);}
	virtual bool addElement(int semantic,const String &name,int index,int format);
	virtual int getNumElements() const{return mSemantics.size();}
	virtual int getSemantic(int i) const{return mSemantics[i];}
	virtual String getName(int i) const{return mNames[i];}
	virtual int getIndex(int i) const{return mIndexes[i];}
	virtual int getFormat(int i) const{return mFormats[i];}
	virtual int getOffset(int i) const{return mOffsets[i];}
	virtual int getVertexSize() const{return mVertexSize;}
	virtual int findElement(int semantic);
	virtual int findElement(const String &name);

	virtual void setBack(VertexFormat::ptr back);
	virtual VertexFormat::ptr getBack(){return mBack;}
	
	static int getSemanticFromName(const String &name);
	static String getNameFromSemantic(int semantic);

protected:
	VertexFormatDestroyedListener *mListener;
	Collection<int> mSemantics;
	Collection<String> mNames;
	Collection<int> mIndexes;
	Collection<int> mFormats;
	Collection<int> mOffsets;
	int mVertexSize;
	
	VertexFormat::ptr mBack;
};

}
}

#endif
