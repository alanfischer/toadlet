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

#include "D3D9Query.h"
#include "D3D9RenderDevice.h"
#include <toadlet/egg/Logger.h>

namespace toadlet{
namespace peeper{

D3D9Query::D3D9Query(D3D9RenderDevice *renderDevice):
	mDevice(NULL),
	mQueryType(QueryType_UNKNOWN),
	mQuery(NULL)
{
	mDevice=renderDevice;
}

bool D3D9Query::create(QueryType queryType){
	mQueryType=queryType;
	D3DQUERYTYPE d3dQueryType=getD3DQUERYTYPE(mQueryType);

	HRESULT result=mDevice->getDirect3DDevice9()->CreateQuery(d3dQueryType,&mQuery);
	TOADLET_CHECK_D3D9ERROR(result,"D3D9Query: CreateQuery");

	return true;
}

void D3D9Query::destroy(){
	if(mQuery!=NULL){
		mQuery->Release();
		mQuery=NULL;
	}

	BaseResource::destroy();
}

void D3D9Query::beginQuery(){
	mQuery->Issue(D3DISSUE_BEGIN);
}

void D3D9Query::endQuery(){
	mQuery->Issue(D3DISSUE_END);
}

uint64 D3D9Query::getResult(){
	DWORD data=0;
	HRESULT result=S_FALSE;
	while(result==S_FALSE){
		result=mQuery->GetData(&data,sizeof(DWORD),D3DGETDATA_FLUSH);
	}
	return data;
}

D3DQUERYTYPE D3D9Query::getD3DQUERYTYPE(QueryType queryType){
	switch(queryType){
		case QueryType_FINISHED:
			return D3DQUERYTYPE_EVENT;
		case QueryType_OCCLUSION:
			return D3DQUERYTYPE_OCCLUSION;
		default:
			Logger::error(Categories::TOADLET_PEEPER,
				"D3D9Query::getD3DQUERYTYPE: Invalid queryType");
			return (D3DQUERYTYPE)0;
		break;
	}
}

}
}
