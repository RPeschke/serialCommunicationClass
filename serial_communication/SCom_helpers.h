#ifndef SCOMHELPER
#define SCOMHELPER




#include <sstream>
#include <string>
#include <TString.h>
#include <vector>

 class TMutex;

namespace SComHelper
{
double string_to_double( const std::string& s ); //if possible, use C++11 function std::stod instead

int string_to_int( const std::string& s ); //if possible, use C++11 function std::stoi instead

bool isNumeric(std::string stringToCheck);

template<typename t>
std::string val2string(t Value) 
{
	 		
	 		std::string outputString;
	 		std::stringstream change;
			change<<Value;
			change>>outputString;
			
			return outputString;

}

TString stdString2TString(std::string input);

std::vector<double> string2Vector(std::string s);


std::vector<std::string> CutOutSubstring(std::string s,const std::string& startString,const std::string& endString);


class MutexLock
{
public:
	MutexLock(TMutex *m);
	~MutexLock();
private:
	TMutex *_mutex;

};
}



#endif