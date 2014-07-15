#include "SCom_helpers.h"
#ifndef min
	#define min(a,b) (((a) < (b)) ? (a) : (b))
#endif
#include <TMutex.h>
#include <iostream>


double SComHelper::string_to_double( const std::string& s ) //if possible, use C++11 function std::stod instead
{
	std::istringstream i(s);
	double x;
	if (!(i >> x))
		return -10000;
	return x;
}


int SComHelper::string_to_int( const std::string& s ) //if possible, use C++11 function std::stoi instead
{
	std::istringstream i(s);
	int x;
	if (!(i >> x))
		return -10000;
	return x;
}


bool SComHelper::isNumeric(std::string stringToCheck) 
{
	// this is a very poor isNumeric method it simple ask what is the first element what is not a number or a separation symbol. in a later version this needs to be replaced by a more dedicated function.
	// because I am sure that i have not include all possibilities of how a number could look like. for example I guess if the string would start with a "TAB" it would say: Not A Number (NAN).
	bool numeric = false;

	if(stringToCheck.find_first_not_of("+-0123456789. \n\r") == std::string::npos)
		numeric = true;

	return numeric;
}

TString SComHelper::stdString2TString(std::string input){

	const char* dummy=input.c_str();
	TString answer(dummy);
	return answer;

}

std::vector<double> SComHelper::string2Vector( std::string s )
{
std::vector<double> returnValue;


auto start_index=s.find_first_of("-+.0123456789");


while(start_index<s.size())
{
	s=s.substr(start_index);
	auto end_index=s.find_first_not_of("-+.0123456789E");
	auto number_index=s.find_first_of("0123456789E");
	if (number_index<=end_index)
	{
		returnValue.push_back(atof(s.substr(0,end_index).c_str()));
	}
	
	s=s.substr(end_index+1);
	start_index=s.find_first_of("-+.0123456789E");
}

if (returnValue.size()==0)
{
	returnValue.push_back(-10000);
}




return returnValue;
}

std::vector<std::string> SComHelper::CutOutSubstring( std::string inputString,const std::string& startString,const std::string& endString )
{
	std::vector<std::string> returnValue;


	auto start_index=inputString.find(startString);
	if (start_index<=inputString.size())
	{
		returnValue.push_back(inputString.substr(0,start_index+startString.size()));
		inputString=inputString.substr(start_index+startString.size());
		auto end_index=inputString.find(endString);

		if (end_index<inputString.size())
		{

			returnValue.push_back(inputString.substr(0,end_index));
			returnValue.push_back(inputString.substr(min(end_index,inputString.size()))); // one has to make sure that the index is not larger than the string.
		} 
		else
		{
			inputString="";
		}

	}else{ // Start string not found returning the input string as first element of the return Vector
		returnValue.push_back(inputString);
	}

	
	
	
	return returnValue;
}

std::string SComHelper::convertInt2PortString( int port )
{
	std::string ret;
#ifdef WIN32

	if (port<9&&port>=0)
	{
		ret="com"+SComHelper::val2string(port);
	}else if(port>9){
		ret="\\\\.\\COM"+SComHelper::val2string(port);
	}else{
		ret="error";
	}

#else

	ret="/dev/ttyS"+SComHelper::val2string(port);
#endif

	return ret;
}

SComHelper::MutexLock::MutexLock( TMutex *m )
{
	_mutex=m;
	_mutex->Lock();

}

SComHelper::MutexLock::~MutexLock()
{
	_mutex->UnLock();

}
