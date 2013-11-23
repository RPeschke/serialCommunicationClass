#include "Keithley2410.h"
#include "SerialCom.h"
#include "SCom_helpers.h"

using namespace std;


Keithley2410::Keithley2410( const char *Port )
{
	SComunication=new SerialCom(Port);
	SComunication->timeBetweenSendAndReciev=5000; // seconds in ms 
	SComunication->send(":SENS:FUNC:ON 'VOLT:DC'");
	SComunication->send(":SENS:FUNC:ON 'CURR:DC'");
}

double Keithley2410::getVoltage()
{
	auto answer=SComunication->query(":READ?");
	auto vec_answer=SComHelper::string2Vector( answer );
	
	if (vec_answer.size()>2)
	{
		return vec_answer.at(0);
	}
	return -100000;
}

double Keithley2410::getCurrent()
{
	auto answer=SComunication->query(":READ?");
	auto vec_answer=SComHelper::string2Vector( answer );
	if (vec_answer.size()>2)
	{
		return vec_answer.at(1);
	}
	return -100000;
}



bool Keithley2410::getStatus()
{
	return 1; //$$ dummy fucntion
}

int Keithley2410::setStatus( bool on_off )
{
if (on_off)
{
	return	enable();
}else{
	return disable();
}
}

int Keithley2410::enable()
{

	return SComunication->send(":OUTP:STAT 1");
}

int Keithley2410::disable()
{
	return SComunication->send(":OUTP:STAT 0");
}

int Keithley2410::setVoltage( double Voltage )
{
	
	SComunication->send(":SOUR:VOLT:MODE FIX");
	SComunication->send(":SOUR:VOLT:RANG "+ SComHelper::val2string(Voltage));
	string s=":SOUR:VOLT:LEV ";
	s+=SComHelper::val2string(Voltage);
	return SComunication->send(s);
}

int Keithley2410::setCurrent( double current )
{



	SComunication->send(":SOUR:CURR:MODE FIX");
	SComunication->send(":SOUR:CURR:RANG "+ SComHelper::val2string(current));
	string s=":SOUR:CURR:LEV ";
	s+=SComHelper::val2string(current);
	return SComunication->send(s);
}

Keithley2410::~Keithley2410( void )
{
	SComunication->disconnect();
	delete SComunication;
}

void Keithley2410::show_errors( void )
{
	
		SComunication->show_errors();
	
}

double Keithley2410::getCurrentRange()
{
	auto answer=SComunication->query(":CURR:RANG?");

	return SComHelper::string_to_double(answer);
}

int Keithley2410::setCurrentRange( double current )
{
	  
	  return SComunication->send(":CURR:RANG "+ SComHelper::val2string(current));
}

void Keithley2410::setAutoRange()
{

	SComunication->send(":SENS:CURR:RANG:AUTO 1");
	SComunication->send(":SENS:VOLT:RANG:AUTO 1");
}

int Keithley2410::setOverVoltageProtection( double Voltage )
{
	return SComunication->send(":VOLT:PROT:LEV "+SComHelper::val2string(Voltage));

}

int Keithley2410::setOverCurrentProtection( double Current )
{
	return SComunication->send(":CURR:PROT:LEV "+SComHelper::val2string(Current));
}

int Keithley2410::setVoltageSource()
{
	return SComunication->send(":SOUR:FUNC VOLT");
}

int Keithley2410::setCurrentSource()
{
	return SComunication->send(":SOUR:FUNC CURR");
}
