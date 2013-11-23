#include "tti_RQSignals.h"
#include <iostream>
using namespace std;

tti_RQSignals::tti_RQSignals(void)
{

}


tti_RQSignals::~tti_RQSignals(void)
{
}



void  tti_RQSignals::RQConnect( sig f )
{
	cout << f <<endl;

}

char * tti_RQSignals::SignalName( sig f )
{
	switch (f)
	{
	case SActualVoltageCh1:
		return "ActualVoltageCh1(double)";
	case SActualVoltageCh2:
		return "ActualVoltageCh2(double)";

	case SActualCurrentCh1:
		return "ActualCurrentCh1(double)";
	case SActualCurrentCh2:
		return "ActualCurrentCh2(double)";
	}
	return "error";
}


