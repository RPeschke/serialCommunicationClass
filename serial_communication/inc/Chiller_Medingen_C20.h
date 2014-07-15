#ifndef Chiller_class
#define Chiller_class

 
// signal "actualTemp(double)"
#include <string>
#include <Rtypes.h>
#include <RQ_OBJECT.h>
#include <TThread.h>


#ifdef WIN32 
	#ifndef __CINT__
		#define DLL_exp _declspec(dllexport) 
		
	#else 
		#define DLL_exp 
	#endif // __CINT__


#else  // Linux
	#define DLL_exp

#endif 

class SerialCom;



class DLL_exp Chiller_Medingen_C20
{
	RQ_OBJECT("Chiller_Medingen_C20")
public:

	static void  WhatClassAmI(void);

	 Chiller_Medingen_C20(void);
	 Chiller_Medingen_C20(const char *port);
	 Chiller_Medingen_C20(int port);
	virtual  ~Chiller_Medingen_C20(void);


	//Chiller Try Begin
	// get ...
	std::string getPort();
	double getStatus( void );
	double getActualTemp( void );
	double getTargetTemp( void );
	//set
	int setTargetTemp( int temp );
	//Chiller Try End

	void disconnect(void);
	void open(const char *port);

#ifndef __CINT__ //is hidden so that one cannot access it from cint
	bool started1;
	bool stop1;
#endif
	
	int startThread(void);
	int stopThread(void);
	int samplingTime; // in Milliseconds
	void show_errors(void); // displays the error list in the command line
private:
	TThread *t1;
	std::string getCommand(std::string);
	int setCommand(std::string);
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Start Signals
	void actualTemp(double temp){Emit("actualTemp(double)",temp);}//*SIGNAL*
	// Stop Signals
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#ifndef __CINT__
	SerialCom *SComunication;
	void startupCommand();
#endif
	
ClassDef(Chiller_Medingen_C20, 1);//needed for the ROOT dict
};

#ifdef __CINT__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

#pragma link C++ class Chiller_Medingen_C20;

#endif

#endif //tti_class
