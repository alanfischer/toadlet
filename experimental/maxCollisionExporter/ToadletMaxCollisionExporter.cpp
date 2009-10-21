/**********************************************************************
 *<
	FILE: ToadletMaxCollisionExporter.cpp

	DESCRIPTION:	Appwizard generated plugin

	CREATED BY: 

	HISTORY: 

 *>	Copyright (c) 2000, All Rights Reserved.
 **********************************************************************/

#include "CollisionExport.h"
#include "ToadletMaxCollisionExporter.h"

#include <stdexcept>
#include <shlobj.h>

#define TOADLETMAXCOLLISIONEXPORTER_CLASS_ID	Class_ID(0x667e8532, 0x233510b0)

class ToadletMaxCollisionExporter : public SceneExport {
	public:
		static HWND hParams;

		int				ExtCount();					// Number of extensions supported
		const TCHAR *	Ext(int n);					// Extension #n (i.e. "3DS")
		const TCHAR *	LongDesc();					// Long ASCII description (i.e. "Autodesk 3D Studio File")
		const TCHAR *	ShortDesc();				// Short ASCII description (i.e. "3D Studio")
		const TCHAR *	AuthorName();				// ASCII Author name
		const TCHAR *	CopyrightMessage();			// ASCII Copyright message
		const TCHAR *	OtherMessage1();			// Other message #1
		const TCHAR *	OtherMessage2();			// Other message #2
		unsigned int	Version();					// Version number * 100 (i.e. v3.01 = 301)
		void			ShowAbout(HWND hWnd);		// Show DLL's "About..." box

		BOOL			SupportsOptions(int ext, DWORD options);
		int				DoExport(const TCHAR *name,ExpInterface *ei,Interface *i, BOOL suppressPrompts=FALSE, DWORD options=0);

		//Constructor/Destructor

		ToadletMaxCollisionExporter();
		~ToadletMaxCollisionExporter();		
};


class ToadletMaxCollisionExporterClassDesc:public ClassDesc2 {
	public:
	int 			IsPublic() { return TRUE; }
	void *			Create(BOOL loading = FALSE) { return new ToadletMaxCollisionExporter(); }
	const TCHAR *	ClassName() { return GetString(IDS_CLASS_NAME); }
	SClass_ID		SuperClassID() { return SCENE_EXPORT_CLASS_ID; }
	Class_ID		ClassID() { return TOADLETMAXCOLLISIONEXPORTER_CLASS_ID; }
	const TCHAR* 	Category() { return GetString(IDS_CATEGORY); }

	const TCHAR*	InternalName() { return _T("ToadletMaxCollisionExporter"); }	// returns fixed parsable name (scripter-visible name)
	HINSTANCE		HInstance() { return hInstance; }				// returns owning module handle
};



static ToadletMaxCollisionExporterClassDesc ToadletMaxCollisionExporterDesc;
ClassDesc2* GetToadletMaxCollisionExporterDesc() { return &ToadletMaxCollisionExporterDesc; }


BOOL CALLBACK ToadletMaxCollisionExporterOptionsDlgProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam) {
	static ExportOptions *opt = NULL;

	switch(message) {
		case WM_INITDIALOG:
			opt = (ExportOptions *)lParam;
			CenterWindow(hWnd,GetParent(hWnd));
			CheckDlgButton(hWnd, IDC_ROOTTRANSFORM, opt->mRootTransform);
			return TRUE;

		case WM_CLOSE:
			EndDialog(hWnd, 0);
			return TRUE;

		case WM_COMMAND:
			switch (LOWORD(wParam)) {
			case IDOK:
				opt->mRootTransform=IsDlgButtonChecked(hWnd, IDC_ROOTTRANSFORM);
				EndDialog(hWnd, 1);
				return TRUE;

			case IDCANCEL:
				EndDialog(hWnd, 0);
				return FALSE;
			}
	}
	return FALSE;
}


//--- ToadletMaxCollisionExporter -------------------------------------------------------
ToadletMaxCollisionExporter::ToadletMaxCollisionExporter()
{

}

ToadletMaxCollisionExporter::~ToadletMaxCollisionExporter() 
{

}

int ToadletMaxCollisionExporter::ExtCount()
{
	// Returns the number of file name extensions supported by the plug-in.
	return 1;
}

const TCHAR *ToadletMaxCollisionExporter::Ext(int n)
{
	// Return the 'i-th' file name extension (i.e. "3DS").
	return _T("xcol");
}

const TCHAR *ToadletMaxCollisionExporter::LongDesc()
{
	// Return long ASCII description (i.e. "Targa 2.0 Image File")
	return _T("Toadlet Collision File");
}
	
const TCHAR *ToadletMaxCollisionExporter::ShortDesc() 
{			
	// Return short ASCII description (i.e. "Targa")
	return _T("Toadlet Collision");
}

const TCHAR *ToadletMaxCollisionExporter::AuthorName()
{			
	// Return ASCII Author name
	return _T("Andrew Fischer");
}

const TCHAR *ToadletMaxCollisionExporter::CopyrightMessage() 
{	
	// Return ASCII Copyright message
	return _T("Copyright Lightning Toads Productions, LLC");
}

const TCHAR *ToadletMaxCollisionExporter::OtherMessage1() 
{		
	// Return Other message #1 if any
	return _T("");
}

const TCHAR *ToadletMaxCollisionExporter::OtherMessage2() 
{		
	// Return other message #2 in any
	return _T("");
}

unsigned int ToadletMaxCollisionExporter::Version()
{				
	// Return Version number * 100 (i.e. v3.01 = 301)
	return 100;
}

void ToadletMaxCollisionExporter::ShowAbout(HWND hWnd)
{			
	// Optional
}

BOOL ToadletMaxCollisionExporter::SupportsOptions(int ext, DWORD options)
{
	// Decide which options to support.  Simply return
	// true for each option supported by each Extension 
	// the exporter supports.

	return TRUE;
}


int	ToadletMaxCollisionExporter::DoExport(const TCHAR *name,ExpInterface *ei,Interface *i, BOOL suppressPrompts, DWORD options)
{
	ExportOptions *opt = new ExportOptions();
	CollisionExport *export=new CollisionExport();
	export->mOptions = opt;

	if(!suppressPrompts){
		int result=DialogBoxParam(hInstance,MAKEINTRESOURCE(IDD_PANEL),GetActiveWindow(),ToadletMaxCollisionExporterOptionsDlgProc,(LPARAM)opt);
		if(result==0){
			return 0;
		}
	}

	int success=0;
#	ifndef TOADLET_DEBUG
		try{
#	endif
			success=export->doExport(name,ei,i,suppressPrompts,options);
#	ifndef TOADLET_DEBUG
		}
		catch(std::exception &e){
			MessageBox(GetActiveWindow(),e.what(),"Assert failure",MB_OK);
		}
		catch(...){
			MessageBox(GetActiveWindow(),"Major internal error","Assert failure",MB_OK);
		}
#	endif

	delete export;
	delete opt;

	return success;
}
