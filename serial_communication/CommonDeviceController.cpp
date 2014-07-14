#include "CommonDeviceController.h"
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include "Riosfwd.h"
#include <iosfwd>
#include <vector>
#include "tti_new.h"
#include "SCom_helpers.h"
#include "chiller_new.h"

using namespace std;

//constructor, reads config file and stores data in class object
CommonDeviceController::CommonDeviceController( const char* Filename)
{
	cout<<"device controller started"<<endl;
	if(strlen(Filename)>0)
		loadConfig(Filename);
}


CommonDeviceController::~CommonDeviceController(void)
{
	
	while(devices.size()>0){

		deleteDevice(devices.at(0).type,devices.at(0).nr);
	}


	while(chillers.size()>0){

		deleteChiller(chillers.at(0).nr);
	}
}


int CommonDeviceController::loadConfig( const char* Filename )
{
	default_status = true;

	double voltage,current,temperature;
	int port,channel,number;

	std::string line,linetype;
	stringstream is;
	ifstream configfile;
	configfile.open (Filename);
	while (getline(configfile,line)) { //loop over lines in config file
		if (line.size()>0)
		{
	
			is.str(line+" "); //+" " is a workaround: lines need a space after the last entry to work properly
			is >> linetype;
			if(linetype=="LVDS" || linetype=="Module" || linetype=="HSIO") {
				is >> number>> port >> channel >> voltage >> current;
				addDevice(linetype,number,port,channel,voltage,current);
			} else if (linetype=="Chiller") {
				is >> number >> port >> temperature;
				addChiller(number,port,temperature);
			} //ignore all other lines
		}
	}
	configfile.close();

	return 1;
}





//functions to set voltage and current.
int CommonDeviceController::SetVoltageCurrent( int ref, double voltage, double current )
{
	if (ref==CDC_ERROR_VALUE)
	{
		cout<<"could not find device to set voltage/current"<<endl;
		return CDC_ERROR_VALUE;
	}
	//device mydevice=devices[ref];
	if(voltage==-1){
		voltage=devices.at(ref).defaultvoltage;
	}
	if(current==-1){
		current=devices.at(ref).defaultcurrent;
	}
	
	int good1=ttiControlObjects[devices.at(ref).ttinr]->setVoltage(devices.at(ref).channel,voltage);
	int good2=ttiControlObjects[devices.at(ref).ttinr]->setCurrent(devices.at(ref).channel,current);
	
	if(good1>=0 && good2>=0){
		return 1;
	}
	
	return CDC_ERROR_VALUE;
}

/* Set voltage/current for LVDS number n
 * both voltage and current are optional: use -1 to set them to their default values (as given by config file) */
int CommonDeviceController::SetLVDS( int number, double voltage, double current ) 
{
	int ref=GetDeviceReference("LVDS",number);
	return SetVoltageCurrent(ref,voltage,current);
}
/* Set voltage/current for Module number n
 * both voltage and current are optional: use -1 to set them to their default values (as given by config file) */
int CommonDeviceController::SetModule( int number, double voltage, double current )
{
	int ref=GetDeviceReference("Module",number);
	return SetVoltageCurrent(ref,voltage,current);
}
/* Set voltage/current for HSIO number n
 * both voltage and current are optional: use -1 to set them to their default values (as given by config file) */
int CommonDeviceController::SetHSIO( int number, double voltage, double current )
{
	int ref=GetDeviceReference("HSIO",number);
	return SetVoltageCurrent(ref,voltage,current);
}
//end voltage and current


//activate/deactivate devices
int CommonDeviceController::SetStatus( int ref, bool newstatus )
{
	if (ref!=CDC_ERROR_VALUE)
	{
		int ttinr=devices[ref].ttinr;
		int channel = devices[ref].channel;
		if(newstatus)
			return ttiControlObjects[ttinr]->enable(channel);
		else
			return ttiControlObjects[ttinr]->disable(channel);
	}
	cout<<"tried to control invalid controller!\n";
	return CDC_ERROR_VALUE;
}

