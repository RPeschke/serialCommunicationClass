#ifndef tti_class
#define tti_class
// signals: 
// "Voltage(int,double)" were the first value is the channel number and the second is the voltage value
// "ActualVoltage(int,double)" /* Returns the output readback voltage 
// 


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

class DLL_exp tti_CPX400	
{
	RQ_OBJECT("tti_CPX400")
public:
	 tti_CPX400(const char *Port); // Constructor: it needs to know the name of the port. up to now there is only one constructor defined. 
	virtual  ~tti_CPX400(void);  // the Destructor has no special implementation but by default it closes all open files. 


// get ...

std::string getPort();
double  getVoltage(int Channel); /*Returns the set voltage of 
								output <Channel> The response is 
								Voltages in volt as double */



double  getActualVoltage(int channel);/*Returns the output 
									 readback voltage for output
									 <channel> The response is voltage 
									 in volt as double*/


double  getCurrent(int channel);/*Returns the set current limit 
							   of output <channel> The response is 
							   current in Ampere as double */

double  getActualCurrent(int channel);/* Returns the output 
									 readback current for output 
									 <channel> The response is current in
									 ampere as double*/
double  getOverVoltageProtection(int channel); /*Returns the voltage trip setting 
											  for output <channel> The response is 
											  Voltage in volt as double  */
double  getOverCurrentProtection(int channel); /*Returns the current trip 
											  setting for output <channel> 
											  The response current in amps 
											  as double  */
bool   getStatus(int channel);/*Returns output <channel> on/off status. 
							  The response is status as bool
							  where 
							  1 (true) = ON,
							  0 (false) = OFF.  */

std::string  getID(void);/*Returns the instrument identification.
								The exact response is determined by the 
						instrument configuration and is of the 
						form <NAME>,<model>,<Serial No.>, <version><rmt> 
						where <NAME> is the manufacturer's name, 
						<model> defines the type of instrument, 
						Serial No. is the unique serial number 
						and <version> is the revision level of 
						the software installed. The format of 
						<version> is X.xx – Y.yy where X.xx is 
						the revision of the main instrument 
						firmware and Y.yy is the revision of 
						the Interface board firmware. The 
						Interface firmware is field updateable 
						by the user via the USB port.*/
TString  getID_TString(void);

int  getSerialNumber(void);// returns the serial number of the device

std::string  getSerialNumber_str(void);// returns the serial number of the device as string
TString  getSerialNumber_TString(void);

std::string  getAddress(void);/*Returns the bus address of the 
							 instrument; This is the address 
							 used by GPIB, if fitted, or may 
							 be used as a general identifier 
							 over the other interfaces.*/
TString  getAddress_TString(void);
//set ...


int  enable(int channel);//enables output <channel>
int  disable(int channel); //disables output <channel>
int  setStatus(int channel, bool on_off); //set output <channel> on/off where <on_off> has the following meaning: 0=OFF, 1=ON
int  setVoltage(int channel,double Voltage);//Set output <channel> to <Voltage> Volts
int  setOverVoltageProtection(int channel, double voltage);//Set output <channel> over voltage protection trip point to <voltage> Volts
int  setCurrent(int channel,double current);// Set output <channel> current limit to <current> Amps
int  setOverCurrentProtection(int channel,double current);//Set output <channel> over current protection trip point to <current> Amps
int  setDeltaVoltage(int channel,double voltage);//Set the output <channel> voltage step size to <voltage> Volts
int  GetNumberOfChannels(); // Returns the Numbers of Channels that the tti device provides.
void  disconnect(); 

static void  WhatClassAmI(void);  // shows Informations about the class



#ifndef __CINT__

//void  connect(sig f,const char* className1,void* class1,const char* slot);
#endif
//void  connect(const char* signal1,const char* className1,void* class1,const char* slot);

#ifndef __CINT__
//tti_RQSignals *RQSignals;

bool started1;
bool stop1;
#endif




int  startThread(void); // starts a thread that will emit the actual voltage and current in a certain time interval 
int  stopThread(bool waitForEnd=1); // Stops the thread. if waitForEnd is 1 by default it will wait until the thread is stop. if waitForEnd is 0 it just send a message to the thread that it should stop. 
void  showSignals(void);
void  show_errors(void); // displays the error list in the command line
TThread *t1;
	int samplingTime; // in Milliseconds
private:
	tti_CPX400(const tti_CPX400&); // disabling the copy constructor since one cannot copy the com ports
	tti_CPX400& operator=(const tti_CPX400&); //disabling assign for the same reson.
	

	

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

		void ActualVoltageCh1(double volt) //*SIGNAL*
				{Emit("ActualVoltageCh1(double)",volt);}
		void ActualVoltageCh2(double volt) //*SIGNAL*
				{Emit("ActualVoltageCh2(double)",volt);}

		void ActualCurrentCh1(double current) //*SIGNAL*
				{Emit("ActualCurrentCh1(double)",current);}
		void ActualCurrentCh2(double current) //*SIGNAL*
				{Emit("ActualCurrentCh2(double)",current);}

		// Stop Signals
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ClassDef(tti_CPX400, 1);//needed for the ROOT dict

};



#endif //tti_class