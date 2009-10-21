#include <toadlet/egg/Error.h>
#include "dump.h"
#include <imaging.h>
#include <initguid.h>
#include <imgguids.h>
#include "dumpuids.h"

using namespace toadlet::egg;

HRESULT RGB565_To_RGB24(BYTE* pSrcBits, BITMAPINFO *srcBmi, int srcStride, int srcX, int srcY, int convWidth, int convHeight, BYTE* pDstBits, BITMAPINFO *dstBmi, int dstStride, int dstX, int dstY);
HRESULT UYVY_To_RGB24(BYTE* pSrcBits, BITMAPINFO *srcBmi, int srcStride, int srcX, int srcY, int convWidth, int convHeight, BYTE* pDstBits, BITMAPINFO *dstBmi, int dstStride, int dstX, int dstY);
HRESULT YV12_To_RGB24(BYTE* pSrcBits, BITMAPINFO *srcBmi, int srcStride, int srcX, int srcY, int convWidth, int convHeight, BYTE* pDstBits, BITMAPINFO *dstBmi, int dstStride, int dstX, int dstY);

// Setup data
const AMOVIESETUP_MEDIATYPE sudPinTypes =
{
    &MEDIATYPE_NULL,            // Major type
    &MEDIASUBTYPE_NULL          // Minor type
};

const AMOVIESETUP_PIN sudPins =
{
    L"Input",                   // Pin string name
    FALSE,                      // Is it rendered
    FALSE,                      // Is it an output
    FALSE,                      // Allowed none
    FALSE,                      // Likewise many
    &CLSID_NULL,                // Connects to filter
    L"Output",                  // Connects to pin
    1,                          // Number of types
    &sudPinTypes                // Pin information
};

const AMOVIESETUP_FILTER sudDump =
{
    &CLSID_Dump,                // Filter CLSID
    L"Dump",                    // String name
    MERIT_DO_NOT_USE,           // Filter merit
    1,                          // Number pins
    &sudPins                    // Pin details
};


//
//  Object creation stuff
//
CFactoryTemplate g_Templates[]= {
    L"Dump", &CLSID_Dump, CDump::CreateInstance, NULL, &sudDump
};
int g_cTemplates = 1;


// Constructor

CDumpFilter::CDumpFilter(CDump *pDump,
                         LPUNKNOWN pUnk,
                         CCritSec *pLock,
                         HRESULT *phr) :
    CBaseFilter(NAME("CDumpFilter"), pUnk, pLock, CLSID_Dump),
    m_pDump(pDump)
{
}


LPAMOVIESETUP_FILTER CDumpFilter::GetSetupData() 
{
    return (LPAMOVIESETUP_FILTER) &sudDump;
}


//
// GetPin
//
CBasePin * CDumpFilter::GetPin(int n)
{
    if (n == 0) {
        return m_pDump->m_pPin;
    } else {
        return NULL;
    }
}


//
// GetPinCount
//
int CDumpFilter::GetPinCount()
{
    return 1;
}

//
//  Definition of CDumpInputPin
//

CDumpInputPin::CDumpInputPin(CDump *pDump,
                             LPUNKNOWN pUnk,
                             CBaseFilter *pFilter,
                             CCritSec *pLock,
                             CCritSec *pReceiveLock,
                             HRESULT *phr) :
    CRenderedInputPin((TCHAR *)NAME("CDumpInputPin"),
                  pFilter,                   // Filter
                  pLock,                     // Locking
                  phr,                       // Return code
                  L"Input"),                 // Pin name
    m_pReceiveLock(pReceiveLock),
    m_pDump(pDump),
    m_tLast(0),
	m_bImageData24(NULL),
	mImagingFactory(NULL)
{
}

CDumpInputPin::~CDumpInputPin(){
	if(mImagingFactory!=NULL){
		mImagingFactory->Release();
		mImagingFactory=NULL;
	}

	if(m_bImageData24!=NULL){
		delete[] m_bImageData24;
		m_bImageData24=NULL;
	}
}

