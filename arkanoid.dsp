# Microsoft Developer Studio Project File - Name="Sokoban" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=Sokoban - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Arkanoid.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Arkanoid.mak" CFG="Sokoban - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Sokoban - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "Sokoban - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Sokoban - Win32 Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /FD /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib ws2_32.lib winmm.lib /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "Sokoban - Win32 Debug"

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
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GR /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x419 /fo"Debug/Arkanoid.res" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib ws2_32.lib winmm.lib dxguid.lib /nologo /version:1 /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "Sokoban - Win32 Release"
# Name "Sokoban - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\Arkanoid.rc
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\client.cpp
# End Source File
# Begin Source File

SOURCE=.\Controls.cpp
# End Source File
# Begin Source File

SOURCE=.\dd_main.cpp
# End Source File
# Begin Source File

SOURCE=.\Ddutil.cpp
# End Source File
# Begin Source File

SOURCE=.\Debug.cpp
# End Source File
# Begin Source File

SOURCE=.\Dsutil.cpp
# End Source File
# Begin Source File

SOURCE=.\dxerrors.cpp
# End Source File
# Begin Source File

SOURCE=.\entities.cpp
# End Source File
# Begin Source File

SOURCE=.\game.cpp
# End Source File
# Begin Source File

SOURCE=.\gdi_main.cpp
# End Source File
# Begin Source File

SOURCE=.\Graphics.cpp
# End Source File
# Begin Source File

SOURCE=.\host.cpp
# End Source File
# Begin Source File

SOURCE=.\main.cpp
# End Source File
# Begin Source File

SOURCE=.\NET.cpp
# End Source File
# Begin Source File

SOURCE=.\NetConnectDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\RealTime.cpp
# End Source File
# Begin Source File

SOURCE=.\Screen.cpp
# End Source File
# Begin Source File

SOURCE=.\server.cpp
# End Source File
# Begin Source File

SOURCE=.\Sound.cpp
# End Source File
# Begin Source File

SOURCE=.\WINException.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\a_shared.h
# End Source File
# Begin Source File

SOURCE=.\Client.h
# End Source File
# Begin Source File

SOURCE=.\Controls.h
# End Source File
# Begin Source File

SOURCE=.\Ddutil.h
# End Source File
# Begin Source File

SOURCE=.\Debug.h
# End Source File
# Begin Source File

SOURCE=.\Dsutil.h
# End Source File
# Begin Source File

SOURCE=.\dxerrors.h
# End Source File
# Begin Source File

SOURCE=.\game.h
# End Source File
# Begin Source File

SOURCE=.\Graphics.h
# End Source File
# Begin Source File

SOURCE=.\host.h
# End Source File
# Begin Source File

SOURCE=.\log.h
# End Source File
# Begin Source File

SOURCE=.\NET.h
# End Source File
# Begin Source File

SOURCE=.\NetConnectDlg.h
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=.\Screen.h
# End Source File
# Begin Source File

SOURCE=.\Server.h
# End Source File
# Begin Source File

SOURCE=.\Sound.h
# End Source File
# Begin Source File

SOURCE=.\win_gr.h
# End Source File
# Begin Source File

SOURCE=.\winarkanoid.h
# End Source File
# Begin Source File

SOURCE=.\winerrors.h
# End Source File
# Begin Source File

SOURCE=.\WINException.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\Arkanoid.ico
# End Source File
# Begin Source File

SOURCE=.\BACK1.BMP
# End Source File
# Begin Source File

SOURCE=.\BACK2.BMP
# End Source File
# Begin Source File

SOURCE=.\BACK3.BMP
# End Source File
# Begin Source File

SOURCE=.\BACK4.BMP
# End Source File
# Begin Source File

SOURCE=.\BACK5.BMP
# End Source File
# Begin Source File

SOURCE=.\Background.bmp
# End Source File
# Begin Source File

SOURCE=.\blocks2.bmp
# End Source File
# Begin Source File

SOURCE=.\small.ico
# End Source File
# Begin Source File

SOURCE=.\Sokoban.ico
# End Source File
# Begin Source File

SOURCE=.\Sprites.bmp
# End Source File
# End Group
# Begin Source File

SOURCE=.\chimes.wav
# End Source File
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
