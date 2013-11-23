#pragma once
#include <RQ_OBJECT.h>
#include <Rtypes.h>
#include <iostream>
#ifndef __CINT__  // since ROOTCINT don't know the command _declspec(dllexport) one has to hide it from ROOTCINT. on the other hand the command is needed to show VS with element to export in THE DLL and that it should create a LIB file where 



#ifdef WIN32
#define DLL_exp _declspec(dllexport)
#else
#define DLL_exp
#endif // WIN32
#else
#define DLL_exp

#endif // __CINT__

enum sig{
	SActualVoltageCh1,
	SActualVoltageCh2,
	SActualCurrentCh1,
	SActualCurrentCh2

};

class tti_RQSignals
{
	RQ_OBJECT("tti_RQSignals")
public:
	 tti_RQSignals(void);
	 ~tti_RQSignals(void);


	void ActualVoltageCh1(double volt) //*SIGNAL*
	{Emit("ActualVoltageCh1(double)",volt);}
	void ActualVoltageCh2(double volt) //*SIGNAL*
	{Emit("ActualVoltageCh2(double)",volt);}

	void ActualCurrentCh1(double current) //*SIGNAL*
	{Emit("ActualCurrentCh1(double)",current);}
	void ActualCurrentCh2(double current) //*SIGNAL*
	{Emit("ActualCurrentCh2(double)",current);}

	char * SignalName(sig f);


 void DLL_exp RQConnect(sig f);

 

 ClassDef(tti_RQSignals,0);
};

