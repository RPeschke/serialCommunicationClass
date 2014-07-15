#include "TempInterlock.h"
#include <iostream>
#include "SCom_helpers.h"
#ifdef WIN32
#include <Windows.h>
#endif // WIN32

#include <time.h>
#include "SerialCom.h"
using namespace std;

TempInterlock::TempInterlock(void)
{
}

TempInterlock::TempInterlock( const char *Port )
{
	s1=new SerialCom(Port,115200);
	startupCommands();
}

TempInterlock::TempInterlock( int Port )
{
	s1=new SerialCom(Port,115200);
	startupCommands();
}


void TempInterlock::startupCommands()
{
	s1->setTerminateCommand("\r");
	s1->timeBetweenSendAndReciev=500; // the interlock is extremely slow therefor one has to wait 500 ms after sending a command before one can receive the answer.
	s1->SizeOfReadString=200;
	lastTime_ReadSettings=0;
	lastTime_TempReadout=0;
}


TempInterlock::~TempInterlock(void)
{
	delete s1;
}

double TempInterlock::getTemp( int channel )
{
	double returnValue=-3000; // this is the default return Value of this function it will only be changed if the device respons was correct.

	auto rec=getTempVector();
	if (channel<rec.size())
	{
		returnValue=rec.at(channel);
	}


	return returnValue;
}

std::vector<double> TempInterlock::getTempVector()
{
	std::vector<double> returnValue;
	signed long long seconds = time (NULL);
	
	if ((seconds-lastTime_TempReadout)>5)
	{

	CurrentTempReadout=queryCommand("idr, 100");  
	lastTime_TempReadout=seconds;
#ifdef _DEBUG
	cout<<"reload Temp"<<endl;
#endif
	}
	returnValue=SComHelper::string2Vector(CurrentTempReadout);
	return returnValue;
}
std::string TempInterlock::getInterlockReadSettings()
{
	signed long long seconds = time (NULL);
	if ((seconds-lastTime_ReadSettings)>5)
	{
		InterlockReadSettings =queryCommand("irs, 100");
		lastTime_ReadSettings=seconds;

#ifdef _DEBUG
		cout<<"reload settings"<<endl;
#endif
	}


	return InterlockReadSettings;  
}

std::string TempInterlock::queryCommand( std::string command )
{
	string returnValue ="error";// error return Value. 

	// it is possible that the device doesn't respond correct therefor it tries up to 10 times to get the correct the Value
	for(int tries=0;tries<10;++tries){ 

// 		s1->send(command);
// 		Sleep(1000);
	string answer=s1->query(command);



	// since the expected structure of the answer is "\r\nReceived: <command>\r\n..."
	// the program checks if it can find the command string in the answer string.
	// the device does not return the received command but the understood command this can give a problem.
	// for example: if one tries to send the command "idr,100" to the device it will understand it correctly but the response from the device is:
	// "\r\nReceived: idr, 100\r\n..." where you have a space between idr, and 100. one has to be very careful to use the command correct.

		auto cuttedString=SComHelper::CutOutSubstring(answer,"Received:","\r\n"); // this function returns a vector of either 1 element if it has not found anything or 3 if it has found something

		if (cuttedString.size()==3)
		{
			auto search_string=cuttedString.at(1);
			auto search_index=search_string.find(command);

			if (search_index< search_string.size() )
			{
				//cout<< "success after "<< tries +1 <<" tries"<<endl;
				returnValue=cuttedString.at(2);
				break;
 			}
//			else{
// 
// 				cout<<"error read: "<< s1->read_str()<<endl;
// 			}
		}
	}
	

	return returnValue;
}


bool TempInterlock::setValue( std::string command,double value )
{
	bool returnValue=false;

	command +=", " + SComHelper::val2string(value);
	auto answer =queryCommand(command);

	if(answer.find("error")>answer.size()){
		lastTime_ReadSettings=0;
		lastTime_TempReadout=0;
		returnValue=true;
	}

	return returnValue;
}

double TempInterlock::getLowerTempHybrid()
{
	
	return SComHelper::string_to_double(getSetting("htl,"));
}

double TempInterlock::getHigherTempHybrid()
{

	return SComHelper::string_to_double(getSetting("hth,"));
}

int TempInterlock::getNumberOfHybridNTCs()
{
	return SComHelper::string_to_int(getSetting("hne,"));
}

double TempInterlock::getLowerTempUserNTC()
{
	return SComHelper::string_to_double(getSetting("utl,"));
}

double TempInterlock::getHigherTempUserNTC()
{
	return SComHelper::string_to_double(getSetting("uth,"));
}

int TempInterlock::getNumberOfUserNTCs()
{
	return SComHelper::string_to_int(getSetting("une,"));
}

double TempInterlock::getLowerTempSHT7x()
{
	return SComHelper::string_to_double(getSetting("rtl,"));
}

double TempInterlock::getHigherTempSHT7x()
{
	return SComHelper::string_to_double(getSetting("rth,"));
}

double TempInterlock::getLowerHumiditySHT7x()
{
	return SComHelper::string_to_double(getSetting("rrl,"));
}

double TempInterlock::getHigherHumiditySHT7x()
{
	return SComHelper::string_to_double(getSetting("rrh,"));
}

int TempInterlock::getNumberOfSHT7xSensors()
{
	return SComHelper::string_to_int(getSetting("rne,"));
}


std::string TempInterlock::getSetting( std::string name )
{
	string returnValue="-1000";  // error Value
	auto settings=getInterlockReadSettings();
	auto cuttedString=SComHelper::CutOutSubstring(settings,name,"\r\n");
	if (cuttedString.size()==3)
	{
		returnValue=cuttedString.at(1);
	}

	return returnValue;
}

bool TempInterlock::setLowerTempHybrid( double temp )
{  
	if(temp>75 || temp<-25) return false; //catch invalid temperatures manually (same in all other functions)
	return setValue("htl",temp);
}

bool TempInterlock::setHigherTempHybrid( double temp )
{
	if(temp>75 || temp<-25) return false; 
	return setValue("hth",temp);
}

bool TempInterlock::setLowerTempUserNTC( double temp )
{
	if(temp>75 || temp<-25) return false; 
	return setValue("utl",temp);
}

bool TempInterlock::setHigherTempUserNTC( double temp )
{
	if(temp>75 || temp<-25) return false; 
	return setValue("uth",temp);
}

bool TempInterlock::setLowerTempSHT7x( double temp )
{
	if(temp>99 || temp<-40) return false;
	return setValue("rtl",temp);
}

bool TempInterlock::setHigherTempSHT7x( double temp )
{
	if(temp>99 || temp<-40) return false;
	return setValue("rth",temp);
}

bool TempInterlock::setLowerHumiditySHT7x( double humidity )
{
	if(humidity>100 || humidity<0) return false;
	return setValue("rrl",humidity);
}

bool TempInterlock::setHigherHumiditySHT7x( double humidity )
{
	if(humidity>100 || humidity<0) return false;
	return setValue("rrh",humidity);
}

bool TempInterlock::InterlockOutputEnable()
{
	return setValue("ena",100); //100 is required as parameter
}

bool TempInterlock::InterlockOutputDisable()
{
	return setValue("dis",100);
}

bool TempInterlock::InterlockOutputSetStatus( bool newstatus )
{
	if(newstatus==true)
	{
		return InterlockOutputEnable();
	} else {
		return InterlockOutputDisable();
	}
}
