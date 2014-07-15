#pragma once


#ifndef __CINT__  // since ROOTCINT don't know the command _declspec(dllexport) one has to hide it from ROOTCINT. on the other hand the command is needed to show VS with element to export in THE DLL and that it should create a LIB file where 
	#include <vector>
	#include <string>
	#ifdef WIN32
		#define DLL_exp _declspec(dllexport)
	#else
		#define DLL_exp
	#endif // WIN32
#else
	#define DLL_exp
#endif // __CINT__
#include "RQ_OBJECT.h"

class SerialCom;

class DLL_exp TempInterlock
{
	RQ_OBJECT("TempInterlock")
public:
	TempInterlock(void);
	TempInterlock(const char *Port);
	TempInterlock(int Port);
	~TempInterlock(void);



	double getTemp(int channel);
	std::vector<double> getTempVector();
	std::string getInterlockReadSettings();
	// getter Functions

	//////////////////////////////////////////////////////////////////////////
	// Hybrid NTCs
	double getLowerTempHybrid();
	double getHigherTempHybrid();	
	int getNumberOfHybridNTCs();	  //Number of sensors enabled on the Hybrid
	//
	//////////////////////////////////////////////////////////////////////////


	//////////////////////////////////////////////////////////////////////////
	// User NTCs
	double getLowerTempUserNTC();     //Lower temperature limit for the user NTCs
	double getHigherTempUserNTC();	  //Higher temperature limit for user NTCs
	int getNumberOfUserNTCs();	      //Number of enabled user NTCs
	
	//////////////////////////////////////////////////////////////////////////


	//////////////////////////////////////////////////////////////////////////
	//  SHT 7x RH/T Sensor
	double getLowerTempSHT7x();       //Lower temperature limit for the SHT 7x RH/T sensor
	double getHigherTempSHT7x(); 	  //Higher temperature limit for SHT 7x RH/T sensor
    double getLowerHumiditySHT7x();    //Lower relative humidity limit for SHT7x sensor
    double getHigherHumiditySHT7x();   //Higher relative humidity limit for SHT7x sensor
	int getNumberOfSHT7xSensors();	  //Number of enabled Sensors
	///////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	//Enable/Disable Interlock Output
	bool InterlockOutputEnable();
	bool InterlockOutputDisable();
	bool InterlockOutputSetStatus(bool newstatus);
	//////////////////////////////////////////////////////////////////////////
	
	//////////////////////////////////////////////////////////////////////////
	// Set Temperature and Humidity limits
	bool setLowerTempHybrid(double temp);
	bool setHigherTempHybrid(double temp);
	bool setLowerTempUserNTC(double temp);
	bool setHigherTempUserNTC(double temp);
	bool setLowerTempSHT7x(double temp);
	bool setHigherTempSHT7x(double temp);
	bool setLowerHumiditySHT7x(double humidity);
	bool setHigherHumiditySHT7x(double humidity);
	// end Set Temperature and Humidity limits
	//////////////////////////////////////////////////////////////////////////
		
private:

std::string queryCommand(std::string command);
std::string getSetting(std::string setting);
bool setValue(std::string command,double value);

std::string InterlockReadSettings;
signed long long lastTime_ReadSettings;

std::string CurrentTempReadout;
signed long long lastTime_TempReadout;


#ifndef __CINT__
	SerialCom *s1;
	void startupCommands();
#endif
	ClassDef(TempInterlock, 1);//needed for the ROOT dict
};

#ifdef __CINT__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

#pragma link C++ class TempInterlock;

#endif