int CommonDeviceController::SetLVDS_Status( int number, bool newstatus )
{
	int ref=GetDeviceReference("LVDS",number);
	return SetStatus(ref, newstatus);
}
int CommonDeviceController::SetModule_Status( int number, bool newstatus )
{
	int ref=GetDeviceReference("Module",number);
	return SetStatus(ref, newstatus);
}
int CommonDeviceController::SetHSIO_Status( int number, bool newstatus )
{
	int ref=GetDeviceReference("HSIO",number);
	return SetStatus(ref, newstatus);
}
//end activate/deactivate devices


//get voltage
double CommonDeviceController::GetVoltage(int ref)
{
	if (ref!=CDC_ERROR_VALUE)
	{
		device mydevice=devices[ref];
		return ttiControlObjects[mydevice.ttinr]->getVoltage(mydevice.channel);
	}
	cout<<"could not find device to read voltage"<<endl;
	return CDC_ERROR_VALUE;
}

double CommonDeviceController::GetLVDSVoltage( int number )
{
	int ref=GetDeviceReference("LVDS",number);
	return GetVoltage(ref);
}
double CommonDeviceController::GetModuleVoltage( int number )
{
	int ref=GetDeviceReference("Module",number);
	return GetVoltage(ref);
}
double CommonDeviceController::GetHSIOVoltage( int number )
{
	int ref=GetDeviceReference("HSIO",number);
	return GetVoltage(ref);
}
//end get voltage


//get current
double CommonDeviceController::GetCurrent(int ref)
{
	if (ref!=CDC_ERROR_VALUE)
	{
		device mydevice=devices[ref];
		return ttiControlObjects[mydevice.ttinr]->getCurrent(mydevice.channel);
	}
	cout<<"could not find device to read current"<<endl;
	return CDC_ERROR_VALUE;
}

double CommonDeviceController::GetLVDSCurrent( int number )
{
	int ref=GetDeviceReference("LVDS",number);
	return GetCurrent(ref);
}
double CommonDeviceController::GetModuleCurrent( int number )
{
	int ref=GetDeviceReference("Module",number);
	return GetCurrent(ref);
}
double CommonDeviceController::GetHSIOCurrent( int number )
{
	int ref=GetDeviceReference("HSIO",number);
	return GetCurrent(ref);
}
//end get Current


//get device status
bool CommonDeviceController::GetStatus(int ref)
{
	if (ref!=CDC_ERROR_VALUE)
	{
		//device mydevice=devices[ref];
		return ttiControlObjects[devices.at(ref).ttinr]->getStatus(devices.at(ref).channel);
	}
	cout<<"could not find device to read status"<<endl;
	return false;
}
bool CommonDeviceController::GetLVDS_Status( int number )
{
	int ref=GetDeviceReference("LVDS",number);
	return GetStatus(ref);
}
bool CommonDeviceController::GetModule_Status( int number )
{
	int ref=GetDeviceReference("Module",number);
	return GetStatus(ref);
}
bool CommonDeviceController::GetHSIO_Status( int number )
{
	int ref=GetDeviceReference("HSIO",number);
	return GetStatus(ref);
}
//end get device status