//
// CheckMediaType
//
// Check if the pin can support this specific proposed type and format
//
HRESULT CDumpInputPin::CheckMediaType(const CMediaType *media)
{
	if(media->Format()==NULL){
		Logger::log(Logger::TOADLET_FLICK,Logger::Level_WARNING,
			"Invalid format");
		return E_FAIL;
	}

	VIDEOINFO *vi=(VIDEOINFO*)media->Format();

	Logger::log(Logger::TOADLET_FLICK,Logger::Level_DEBUG,
		String("width:")+(int)vi->bmiHeader.biWidth+" height:"+(int)vi->bmiHeader.biHeight+" bitrate:"+(unsigned int)vi->dwBitRate+
		" compression:"+(unsigned int)vi->bmiHeader.biCompression+" bitcount:"+(unsigned int)vi->bmiHeader.biBitCount+" sizeimage:"+(unsigned int)vi->bmiHeader.biSizeImage);

	if(	IsEqualGUID(*media->Subtype(),MEDIASUBTYPE_RGB565)==false &&
		IsEqualGUID(*media->Subtype(),MEDIASUBTYPE_UYVY)==false &&
		IsEqualGUID(*media->Subtype(),MEDIASUBTYPE_YV12)==false &&
		IsEqualGUID(*media->Subtype(),MEDIASUBTYPE_IJPG)==false){
		#if defined(TOADLET_DEBUG)
			wchar_t guid[1024];
			StringFromGUID2(*media->Subtype(),guid,1024);
			Logger::log(Logger::TOADLET_FLICK,Logger::Level_DEBUG,
				String("CheckMediaType failed. Type is:")+guid);
		#endif
		return E_FAIL;
	}

	Logger::log(Logger::TOADLET_FLICK,Logger::Level_DEBUG,
		"CheckMediaType success");

	m_gSubtype=*media->Subtype();

	ZeroMemory(&m_sSrcBitmapInfo,sizeof(m_sSrcBitmapInfo));
	ZeroMemory(&m_sDstBitmapInfo,sizeof(m_sDstBitmapInfo));

	memcpy(m_sSrcBitmapInfo.bmiColors,vi->bmiColors,sizeof(m_sSrcBitmapInfo.bmiColors));
	m_sSrcBitmapInfo.bmiHeader=vi->bmiHeader;

	m_sDstBitmapInfo.bmiHeader.biBitCount=24;
	m_sDstBitmapInfo.bmiHeader.biPlanes=1;
	m_sDstBitmapInfo.bmiHeader.biCompression=BI_RGB;
	m_sDstBitmapInfo.bmiHeader.biWidth=m_sSrcBitmapInfo.bmiHeader.biWidth;
	if(m_sSrcBitmapInfo.bmiHeader.biHeight<0){
		m_sDstBitmapInfo.bmiHeader.biHeight=-m_sSrcBitmapInfo.bmiHeader.biHeight;
		m_iStride=media->GetSampleSize()/-m_sSrcBitmapInfo.bmiHeader.biHeight;
	}
	else{
		m_sDstBitmapInfo.bmiHeader.biHeight=m_sSrcBitmapInfo.bmiHeader.biHeight;
		m_iStride=media->GetSampleSize()/m_sSrcBitmapInfo.bmiHeader.biHeight;
	}
	m_sDstBitmapInfo.bmiHeader.biSize=sizeof(m_sDstBitmapInfo.bmiHeader);
	m_sDstBitmapInfo.bmiHeader.biSizeImage=m_sDstBitmapInfo.bmiHeader.biWidth*m_sDstBitmapInfo.bmiHeader.biHeight*3;

	m_bImageData24=new BYTE[m_sDstBitmapInfo.bmiHeader.biSizeImage];

	m_pDump->m_iImageWidth=m_sDstBitmapInfo.bmiHeader.biWidth;
	m_pDump->m_iImageHeight=m_sDstBitmapInfo.bmiHeader.biHeight;
	if(m_pDump->m_pImageData!=NULL){
		delete[] m_pDump->m_pImageData;
	}
	m_pDump->m_pImageData=new int[m_pDump->m_iImageWidth*m_pDump->m_iImageHeight];

	if(IsEqualGUID(m_gSubtype,MEDIASUBTYPE_IJPG)){
		CoInitializeEx(NULL,COINIT_MULTITHREADED);

		mImagingFactory=NULL;
		HRESULT hr=CoCreateInstance(CLSID_ImagingFactory,NULL,CLSCTX_INPROC_SERVER,IID_IImagingFactory,(void**)&mImagingFactory);
		if(FAILED(hr)){
			Error::unknown(Logger::TOADLET_FLICK,
				"Unable to create Imaging Factory");
			return E_FAIL;
		}
	}

	return S_OK;
}


