// testDLL.cpp : Definiert den Einstiegspunkt für die Konsolenanwendung.
//
#include <iostream>
#include <string>
#include "stdafx.h"
#include "../serial_communication/SerialCom.h"
#include "../serial_communication/tti_new.h"
#include "../serial_communication/SCom_helpers.h"
#include "../serial_communication/chiller_new.h"
#include <Windows.h>
//#include "../serial_communication/tti_RQSignals.h"
//#include <winnt.rh>
#include <TApplication.h>
#include <TF1.h>
#include "../serial_communication/TempInterlock.h"
#include "../serial_communication/CommonDeviceController.h"
#include "../serial_communication/Keithley2410.h"
#include <TTimeStamp.h>
#include <fstream>

using namespace std;
Chiller_Medingen_C20 *chill;
tti_CPX400 *tti;
TempInterlock *interlock;
void logTemp(string filename,double targetTemp,bool LVDSon,bool ModuleOn);
int main(int argc, char **argv) 
{


Keithley2410 s1("com9");

s1.setVoltage(20);
	cout<<s1.getCurrent()<<endl;
s1.enable();
s1.setCurrent(0.00001);
s1.disable();
// 	SerialCom S1("COM9");
// 	S1.timeBetweenSendAndReciev=5000;
// 	string s;
// 
// 	while(true){
// 	getline(cin,s);
// 	S1.query_cout(s);
// 	}
 	return 1;

}


void logTemp(string filename,double targetTemp,bool LVDSon,bool ModuleOn){

	tti->setStatus(1,LVDSon);
	tti->setStatus(2,ModuleOn);
	chill->setTargetTemp(targetTemp);
	ofstream out(filename);
	out<<"Time,      Target_Temp,     Actual_Temp_Chiller,    Actual_Tempt_NTC_1,     Actual_Tempt_NTC_2, LVDS_Voltage,  LVDS_current,  Module_Voltage, Module_Current"<<endl;
	TTimeStamp startTime,now;
	startTime.Set();
	now.Set();
	double tempdiff=interlock->getTemp(1)-targetTemp;
	
	while((now.AsDouble()-startTime.AsDouble())<(3600) ){//4*3600

		now.Set();
		out<<(now.AsDouble()-startTime.AsDouble())<<";         "<< chill->getTargetTemp()<<";                "<<chill->getActualTemp()<<";                   "<< interlock->getTemp(0)<<";                       "<< interlock->getTemp(1)<<";           "<<tti->getActualVoltage(1)<<";        "<<tti->getActualCurrent(1)<<";          "<<tti->getActualVoltage(2)<<";     "<<tti->getActualCurrent(2)<<endl;
		Sleep(10000);

	}

	out.close();
}
