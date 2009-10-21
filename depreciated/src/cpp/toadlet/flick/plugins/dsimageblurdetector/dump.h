#include "DSIncludes.h"

class CDumpInputPin;
class CDump;
class CDumpFilter;

#define BYTES_PER_LINE 20
#define FIRST_HALF_LINE TEXT("   %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x")
#define SECOND_HALF_LINE TEXT(" %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x")


// Main filter object
interface IImagingFactory;

class CDumpFilter : public CBaseFilter
{
    CDump * const m_pDump;

public:

    // Constructor
    CDumpFilter(CDump *pDump,
                LPUNKNOWN pUnk,
                CCritSec *pLock,
                HRESULT *phr);
    
    // returns setup data for filter registration
    LPAMOVIESETUP_FILTER GetSetupData();

    // Pin enumeration
    CBasePin * GetPin(int n);
    int GetPinCount();
};


class CDump;

//  Pin object
class CDumpInputPin : public CRenderedInputPin
{
    friend class CDump;

    CDump    * const m_pDump;           // Main renderer object
    CCritSec * const m_pReceiveLock;    // Sample critical section
    REFERENCE_TIME m_tLast;             // Last sample receive time
	GUID m_gSubtype;
	int m_iStride;
	BITMAPINFO m_sSrcBitmapInfo;
	BITMAPINFO m_sDstBitmapInfo;
	BYTE *m_bImageData24;
    IImagingFactory *mImagingFactory;

public:

    CDumpInputPin(CDump *pDump,
                  LPUNKNOWN pUnk,
                  CBaseFilter *pFilter,
                  CCritSec *pLock,
                  CCritSec *pReceiveLock,
                  HRESULT *phr);

	virtual ~CDumpInputPin();

    // Do something with this media sample
    STDMETHODIMP Receive(IMediaSample *pSample);
    STDMETHODIMP ReceiveCanBlock();

    // Check if the pin can support this specific proposed type and format
    HRESULT CheckMediaType(const CMediaType *);

	// Get the prefered media type
	HRESULT GetMediaType(int iPosition,CMediaType *pMediaType);

    // Break connection
    HRESULT BreakConnect();

    // Track NewSegment
    STDMETHODIMP NewSegment(REFERENCE_TIME tStart,
                            REFERENCE_TIME tStop,
                            double dRate);
};


//  CDump object which has filter and pin members

class CDump : public CUnknown
{
    friend class CDumpFilter;
    friend class CDumpInputPin;

    CDumpFilter *m_pFilter;         // Methods for filter interfaces
    CDumpInputPin *m_pPin;          // A simple rendered input pin
    CCritSec m_Lock;                // Main renderer critical section
    CCritSec m_ReceiveLock;         // Sublock for received samples
    CPosPassThru *m_pPosition;      // Renderer position controls

public:

    DECLARE_IUNKNOWN

    CDump(LPUNKNOWN pUnk, HRESULT *phr);
    ~CDump();

    static CUnknown * WINAPI CreateInstance(LPUNKNOWN punk, HRESULT *phr);

	virtual CDumpInputPin *GetInputPin();
	virtual int GetImageWidth();
	virtual int GetImageHeight();
	virtual int *GetImageData();
	virtual const GUID &GetConnectedSubtype();

private:

    // Overriden to say what interfaces we support where
    STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void ** ppv);

	int m_iImageWidth;
	int m_iImageHeight;
	int *m_pImageData;
};

