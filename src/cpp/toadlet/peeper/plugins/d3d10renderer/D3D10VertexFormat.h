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

#ifndef TOADLET_PEEPER_D3D10VERTEXFORMAT_H
#define TOADLET_PEEPER_D3D10VERTEXFORMAT_H

#include <toadlet/peeper/VertexFormat.h>
#include "D3D10Includes.h"

namespace toadlet{
namespace peeper{

class D3D10Renderer;

class TOADLET_API D3D10VertexFormat:public VertexFormat{
public:
	D3D10VertexFormat(D3D10Renderer *renderer);
	virtual ~D3D10VertexFormat();

	VertexFormat *getRootVertexFormat(){return this;}

	void setVertexFormatDestroyedListener(VertexFormatDestroyedListener *listener){mListener=listener;}

	void addElement(int semantic,int index,int format);
	bool create();
	void destroy();

	int getNumElements() const{return mSemantics.size();}
	int getSemantic(int i) const{return mSemantics[i];}
	int getIndex(int i) const{return mIndexes[i];}
	int getFormat(int i) const{return mFormats[i];}
	int getOffset(int i) const{return mOffsets[i];}
	int findSemantic(int semantic);
	int getVertexSize() const{return mVertexSize;}

protected:
	bool createContext();
	bool destroyContext();

	D3D10Renderer *mRenderer;

	VertexFormatDestroyedListener *mListener;
	egg::Collection<int> mSemantics;
	egg::Collection<int> mIndexes;
	egg::Collection<int> mFormats;
	egg::Collection<int> mOffsets;
	int mVertexSize;

	D3D10_INPUT_ELEMENT_DESC *mElements;
	ID3D10InputLayout *mLayout;

	friend class D3D10Renderer;
};

}
}

#endif
