/**********************************************************************
 *<
	FILE: ToadletMaxMAPExporter.cpp

	DESCRIPTION:	Appwizard generated plugin

	CREATED BY: 

	HISTORY: 

 *>	Copyright (c) 2000, All Rights Reserved.
 **********************************************************************/

#include "MAPExport.h"
#include "ToadletMaxMAPExporter.h"

#include <stdexcept>
#include <shlobj.h>

#define TOADLETMAXMAPEXPORTER_CLASS_ID	Class_ID(0x667e8533, 0x233510b0)

class ToadletMaxMAPExporter : public SceneExport {
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

		toadlet::egg::String getTexturePath();
		void setTexturePath(const toadlet::egg::String &path);

		//Constructor/Destructor

		ToadletMaxMAPExporter();
		~ToadletMaxMAPExporter();		
};


class ToadletMaxMAPExporterClassDesc:public ClassDesc2 {
	public:
	int 			IsPublic() { return TRUE; }
	void *			Create(BOOL loading = FALSE) { return new ToadletMaxMAPExporter(); }
	const TCHAR *	ClassName() { return GetString(IDS_CLASS_NAME); }
	SClass_ID		SuperClassID() { return SCENE_EXPORT_CLASS_ID; }
	Class_ID		ClassID() { return TOADLETMAXMAPEXPORTER_CLASS_ID; }
	const TCHAR* 	Category() { return GetString(IDS_CATEGORY); }

	const TCHAR*	InternalName() { return _T("ToadletMaxMAPExporter"); }	// returns fixed parsable name (scripter-visible name)
	HINSTANCE		HInstance() { return hInstance; }				// returns owning module handle
};



static ToadletMaxMAPExporterClassDesc ToadletMaxMAPExporterDesc;
ClassDesc2* GetToadletMaxMAPExporterDesc() { return &ToadletMaxMAPExporterDesc; }


BOOL CALLBACK ToadletMaxMAPExporterOptionsDlgProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam) {
	static ExportOptions *opt = NULL;

	switch(message) {
		case WM_INITDIALOG:
			opt = (ExportOptions *)lParam;
			CenterWindow(hWnd,GetParent(hWnd));
			CheckDlgButton(hWnd,IDC_CHECK1,opt->mInvertYTexCoord);
			SendDlgItemMessage(hWnd,IDC_EDIT1,WM_SETTEXT,0,(DWORD)opt->mTexturePath);
			return TRUE;

		case WM_CLOSE:
			EndDialog(hWnd, 0);
			return TRUE;

		case WM_COMMAND:
			switch (LOWORD(wParam)) {
			case IDOK:
				opt->mInvertYTexCoord=IsDlgButtonChecked(hWnd,IDC_CHECK1)!=0;
				{
					char data[1024];
					int length=SendDlgItemMessage(hWnd,IDC_EDIT1,WM_GETTEXTLENGTH,0,0);
					SendDlgItemMessage(hWnd,IDC_EDIT1,WM_GETTEXT,length+1,(DWORD)data);
					strcpy(opt->mTexturePath,data);
				}
				EndDialog(hWnd, 1);
				return TRUE;

			case IDC_BUTTON1:
				{
					BROWSEINFO bi = { 0 };
					bi.lpszTitle = _T("Choose a texture directory");
					LPITEMIDLIST pidl = SHBrowseForFolder ( &bi );
					if ( pidl != 0 ){
						// get the name of the folder
						TCHAR path[MAX_PATH];
						if ( SHGetPathFromIDList ( pidl, path ) ){
							SendDlgItemMessage(hWnd,IDC_EDIT1,WM_SETTEXT,0,(DWORD)path);
						}

						// free memory used
						IMalloc * imalloc = 0;
						if ( SUCCEEDED( SHGetMalloc ( &imalloc )) ){
							imalloc->Free ( pidl );
							imalloc->Release ( );
						}
					}
				}
				return TRUE;

			case IDCANCEL:
				EndDialog(hWnd, 0);
				return FALSE;
			}
	}
	return FALSE;
}


