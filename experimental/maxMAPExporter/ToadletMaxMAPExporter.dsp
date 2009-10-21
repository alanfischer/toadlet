# Microsoft Developer Studio Project File - Name="ToadletMaxMAPExporter" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=ToadletMaxMAPExporter - Win32 Hybrid
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "ToadletMaxMAPExporter.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ToadletMaxMAPExporter.mak" CFG="ToadletMaxMAPExporter - Win32 Hybrid"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ToadletMaxMAPExporter - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "ToadletMaxMAPExporter - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "ToadletMaxMAPExporter - Win32 Hybrid" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "ToadletMaxMAPExporter - Win32 Release"

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
# ADD CPP /nologo /G6 /MT /W3 /GR /GX /O2 /I "$(3DSMAX_DIR)\maxsdk\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_USRDLL" /D "TOADLET_USE_STATIC" /YX /FD /LD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib core.lib geom.lib gfx.lib mesh.lib maxutil.lib maxscrpt.lib paramblk2.lib /nologo /base:"0x3900000" /subsystem:windows /dll /machine:I386 /nodefaultlib:"libc.lib" /out:"$(3DSMAX_DIR)\plugins\ToadletMaxMAPExporter.dle" /libpath:"$(3DSMAX_DIR)\maxsdk\lib" /release
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "ToadletMaxMAPExporter - Win32 Debug"

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
# ADD CPP /nologo /G6 /MTd /W3 /Gm /GR /GX /ZI /Od /I "$(3DSMAX_DIR)\maxsdk\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_USRDLL" /D "TOADLET_USE_STATIC" /YX /FD /LD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib core.lib geom.lib gfx.lib mesh.lib maxutil.lib maxscrpt.lib paramblk2.lib /nologo /base:"0x3900000" /subsystem:windows /dll /debug /machine:I386 /nodefaultlib:"libc.lib" /out:"$(3DSMAX_DIR)\plugins\ToadletMaxMAPExporter.dle" /pdbtype:sept /libpath:"$(3DSMAX_DIR)\maxsdk\lib"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "ToadletMaxMAPExporter - Win32 Hybrid"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "ToadletMaxMAPExporter___Win32_Hybrid"
# PROP BASE Intermediate_Dir "ToadletMaxMAPExporter___Win32_Hybrid"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "ToadletMaxMAPExporter___Win32_Hybrid"
# PROP Intermediate_Dir "ToadletMaxMAPExporter___Win32_Hybrid"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /GZ /c
# ADD CPP /nologo /G6 /MTd /W3 /Gm /GR /GX /ZI /Od /I "$(3DSMAX_DIR)\maxsdk\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "TOADLET_USE_STATIC" /YX /FD /LD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib core.lib geom.lib gfx.lib mesh.lib maxutil.lib maxscrpt.lib paramblk2.lib /nologo /base:"0x3900000" /subsystem:windows /dll /debug /machine:I386 /nodefaultlib:"libc.lib" /out:"$(3DSMAX_DIR)\plugins\ToadletMaxMAPExporter.dle" /pdbtype:sept /libpath:"$(3DSMAX_DIR)\maxsdk\lib"
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "ToadletMaxMAPExporter - Win32 Release"
# Name "ToadletMaxMAPExporter - Win32 Debug"
# Name "ToadletMaxMAPExporter - Win32 Hybrid"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "csg"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\csg\Brush.cpp
# End Source File
# Begin Source File

SOURCE=..\csg\Brush.h
# End Source File
# Begin Source File

SOURCE=..\csg\BrushCollection.h
# End Source File
# Begin Source File

SOURCE=..\csg\BrushFace.cpp
# End Source File
# Begin Source File

SOURCE=..\csg\BrushFace.h
# End Source File
# Begin Source File

SOURCE=..\csg\Map.h
# End Source File
# Begin Source File

SOURCE=..\csg\MAPHandler.cpp
# End Source File
# Begin Source File

SOURCE=..\csg\MAPHandler.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\DllEntry.cpp
# End Source File
# Begin Source File

SOURCE=.\ExportOptions.h
# End Source File
# Begin Source File

SOURCE=.\MAPExport.cpp
# End Source File
# Begin Source File

SOURCE=.\MAPExport.h
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=.\ToadletMaxMAPExporter.cpp
# End Source File
# Begin Source File

SOURCE=.\ToadletMaxMAPExporter.h
# End Source File
# Begin Source File

SOURCE=.\TreeTraversalClasses.cpp
# End Source File
# Begin Source File

SOURCE=.\TreeTraversalClasses.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\ToadletMaxMAPExporter.rc
# End Source File
# End Group
# End Target
# End Project
