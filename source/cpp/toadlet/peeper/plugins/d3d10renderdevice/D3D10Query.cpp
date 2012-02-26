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

#include "D3D10Query.h"
#include "D3D10RenderDevice.h"
#include <toadlet/egg/Logger.h>

namespace toadlet{
namespace peeper{

D3D10Query::D3D10Query(D3D10RenderDevice *renderDevice):
	mDevice(NULL),
	mQueryType(QueryType_UNKNOWN),
	mQuery(NULL)
{
	mDevice=renderDevice;
}

D3D10Query::~D3D10Query(){
	destroy();
}

bool D3D10Query::create(QueryType queryType){
	mQueryType=queryType;
	
	D3D10_QUERY_DESC desc;
	desc.Query=getD3D10_QUERY(mQueryType);
	desc.MiscFlags=0;
	
	HRESULT result=mDevice->getD3D10Device()->CreateQuery(&desc,&mQuery);
	TOADLET_CHECK_D3D10ERROR(result,"D3D10Query: CreateQuery");

	return true;
}

void D3D10Query::destroy(){
	if(mQuery!=NULL){
		mQuery->Release();
		mQuery=NULL;
	}

	BaseResource::destroy();
}

void D3D10Query::beginQuery(){
	mQuery->Begin();
}

void D3D10Query::endQuery(){
	mQuery->End();
}

uint64 D3D10Query::getResult(){
	UINT64 data=0;
	HRESULT result=S_FALSE;
	while(result==S_FALSE){
		result=mQuery->GetData(&data,sizeof(UINT64),0);
	}
	return data;
}

D3D10_QUERY D3D10Query::getD3D10_QUERY(QueryType queryType){
	switch(queryType){
		case QueryType_FINISHED:
			return D3D10_QUERY_EVENT;
		case QueryType_OCCLUSION:
			return D3D10_QUERY_OCCLUSION;
		default:
			Logger::error(Categories::TOADLET_PEEPER,
				"D3D10Query::getD3D10_QUERY: Invalid queryType");
			return (D3D10_QUERY)0;
		break;
	}
}

}
}
