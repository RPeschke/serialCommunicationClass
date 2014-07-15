#include "tti_CPX400.h"

#include <iostream>
#include <string>
#include <sstream>
#include "SCom_helpers.h"
#include "SerialCom.h"
#include <TGraph.h>
#include <TTimeStamp.h>
#include <TCanvas.h>
#ifdef WIN32
#include <Windows.h>
#else
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#define Sleep(a) gSystem->Sleep(a)

#endif
#include <stdlib.h>


using namespace std;
using namespace SComHelper;


/// <summary>
/// this is the background worker of the tti class
///</summary>
void *Worker_TTi(void *ptr) {

	tti_CPX400* tti_device=(tti_CPX400*)ptr;

	TTimeStamp stamp, now;
	Double_t start_time=stamp.AsDouble();
// 
 	while(tti_device->started1){
		stamp.Set();
		tti_device->getActualVoltage(1); // these functions emits signals when they were called. 
		tti_device->getActualVoltage(2);
		tti_device->getActualCurrent(1);
		tti_device->getActualCurrent(2);
	

		now.Set();
#ifdef _DEBUG
		std::cout<<tti_device->samplingTime<<std::endl;
		std::cout<<(now.AsDouble()-stamp.AsDouble())<<std::endl;
		std::cout<<((tti_device->samplingTime)-now.AsDouble()*1000+stamp.AsDouble()*1000)<<std::endl;

#endif
	Sleep(max(((tti_device->samplingTime)/1000-now.AsDouble()+stamp.AsDouble())*1000,1000)); // controls the sampling rate 
	//Sleep(1000);
	now.Set();
#ifdef _DEBUG
	//td::cout<<tti_device->samplingTime<<std::endl;
	//std::cout<<(now.AsDouble()-stamp.AsDouble())<<std::endl;
#endif
 	}
#ifdef _DEBUG
	std::cout<<"end"<<std::endl;
#endif
	tti_device->stop1=1;
	return 0;
}


tti_CPX400::tti_CPX400( const char * port)
{
	SComunication =new SerialCom(port);
//	RQSignals=new tti_RQSignals();
	SComunication->timeBetweenSendAndReciev=10;
	SComunication->TimeOut_=500;
	samplingTime=1000;
	started1=0;
	stop1=1;
getID();
}

tti_CPX400::tti_CPX400( int Port )
{
	SComunication =new SerialCom(Port);
	//	RQSignals=new tti_RQSignals();
	samplingTime=1000;
	started1=0;
	stop1=1;
	getID();
}

tti_CPX400::~tti_CPX400(void)
{
	
#ifdef _DEBUG

	cout<< "destructor of tti"<<endl;
#endif // _DEBUG
	delete SComunication;


}

string tti_CPX400::getID( void )
{


	string s2=SComunication->query("*IDN?");
	int tries=0;
	size_t foundError=foundError=s2.find("error"); // since query returns "error" if something went wrong one has to check for it
	while(s2.size()<1&&foundError!=string::npos&&tries<10) // it occurs that the device the device doesn't respond correctly even if everything is correct, therefore i want to redo the query 10 times or untile it has worked. 
	{
		s2=SComunication->query("*IDN?");
		foundError=s2.find("error"); // since query returns "error" if something went wrong one has to check for it
		++tries;
	}
	
	if(s2.size()>0&&foundError==string::npos){
		string temp;
		size_t index1=s2.find(","); // the individual parts of the identification string are separated by commas.
									// it is expected that the string looks like this "THURLBY THANDAR, CPX400DP,  354966, 1.01 - 3.00"
									//													<NAME>        ,<Modell> ,<Seriennummer>,<Version><rmt>,

		#if _DEBUG
			cout<<index1<<endl;
		#endif

		if (index1<s2.capacity()) 
		{
				name=s2.substr(0,index1); // cuts out the name
				#if _DEBUG  // in debug mode it is nice to have all this information printed out.
					cout<<name<<endl;
				#endif
		temp=s2.substr(index1+1);// temp only contains everything after the first comma
		index1=temp.find(",");   // returns the first comma in the remaining string 
		modell=temp.substr(1,index1-1);// cuts out the modell information. the string has a space direct after the comma so it starts with 1 and not with 0
#if _DEBUG
		cout<<modell<<endl; 
#endif

		temp=temp.substr(index1+1); // same procedure  as in the block above maybe one should but it in a helper function...
		index1=temp.find(",");
		
		SerialNumber=string_to_double(temp.substr(0,index1));// converts the string to an int here it is not needed to take care of the space before the number
		version=temp.substr(index1+1);           // everything what remains is the version number 
		if (modell.compare("CPX400SP")==0)  // the SP version of this device has only one channel
		{
			numberOfChannels=1;
		}
		else if(modell.compare("CPX400DP")==0) // the DP version has two channels

		{
			numberOfChannels=2;
		}
		else if(modell.compare("QL355TP")==0) // the DP version has two channels

		{
			numberOfChannels=3;
		}
		else
		{
			SComunication->addError("error in string tti_CPX400::getID( void )");
			SComunication->addError("device not recognized");
			SComunication->addError("----------");
		}
	
		return s2;
		}
		else {

			SComunication->addError("error in string tti_CPX400::getID( void )");
			SComunication->addError("string pattern not recognized");
			SComunication->addError("----------");
			return "error" ;  // returns error if the string does not contains a comma code "if (index1<s2.capacity()) "
		}
	}else
	{
		SComunication->addError("error in string tti_CPX400::getID( void )");
		SComunication->addError("device does not respond");
		SComunication->addError("----------");
	return "error";} // returns error if the string is empty or contains "error"


}



