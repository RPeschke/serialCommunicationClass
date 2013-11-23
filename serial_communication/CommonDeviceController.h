#pragma once
#include <vector>
//#include "tti_new.h"
#include <string>
#include "Rtypes.h"
//#include "chiller_new.h"
#include <Rtypes.h>
#include <RQ_OBJECT.h>
#define CDC_ERROR_VALUE -1000

//using namespace std;

#ifndef __CINT__  // since ROOTCINT don't know the command _declspec(dllexport) one has to hide it from ROOTCINT. on the other hand the command is needed to show VS with element to export in THE DLL and that it should create a LIB file where 
	#ifdef WIN32
		#define DLL_exp _declspec(dllexport)
	#else
		#define DLL_exp
	#endif // WIN32

	class Chiller_Medingen_C20;
	class tti_CPX400;
	

#else
	#define DLL_exp
#endif // __CINT__

class DLL_exp CommonDeviceController
{
	RQ_OBJECT("CommonDeviceController")
private:
#ifndef __CINT__ 
	bool default_status; //if true, enable devices at startup

	std::vector<tti_CPX400*> ttiControlObjects;

	struct device {std::string type; int nr; int port; int ttinr; int channel; double defaultvoltage; double defaultcurrent;};
	std::vector<device> devices;

	struct chiller {int nr; Chiller_Medingen_C20* chillercontrol; int port;double temperature;};
	std::vector <chiller> chillers;
	

	int SetVoltageCurrent( int ref, double voltage, double current );
	int SetStatus        ( int ref, bool newstatus );

	int GetDeviceReference(std::string type, int nr);
	int GetChillerReference(int nr);
	int GetPortReference(int port);
	int GetTTI(int port);
	int FindPortChannel(int port, int channel);

	double GetVoltage(int ref);
	double GetCurrent(int ref);
	bool GetStatus(int ref);
	bool IsFreeConnection(int port, int channel=-1);
	std::string PortString(int port);
	int addDevice(std::string type, int number, int port, int channel,double voltage, double current);
	int deleteDevice(std::string type,int number);
	//std::vector<deviceparameters> LVDS;
	//std::vector<port_index> device_register;
	//int getPortIndex(int port,int deviceType);
	//int getChannel(int portInd,int channel);
#endif

public:
	CommonDeviceController(const char* Filename="");
	~CommonDeviceController(void);

	int loadConfig(const char* filename);
	void debugdata();

	//LVDS Control functions
	int    addLVDS(int number, int port, int channel,double voltage, double current);
	int    deleteLVDS(int number);
	int    SetLVDS(int number, double voltage, double current);
	int    SetLVDS_Status  (int number, bool newstatus);
	double GetLVDSVoltage  (int number);
	double GetLVDSCurrent  (int number);
	bool   GetLVDS_Status  (int number);


	// Module Control functions
	int    addModule(int number, int port, int channel,double voltage, double current);
	int    deleteModule(int Number);
	int    SetModule(int number, double voltage, double current);
	int    SetModule_Status(int number, bool newstatus);
	double GetModuleVoltage(int number);
	double GetModuleCurrent(int number);
	bool   GetModule_Status(int number);


	// HSIO Control function
	int    addHSIO(int number, int port, int channel,double voltage, double current);
	int	   deleteHSIO(int Number);
	int    SetHSIO  (int number, double voltage, double current);
	int    SetHSIO_Status  (int number, bool newstatus);
	double GetHSIOVoltage  (int number);
	double GetHSIOCurrent  (int number);
	bool   GetHSIO_Status  (int number);


	//chiller control functions
	int    addChiller(int number, int port, double temperature);
	int    deleteChiller(int number);
	int    SetChillerTargetTemp(int number, double temperature);
	double GetChillerTargetTemp(int number);
	double GetChillerActualTemp(int number);
	double GetChiller_Status    (int number);

	ClassDef(CommonDeviceController, 1);//needed for the ROOT dict
};