HRESULT CDumpInputPin::GetMediaType(int iPosition,CMediaType *pMediaType){
	if(iPosition>0){
		return VFW_S_NO_MORE_ITEMS;
	}
	else if(iPosition<0){
		return E_INVALIDARG;
	}

	VIDEOINFO vi;
	memset(&vi,0,sizeof(vi));
	vi.bmiHeader.biWidth=160;
	vi.bmiHeader.biHeight=120;
	vi.bmiHeader.biPlanes=1;
	vi.bmiHeader.biBitCount=16;
	vi.bmiHeader.biCompression=BI_RGB;
	pMediaType->SetFormat((BYTE*)&vi,sizeof(VIDEOINFO));

	pMediaType->SetType(&MEDIATYPE_Video);
	pMediaType->SetFormatType(&Format_VideoInfo);
	pMediaType->SetSubtype(&MEDIASUBTYPE_RGB565);

	return S_OK;
}



//
// BreakConnect
//
// Break a connection
//
HRESULT CDumpInputPin::BreakConnect()
{
    if (m_pDump->m_pPosition != NULL) {
        m_pDump->m_pPosition->ForceRefresh();
    }

	return CRenderedInputPin::BreakConnect();
}


//
// ReceiveCanBlock
//
// We don't hold up source threads on Receive
//
STDMETHODIMP CDumpInputPin::ReceiveCanBlock()
{
    return S_FALSE;
}