std::string tti_CPX400::getCommand( std::string command1,int channel,std::string command2 )
{// this is the get function. all other get methods will just call this function. 

	string temp;

	if(channel>=1&&channel<=GetNumberOfChannels())// it checks if the number of channels are correct for the device you are using
	{

		temp=val2string(channel);// this converts the int in a string 

		command1+=temp;
		command1+=command2;// this connects all 3 parts of the string together 

	} 

	else
	{
		SComunication->addError("error in std::string tti_CPX400::getCommand( std::string command1,int channel,std::string command2 )");
		SComunication->addError("wrong Channel");
		SComunication->addError("----------");
		return "wrong channel"; // returns "wrong channel" if you are trying to acces a wrong channel.
	}

	  
	  string device_Answer=SComunication->query(command1); // sends the command to the device. and waits for the answer
	  int tries=0; 
	  size_t foundError=device_Answer.find("error"); // since query returns "error" if something went wrong one has to check for it
	  while(device_Answer.size()<1&&foundError!=string::npos&&tries<10) // it occurs that the device the device doesn't respond correctly even if everything is correct, therefore i want to redo the query 10 times or until it has worked. 
	  {
		  device_Answer=SComunication->query(command1);
		  foundError=device_Answer.find("error"); // since query returns "error" if something went wrong one has to check for it
		  ++tries;
	  }
	return device_Answer;  // eventually it will return the output from the query method this means it is possible to return either the correct value, an empty string or an string which contains some error message
		
}


int  tti_CPX400::setCommand( std::string command,int channel,double value )
{
	if (channel==0)// if you want to change a value in both channels type in channel 0 
	{
		int returnValue_New,returnValue_Old=1;// the return value will be the lowest of all individual return values

		for (int i=1;i<=GetNumberOfChannels();++i)
		{
			returnValue_New=setCommand(command,i,value);
			returnValue_Old=min(returnValue_Old,returnValue_New);// since the min macro is just a macro it will execute the get command twice. once to make the comparison and once to return the value
		}
		return returnValue_Old;
	}

	if (channel>=1&&channel<=GetNumberOfChannels())
	{

		command+=val2string(channel); // since val2string is a template one can use it for int and double
		command+=" ";

		command+=val2string(value);
	} 

	else
	{
		SComunication->addError("error in int  tti_CPX400::setCommand( std::string command,int channel,double value )");
		SComunication->addError("wrong Channel");
		SComunication->addError("----------");

		return -2;
	}
	return SComunication->send(command); // returns 0 if ok else -1 // it is somehow inconsistent which one is the default value some times it is 0 some times it is 1 
}


double tti_CPX400::getVoltage( int Channel )
{

	std::string answers =getCommand("V",Channel,"?"); // ask the question

	if (answers.size()<3) {
		return -2;
	}

	if (isNumeric(answers.substr(3))) {
		return string_to_double(answers.substr(3)); // deletes the first 3 letters and converts to a double
	}
	else {
		SComunication->addError("error in double tti_CPX400::getVoltage( int Channel )");
		SComunication->addError("return value is not a number");
		SComunication->addError("----------");
		
		return -1;
	}

}

