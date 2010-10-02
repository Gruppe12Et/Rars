# Microsoft Developer Studio Project File - Name="vc_rars" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 5.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=vc_rars - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "vc_rars.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "vc_rars.mak" CFG="vc_rars - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "vc_rars - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "vc_rars - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "vc_rars - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\Release"
# PROP BASE Intermediate_Dir ".\Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\Release"
# PROP Intermediate_Dir ".\Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MD /W3 /GX /O1 /I "." /I ".." /I "..\dos" /I "..\graphics" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /YX"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x80c /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x80c /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 opengl32.lib glu32.lib /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "vc_rars - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\Debug"
# PROP BASE Intermediate_Dir ".\Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\Debug"
# PROP Intermediate_Dir ".\Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /I "." /I ".." /I "..\dos" /I "..\graphics" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /YX"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x80c /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x80c /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 opengl32.lib glu32.lib /nologo /subsystem:windows /debug /machine:I386

!ENDIF 

# Begin Target

# Name "vc_rars - Win32 Release"
# Name "vc_rars - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;hpj;bat;for;f90"
# Begin Group "Robots"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\robots\apex84.cpp
# End Source File
# Begin Source File

SOURCE=..\robots\bug.cpp
# End Source File
# Begin Source File

SOURCE=..\robots\bulle.cpp
# End Source File
# Begin Source File

SOURCE=..\robots\bulle2.cpp
# End Source File
# Begin Source File

SOURCE=..\robots\burns.cpp
# End Source File
# Begin Source File

SOURCE=..\robots\dgn.cpp
# End Source File
# Begin Source File

SOURCE=..\robots\djoefe.cpp
# End Source File
# Begin Source File

SOURCE=..\robots\dodger6.cpp
# End Source File
# Begin Source File

SOURCE=..\robots\douge1.cpp
# End Source File
# Begin Source File

SOURCE=..\robots\felix16.cpp
# End Source File
# Begin Source File

SOURCE=..\robots\indretti.cpp
# End Source File
# Begin Source File

SOURCE=..\robots\jammer.cpp
# End Source File
# Begin Source File

SOURCE=..\robots\jas.cpp
# End Source File
# Begin Source File

SOURCE=..\robots\jocosa83.cpp
# End Source File
# Begin Source File

SOURCE=..\robots\k1999v11.cpp
# End Source File
# Begin Source File

SOURCE=..\robots\lisa15.cpp
# End Source File
# Begin Source File

SOURCE=..\robots\magic.cpp
# End Source File
# Begin Source File

SOURCE=..\robots\mat1.cpp
# End Source File
# Begin Source File

SOURCE=..\robots\oscar1.cpp
# End Source File
# Begin Source File

SOURCE=..\robots\oscarstr.cpp
# End Source File
# Begin Source File

SOURCE=..\robots\rach221b.cpp
# End Source File
# Begin Source File

SOURCE=..\robots\ralph2.cpp
# End Source File
# Begin Source File

SOURCE=..\robots\rusty.cpp
# End Source File
# Begin Source File

SOURCE=..\robots\sad01_1.cpp
# End Source File
# Begin Source File

SOURCE=..\robots\sarahnc1.cpp
# End Source File
# Begin Source File

SOURCE=..\robots\sparky5.cpp
# End Source File
# Begin Source File

SOURCE=..\robots\stocker.cpp
# End Source File
# Begin Source File

SOURCE=..\robots\turtle5.cpp
# End Source File
# Begin Source File

SOURCE=..\robots\tutorial1.cpp
# End Source File
# Begin Source File

SOURCE=..\robots\tutorial2.cpp
# End Source File
# Begin Source File

SOURCE=..\robots\tutorial3.cpp
# End Source File
# Begin Source File

SOURCE=..\robots\tutorial4.cpp
# End Source File
# Begin Source File

SOURCE=..\robots\vector.cpp
# End Source File
# Begin Source File

SOURCE=..\robots\viper2hc.cpp
# End Source File
# Begin Source File

SOURCE=..\robots\wappucar.cpp
# End Source File
# End Group
# Begin Group "Graphics"

# PROP Default_Filter ""
# Begin Group "3ds"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\3ds\gl_texture.cpp
# End Source File
# Begin Source File

SOURCE=..\3ds\model_3ds.cpp
# End Source File
# End Group
# Begin Source File

SOURCE=..\graphics\g_bitmap.cpp
# End Source File
# Begin Source File

SOURCE=..\graphics\g_divers.cpp
# End Source File
# Begin Source File

SOURCE=..\graphics\g_draw.cpp
# End Source File
# Begin Source File

SOURCE=..\graphics\g_lowgra.cpp
# End Source File
# Begin Source File

SOURCE=..\graphics\g_main.cpp
# End Source File
# Begin Source File

SOURCE=..\graphics\g_opengl.cpp
# End Source File
# Begin Source File

SOURCE=..\graphics\g_os.cpp
# End Source File
# Begin Source File

SOURCE=..\graphics\g_palett.cpp
# End Source File
# Begin Source File

SOURCE=..\graphics\g_rars_ini.cpp
# End Source File
# Begin Source File

SOURCE=..\graphics\g_view.cpp
# End Source File
# Begin Source File

SOURCE=..\graphics\g_view2d.cpp
# End Source File
# Begin Source File

SOURCE=..\graphics\g_view3d.cpp
# End Source File
# Begin Source File