//
// Receive
//
// Do something with this media sample
//
STDMETHODIMP CDumpInputPin::Receive(IMediaSample *pSample)
{
    CAutoLock lock(m_pReceiveLock);
    PBYTE pbData;

	REFERENCE_TIME tStart, tStop;
    pSample->GetTime(&tStart, &tStop);
    DbgLog((LOG_TRACE, 1, TEXT("tStart(%s), tStop(%s), Diff(%d ms), Bytes(%d)"),
           (LPCTSTR) CDisp(tStart),
           (LPCTSTR) CDisp(tStop),
           (LONG)((tStart - m_tLast) / 10000),
           pSample->GetActualDataLength()));

    m_tLast = tStart;

    HRESULT hr = pSample->GetPointer(&pbData);
    if (FAILED(hr)) {
        return hr;
    }

	if(IsEqualGUID(m_gSubtype,MEDIASUBTYPE_RGB565)){
		hr=RGB565_To_RGB24(	pbData,&m_sSrcBitmapInfo,0,0,0,m_pDump->m_iImageWidth,m_pDump->m_iImageHeight,
							m_bImageData24,&m_sDstBitmapInfo,0,0,0);
	}
	else if(IsEqualGUID(m_gSubtype,MEDIASUBTYPE_UYVY)){
		hr=UYVY_To_RGB24(	pbData,&m_sSrcBitmapInfo,0,0,0,m_pDump->m_iImageWidth,m_pDump->m_iImageHeight,
							m_bImageData24,&m_sDstBitmapInfo,0,0,0);
	}
	else if(IsEqualGUID(m_gSubtype,MEDIASUBTYPE_YV12)){
		hr=YV12_To_RGB24(	pbData,&m_sSrcBitmapInfo,0,0,0,m_pDump->m_iImageWidth,m_pDump->m_iImageHeight,
							m_bImageData24,&m_sDstBitmapInfo,0,0,0);
	}
	else if(IsEqualGUID(m_gSubtype,MEDIASUBTYPE_IJPG)){
		IImage *image=NULL;
		mImagingFactory->CreateImageFromBuffer(pbData,pSample->GetActualDataLength(),BufferDisposalFlagNone,&image);
		if(image!=NULL){
			IBitmapImage *bitmap=NULL;
			mImagingFactory->CreateBitmapFromImage(image,0,0,PixelFormat24bppRGB,InterpolationHintNearestNeighbor,&bitmap);
			if(bitmap!=NULL){
				RECT rect={0,0,m_pDump->m_iImageWidth,m_pDump->m_iImageHeight};
				BitmapData bitmapData;
				bitmap->LockBits(&rect,ImageLockModeRead,PixelFormatDontCare,&bitmapData);
				int i;
				for(i=0;i<m_pDump->m_iImageHeight;++i){
					memcpy(m_bImageData24+(m_pDump->m_iImageWidth*3*i),((char*)bitmapData.Scan0)+(m_pDump->m_iImageWidth*3*(m_pDump->m_iImageHeight-i-1)),m_pDump->m_iImageWidth*3);
				}
				bitmap->UnlockBits(&bitmapData);
				bitmap->Release();
			}
			image->Release();
		}
		else{
			// Notify it anyways
			m_pDump->m_pFilter->NotifyEvent(EC_CAP_FILE_COMPLETED,0,0);

			return E_FAIL;
		}
	}
	else{
		// Notify it anyways
		m_pDump->m_pFilter->NotifyEvent(EC_CAP_FILE_COMPLETED,0,0);

		return E_FAIL;
	}

	#if defined(TOADLET_DEBUG)
		BITMAPFILEHEADER hdr;

		// Fill in the fields of the file header 
		hdr.bfType		= ((WORD) ('M' << 8) | 'B');	// is always "BM"
		hdr.bfSize		= sizeof(m_sDstBitmapInfo) + (m_sDstBitmapInfo.bmiHeader.biWidth*m_sDstBitmapInfo.bmiHeader.biHeight*3);
		hdr.bfReserved1 	= 0;
		hdr.bfReserved2 	= 0;
		hdr.bfOffBits		= (DWORD)(sizeof(hdr) + m_sDstBitmapInfo.bmiHeader.biSize);

		FILE *fp=fopen("\\ld-test.bmp","wb");
		fwrite(&hdr,sizeof(hdr),1,fp);
		fwrite(&m_sDstBitmapInfo.bmiHeader,sizeof(m_sDstBitmapInfo.bmiHeader),1,fp);
		fwrite(m_bImageData24,m_sDstBitmapInfo.bmiHeader.biWidth*m_sDstBitmapInfo.bmiHeader.biHeight*3,1,fp);
		fclose(fp);
	#endif

	// Convert to 32 bpp
	if(m_pDump->m_pImageData!=NULL){
		int i;
		for(i=0;i<m_pDump->m_iImageWidth*m_pDump->m_iImageHeight;++i){
			m_pDump->m_pImageData[i]=(m_bImageData24[i*3+0]<<16) | (m_bImageData24[i*3+1]<<8) | m_bImageData24[i*3+2];
		}
	}

	m_pDump->m_pFilter->NotifyEvent(EC_CAP_FILE_COMPLETED,0,0);

	return hr;
}


//
// NewSegment
//
// Called when we are seeked
//
STDMETHODIMP CDumpInputPin::NewSegment(REFERENCE_TIME tStart,
                                       REFERENCE_TIME tStop,
                                       double dRate)
{
    m_tLast = 0;
    return S_OK;

} // NewSegment


//
//  CDump class
//
CDump::CDump(LPUNKNOWN pUnk, HRESULT *phr) :
    CUnknown(NAME("CDump"), pUnk),
    m_pFilter(NULL),
    m_pPin(NULL),
    m_pPosition(NULL),
	m_iImageWidth(0),
	m_iImageHeight(0),
	m_pImageData(NULL)
{
    m_pFilter = new CDumpFilter(this, GetOwner(), &m_Lock, phr);
    if (m_pFilter == NULL) {
        *phr = E_OUTOFMEMORY;
        return;
    }

    m_pPin = new CDumpInputPin(this,GetOwner(),
                               m_pFilter,
                               &m_Lock,
                               &m_ReceiveLock,
                               phr);
    if (m_pPin == NULL) {
        *phr = E_OUTOFMEMORY;
        return;
    }
}