//--- ToadletMaxMAPExporter -------------------------------------------------------
ToadletMaxMAPExporter::ToadletMaxMAPExporter()
{

}

ToadletMaxMAPExporter::~ToadletMaxMAPExporter() 
{

}

int ToadletMaxMAPExporter::ExtCount()
{
	// Returns the number of file name extensions supported by the plug-in.
	return 1;
}

const TCHAR *ToadletMaxMAPExporter::Ext(int n)
{
	// Return the 'i-th' file name extension (i.e. "3DS").
	return _T("map");
}

const TCHAR *ToadletMaxMAPExporter::LongDesc()
{
	// Return long ASCII description (i.e. "Targa 2.0 Image File")
	return _T("Half-life MAP File");
}
	
const TCHAR *ToadletMaxMAPExporter::ShortDesc() 
{			
	// Return short ASCII description (i.e. "Targa")
	return _T("MAP");
}

const TCHAR *ToadletMaxMAPExporter::AuthorName()
{			
	// Return ASCII Author name
	return _T("Andrew Fischer");
}

const TCHAR *ToadletMaxMAPExporter::CopyrightMessage() 
{	
	// Return ASCII Copyright message
	return _T("Copyright Lightning Toads Productions, LLC");
}

const TCHAR *ToadletMaxMAPExporter::OtherMessage1() 
{		
	// Return Other message #1 if any
	return _T("");
}

const TCHAR *ToadletMaxMAPExporter::OtherMessage2() 
{		
	// Return other message #2 in any
	return _T("");
}

unsigned int ToadletMaxMAPExporter::Version()
{				
	// Return Version number * 100 (i.e. v3.01 = 301)
	return 100;
}

void ToadletMaxMAPExporter::ShowAbout(HWND hWnd)
{			
	// Optional
}

BOOL ToadletMaxMAPExporter::SupportsOptions(int ext, DWORD options)
{
	// Decide which options to support.  Simply return
	// true for each option supported by each Extension 
	// the exporter supports.

	return TRUE;
}


int	ToadletMaxMAPExporter::DoExport(const TCHAR *name,ExpInterface *ei,Interface *i, BOOL suppressPrompts, DWORD options)
{
	ExportOptions *opt = new ExportOptions();
	strcpy(opt->mTexturePath,getTexturePath().c_str());
	MAPExport *export=new MAPExport();
	export->mOptions=opt;

	if(!suppressPrompts){
		int result=DialogBoxParam(hInstance,MAKEINTRESOURCE(IDD_PANEL),GetActiveWindow(),ToadletMaxMAPExporterOptionsDlgProc,(LPARAM)opt);
		if(result==0){
			return 0;
		}
	}

	setTexturePath(opt->mTexturePath);

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

toadlet::egg::String ToadletMaxMAPExporter::getTexturePath(){
	toadlet::egg::String path;

	HKEY key;
	DWORD disposition=0;
	LONG result=RegCreateKeyEx(HKEY_CURRENT_USER,"SOFTWARE\\Lightning Toads\\ToadletMaxMAPExporter",0,NULL,REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL,&key,&disposition);
	if(result==ERROR_SUCCESS){
		unsigned char data[1025];
		memset(data,0,1025);
		DWORD length=1024;
		RegQueryValueEx(key,"TexturePath",NULL,NULL,data,&length);
		data[length]=0;
		RegCloseKey(key);
		path=(char*)data;
	}

	return path;
}

void ToadletMaxMAPExporter::setTexturePath(const toadlet::egg::String &path){
	HKEY key;
	DWORD disposition=0;
	LONG result=RegCreateKeyEx(HKEY_CURRENT_USER,"SOFTWARE\\Lightning Toads\\ToadletMaxMAPExporter",0,NULL,REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL,&key,&disposition);
	if(result==ERROR_SUCCESS){
		RegSetValueEx(key,"TexturePath",NULL,REG_SZ,(unsigned char*)path.c_str(),path.length()+1);
		RegCloseKey(key);
	}
}
