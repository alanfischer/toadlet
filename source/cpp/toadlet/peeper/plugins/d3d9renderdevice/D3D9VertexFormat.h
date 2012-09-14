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

#ifndef TOADLET_PEEPER_D3D9VERTEXFORMAT_H
#define TOADLET_PEEPER_D3D9VERTEXFORMAT_H

#include "D3D9Includes.h"
#include <toadlet/egg/BaseResource.h>
#include <toadlet/peeper/VertexFormat.h>

namespace toadlet{
namespace peeper{

class D3D9RenderDevice;

class TOADLET_API D3D9VertexFormat:public BaseResource,public VertexFormat{
public:
	TOADLET_RESOURCE(D3D9VertexFormat,VertexFormat);

	D3D9VertexFormat(D3D9RenderDevice *renderDevice);

	VertexFormat *getRootVertexFormat(){return this;}

	bool addElement(int semantic,int index,int format){return addElement(semantic,(char*)NULL,index,format);}
	bool addElement(const String &name,int index,int format){return addElement(Semantic_UNKNOWN,name,index,format);}
	bool addElement(int semantic,const String &name,int index,int format);
	bool create();
	void destroy();

	int getNumElements() const{return mSemantics.size();}
	int getElementSemantic(int i) const{return mSemantics[i];}
	String getElementName(int i) const{return mNames[i];}
	int getElementIndex(int i) const{return mIndexes[i];}
	int getElementFormat(int i) const{return mFormats[i];}
	int getElementOffset(int i) const{return mOffsets[i];}
	int findElement(int semantic);
	int findElement(const String &name);
	int getVertexSize() const{return mVertexSize;}

	inline bool getContextCreated(){return mContextCreated;}

	inline DWORD getFVF(){return mFVF;}

	#if !defined(TOADLET_SET_D3DM)
		inline IDirect3DVertexDeclaration9 *getDeclaration(){return mDeclaration;}

		static BYTE getD3DDECLTYPE(int format);
		static BYTE getD3DDECLUSAGE(int semantic);
	#endif

protected:
	bool createContext();
	bool destroyContext();

	D3D9RenderDevice *mDevice;
	Collection<int> mSemantics;
	Collection<String> mNames;
	Collection<int> mIndexes;
	Collection<int> mFormats;
	Collection<int> mOffsets;
	int mVertexSize;

	bool mContextCreated;
	DWORD mFVF;
	#if !defined(TOADLET_SET_D3DM)
		D3DVERTEXELEMENT9 *mElements;
		IDirect3DVertexDeclaration9 *mDeclaration;
	#endif

	friend class D3D9RenderDevice;
	friend class D3D9VertexBuffer;
};

}
}

#endif