// Destructor

CDump::~CDump()
{
    delete m_pPin;
    delete m_pFilter;
    delete m_pPosition;
	delete[] m_pImageData;
}


//
// CreateInstance
//
// Provide the way for COM to create a dump filter
//
CUnknown * WINAPI CDump::CreateInstance(LPUNKNOWN punk, HRESULT *phr)
{
    CDump *pNewObject = new CDump(punk, phr);
    if (pNewObject == NULL) {
        *phr = E_OUTOFMEMORY;
    }
    return pNewObject;

} // CreateInstance


//
// NonDelegatingQueryInterface
//
// Override this to say what interfaces we support where
//
STDMETHODIMP CDump::NonDelegatingQueryInterface(REFIID riid, void ** ppv)
{
    CheckPointer(ppv,E_POINTER);
    CAutoLock lock(&m_Lock);

    // Do we have this interface

	if (riid == IID_IBaseFilter || riid == IID_IMediaFilter || riid == IID_IPersist) {
	return m_pFilter->NonDelegatingQueryInterface(riid, ppv);
    } else if (riid == IID_IMediaPosition || riid == IID_IMediaSeeking) {
        if (m_pPosition == NULL) {

            HRESULT hr = S_OK;
            m_pPosition = new CPosPassThru(NAME("Dump Pass Through"),
                                           (IUnknown *) GetOwner(),
                                           (HRESULT *) &hr, m_pPin);
            if (m_pPosition == NULL) {
                return E_OUTOFMEMORY;
            }

            if (FAILED(hr)) {
                delete m_pPosition;
                m_pPosition = NULL;
                return hr;
            }
        }
        return m_pPosition->NonDelegatingQueryInterface(riid, ppv);
    } else {
	return CUnknown::NonDelegatingQueryInterface(riid, ppv);
    }

} // NonDelegatingQueryInterface

CDumpInputPin *CDump::GetInputPin(){
	return m_pPin;
}

int CDump::GetImageWidth(){
	return m_iImageWidth;
}

int CDump::GetImageHeight(){
	return m_iImageHeight;
}

int *CDump::GetImageData(){
	return m_pImageData;
}

const GUID &CDump::GetConnectedSubtype(){
	// m_pPin cannot be NULL during the life of the CDump
	return m_pPin->m_gSubtype;
}

#define RGB2YUV(r, g, b, y, u, v) \
y = (0.257f*r) + (0.504f*g) + (0.098f*b) + 16; \
u = -(0.148f*r) - (0.291f*g) + (0.439f*b) + 128; \
v = (0.439f*r) - (0.368f*g) - (0.071f*b) + 128;

#define YUV2RGB(y, u, v, r, g, b) \
r = 1.164f*(y - 16) + 1.596f*(v - 128); \
g = 1.164f*(y - 16) - 0.813f*(v - 128) - 0.391f*(u - 128); \
b = 1.164f*(y - 16) + 2.018f*(u - 128)

#define CLAMP(x, lo, hi)		\
	if (x < lo)					\
		x = lo;					\
	if (x > hi)					\
		x = hi;

