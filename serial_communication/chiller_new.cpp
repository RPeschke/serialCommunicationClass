#include "chiller_new.h"
#include "SCom_helpers.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <TTimeStamp.h>
#include "SerialCom.h"
#ifdef WIN32
#include <Windows.h>
#else
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#define Sleep(a) gSystem->Sleep(a)

#ifndef nullptr
nullptr =NULL
#endif //nullptr

#endif
//#include <Windows.h>
using namespace std;

void *Worker_Chiller(void *ptr) {
	
	Chiller_Medingen_C20* Chiller=(Chiller_Medingen_C20*)ptr;

	TTimeStamp stamp, now;
	Double_t start_time=stamp.AsDouble();
	// 
	while(Chiller->started1){
		stamp.Set();
		Chiller->getActualTemp(); // these functions emits signals when they were called. 
		
		now.Set();
#ifdef _DEBUG
		std::cout<<(1-now.AsDouble()+stamp.AsDouble())*1000<<std::endl;
#endif
		Sleep(max(((Chiller->samplingTime)/1000-now.AsDouble()+stamp.AsDouble())*1000,1000)); // controles the sampling rate 

	}
#ifdef _DEBUG
	std::cout<<"end"<<std::endl;
#endif
	Chiller->stop1=1;
	return 0;
}

Chiller_Medingen_C20::Chiller_Medingen_C20( const char * port)
{startupCommand();
	SComunication=new SerialCom(port);
}

Chiller_Medingen_C20::Chiller_Medingen_C20( void )
{
startupCommand();
	SComunication=nullptr;
}

Chiller_Medingen_C20::Chiller_Medingen_C20( int port )
{
	startupCommand();
	SComunication=new SerialCom(port);
}

void Chiller_Medingen_C20::startupCommand()
{
	stop1=1;
	started1=0;
	samplingTime=1000;
}

double Chiller_Medingen_C20::getStatus( void )
{
	string answers = getCommand("STATUS"); // ask the question

	return SComHelper::string_to_double(answers);

}

double Chiller_Medingen_C20::getActualTemp( void )
{
#ifdef _DEBUG
	std::cout<<"test"<<std::endl;
#endif

	std::string answers =getCommand("IN PV 1"); // ask the question
	double temp=SComHelper::string_to_double(answers);
	actualTemp(temp);
	return temp;

}

double Chiller_Medingen_C20::getTargetTemp( void )
{
	string answers =getCommand("IN SP 1"); // ask the question

	return SComHelper::string_to_double(answers);
}

int DLL_exp Chiller_Medingen_C20::setTargetTemp( int temp )
{
	{

		

		string command="OUT SP 1 " + SComHelper::val2string(temp);
		return setCommand(command); // ask the question
		
	}
}

Chiller_Medingen_C20::~Chiller_Medingen_C20( void )
{
SComunication->disconnect();
delete SComunication;

}

void  Chiller_Medingen_C20::disconnect( void )
{
SComunication->disconnect();
SComunication=nullptr;
}

string Chiller_Medingen_C20::getCommand( std::string input)
{
	

	string device_answer=SComunication->query(input);
	int tries=0;
	size_t foundError=device_answer.find("error"); // since query returns "error" if something went wrong one has to check for it
	while(device_answer.size()<1&&foundError!=string::npos&&tries<10) // it occurs that the device doesn't respond correctly even if everything is correct, therefore i want to redo the query 10 times or untile it has worked. 
	{
		device_answer=SComunication->query(input);
		foundError=device_answer.find("error"); // since query returns "error" if something went wrong one has to check for it
		++tries;
	}
	return device_answer;
}

int Chiller_Medingen_C20::setCommand( std::string input)
{
	return SComunication->send(input);
}

void Chiller_Medingen_C20::open( const char *port )
{
	if (SComunication!=nullptr)
	{
	
		
		delete SComunication;
	}

	SComunication=new SerialCom(port);
}

void Chiller_Medingen_C20::WhatClassAmI( void )
{
	cout<<" -----------------------------------------------------------                 "<<endl; 
	cout<<" |  This is a Controller class for a Medingen C20 Chiller  |                  "<<endl;
	cout<<" -----------------------------------------------------------                  "<<endl;
	cout<<" list of Commands:                                                        "<<endl;
	cout<<" - Chiller_Medinger_C20(void)                                             "<<endl;
	cout<<" - Chiller_Medinger_C20(const char* Port                                  "<<endl;
	cout<<" - connect(const char * port)                                             "<<endl;
	cout<<" - disconnect(void)                                                       "<<endl;
	cout<<" - getStatus(void)                                                        "<<endl;
	cout<<" - getActualTemp(void)                                                    "<<endl;
	cout<<" - getTargetTemp(void)                                                    "<<endl;
	cout<<" - setTargeTemp(int Temp)                                                 "<<endl;
	cout<<"                                                                          "<<endl;
	// put any informations you want to add right here!

}

int Chiller_Medingen_C20::startThread( void )
{
	if(started1==0)
	{
		started1=1;
		stop1=0;
		t1 = new TThread("h1", Worker_Chiller, (void*) this); // this creates the threat which does all the interaction with the tti

		t1->Run();
		return 1;
	}

	return -1;


}

int  Chiller_Medingen_C20::stopThread( void )
{
	if (started1>0){

		started1=0;
			t1->Join();
		return 1;
	}
	return -1;
}

void Chiller_Medingen_C20::show_errors( void )
{
SComunication->show_errors();
}

std::string Chiller_Medingen_C20::getPort()
{
	return SComunication->getPort();
}