double tti_CPX400::getActualVoltage( int channel )
{
	std::string answers =getCommand("V",channel,"O?"); // ask the question

	if (isNumeric(answers.substr(0,answers.size()-3)))
	{
		double return_value=string_to_double(answers.substr(0,answers.size()-3)); // deletes last letters and converts to a double
	
		if (channel==1){ // this part is needed to emit the voltage as signal. 
			ActualVoltageCh1(return_value);
		}
		else if (channel==2)
		{
			ActualVoltageCh2(return_value);
		}
		return return_value;
	}
	else {
		#ifdef _DEBUG
			ActualVoltageCh1(-1); // in release it is not wanted that the device is sending error messages around. but for debug purpose it can be useful. especially if you don't have a TTi in reach  
		#endif
			SComunication->addError("error in double tti_CPX400::getActualVoltage( int channel )");
			SComunication->addError("return value is not a number");
			SComunication->addError("----------");

		return -1;
	}
}


double tti_CPX400::getCurrent( int Channel )
{
	std::string answers =getCommand("I",Channel,"?"); // ask the question
	if (answers.size() < 3){
		return -2;
	}
	else {
		if (isNumeric(answers.substr(3)))
		{	
			double return_value=string_to_double(answers.substr(3)); // deletes the first two letters and converts to a double  // the device does not return just the numbers so one has to cut the numbers out of the string

			return return_value;
	
		}
		else {
			SComunication->addError("error in getCurrent(int Channel)");
			SComunication->addError("return value is not a number");
			SComunication->addError("----------");
			return -1;
		}
	}
}

double tti_CPX400::getActualCurrent( int channel )
{

	std::string answers =getCommand("I",channel,"O?"); // ask the question

	std::string substring=answers.substr(0,answers.size()-3);// deletes last letters // the device does not return just the numbers so one has to cut the numbers out of the string
	if (isNumeric(substring))
	{
		double return_value=string_to_double(substring); // converts to a double
		if (channel==1){
			ActualCurrentCh1(return_value);  //emits the signal 
		}
		else if (channel==2)
		{
			ActualCurrentCh2(return_value); //emits the signal 
		}
		return return_value;
	}
	else {
		SComunication->addError("error in double tti_CPX400::getActualCurrent( int channel )");
		SComunication->addError("return value is not a number");
		SComunication->addError("----------");
		return -1;// since I don t want to use the signal slot system for debugging it don t emit error values 
	}
}


int tti_CPX400::enable( int channel )
{
	return setStatus(channel,1); 
}

int tti_CPX400::disable( int channel )
{
	return setStatus(channel,0);

}

int tti_CPX400::setStatus( int channel, bool on_off )
{
	return setCommand("OP",channel,on_off);// returns 1 if ok else -1
}

int tti_CPX400::setVoltage( int channel,double Voltage )
{
	return setCommand("V",channel,Voltage);// returns 1 if ok else -1
}

int tti_CPX400::setOverVoltageProtection( int channel, double voltage )
{

	return setCommand("OVP",channel,voltage);// returns 1 if ok else -1
}

int tti_CPX400::setCurrent( int channel,double current )
{

	return setCommand("I",channel,current);// returns 1 if ok else -1
}

int tti_CPX400::setOverCurrentProtection( int channel,double current )
{

	return setCommand("OCP",channel,current);
}

double tti_CPX400::getOverVoltageProtection( int channel )
{

	std::string answers =getCommand("OVP",channel,"?"); // ask the question

	return string_to_double(answers); // converts to a double

}

double tti_CPX400::getOverCurrentProtection( int channel )
{

	std::string answers =getCommand("OCP",channel,"?"); // ask the question
	return string_to_double(answers); //  converts to a double

}




bool tti_CPX400::getStatus( int channel )
{

	string answers =getCommand("OP",channel,"?"); // ask the question
	return SComHelper::string_to_double(answers); //  converts to a double
}



int tti_CPX400::GetNumberOfChannels()
{ 

	if (numberOfChannels) // checks if the class knows already the numbers of channel. this should be the normal case because the channel number is set in the getID() method and this one is called in the constructor.
	{
		return numberOfChannels;
	} 
	else
	{
		getID(); // if for some reason the number of channels in not known it will try to find it out
		return numberOfChannels;
	}
}

string  tti_CPX400::getAddress( void )
{

		return SComunication->query("ADDRESS?");


}

int tti_CPX400::getSerialNumber( void )
{
	if (!SerialNumber)
	{
		getID();
	} 


	return SerialNumber; 

}