//chiller handling
int CommonDeviceController::SetChillerTargetTemp( int number, double temperature )
{
	int ref=GetChillerReference(number);
	if(ref!=CDC_ERROR_VALUE){
		return chillers[ref].chillercontrol->setTargetTemp(temperature);
	}
	cout<<"Could not find chiller number "<<number<<"!"<<endl;
	return CDC_ERROR_VALUE;
}
double CommonDeviceController::GetChillerTargetTemp( int number )
{
	int ref=GetChillerReference(number);
	if(ref!=CDC_ERROR_VALUE)
		return chillers[ref].chillercontrol->getTargetTemp();
	cout<<"Could not find chiller number "<<number<<"!"<<endl;
	return CDC_ERROR_VALUE;
}
double CommonDeviceController::GetChillerActualTemp( int number )
{
	int ref=GetChillerReference(number);
	if(ref!=CDC_ERROR_VALUE)
		return chillers[ref].chillercontrol->getActualTemp();
	cout<<"Could not find chiller number "<<number<<"!"<<endl;
	return CDC_ERROR_VALUE;
}
double CommonDeviceController::GetChiller_Status( int number )
{
	int ref=GetChillerReference(number);
	if(ref!=CDC_ERROR_VALUE)
		return chillers[ref].chillercontrol->getStatus();
	cout<<"Could not find chiller number "<<number<<"!"<<endl;
	return CDC_ERROR_VALUE;
}
//end chiller handling

// internal stuff
//returns the port id for the tti power supply. 
int CommonDeviceController::GetTTI( int port )
{
	int portind=GetPortReference(port);	
	if (portind==CDC_ERROR_VALUE)// port not opened yet. creating a new connection
	{

		if(IsFreeConnection(port)) //verify that port is not used by a chiller
		{
			ttiControlObjects.push_back(new tti_CPX400(port));
			return ttiControlObjects.size()-1;
		} else {
			cout<<"port "<<port<<" is already in use by something else (Chiller?)"<<endl;
			return CDC_ERROR_VALUE;
		}

	} else if(portind>=0) { //ok, port is already open, we can use it
		return portind;
	}
	//this should never happen
	cout<<"bug in CDC::addTTi or CDC::GetPortReference: incompatible return value";
	return CDC_ERROR_VALUE;
}

int CommonDeviceController::GetPortReference(int port)
{
	for (int i=0 ; i<ttiControlObjects.size() ; ++i) {
		if (ttiControlObjects[i]->getPort()==PortString(port)){
			return i;
		}
	}
	return CDC_ERROR_VALUE;
}

int CommonDeviceController::GetDeviceReference( std::string type, int nr )
{
	for(int i=0 ; i<devices.size() ; ++i)
		if(devices[i].type==type && devices[i].nr==nr){
			return i;
		}

	return CDC_ERROR_VALUE;
}

int CommonDeviceController::GetChillerReference( int nr )
{
	for(int i=0 ; i<chillers.size(); ++i)
		if(chillers[i].nr==nr){
			return i;
		}
	return CDC_ERROR_VALUE;
}

int CommonDeviceController::FindPortChannel( int port, int channel )
{
	for(int i=0 ; i<devices.size() ; ++i)
		if(devices[i].port==port && devices[i].channel==channel){
			return i;
		}
	return CDC_ERROR_VALUE;
}

void CommonDeviceController::debugdata()
{
	for (int i=0 ; i<ttiControlObjects.size() ; ++i)
		cout<<"tti "<<i<<" has port "<<ttiControlObjects[i]->getPort()<<endl;
	for(int i=0 ; i<devices.size() ; ++i)
		cout<<"Controller "<<devices[i].type<<" "<<devices[i].nr<<" (obj "<<i<<") has port:"<<devices[i].port<<" channel:"<<devices[i].channel<<" ttinr:"<<devices[i].ttinr<<endl;
	for(int i=0 ; i<chillers.size() ; ++i)
		cout<<"Chiller "<<chillers[i].nr<<"(obj "<<i<<") has port "<<chillers[i].chillercontrol->getPort()<<endl;
}

std::string CommonDeviceController::PortString( int port )
{
	return SComHelper::convertInt2PortString(port);
}

bool CommonDeviceController::IsFreeConnection(int port, int channel)
{
	bool IsFree=true;
	for(int i=0 ; i<chillers.size(); ++i)
		if(chillers[i].port==PortString(port))
			IsFree=false;
	for(int i=0 ; i<devices.size() ; ++i)
		if(devices[i].port==port)
			if(channel==-1 || channel==devices[i].channel)
				IsFree=false;
	return IsFree;
}
//end internal stuff


