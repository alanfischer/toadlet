/**********************************************************************
 *<
	FILE: MaxTMSHExporter.cpp

	DESCRIPTION:	Appwizard generated plugin

	CREATED BY: 

	HISTORY: 

 *>	Copyright (c) 2000, All Rights Reserved.
 **********************************************************************/

#include "MeshExport.h"
#include "MaxTMSHExporter.h"

#include <shlobj.h>

#define MAXTMSHEXPORTER_CLASS_ID	Class_ID(0x667e8531, 0x233510b0)

class MaxTMSHExporter : public SceneExport {
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

		MaxTMSHExporter();
		~MaxTMSHExporter();		
};


class MaxTMSHExporterClassDesc:public ClassDesc2 {
	public:
	int 			IsPublic() { return TRUE; }
	void *			Create(BOOL loading = FALSE) { return new MaxTMSHExporter(); }
	const TCHAR *	ClassName() { return GetString(IDS_CLASS_NAME); }
	SClass_ID		SuperClassID() { return SCENE_EXPORT_CLASS_ID; }
	Class_ID		ClassID() { return MAXTMSHEXPORTER_CLASS_ID; }
	const TCHAR* 	Category() { return GetString(IDS_CATEGORY); }

	const TCHAR*	InternalName() { return _T("MaxTMSHExporter"); }	// returns fixed parsable name (scripter-visible name)
	HINSTANCE		HInstance() { return hInstance; }				// returns owning module handle
};



static MaxTMSHExporterClassDesc MaxTMSHExporterDesc;
ClassDesc2* GetMaxTMSHExporterDesc() { return &MaxTMSHExporterDesc; }