string  tti_CPX400::getSerialNumber_str( void )
{

return SComHelper::val2string(getSerialNumber());

	
}

int  tti_CPX400::setDeltaVoltage( int channel,double voltage )
{
	return setCommand("DELTAV",channel,voltage);
}

void  tti_CPX400::disconnect()
{
SComunication->disconnect();
}

void  tti_CPX400::WhatClassAmI( void )
{
	// this method shows all methods of the class
	// it is expected that a user might not have the header or source full but just the DLL.
	// so put everything of importance in here. right now it is just some information from the header file.
cout<<" ------------------------------------------------------------------           "<<endl; 
cout<<" |  This is a controller class for a THURLBY THANDAR INSTRUMENTS  |           "<<endl;
cout<<" |  power supply CPX400SP and CPX400DP                            |           "<<endl;
cout<<" |  http://www.tti-test.com                                       |           "<<endl;
cout<<" ------------------------------------------------------------------           "<<endl;
cout<<" list of Commands:"<<endl;
cout<<""<<endl;
cout<<""<<endl;
cout<<"double getVoltage(int Channel); "<<endl;
cout<<"		//Returns the set voltage of"<<endl;
cout<<"		//output <Channel> The response is "<<endl;
cout<<"		//Voltages in volt as double "<<endl;
cout<<""<<endl;
cout<<"double getActualVoltage(int channel);"<<endl;
cout<<"		// Returns the output "<<endl;
cout<<"		// readback voltage for output"<<endl;
cout<<"		// <channel> The response is voltage "<<endl;
cout<<"		// in volt as double"<<endl;
cout<<""<<endl;
cout<<"double getCurrent(int channel);"<<endl;
cout<<"		// Returns the set current limit "<<endl;
cout<<"		// of output <channel> The response is "<<endl;
cout<<"		// current in Ampere as double "<<endl;
cout<<""<<endl;
cout<<"double getActualCurrent(int channel);"<<endl;
cout<<"		// Returns the output "<<endl;
cout<<"		// readback current for output "<<endl;
cout<<"		// <channel> The response is current in"<<endl;
cout<<"		// ampere as double"<<endl;
cout<<""<<endl;
cout<<"double getOverVoltageProtection(int channel);"<<endl;
cout<<"		// Returns the voltage trip setting "<<endl;
cout<<"		// for output <channel> The response is "<<endl;
cout<<"		// Voltage in volt as double  "<<endl;
cout<<""<<endl;
cout<<"double getOverCurrentProtection(int channel);"<<endl;
cout<<"		// Returns the current trip "<<endl;
cout<<"		// setting for output <channel> "<<endl;
cout<<"		// The response current in amps "<<endl;
cout<<"		// as double  "<<endl;
cout<<""<<endl;
cout<<"bool  getStatus(int channel);"<<endl;
cout<<"		// Returns output <channel> on/off status. "<<endl;
cout<<"		// The response is status as bool"<<endl;
cout<<"		// where "<<endl;
cout<<"		// 1 (true) = ON,"<<endl;
cout<<"		// 0 (false) = OFF.  "<<endl;
cout<<""<<endl;
cout<<"std::string getID(void);"<<endl;
cout<<"		// Returns the instrument identification."<<endl;
cout<<"		// The exact response is determined by the "<<endl;
cout<<"		// instrument configuration and is of the "<<endl;
cout<<"		// form <NAME>,<model>,<Serial No.>, <version><rmt> "<<endl;
cout<<"		// where <NAME> is the manufacturer's name, "<<endl;
cout<<"		// <model> defines the type of instrument, "<<endl;
cout<<"		// Serial No. is the unique serial number "<<endl;
cout<<"		// and <version> is the revision level of "<<endl;
cout<<"		// the software installed. The format of "<<endl;
cout<<"		// <version> is X.xx – Y.yy where X.xx is "<<endl;
cout<<"		// the revision of the main instrument "<<endl;
cout<<"		// firmware and Y.yy is the revision of "<<endl;
cout<<"		// the Interface board firmware. The "<<endl;
cout<<"		// Interface firmware is field updateable "<<endl;
cout<<"		// by the user via the USB port."<<endl;
cout<<""<<endl;
cout<<"int getSerialNumber(void);"<<endl;
cout<<"		// returns the serial number of the device"<<endl;
cout<<""<<endl;
cout<<"std::string DLL_exp getSerialNumber_str(void);"<<endl;
cout<<"	// returns the serial number of the device as string"<<endl;
cout<<""<<endl;
cout<<"std::string getAddress(void);"<<endl;
cout<<"		// Returns the bus address of the "<<endl;
cout<<"		// instrument; This is the address "<<endl;
cout<<"		// used by GPIB, if fitted, or may "<<endl;
cout<<"		// be used as a general identifier "<<endl;
cout<<"		// over the other interfaces.*/"<<endl;
cout<<""<<endl;
cout<<"//set ..."<<endl;
cout<<""<<endl;
cout<<"int enable(int channel);"<<endl;
cout<<"		// enables output <channel>"<<endl;
cout<<""<<endl;
cout<<"int disable(int channel);"<<endl;
cout<<"		// disables output <channel>"<<endl;
cout<<""<<endl;
cout<<"int setStatus(int channel, bool on_off);"<<endl;
cout<<"		// set output <channel> on/off where <on_off> "<<endl;
cout<<"		// has the following meaning: 0=OFF, 1=ON"<<endl;
cout<<""<<endl;
cout<<"int setVoltage(int channel,double Voltage);"<<endl;
cout<<"		// Set output <channel> to <Voltage> Volts"<<endl;
cout<<""<<endl;
cout<<"int setOverVoltageProtection(int channel, double voltage);"<<endl;
cout<<"		// Set output <channel> over voltage protection trip "<<endl;
cout<<"		// point to <voltage> Volts"<<endl;
cout<<""<<endl;
cout<<"int setCurrent(int channel,double current);"<<endl;
cout<<"		// Set output <channel> current limit to <current> Amps"<<endl;
cout<<""<<endl;
cout<<"int setOverCurrentProtection(int channel,double current);"<<endl;
cout<<"		// Set output <channel> over current protection trip "<<endl;
cout<<"		// point to <current> Amps"<<endl;
cout<<""<<endl;
cout<<"int setDeltaVoltage(int channel,double voltage);"<<endl;
cout<<"		// Set the output <channel> voltage step size to "<<endl;
cout<<"		// <voltage> Volts"<<endl;
cout<<""<<endl;
cout<<"int GetNumberOfChannels();"<<endl;
cout<<"		// Returns the Numbers of Channels that the tti "<<endl;
cout<<"		// device provides."<<endl;
	// put any informations you want to add right here!
}