HRESULT RGB565_To_RGB24(BYTE* pSrcBits, BITMAPINFO *srcBmi, int srcStride, int srcX, int srcY, int convWidth, int convHeight, BYTE* pDstBits, BITMAPINFO *dstBmi, int dstStride, int dstX, int dstY)
{
    // Basic pointer checking
    if (!pSrcBits || !pDstBits)
        return E_POINTER;

    int srcHeight = srcBmi->bmiHeader.biHeight;
    int srcWidth = srcBmi->bmiHeader.biWidth;
    int dstHeight = dstBmi->bmiHeader.biHeight;
    int dstWidth = dstBmi->bmiHeader.biWidth;

    BYTE r, g, b;
    BYTE* ptr;

    // TODO: Today, only RGB with BI_RGB (default bitmasks) is handled.
    if((dstBmi->bmiHeader.biCompression & ~BI_SRCPREROTATE) != BI_RGB)
        return E_NOTIMPL;

    // if the passed in stride is invalid, assume a standard stride
    if(srcStride == 0)
        srcStride = srcWidth * 2;

    // this is a top down bitmap, so swap the direction traveled.
    if(srcHeight > 0)
    {
        srcStride = -srcStride;
        pSrcBits = pSrcBits + ((srcHeight - 1)*abs(srcStride));
    }
    pSrcBits = pSrcBits + srcY*srcStride + srcX*2;

    // if the passed in stride is invalid, assume a standard stride
    if(dstStride == 0)
        dstStride = dstWidth * 3;

    // this is a top down bitmap, so swap the direction traveled.
    if(dstHeight > 0)
    {
        dstStride = -dstStride;
        pDstBits = pDstBits + ((dstHeight - 1)*abs(dstStride));
    }
    pDstBits = pDstBits + dstY*dstStride + dstX*3;

    for(int i = 0; i < convHeight; i++)
    {
        for(int j = 0; j < convWidth; j++)
        {
            // BUGBUG: Split r:5 g:6 b:5
            ptr = pSrcBits + i*srcStride + j*2;

            WORD rgb565 = *(WORD*)ptr;

            r = (rgb565 >> 8) & 0xF8;
            r |= r >> 5;
            g = (rgb565 >> 3) & 0xFC;
            g |= g >> 6;
            b = (rgb565 << 3) & 0xF8;
            b |= b >> 5;

            ptr = (pDstBits + i*dstStride + j*3);
            *ptr = b;
            *(ptr + 1) = g;
            *(ptr + 2) = r;
        }
    }
    return S_OK;
}

HRESULT UYVY_To_RGB24(BYTE* pSrcBits, BITMAPINFO *srcBmi, int srcStride, int srcX, int srcY, int convWidth, int convHeight, BYTE* pDstBits, BITMAPINFO *dstBmi, int dstStride, int dstX, int dstY)
{
    // Basic pointer checking
    if (!pSrcBits || !pDstBits)
        return E_POINTER;

    int srcHeight = srcBmi->bmiHeader.biHeight;
    int srcWidth = srcBmi->bmiHeader.biWidth;
    int dstHeight = dstBmi->bmiHeader.biHeight;
    int dstWidth = dstBmi->bmiHeader.biWidth;

    float r, g, b;
    float y0, u0, v0, y1;

    unsigned char *ptr;
    unsigned char *dstptr;

    // TODO: Today, only RGB with BI_RGB (default bitmasks) is handled.
    if((dstBmi->bmiHeader.biCompression & ~BI_SRCPREROTATE) != BI_RGB)
        return E_NOTIMPL;

    // ignore the orientation of the height for fourcc formats
    srcHeight = abs(srcHeight);

    // if the passed in stride is invalid, assume a standard stride
    if(srcStride == 0)
        srcStride = srcWidth * 2;

    pSrcBits = pSrcBits + srcY*srcStride + srcX*2;

    // if the passed in stride is invalid, assume a standard stride
    if(dstStride == 0)
        dstStride = dstWidth * 3;

    // this is a top down bitmap, so swap the direction traveled.
    if(dstHeight > 0)
    {
        dstStride = -dstStride;
        pDstBits = pDstBits + ((dstHeight - 1)*abs(dstStride));
    }
    pDstBits = pDstBits + dstY*dstStride + dstX*3;

    for(int i = 0; i < convHeight; i++)
    {
        // Iterate over every second pixel since we deal with 2 at a time
        for(int j = 0; j < convWidth; j += 2)
        {
            // Each row has U0Y0V1Y1U2Y2 etc. so that means you have to access two pixels at a time, with four bytes per two pixels.
            // so addressing has to be done as though it's two bytes per pixel.
            ptr = (pSrcBits + i*srcStride + j*2);
            u0 = (float)*ptr;
            y0 = (float)*(ptr + 1);
            v0 = (float)*(ptr + 2);
            y1 = (float)*(ptr + 3);

            // Convert 1st pixel
            YUV2RGB(y0, u0, v0, r, g, b);
    
            CLAMP(r, 0, 255);
            CLAMP(g, 0, 255);
            CLAMP(b, 0, 255);

            dstptr = (pDstBits + i*dstStride + j*3);

            *dstptr = (unsigned char)b;
            *(dstptr + 1) = (unsigned char)g;
            *(dstptr + 2) = (unsigned char)r;

            // Convert 2nd pixel
            YUV2RGB(y1, u0, v0, r, g, b);
            CLAMP(r, 0, 255);
            CLAMP(g, 0, 255);
            CLAMP(b, 0, 255);

            dstptr += 3;

            *dstptr = (unsigned char)b;
            *(dstptr + 1) = (unsigned char)g;
            *(dstptr + 2) = (unsigned char)r;
        }
    }
    return S_OK;
}