//functions to add devices
int CommonDeviceController::addDevice(string type, int number, int port, int channel,double voltage, double current )
{
	if(GetDeviceReference(type,number)!=CDC_ERROR_VALUE)
	{
		cout<<"error in adding "<<type<<" "<<number<<": This number is already in use!"<<endl;
		cout<<"Ignoring this command!"<<endl;
		return CDC_ERROR_VALUE;
	}
	if(!IsFreeConnection(port,channel))
	{
		cout<<"error in adding "<<type<<" "<<number<<": port "<<port<<", channel "<<channel<<" is already in use!"<<endl;
		cout<<"Ignoring this command!"<<endl;
		return CDC_ERROR_VALUE;
	}
	int portindex=GetTTI(port);
	device newdevice = {type, number, port, portindex, channel, voltage, current};
	devices.push_back(newdevice);
	SetVoltageCurrent(devices.size()-1,voltage,current);
	SetStatus(devices.size()-1,default_status);
	return number;
}

int CommonDeviceController::addLVDS(int number, int port, int channel,double voltage, double current)
{
	return addDevice("LVDS",number,port,channel,voltage,current);
}
int CommonDeviceController::addModule( int number, int port, int channel,double voltage, double current )
{
	return addDevice("Module",number,port,channel,voltage,current);
}
int CommonDeviceController::addHSIO( int number, int port, int channel,double voltage, double current )
{
	return addDevice("HSIO",number,port,channel,voltage,current);
}
int CommonDeviceController::addChiller(int number, int port, double temperature)
{
	if(!IsFreeConnection(port))
	{
		cout<<"error in adding chiller "<<number<<": port "<<port<<" is already in use!"<<endl;
		cout<<"Ignoring this command!"<<endl;
		return CDC_ERROR_VALUE;
	} else {
		chiller newchiller = {number, new Chiller_Medingen_C20(PortString(port).c_str()),port, temperature};
		chillers.push_back(newchiller);
		SetChillerTargetTemp(number,temperature);
	}
}

//end functions to add devices

// functions to delete devices
int CommonDeviceController::deleteDevice( std::string type,int number )
{ 
	int ref=GetDeviceReference(type,number);
	if(ref==CDC_ERROR_VALUE)
	{
		cout<<"error in deleting "<<type<<" "<<number<<": This number does not exist!"<<endl;
		cout<<"Ignoring this command!"<<endl;
		return CDC_ERROR_VALUE;
	}

	int port=devices.at(ref).port;
	int channel=devices.at(ref).channel;
	int portid=	GetPortReference(port);
	

	devices.erase(devices.begin()+ref);
	
	if(IsFreeConnection(port,-1))
	{  // device is free to be deleted
		if (ttiControlObjects.at(portid)!=nullptr)
		{
		
		delete ttiControlObjects.at(portid);

		ttiControlObjects.erase(ttiControlObjects.begin()+portid);
		}
	}

	return 1;
}


int CommonDeviceController::deleteHSIO( int Number )
{
	return deleteDevice("HSIO",Number);
}

int CommonDeviceController::deleteModule( int Number )
{
	return deleteDevice("Module",Number);
}

int CommonDeviceController::deleteLVDS( int number )
{
	return deleteDevice("LVDS",number);
}

int CommonDeviceController::deleteChiller( int number )
{
	 int ref=GetChillerReference(number);
	 if (ref==CDC_ERROR_VALUE)
	 {
		 cout<<"error in deleting Chiller "<<number<<": This number does not exist!"<<endl;
		 cout<<"Ignoring this command!"<<endl;

		 return CDC_ERROR_VALUE;

	 }
	 
	 if (chillers.at(ref).chillercontrol!=nullptr)
	 {
		 delete chillers.at(ref).chillercontrol;

	 }
	 chillers.erase(chillers.begin()+ref);


	 return 1;
}

