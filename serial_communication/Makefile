
PATH=/opt/root/5.34.09/bin:/usr/lib/lightdm/lightdm:/usr/sbin:/usr/bin:/sbin:/bin:/usr/games:/usr/X11R6/bin:/usr/local/sbin:/usr/local/bin
LD_LIBRARY_PATH = /opt/root/5.34.09/lib/root/

#compiler
CC=g++
#ROOTCINT
ROOTCINT=rootcint
#directory for storing created objects
ODIR=obj

ROOTLIBS=$(shell root-config --libs)
ROOTINCS=$(shell root-config --cflags)

CFLAGS:= -std=c++0x -c -fPIC -Wall $(ROOTINCS)
HEADERS:= $(wildcard *.h)


_OBJ := SerialCom.o SerialComDict.o tti_new.o SCom_Helpers.o TTiDict.o Keithley2410.o KeithDict.o TempInterlock.o TempDict.o
#OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

SerialComV1p5.so: $(_OBJ)
	$(CC) -shared $(_OBJ) -fPIC -o libSerialComV1p5.so $(ROOTLIBS)

%.o: %.cpp $(HEADERS)
	$(CC) $(CFLAGS) $^

%.o: %.cxx $(HEADERS)
	$(CC) $(CFLAGS) $^

CDCDict.cxx: CDCLinkDef.h CommonDeviceController.h
	rm -f CDCDict.cxx CDCDict.h
	$(ROOTCINT) CDCDict.cxx -c -p CommonDeviceController.h CDCLinkDef.h

chillerDict.cxx: chiller_new.h chiller_LinkDef.h
	rm -f chillerDict.cxx chillerDict.h
	$(ROOTCINT) chillerDict.cxx -c -p chiller_new.h chiller_LinkDef.h

KeithDict.cxx: Keithley2410.h Keithley_LinkDef.h  
	rm -f KeithDict.cxx KeithDict.h
	$(ROOTCINT) KeithDict.cxx -c -p Keithley2410.h Keithley_LinkDef.h  

SerialComDict.cxx: SerialCom.h SerialCom_LinkDef.h
	rm -f SerialComDict.cxx SerialComDict.h
	$(ROOTCINT) SerialComDict.cxx -c -p SerialCom.h SerialCom_LinkDef.h

TempDict.cxx: tti_new.h TempInterlock.h TempInterlockLinkDef.h
	rm -f TempDict.cxx TempDict.h
	$(ROOTCINT)  TempDict.cxx -c -p TempInterlock.h TempInterlockLinkDef.h

TTiDict.cxx: tti_new.h tti_LinkDef.h 
	rm -f TTiDict.cxx TTiDict.h
	$(ROOTCINT) TTiDict.cxx -c -p tti_new.h tti_LinkDef.h 

clean:
	rm *.o *.cxx