HRESULT YV12_To_RGB24(BYTE* pSrcBits, BITMAPINFO *srcBmi, int srcStride, int srcX, int srcY, int convWidth, int convHeight, BYTE* pDstBits, BITMAPINFO *dstBmi, int dstStride, int dstX, int dstY)
{
    // Basic pointer checking
    if (!pSrcBits || !pDstBits)
        return E_POINTER;

    int srcHeight = srcBmi->bmiHeader.biHeight;
    int srcWidth = srcBmi->bmiHeader.biWidth;
    int dstHeight = dstBmi->bmiHeader.biHeight;
    int dstWidth = dstBmi->bmiHeader.biWidth;

    float r, g, b;
    float y0, u0, v0;

    unsigned char *yptr;
    unsigned char *uptr;
    unsigned char *vptr;
    unsigned char* dstptr;

    // TODO: Today, only RGB with BI_RGB (default bitmasks) is handled.
    if((dstBmi->bmiHeader.biCompression & ~BI_SRCPREROTATE) != BI_RGB)
        return E_NOTIMPL;

    // ignore the orientation of the height for fourcc formats
    srcHeight = abs(srcHeight);

    // if the passed in stride is invalid, assume a standard stride
    if(srcStride == 0)
        srcStride = srcWidth;
    int uvsrcStride = srcStride/2;

    BYTE * yptrStart = pSrcBits;
    BYTE * vptrStart = yptrStart + srcHeight*srcStride;
    BYTE * uptrStart = vptrStart + (srcHeight/2)*uvsrcStride;
    BYTE *pRGBBits;

    // if the passed in stride is invalid, assume a standard stride
    if(dstStride == 0)
        dstStride = dstWidth * 3;

    // this is a top down bitmap, so swap the direction traveled.
    if(dstHeight > 0)
    {
        dstStride = -dstStride;
        pDstBits = pDstBits + ((dstHeight - 1)*abs(dstStride));
    }
    pRGBBits = pDstBits + dstY*dstStride + dstX*3;

    for(int i = 0; i < convHeight; i++)
    {
        for(int j = 0; j < convWidth; j++)
        {
            // YV12 is planar
            // First the srcWidth*srcHeight Y plane followed by (srcWidth/2)*(srcHeight/2) u plane and then the v plane
            yptr = yptrStart + (srcY + i)*srcStride + (srcX + j);
            vptr = vptrStart + ((srcY + i)/2)*uvsrcStride + (j/2);
            uptr = uptrStart + ((srcY + i)/2)*uvsrcStride + (j/2);

            y0 = (float)*yptr;
            v0 = (float)*vptr;
            u0 = (float)*uptr;

            // Convert 1st pixel
            YUV2RGB(y0, u0, v0, r, g, b);

            CLAMP(r, 0, 255);
            CLAMP(g, 0, 255);
            CLAMP(b, 0, 255);

            dstptr = pRGBBits + i*dstStride + j*3;

            *dstptr = (unsigned char)b;
            *(dstptr + 1) = (unsigned char)g;
            *(dstptr + 2) = (unsigned char)r;
        }
    }
    return S_OK;
}