BOOL CALLBACK MaxTMSHExporterOptionsDlgProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam) {
	static ExportOptions *opt = NULL;

	switch(message) {
		case WM_INITDIALOG:
			opt = (ExportOptions *)lParam;
			CenterWindow(hWnd,GetParent(hWnd));
			CheckDlgButton(hWnd, IDC_SKELETON, opt->mExportSkeleton);
			CheckDlgButton(hWnd, IDC_ANIMATION, opt->mExportAnimations);
			CheckDlgButton(hWnd, IDC_GEOMETRY, opt->mExportGeometry);
			CheckDlgButton(hWnd, IDC_VERTEXNORMAL, opt->mExportVertexNormals);
			CheckDlgButton(hWnd, IDC_VERTEXCOLOR, opt->mExportVertexColors);
			CheckDlgButton(hWnd, IDC_TEXTURECOORD, opt->mExportTextureCoordinates);
			CheckDlgButton(hWnd, IDC_MATERIAL, opt->mExportMaterials);
			CheckDlgButton(hWnd, IDC_PREVIEW, opt->mPreview);
			CheckDlgButton(hWnd, IDC_SKIPNULLMAT, opt->mSkipNullMaterials);
			SendDlgItemMessage(hWnd,IDC_EDIT1,WM_SETTEXT,0,(DWORD)opt->mPreviewTexturePath);
			return TRUE;

		case WM_CLOSE:
			EndDialog(hWnd, 0);
			return TRUE;

		case WM_COMMAND:
			switch (LOWORD(wParam)) {
			case IDOK:
				opt->mExportSkeleton=IsDlgButtonChecked(hWnd, IDC_SKELETON);
				opt->mExportAnimations=IsDlgButtonChecked(hWnd, IDC_ANIMATION);
				opt->mExportGeometry=IsDlgButtonChecked(hWnd, IDC_GEOMETRY);
				opt->mExportVertexNormals=IsDlgButtonChecked(hWnd, IDC_VERTEXNORMAL);
				opt->mExportVertexColors=IsDlgButtonChecked(hWnd, IDC_VERTEXCOLOR);
				opt->mExportTextureCoordinates=IsDlgButtonChecked(hWnd, IDC_TEXTURECOORD);
				opt->mExportMaterials=IsDlgButtonChecked(hWnd, IDC_MATERIAL);
				opt->mPreview=IsDlgButtonChecked(hWnd, IDC_PREVIEW);
				opt->mSkipNullMaterials=IsDlgButtonChecked(hWnd, IDC_SKIPNULLMAT);
				{
					char data[1024];
					int length=SendDlgItemMessage(hWnd,IDC_EDIT1,WM_GETTEXTLENGTH,0,0);
					SendDlgItemMessage(hWnd,IDC_EDIT1,WM_GETTEXT,length+1,(DWORD)data);
					strcpy(opt->mPreviewTexturePath,data);
				}
				EndDialog(hWnd,1);
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


//--- MaxTMSHExporter -------------------------------------------------------
MaxTMSHExporter::MaxTMSHExporter()
{

}

MaxTMSHExporter::~MaxTMSHExporter() 
{

}

int MaxTMSHExporter::ExtCount()
{
	// Returns the number of file name extensions supported by the plug-in.
	return 2;
}

const TCHAR *MaxTMSHExporter::Ext(int n)
{
	// Return the 'i-th' file name extension (i.e. "3DS").
	if(n==0){
		return _T("xmsh");
	}
	else{
		return _T("xanm");
	}
}

const TCHAR *MaxTMSHExporter::LongDesc()
{
	// Return long ASCII description (i.e. "Targa 2.0 Image File")
	return _T("Toadlet Mesh/Animation File");
}
	
const TCHAR *MaxTMSHExporter::ShortDesc() 
{			
	// Return short ASCII description (i.e. "Targa")
	return _T("Toadlet Mesh/Animation");
}

const TCHAR *MaxTMSHExporter::AuthorName()
{			
	// Return ASCII Author name
	return _T("Andrew Fischer");
}

const TCHAR *MaxTMSHExporter::CopyrightMessage() 
{	
	// Return ASCII Copyright message
	return _T("Copyright Lightning Toads Productions, LLC");
}

const TCHAR *MaxTMSHExporter::OtherMessage1() 
{		
	// Return Other message #1 if any
	return _T("");
}

const TCHAR *MaxTMSHExporter::OtherMessage2() 
{		
	// Return other message #2 in any
	return _T("");
}

unsigned int MaxTMSHExporter::Version()
{				
	// Return Version number * 100 (i.e. v3.01 = 301)
	return 100;
}

void MaxTMSHExporter::ShowAbout(HWND hWnd)
{			
	// Optional
}

BOOL MaxTMSHExporter::SupportsOptions(int ext, DWORD options)
{
	// Decide which options to support.  Simply return
	// true for each option supported by each Extension 
	// the exporter supports.

	return TRUE;
}


int	MaxTMSHExporter::DoExport(const TCHAR *name,ExpInterface *ei,Interface *i, BOOL suppressPrompts, DWORD options)
{
	ExportOptions *opt=new ExportOptions();
	strcpy(opt->mPreviewTexturePath,getTexturePath().c_str());
	MeshExport *export=new MeshExport();
	export->mOptions=opt;

	toadlet::egg::String sname=name;
	toadlet::egg::toLowerCase(sname);
	bool animation=(sname.length()>=5 && sname.substr(sname.length()-5,5)==".xanm");

	if(animation){
		opt->mExportSkeleton=true;
		opt->mExportAnimations=true;
	}

	if(!suppressPrompts){
		int result=DialogBoxParam(hInstance,MAKEINTRESOURCE(IDD_PANEL),GetActiveWindow(),MaxTMSHExporterOptionsDlgProc,(LPARAM)opt);
		if(result==0){
			return 0;
		}
	}

	if(animation && opt->mExportAnimations==false){
		MessageBox(GetActiveWindow(),"Exporting to .xanm, but no animations exported","Error",MB_OK);
		return 0;
	}

	setTexturePath(opt->mPreviewTexturePath);

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

toadlet::egg::String MaxTMSHExporter::getTexturePath(){
	toadlet::egg::String path;

	HKEY key;
	DWORD disposition=0;
	LONG result=RegCreateKeyEx(HKEY_CURRENT_USER,"SOFTWARE\\Lightning Toads\\MaxTMSHExporter",0,NULL,REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL,&key,&disposition);
	if(result==ERROR_SUCCESS){
		unsigned char data[1025];
		memset(data,0,1025);
		DWORD length=1024;
		RegQueryValueEx(key,"PreviewTexturePath",NULL,NULL,data,&length);
		data[length]=0;
		RegCloseKey(key);
		path=(char*)data;
	}

	return path;
}

void MaxTMSHExporter::setTexturePath(const toadlet::egg::String &path){
	HKEY key;
	DWORD disposition=0;
	LONG result=RegCreateKeyEx(HKEY_CURRENT_USER,"SOFTWARE\\Lightning Toads\\MaxTMSHExporter",0,NULL,REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL,&key,&disposition);
	if(result==ERROR_SUCCESS){
		RegSetValueEx(key,"PreviewTexturePath",NULL,REG_SZ,(unsigned char*)path.c_str(),path.length()+1);
		RegCloseKey(key);
	}
}
