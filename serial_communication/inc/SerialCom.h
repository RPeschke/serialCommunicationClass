#ifndef SERIALCOM
#define SERIALCOM


#define SERIAL_COM_ERROR "!!!ERROR!!!" // to avoid collisions with possible return values of new devices the is a bit cryptic.  
#define SERIAL_COM_ERROR_NOT_CONNECTED "!!!ERROR!!!: Not Connected "
#define SERIAL_COM_ERROR_BUFFER_IS_EMPTY "!!!ERROR!!!: Buffer is Empty"
#define SERIAL_COM_ERROR_NO_BYTES_READ "!!!ERROR!!!: no bytes read"

#include <string>
#include <TThread.h>
#include <Rtypes.h>
#include "TString.h"


#ifndef __CINT__  // since ROOTCINT don't know the command _declspec(dllexport) one has to hide it from ROOTCINT. on the other hand the command is needed to show VS with element to export in THE DLL and that it should create a LIB file where 
#include <vector>
#include <string>


#ifdef WIN32
#define DLL_exp _declspec(dllexport)
#else
#define DLL_exp
#endif // WIN32
#else
#define DLL_exp

#endif // __CINT__

class DLL_exp SerialCom
{
	
public:
	 SerialCom(void);
	 SerialCom(const char* port,int baudRate=9600,int ByteSize=8,int StopBits=0,int Parity=0); 
	 SerialCom(int port,int baudRate=9600,int ByteSize=8,int StopBits=0,int Parity=0); 
	virtual ~SerialCom(void);

	//Control Functions
	int  connect(void); // connects if all variables are set
	int  connect(const char* port);
	int  connect(const char* port,int baudRate);
	int  connect(const char* port,int baudRate,int ByteSize);
	int  connect(const char* port,int baudRate,int ByteSize,int StopBits);
	int  connect(const char* port,int baudRate,int ByteSize,int StopBits,int Parity); 

	int  disconnect(void);

	int  send(std::string  command);
	std::string  query(std::string  command);
	void  query_cout(std::string  command);
	TString  query_TString(std::string  command);
	
	std::string  read_str(void);
	TString  read_TString(void);
	// Setter function 
	// they will return 0 if ok else -1
	int  set_Port(const char* port) // no checking if the port exists
	{
		_port=port;
		return 0;
	}
	int  set_Port(int port); // no checking if the port exists;
	int  set_baudRate(int baudRate); // checks if the baudRate value is possible. not jet implemented for linux

	int  set_ByteSize(int byteSize); // checks if the byteSize value is possible. not jet implemented for linux
	int  set_StopBits(int stopBits); // checks if the stopBits value is possible. not jet implemented for linux

	int  set_Parity(int); // checks if the parity value is possible. not jet implemented for linux

	void  show_errors(void); // displays the error list in the command line
	void   addError(std::string new_error);// adds new errors to the error list if _DEBUG is defined it will print the error
		int con_stat; // indicants whether the device is connected or not 

		void setTerminateCommand(std::string term);
		unsigned long timeBetweenSendAndReciev;
		unsigned long TimeOut_;
		int SizeOfReadString;
		std::string getPort(){return _port;}

    static bool isErrorValue(std::string& returnString);
    static bool isErrorValueNotConnected(std::string& returnString);
    static bool isErrorValueBufferIsEmpty(std::string& returnString);
    static bool isErrorValueNoBytesRead(std::string& returnString);
private:
	

#ifndef __CINT__
	std::vector<std::string> listOfErrors;
#ifdef WIN32
	void *hSerial; // this is the handle for the Serial port
#else
	int hSerial;
#endif
#endif

	int checkForUnsetValues(void); //checks if all values are set. if the value is 0 then its ok if negative then not. 
	int _send(std::string command); // this is the intern send function it is not multi thread save.
	std::string  _read(void);   // this is the intern read function it is not multi thread save.

	
	// properties of the serial interface
	std::string _port;
	std::string terminateCommand;
	int _baudRate;
	int _ByteSize;
	int _StopBits;
	int _Parity;
	// for handling modern USB ACM devices
	bool ttyACM_mode;
	
	//Thread lock
	 TMutex *SerialMutex;
	 

//	TTimeStamp mutexTime;
	ClassDef(SerialCom, 1);//needed for the ROOT dict

};



#ifdef __CINT__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

#pragma link C++ class SerialCom;

#endif

#endif
