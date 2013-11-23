echo ROOT 5.28
IF "%DevEnvDir%"=="" set DevEnvDir=C:\Program Files (x86)\Microsoft Visual Studio 10.0\Common7\IDE
IF "%VCINSTALLDIR%"=="" set VCINSTALLDIR=C:\Program Files (x86)\Microsoft Visual Studio 10.0\VC
path=%DevEnvDir%;%VCINSTALLDIR%\bin;%path%
set thisROOT=d:\root-5.28
del chillerDict.cxx
del chillerDict.h
%thisROOT%\bin\rootcint.exe chillerDict.cxx -c -p chiller_new.h Chiller_LinkDef.h
del SerialComDict.cxx
del SerialComDict.h
%thisROOT%\bin\rootcint.exe SerialComDict.cxx -c -p SerialCom.h SerialCom_LinkDef.h
del TTiDict.cxx
del TTiDict.h
%thisROOT%\bin\rootcint.exe TTiDict.cxx -c -p tti_new.h tti_LinkDef.h
del CDCDict.cxx
del CDCDict.h
%thisROOT%\bin\rootcint.exe CDCDict.cxx -c -p CommonDeviceController.h CDCLinkDef.h
del KeithDict.cxx
del KeithDict.h
%thisROOT%\bin\rootcint.exe KeithDict.cxx -c -p Keithley2410.h Keithley_LinkDef.h
del TempDict.h
del TempDict.cxx
%thisROOT%\bin\rootcint.exe TempDict.cxx -c -p TempInterlock.h TempInterlockLinkDef.h

echo finished
pause