// void tti_CPX400::connect( const char* signal1,const char* className1,void* class1,const char* slot )
// {
// 	RQSignals->Connect(signal1,className1,class1,slot);// connect a signal to a slot in a remote class. 
// }
// 
// void tti_CPX400::connect( sig f,const char* className1,void* class1,const char* slot )
// {// this method has the advantage of using enum to specify the Signals it make the use of it much more convinient but is not for use with CINT 
// 	RQSignals->Connect(RQSignals->SignalName(f),className1,class1,slot);
// }

int  tti_CPX400::startThread()
{
	if(started1==0) // checks if the thread is already started since it cannot run twice.
	{
	started1=1;// sets value to started 
	stop1=0;
	t1 = new TThread("h1", Worker_TTi, (void*) this); // this thread will ask the TTi for its actual current and its actual Voltage and emits this information as signals. 
	
	t1->Run();
	return 1;
	}

	return -1;


}

int tti_CPX400::stopThread(bool waitForEnd)
{
	if (started1>0){ 

	started1=0;// says to the worker function to stop asap. this can take some seconds depending on the sampling rate
	if(waitForEnd==1) // if waitForEnd is 0 it will skip the waiting process
	{
t1->Join();
	}

return 1;
}
return -1;
}

void  tti_CPX400::showSignals()
{


				cout<<"ActualVoltageCh1(double)"<<endl;
				cout<<"ActualVoltageCh2(double)"<<endl;
				cout<<"ActualCurrentCh1(double)"<<endl;
				cout<<"ActualCurrentCh2(double)"<<endl;
}

void tti_CPX400::show_errors( void )
{
SComunication->show_errors();
}

TString  tti_CPX400::getAddress_TString( void )
{
return stdString2TString(getAddress());
}

TString tti_CPX400::getSerialNumber_TString( void )
{
return stdString2TString(getSerialNumber_str());
}

TString  tti_CPX400::getID_TString( void )
{
	return stdString2TString(getID());
}

std::string tti_CPX400::getPort()
{
	return SComunication->getPort();
}

// void DLL_exp tti_CPX400::connect( sig f,const char* className1,void* class1,const char* slot )
// {
// 
// }
// 
