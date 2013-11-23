#ifndef Keithlay2410_h__
#define Keithlay2410_h__


#include <string>
#include <Rtypes.h>  //needed for the ROOT dict
#include <RQ_OBJECT.h>



#include <iostream>
//#include <TCanvas.h>
#include <TThread.h>

#ifndef __CINT__  // since ROOTCINT don't know the command _declspec(dllexport) one has to hide it from ROOTCINT. on the other hand the command is needed to show VS with element to export in THE DLL and that it should create a LIB file where 
// the connection is made between the names in the header file and the names in the DLL. There is a solution to avoid this by using a DEF file but since is not easier i go on with this way.
//#include "tti_RQSignals.h"
#ifdef WIN32
#define DLL_exp _declspec(dllexport) 
#else
#define DLL_exp 
#endif

#else 

#define DLL_exp 
#endif // __CINT__


class SerialCom;

class DLL_exp Keithley2410	
{
	RQ_OBJECT("Keithley2410")
public:
	 Keithley2410(const char *Port); // Constructor: it needs to know the name of the port. up to now there is only one constructor defined. 
	virtual  ~Keithley2410(void);  // the Destructor has no special implementation but by default it closes all open files. 


// get ...


double  getVoltage(); 



//double  getActualVoltage();


double  getCurrent();
double getCurrentRange();
//double  getActualCurrent();

bool   getStatus();

// std::string  getID(void);
// 
// TString  getID_TString(void);
// 
// int  getSerialNumber(void);// returns the serial number of the device





//set ...


int  enable();//enables output 
int  disable(); //disables output 
int  setStatus(bool on_off); //set output on/off where <on_off> has the following meaning: 0=OFF, 1=ON
int  setVoltage(double Voltage);//Set output to <Voltage> Volts
int  setCurrent(double current);// Set output current limit to <current> Amps
void setAutoRange();
int setCurrentRange( double current );
int setOverVoltageProtection(double Voltage); // keep in mind these function are not very precise 
int setOverCurrentProtection(double Current);
int setVoltageSource();
int setCurrentSource();
//static void  WhatClassAmI(void);  // shows Informations about the class





#ifndef __CINT__
//tti_RQSignals *RQSignals;

bool started1;
bool stop1;
#endif

void  show_errors(void); // displays the error list in the command line


// int  startThread(void); // starts a thread that will emit the actual voltage and current in a certain time interval 
// int  stopThread(bool waitForEnd=1); // Stops the thread. if waitForEnd is 1 by default it will wait until the thread is stop. if waitForEnd is 0 it just send a message to the thread that it should stop. 
// void  showSignals(void);

// TThread *t1;
// 	int samplingTime; // in Milliseconds
private:
	Keithley2410(const Keithley2410&); // disabling the copy constructor since one cannot copy the com ports
	Keithley2410& operator=(const Keithley2410&); //disabling assign for the same reason.
	

	

	std::string name;
	std::string modell;
	int SerialNumber;
	std::string version;


	int numberOfChannels;
#ifndef __CINT__
	SerialCom *SComunication;


#endif
	
	std::string getCommand(std::string command1,int channel,std::string command2); /* standard get Command 
																					sends 'command' + 'channel +'?'
																					or
																					sends 'comand1' + 'channel + 'command2'
																					and returns the answer as string*/


	int setCommand(std::string command,int channel,double vlaue); /* standard set command 
																  sends 'command'+channel +' ' +'value'
																  does not wait for an answer
																  returns: 
																  1 if ok 
																  -1 if some send error occurred
																  -2 if the channel is not 1 or 2*/ 
	
	
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Start Signals

// 		void ActualVoltageCh1(double volt) //*SIGNAL*
// 				{Emit("ActualVoltageCh1(double)",volt);}
// 		void ActualVoltageCh2(double volt) //*SIGNAL*
// 				{Emit("ActualVoltageCh2(double)",volt);}
// 
// 		void ActualCurrentCh1(double current) //*SIGNAL*
// 				{Emit("ActualCurrentCh1(double)",current);}
// 		void ActualCurrentCh2(double current) //*SIGNAL*
// 				{Emit("ActualCurrentCh2(double)",current);}

		// Stop Signals
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ClassDef(Keithley2410, 1);//needed for the ROOT dict

};


#endif // Keithlay2410_h__
