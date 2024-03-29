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

#include "D3D9WindowRenderTarget.h"

namespace toadlet{
namespace peeper{

#if defined(TOADLET_SET_D3DM)
	TOADLET_C_API RenderTarget *new_D3DMWindowRenderTarget(void *display,void *window,WindowRenderTargetFormat *format){
		return new D3D9WindowRenderTarget((HWND)window,format);
	}
#else
	TOADLET_C_API RenderTarget *new_D3D9WindowRenderTarget(void *display,void *window,WindowRenderTargetFormat *format){
		return new D3D9WindowRenderTarget((HWND)window,format);
	}
#endif

D3D9WindowRenderTarget::D3D9WindowRenderTarget():D3D9RenderTarget(),
	mSamples(0),
	mD3DLibrary(0),mD3DXLibrary(0),
	mD3D(NULL),
	mD3DDevice(NULL),
	mColorSurface(NULL),
	mDepthSurface(NULL),
	mWindow(0)
{
}

D3D9WindowRenderTarget::D3D9WindowRenderTarget(HWND wnd,WindowRenderTargetFormat *format):D3D9RenderTarget(),
	mSamples(0),
	mD3DLibrary(0),mD3DXLibrary(0),
	mD3DDevice(NULL),
	mColorSurface(NULL),
	mDepthSurface(NULL),
	mWindow(0)
{
	createContext(wnd,format);
}

void D3D9WindowRenderTarget::destroy(){
	destroyContext();
	
	BaseResource::destroy();
}

bool D3D9WindowRenderTarget::activate(){
	#if defined(TOADLET_SET_D3DM)
		HRESULT result=mD3DDevice->SetRenderTarget(mColorSurface,mDepthSurface);
		TOADLET_CHECK_D3D9ERROR(result,"SetRenderTarget");
	#else
		HRESULT result=mD3DDevice->SetRenderTarget(0,mColorSurface);
		TOADLET_CHECK_D3D9ERROR(result,"SetRenderTarget");

		result=mD3DDevice->SetDepthStencilSurface(mDepthSurface);
		TOADLET_CHECK_D3D9ERROR(result,"SetDepthStencilSurface");
	#endif

	return true;
}

bool D3D9WindowRenderTarget::deactivate(){
	#if !defined(TOADLET_SET_D3DM)
		// Can't assign NULL to COLOR_0, so just NULL DEPTH_STENCIL
		HRESULT result=mD3DDevice->SetDepthStencilSurface(NULL);
		TOADLET_CHECK_D3D9ERROR(result,"SetDepthStencilSurface");
	#endif

	return true;
}

void D3D9WindowRenderTarget::swap(){
	HRESULT result=mD3DDevice->Present(NULL,NULL,NULL,NULL);
	TOADLET_CHECK_D3D9ERROR(result,"Present");
}

void D3D9WindowRenderTarget::reset(){
	if(mColorSurface!=NULL){
		mColorSurface->Release();
		mColorSurface=NULL;
	}

	if(mDepthSurface!=NULL){
		mDepthSurface->Release();
		mDepthSurface=NULL;
	}

	mPresentParameters.MultiSampleType=(D3DMULTISAMPLE_TYPE)getClosestSamples(mSamples);
	#if !defined(TOADLET_SET_D3DM)
		mPresentParameters.BackBufferWidth	=getWidth();
		mPresentParameters.BackBufferHeight	=getHeight();
	#endif

	HRESULT result=mD3DDevice->Reset(&mPresentParameters);
	TOADLET_CHECK_D3D9ERROR(result,"Reset");

	#if defined(TOADLET_SET_D3DM)
		mD3DDevice->GetRenderTarget(&mColorSurface);
	#else
		mD3DDevice->GetRenderTarget(0,&mColorSurface);
	#endif
	mD3DDevice->GetDepthStencilSurface(&mDepthSurface);
}

bool D3D9WindowRenderTarget::createContext(HWND wnd,WindowRenderTargetFormat *format){
	HRESULT result;

	Log::alert(Categories::TOADLET_PEEPER,
		"D3D9WindowRenderTarget::createContext");

	char *d3dName=format->debug?TOADLET_D3D9_DEBUG_DLL_NAME:TOADLET_D3D9_DLL_NAME;
	mD3DLibrary=LoadLibrary(d3dName);
	if(mD3DLibrary==0){
		Error::libraryNotFound(Categories::TOADLET_PEEPER,
			String("D3D9WindowRenderTarget: Error loading ")+d3dName);
		return false;
	}

	char *createName=TOADLET_D3D9_CREATE_NAME;
	Direct3DCreate9=(Direct3DCreate9_)GetProcAddress(mD3DLibrary,createName);
	if(Direct3DCreate9==NULL){
		Error::symbolNotFound(Categories::TOADLET_PEEPER,
			String("D3D9WindowRenderTarget: Error finding ")+createName);
		return NULL;
	}

	String d3dxName;
	// 43 is most recent, and use 37 instead of 24 as minimum, since we get:
	//  HLSL error: error X5796: Dest register type for LRP must be temp (r#).
	//  DLL: D3DX9_36.DLL
	//  Driver:igdumdx32.dll 
	//  Description:Intel(R) HD Graphics Family 
	int maxD3DX=43,minD3DX=37;
	int i;
	for(i=maxD3DX;i>=minD3DX && mD3DXLibrary==0;i--){
		d3dxName=(format->debug?String("D3DX9D_"):String("D3DX9_"))+i+".DLL";
		mD3DXLibrary=LoadLibrary(d3dxName);
		if(mD3DXLibrary!=0){
			char *compileShaderName=TOADLET_D3DX9_COMPILE_SHADER_NAME;
			D3DXCompileShader=(D3DXCompileShader_)GetProcAddress(mD3DXLibrary,compileShaderName);
			if(D3DXCompileShader==NULL){
				Log::warning(Categories::TOADLET_PEEPER,
					String("D3D9WindowRenderTarget: Error finding ")+compileShaderName);
			}
		}
	}
	if(mD3DXLibrary==0){
		Log::warning(Categories::TOADLET_PEEPER,
			"D3D9WindowRenderTarget: Error loading "+d3dxName);
	}
	else{
		Log::alert(Categories::TOADLET_PEEPER,
			"Found "+d3dxName);
	}

	mD3D=Direct3DCreate9(D3D_SDK_VERSION);
	if(mD3D==NULL){
		Error::unknown(Categories::TOADLET_PEEPER,
			"D3D9WindowRenderTarget: Error creating Direct3D object");
		return false;
	}

	mAdaptor=D3DADAPTER_DEFAULT;
	D3DADAPTER_IDENTIFIER9 identifier={0};
	mD3D->GetAdapterIdentifier(mAdaptor,0,&identifier);

	mAdaptorInfo=new AdaptorInfo();
	mAdaptorInfo->setDescription(identifier.Description);

	Log::alert(Categories::TOADLET_PEEPER,
		String("D3D Driver:") + identifier.Driver);
	Log::alert(Categories::TOADLET_PEEPER,
		String("D3D Description:") + identifier.Description);

	#if defined(TOADLET_SET_D3DM)
		mDevType=D3DMDEVTYPE_DEFAULT;
	#else
		mDevType=D3DDEVTYPE_HAL;
	#endif
	
	int flags=format->flags;

	#if !defined(TOADLET_SET_D3DM)
		result=mD3D->CheckDeviceType(mAdaptor,mDevType,D3DFMT_X8R8G8B8,D3DFMT_X8R8G8B8,FALSE);
		if(FAILED(result)){
			Error::unknown(Categories::TOADLET_PEEPER,
				"D3D9WindowRenderTarget: Error creating 8,8,8 bit back buffer");
			return false;
		}

		result=mD3D->CheckDeviceFormat(mAdaptor,mDevType,D3DFMT_X8R8G8B8,D3DUSAGE_DEPTHSTENCIL,D3DRTYPE_SURFACE,D3DFMT_D24S8);
		if(FAILED(result)){
			Error::unknown(Categories::TOADLET_PEEPER,
				"D3D9WindowRenderTarget: Error creating 16 bit depth, 8 bit stencil back buffer");
			return false;
		}

		D3DCAPS9 caps;
		ZeroMemory(&caps,sizeof(D3DCAPS9));
		result=mD3D->GetDeviceCaps(mAdaptor,mDevType,&caps);
		if(FAILED(result)){
			Error::unknown(Categories::TOADLET_PEEPER,
				"D3D9WindowRenderTarget: Error getting device caps");
			return false;
		}

		if((flags&(D3DCREATE_HARDWARE_VERTEXPROCESSING|D3DCREATE_SOFTWARE_VERTEXPROCESSING))==0){
			if(caps.VertexProcessingCaps!=0){
				flags|=D3DCREATE_HARDWARE_VERTEXPROCESSING;
			}
			else{
				flags|=D3DCREATE_SOFTWARE_VERTEXPROCESSING;
			}
		}
	#endif

	if(format->threads>1){
		flags|=D3DCREATE_MULTITHREADED;
	}

	mSamples=format->multisamples;
	mWindow=wnd;

	D3DPRESENT_PARAMETERS *presentParameters=&mPresentParameters;
	memset(presentParameters,0,sizeof(D3DPRESENT_PARAMETERS));
	#if defined(TOADLET_SET_D3DM)
		presentParameters->AutoDepthStencilFormat=D3DMFMT_D16;
		presentParameters->EnableAutoDepthStencil=TRUE;
		presentParameters->Windowed			=TRUE;
		presentParameters->SwapEffect		=D3DMSWAPEFFECT_DISCARD;
		presentParameters->BackBufferFormat	=D3DMFMT_UNKNOWN;
	#else
		presentParameters->AutoDepthStencilFormat=D3DFMT_D24S8;
		presentParameters->EnableAutoDepthStencil=TRUE;
		presentParameters->Windowed			=TRUE;
		presentParameters->SwapEffect		=D3DSWAPEFFECT_DISCARD;
		presentParameters->BackBufferWidth	=getWidth();
		presentParameters->BackBufferHeight	=getHeight();
		presentParameters->BackBufferFormat	=D3DFMT_X8R8G8B8;
		if(format->vsync){
			presentParameters->PresentationInterval=D3DPRESENT_INTERVAL_DEFAULT;
		}
		else{
			presentParameters->PresentationInterval=D3DPRESENT_INTERVAL_IMMEDIATE;
		}
		presentParameters->MultiSampleType=(D3DMULTISAMPLE_TYPE)getClosestSamples(mSamples);
	#endif

	result=mD3D->CreateDevice(mAdaptor,mDevType,wnd,flags,&mPresentParameters,&mD3DDevice);
	if(FAILED(result)){
		TOADLET_CHECK_D3D9ERROR(result,"CreateDevice");
		return false;
	}

	#if defined(TOADLET_SET_D3DM)
		mD3DDevice->GetRenderTarget(&mColorSurface);
	#else
		mD3DDevice->GetRenderTarget(0,&mColorSurface);
	#endif
	mD3DDevice->GetDepthStencilSurface(&mDepthSurface);

	return true;
}

bool D3D9WindowRenderTarget::destroyContext(){
	if(mColorSurface!=NULL){
		mColorSurface->Release();
		mColorSurface=NULL;
	}

	if(mDepthSurface!=NULL){
		mDepthSurface->Release();
		mDepthSurface=NULL;
	}

	if(mD3DDevice!=NULL){
		mD3DDevice->Release();
		mD3DDevice=NULL;
	}

	if(mD3D!=NULL){
		mD3D->Release();
		mD3D=NULL;
	}

	if(mD3DLibrary!=0){
		FreeLibrary(mD3DLibrary);
		mD3DLibrary=0;
	}

	if(mD3DXLibrary!=0){
		FreeLibrary(mD3DXLibrary);
		mD3DXLibrary=0;
	}

	return true;
}

int D3D9WindowRenderTarget::getClosestSamples(int samples){
	#if !defined(TOADLET_SET_D3DM)
		if(samples>0){
			HRESULT result;
			do{
				result=mD3D->CheckDeviceMultiSampleType(mAdaptor,mDevType,mPresentParameters.BackBufferFormat,mPresentParameters.Windowed,(D3DMULTISAMPLE_TYPE)samples,NULL);
			}while(FAILED(result) && --samples>0);
		}
	#endif
	return samples;
}

}
}