SOURCE=..\graphics\g_viewbd.cpp
# End Source File
# Begin Source File

SOURCE=..\graphics\g_viewcl.cpp
# End Source File
# Begin Source File

SOURCE=..\graphics\g_viewtl.cpp
# End Source File
# Begin Source File

SOURCE=..\graphics\g_vmanag.cpp
# End Source File
# Begin Source File

SOURCE=..\graphics\vc_lowgr.cpp
# End Source File
# End Group
# Begin Group "Optimizer"

# PROP Default_Filter ""
# Begin Group "Bulle"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\robots\bulle\_opti.cpp
# End Source File
# Begin Source File

SOURCE=..\robots\bulle\dlgbulle.cpp
# End Source File
# End Group
# End Group
# Begin Group "Common"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\carz.cpp
# End Source File
# Begin Source File

SOURCE=..\driver.cpp
# End Source File
# Begin Source File

SOURCE=..\driver_old.cpp
# End Source File
# Begin Source File

SOURCE=..\drivers.cpp
# End Source File
# Begin Source File

SOURCE=..\misc.cpp
# End Source File
# Begin Source File

SOURCE=..\movie.cpp
# End Source File
# Begin Source File

SOURCE=..\report.cpp
# End Source File
# Begin Source File

SOURCE=..\track.cpp
# End Source File
# Begin Source File

SOURCE=..\trackgen.cpp
# End Source File
# End Group
# Begin Source File

SOURCE=.\DlgInitCars.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgRars.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgStart.cpp
# End Source File
# Begin Source File

SOURCE=.\MyButton.cpp
# End Source File
# Begin Source File

SOURCE=.\stdafx.cpp
# End Source File
# Begin Source File

SOURCE=.\vc_rars.cpp
# End Source File
# Begin Source File

SOURCE=.\Vc_rars.rc

!IF  "$(CFG)" == "vc_rars - Win32 Release"

!ELSEIF  "$(CFG)" == "vc_rars - Win32 Debug"

!ENDIF 

# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# Begin Source File

SOURCE=..\car.h
# End Source File
# Begin Source File

SOURCE=.\DlgInitCars.h
# End Source File
# Begin Source File

SOURCE=.\DlgRars.h
# End Source File
# Begin Source File

SOURCE=.\DlgStart.h
# End Source File
# Begin Source File

SOURCE=..\graphics\g_bitmap.h
# End Source File
# Begin Source File

SOURCE=..\graphics\g_define.h
# End Source File
# Begin Source File

SOURCE=..\graphics\g_global.h
# End Source File
# Begin Source File

SOURCE=..\graphics\g_main.h
# End Source File
# Begin Source File

SOURCE=..\graphics\g_rars_ini.h
# End Source File
# Begin Source File

SOURCE=..\graphics\vc_defin.h
# End Source File
# Begin Source File

SOURCE=..\graphics\g_view.h
# End Source File
# Begin Source File

SOURCE=..\gi.h
# End Source File
# Begin Source File

SOURCE=..\main.h
# End Source File
# Begin Source File

SOURCE=..\misc.h
# End Source File
# Begin Source File

SOURCE=..\movie.h
# End Source File
# Begin Source File

SOURCE=.\MyButton.h
# End Source File
# Begin Source File

SOURCE=..\os.h
# End Source File
# Begin Source File

SOURCE=..\track.h
# End Source File
# Begin Source File

SOURCE=.\vc_rars.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\Res\Rars.ico
# End Source File
# Begin Source File

SOURCE=.\Res\toc.bmp
# End Source File
# Begin Source File

SOURCE=.\res\vc_rars.ico
# End Source File
# Begin Source File

SOURCE=.\res\vc_rars.rc2
# End Source File
# End Group
# End Target
# End Project
# Section vc_rars : {A8C3B720-0B5A-101B-B22E-00AA0037B2FC}
# 	0:8:Font.cpp:C:\rars\msvc\Font.cpp
# 	0:9:Picture.h:C:\rars\msvc\Picture.h
# 	0:12:GridCtrl.cpp:C:\rars\msvc\GridCtrl.cpp
# 	0:6:Font.h:C:\rars\msvc\Font.h
# 	0:10:GridCtrl.h:C:\rars\msvc\GridCtrl.h
# 	0:11:Picture.cpp:C:\rars\msvc\Picture.cpp
# 	2:21:DefaultSinkHeaderFile:gridctrl.h
# 	2:16:DefaultSinkClass:CGridCtrl
# End Section
# Section vc_rars : {BEF6E003-A874-101A-8BBA-00AA00300CAB}
# 	2:5:Class:COleFont
# 	2:10:HeaderFile:font.h
# 	2:8:ImplFile:font.cpp
# End Section
# Section vc_rars : {7BF80981-BF32-101A-8BBB-00AA00300CAB}
# 	2:5:Class:CPicture
# 	2:10:HeaderFile:picture.h
# 	2:8:ImplFile:picture.cpp
# End Section
# Section OLE Controls
# 	{A8C3B720-0B5A-101B-B22E-00AA0037B2FC}
# End Section
# Section vc_rars : {F4392540-0CFE-101B-B22E-00AA0037B2FC}
# 	2:5:Class:CGridCtrl
# 	2:10:HeaderFile:gridctrl.h
# 	2:8:ImplFile:gridctrl.cpp
# End Section
