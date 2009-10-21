# Microsoft Developer Studio Project File - Name="ToadletMaxCollisionExporter" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=ToadletMaxCollisionExporter - Win32 Hybrid
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "ToadletMaxCollisionExporter.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ToadletMaxCollisionExporter.mak" CFG="ToadletMaxCollisionExporter - Win32 Hybrid"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ToadletMaxCollisionExporter - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "ToadletMaxCollisionExporter - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "ToadletMaxCollisionExporter - Win32 Hybrid" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "ToadletMaxCollisionExporter - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /YX /FD /c
# ADD CPP /nologo /G6 /MT /W3 /GR /GX /O2 /I "C:\3dsmax6\maxsdk\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_USRDLL" /D "TOADLET_USE_STATIC" /YX /FD /LD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib core.lib geom.lib gfx.lib mesh.lib maxutil.lib maxscrpt.lib paramblk2.lib /nologo /base:"0x3900000" /subsystem:windows /dll /machine:I386 /nodefaultlib:"libc.lib" /out:"$(3DSMAX_DIR)\plugins\ToadletMaxCollisionExporter.dle" /libpath:"C:\3dsmax6\maxsdk\lib" /release

!ELSEIF  "$(CFG)" == "ToadletMaxCollisionExporter - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /YX /FD /GZ /c
# ADD CPP /nologo /G6 /MTd /W3 /Gm /GR /GX /ZI /Od /I "C:\3dsmax6\maxsdk\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_USRDLL" /D "TOADLET_USE_STATIC" /YX /FD /LD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib core.lib geom.lib gfx.lib mesh.lib maxutil.lib maxscrpt.lib paramblk2.lib /nologo /base:"0x3900000" /subsystem:windows /dll /debug /machine:I386 /nodefaultlib:"libc.lib" /out:"$(3DSMAX_DIR)\plugins\ToadletMaxCollisionExporter.dle" /pdbtype:sept /libpath:"C:\3dsmax6\maxsdk\lib"

!ELSEIF  "$(CFG)" == "ToadletMaxCollisionExporter - Win32 Hybrid"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "ToadletMaxCollisionExporter___Win32_Hybrid"
# PROP BASE Intermediate_Dir "ToadletMaxCollisionExporter___Win32_Hybrid"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "ToadletMaxCollisionExporter___Win32_Hybrid"
# PROP Intermediate_Dir "ToadletMaxCollisionExporter___Win32_Hybrid"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /GZ /c
# ADD CPP /nologo /G6 /MT /W3 /Gm /GR /GX /ZI /Od /I "C:\3dsmax6\maxsdk\include" /I "../include" /I "." /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "TOADLET_USE_STATIC" /YX /FD /LD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib core.lib geom.lib gfx.lib mesh.lib maxutil.lib maxscrpt.lib paramblk2.lib /nologo /base:"0x3900000" /subsystem:windows /dll /debug /machine:I386 /nodefaultlib:"libc.lib" /out:"$(3DSMAX_DIR)\plugins\ToadletMaxCollisionExporter.dle" /pdbtype:sept /libpath:"C:\3dsmax6\maxsdk\lib"

!ENDIF 

# Begin Target

# Name "ToadletMaxCollisionExporter - Win32 Release"
# Name "ToadletMaxCollisionExporter - Win32 Debug"
# Name "ToadletMaxCollisionExporter - Win32 Hybrid"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\CollisionExport.cpp
# End Source File
# Begin Source File

SOURCE=.\CollisionExport.h
# End Source File
# Begin Source File

SOURCE=.\DllEntry.cpp
# End Source File
# Begin Source File

SOURCE=.\ExportOptions.h
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=.\ToadletMaxCollisionExporter.cpp
# End Source File
# Begin Source File

SOURCE=.\ToadletMaxCollisionExporter.h
# End Source File
# Begin Source File

SOURCE=.\TreeTraversalClasses.cpp
# End Source File
# Begin Source File

SOURCE=.\TreeTraversalClasses.h
# End Source File
# Begin Source File

SOURCE=..\XCOLHandler\XCOLHandler.cpp
# End Source File
# Begin Source File

SOURCE=..\XCOLHandler\XCOLHandler.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\ToadletMaxCollisionExporter.rc
# End Source File
# End Group
# End Target
# End Project
