#include "Keithlay2410.h"
#include "SerialCom.h"
#include "SCom_helpers.h"

using namespace std;


Keithlay2410::Keithlay2410( const char *Port )
{
	SComunication=new SerialCom(Port);
	SComunication->timeBetweenSendAndReciev=5000; // seconds in ms 

}

double Keithlay2410::getVoltage()
{
	auto answer=SComunication->query(":SENS:FUNC:ON 'VOLT:DC'");

	return SComHelper::string_to_double(answer);
}

double Keithlay2410::getCurrent()
{
	auto answer=SComunication->query(":SENS:FUNC:ON 'CURR:DC'");

	return SComHelper::string_to_double(answer);
}



bool Keithlay2410::getStatus()
{
	return 1; //$$ dummy fucntion
}

int Keithlay2410::setStatus( bool on_off )
{
if (on_off)
{
	return	enable();
}else{
	return disable();
}
}

int Keithlay2410::enable()
{

	return SComunication->send(":OUTP:STAT 1");
}

int Keithlay2410::disable()
{
	return SComunication->send(":OUTP:STAT 0");
}

int Keithlay2410::setVoltage( double Voltage )
{
	
	SComunication->send(":SOUR:VOLT:MODE FIX");
	SComunication->send(":SOUR:VOLT:RANG "+ SComHelper::val2string(Voltage));
	string s=":SOUR:VOLT:LEV ";
	s+=SComHelper::val2string(Voltage);
	return SComunication->send(s);
}

int Keithlay2410::setCurrent( double current )
{



	SComunication->send(":SOUR:CURR:MODE FIX");
	SComunication->send(":SOUR:CURR:RANG "+ SComHelper::val2string(current));
	string s=":SOUR:CURR:LEV ";
	s+=SComHelper::val2string(current);
	return SComunication->send(s);
}

Keithlay2410::~Keithlay2410( void )
{
	SComunication->disconnect();
	delete SComunication;
}

void Keithlay2410::show_errors( void )
{
	
		SComunication->show_errors();
	
}
