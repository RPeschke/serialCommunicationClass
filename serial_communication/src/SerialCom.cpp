#include "SerialCom.h"

#include <iostream>
#include <sstream>
#include <iosfwd>
#include "SCom_helpers.h"
#include <TTimeStamp.h>
#ifdef WIN32
#include <windows.h>
#include <winbase.h>
#else
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#endif // WIN32



#define NOPARITY            0
#define ODDPARITY           1
#define EVENPARITY          2
#define MARKPARITY          3
#define SPACEPARITY         4


#define ONESTOPBIT          0
#define ONE5STOPBITS        1
#define TWOSTOPBITS         2
#define Sleep(a) gSystem->Sleep(a)




using namespace std;







SerialCom::SerialCom( void ):con_stat(-5),_baudRate(-5),_ByteSize(-5),_Parity(-5),_StopBits(-5),hSerial(nullptr)// i want to make sure that the values are set but they should be set to the actual value by the setter functions
{
	SerialMutex=new TMutex();
	SerialMutex->UnLock();
	set_baudRate(9600);
	set_ByteSize(8);
	set_Parity(NOPARITY);
	set_StopBits(ONESTOPBIT);
	terminateCommand = 13;
	terminateCommand+=10;
	timeBetweenSendAndReciev=0;
	TimeOut_=0;
	ttyACM_mode=false;
	SizeOfReadString=100;
	if (checkForUnsetValues()>=0)
	{
		con_stat = -1;
	}
}

SerialCom::SerialCom( const char* port,int baudRate,int ByteSize,int StopBits,int Parity ):con_stat(-5),_baudRate(-5),_ByteSize(-5),_Parity(-5),_StopBits(-5),hSerial(nullptr)
{	SerialMutex=new TMutex();
	SerialMutex->UnLock();
	
	terminateCommand = 13;
	terminateCommand+=10;

	timeBetweenSendAndReciev=0;
	TimeOut_=0;	
	ttyACM_mode=false;
	SizeOfReadString=100;
	con_stat=connect(port,baudRate,ByteSize,StopBits,Parity); 

}

SerialCom::SerialCom( int port,int baudRate/*=9600*/,int ByteSize/*=8*/,int StopBits/*=0*/,int Parity/*=0*/ ):con_stat(-5),_baudRate(-5),_ByteSize(-5),_Parity(-5),_StopBits(-5),hSerial(nullptr)
{
	SerialMutex=new TMutex();
	SerialMutex->UnLock();

	terminateCommand = 13;
	terminateCommand+=10;

	timeBetweenSendAndReciev=0;
	SizeOfReadString=100;

set_baudRate(baudRate);
set_ByteSize(ByteSize);
set_StopBits(StopBits);
set_Parity(Parity);
set_Port(port);
con_stat=connect();
}


SerialCom::~SerialCom(void)
{
	SerialMutex->UnLock();
	disconnect();
#ifdef _DEBUG

	cout<< "destructor of SerialCom"<<endl;
#endif // _DEBUG
	
	delete SerialMutex;


}

int SerialCom::connect( void )
{
#ifdef WIN32

	hSerial = CreateFileA(_port.c_str(),
		GENERIC_READ | GENERIC_WRITE,
		0,
		0,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		0);// opening the com port 
	con_stat=0;
	if(hSerial==INVALID_HANDLE_VALUE){
		if(GetLastError()==ERROR_FILE_NOT_FOUND){


				addError("error in SerialCom::connect( void ) //serial port does not exist.");
				addError("GetLastError()==ERROR_FILE_NOT_FOUND is true");
				addError("----------");
			con_stat-=10;
			//serial port does not exist. Inform user.
		}

			addError("error in SerialCom::connect( void )");
			addError("hSerial==INVALID_HANDLE_VALUE is true");
			addError("----------");
		//some other error occurred. Inform user.
	}
	else
	{	
		// Setting Parameters
		DCB dcbSerialParams = {0};
		dcbSerialParams.DCBlength=sizeof(dcbSerialParams);
		if (!GetCommState(hSerial, &dcbSerialParams)) {

			addError("error in SerialCom::connect( void )");
			addError("!GetCommState(hSerial, &dcbSerialParams) is true");
			addError("----------");
			con_stat-=100;
		}
		else {
			dcbSerialParams.BaudRate=_baudRate; 
			dcbSerialParams.ByteSize=_ByteSize;
			dcbSerialParams.StopBits=_StopBits;
			dcbSerialParams.Parity=_Parity;
			if(!SetCommState(hSerial, &dcbSerialParams)){
				//error setting serial port state

					addError("error in SerialCom::connect( void )");
					addError("!SetCommState(hSerial, &dcbSerialParams) is true");
					addError("----------");
				con_stat-=1000;
			}else {
				// setting timeout condition
				COMMTIMEOUTS timeouts={0};
				timeouts.ReadIntervalTimeout=50;
				timeouts.ReadTotalTimeoutConstant=50;
				timeouts.ReadTotalTimeoutMultiplier=10;
				timeouts.WriteTotalTimeoutConstant=50;
				timeouts.WriteTotalTimeoutMultiplier=10;
				if(!SetCommTimeouts(hSerial, &timeouts)){
					//error occurred. Inform user

						addError("error in SerialCom::connect( void )");
						addError("!SetCommTimeouts(hSerial, &timeouts) is true");
						addError("----------");
					con_stat-=10000;
				}	
			}}}

	#else  // linux

// handling ttyACM mode
	if (_port.find("ttyACM")!=string::npos)
	{
		ttyACM_mode=true;
		std::cout <<"TTYACM mode enabled" << std::endl; 
	}

	hSerial = open(_port.c_str(),
		(O_RDWR | O_NOCTTY) & ~O_NONBLOCK); //opening the port
	con_stat=0;
	if (hSerial == -1)
	{
		//cout<<"error in tti::tti(const char* port) while opening port"<<endl;
		addError("error in SerialCom::connect( void ) while opening port");
		addError("----------");
		con_stat=-1;
	}
	else
	{
		//setting parameters
		struct termios options; //create the struct
		tcgetattr(hSerial,&options); //get the current settings of the serial port
		cfsetispeed(&options,B9600); //set read and write speed to 19200 BAUD
		cfsetospeed(&options,B9600);
		options.c_cflag &= ~PARENB; //set no parity
		options.c_cflag &= ~CSTOPB; //set one stop bit
		options.c_cflag &= ~CSIZE; //clear current data size setting
		options.c_cflag |= CS8; //set 8 bit per work
		options.c_cc[VMIN] = 2; //minimum amount of characters to read
		options.c_cc[VTIME] = 10; //amount of time to wait for amount of data specified in VMIN in tenths of a second
		options.c_cflag |= (CLOCAL | CREAD); //don't allow changing of port control + enable the receiver
		if (tcsetattr(hSerial,TCSANOW,&options)!=0)
		{
			
			addError("error in SerialCom::connect( void ) while setting options");
			addError("----------");
			con_stat-=10;
		}
		//cout<<"TTi Opened Successfully. ID is: "<<getID()<<endl;
	}


	#endif



	return con_stat;
}


int SerialCom::connect( const char* port )
{
	if (!set_Port(port))
	{
	
	return connect();
	}

	addError("error in int SerialCom::connect( const char* port,int baudRate ). !set_baudRate(baudRate) is false");
	addError("----------");
	return -1;
}
int SerialCom::connect( const char* port,int baudRate )
{
	if (!set_baudRate(baudRate))
	{
		return connect(port);
	} 


	addError("error in int SerialCom::connect( const char* port,int baudRate ). !set_baudRate(baudRate) is false");
	addError("----------");
	return -2;
}

int SerialCom::connect( const char* port,int baudRate,int ByteSize )
{
	if (!set_ByteSize(ByteSize))
	{
		return connect(port,baudRate);
	} 



	addError("error in int SerialCom::connect( const char* port,int baudRate,int ByteSize ). !set_ByteSize(ByteSize) is false");
	addError("----------");
	return -3;
}

int SerialCom::connect( const char* port,int baudRate,int ByteSize,int StopBits )
{
	if (!set_StopBits(StopBits))
	{
		return connect(port,baudRate,ByteSize);
	} 


	addError("error in int SerialCom::connect( const char* port,int baudRate,int ByteSize,int StopBits ). !set_StopBits(StopBits) is false");
	addError("----------");
	return -4;
}

int SerialCom::connect( const char* port,int baudRate,int ByteSize,int StopBits,int Parity )
{	
	if (!set_Parity(Parity))
	{
		return connect(port,baudRate,ByteSize,StopBits);
	} 


	addError("error in int SerialCom::connect( const char* port,int baudRate,int ByteSize,int StopBits,int Parity ). !set_Parity(Parity) is false");
	addError("----------");
	return -5;
}

int SerialCom::disconnect( void )
{
	SerialMutex->UnLock();
	if (con_stat>=0)
	{
#ifdef WIN32

	if(CloseHandle(hSerial)==0){
		addError("error in SerialCom::disconnect( void )");
		addError("----------");
	}
	
	con_stat=-1;
#else

	if(close(hSerial)==-1)

		addError("error in SerialCom::disconnect( void )");
		addError("----------");
#endif
	}
	return 0;
}

int SerialCom::send( string input )
{

SComHelper::MutexLock m(SerialMutex);

return _send(input);

}

string SerialCom::query( string input )
{

	SComHelper::MutexLock m(SerialMutex);
	TTimeStamp stamp, now;
	//Double_t start_time=stamp.AsDouble();	
	_send(input);
	stamp.Set();
	Double_t start_time=stamp.AsDouble();	
	Sleep(timeBetweenSendAndReciev); // some device are very slow therefore one needs to wait a certain time to look for the response.
	string s2;
	
	do 
	{
		
		s2=_read();
		now.Set();
	} while (
		isErrorValueBufferIsEmpty(s2)
		&&
		(  (now.AsDouble()-start_time)   <   (TimeOut_/1000)  )
		);
	

	
	return s2;

}

string SerialCom::read_str( void )
{

SComHelper::MutexLock m(SerialMutex);

return _read();

}

int SerialCom::set_StopBits( int stopBits )
{
	if (stopBits == ONESTOPBIT 
		||
		stopBits ==ONE5STOPBITS
		|| 
		stopBits ==TWOSTOPBITS
		)
	{
		_StopBits=stopBits;
		return 0;
	}
	addError("error in SerialCom::set_StopBits( int stopBits )");
	addError("wrong stopBits");
	addError( SComHelper::val2string(stopBits));
	addError("----------");
	_StopBits=-1;
	return -1;
}

int SerialCom::set_Parity( int parity)
{
	if (parity==NOPARITY
		||
		parity==ODDPARITY
		||
		parity==EVENPARITY
		||
		parity==MARKPARITY
		||
		parity==SPACEPARITY)
	{
		_Parity=parity;
		return 0;
	}
	addError("error in SerialCom::set_Parity( int parity)");
	addError("wrong Parity");
	addError( SComHelper::val2string(parity));
	addError("----------");
	_Parity=-1;
	return -1;
}

int SerialCom::set_ByteSize( int byteSize )
{
	if(byteSize==5
		||
		byteSize ==6
		||
		byteSize==7
		||
		byteSize==8
		||
		byteSize ==16){
			_ByteSize=byteSize;
			return 0;
	}
	addError("error in SerialCom::set_ByteSize( int byteSize )");
	addError("wrong byteSize:");
	addError( SComHelper::val2string(byteSize));
	addError("----------");
	_ByteSize=-1;
	return -1;
}

int SerialCom::set_baudRate( int baudRate )
{
	if (baudRate==2400
		||
		baudRate==4800
		||
		baudRate==9600
		||
		baudRate==19200
		||
		baudRate==38400
		||
		baudRate==57600
		||
		baudRate==115200
		||
		baudRate==31250)
	{
		_baudRate=baudRate;
		return 0;
	}
	addError("error in SerialCom::set_baudRate( int baudRate )");
	addError("wrong baudRate:");
	addError( SComHelper::val2string(baudRate));
	addError("----------");
	_baudRate=-1;
	return -1;
}

int SerialCom::checkForUnsetValues( void )
{
	int return_value=0;
	if (_baudRate<0)
	{
		return_value -=1;
	}

	if (_ByteSize<0)
	{
		return_value-=10;
	}
	if (_Parity<0)
	{
		return_value-=100;
	}
	if (_StopBits<0)
	{
		return_value-=1000;
	}
	return return_value;
}

void DLL_exp SerialCom::show_errors( void )
{

	cout<<"List of errors in SerrialCom"<<endl;
	cout<<"----------------------------"<<endl;
	for(unsigned int i=0; i<listOfErrors.size();++i){

		string currentError=listOfErrors.at(i);
		cout<<currentError<<endl;
	}
}

void SerialCom::addError( string new_error )
{
#ifdef _DEBUG
	cout<<new_error<<endl;
#endif

	listOfErrors.push_back(new_error);
}

void SerialCom::query_cout( std::string command )
{

		string answer=query(command); // if one does not uses a step in between in will crash using CINT

		cout<<answer<<endl;


}

void  SerialCom::query_TString(const TString& command,TString &returnString)
{
  std::string dummy=command.Data();
	returnString = SComHelper::stdString2TString(query(dummy));
}


void SerialCom::read_TString( TString& returnValue )
{

	returnValue =SComHelper::stdString2TString(read_str());
}

void SerialCom::setTerminateCommand( std::string term )
{
	terminateCommand=term;
}


int SerialCom::_send( std::string input)
{
	if (con_stat>=0)
	{

		

		input+=terminateCommand;
		// 	input+=13;
		// 	input+=10;

#ifdef WIN32
		//////////////////////////////////////////////////////////////////////////
		// Windows Part

		DWORD dwBytesRead = 0;
		if(!WriteFile(hSerial, input.c_str(), input.size(), &dwBytesRead, NULL)){
			//error occurred. Report to user.

			addError("error in SerialCom::_send( string input )");
			addError("----------");
			
			return -1;
		}
		// end Windows Part
		//////////////////////////////////////////////////////////////////////////

#else //linux
		//////////////////////////////////////////////////////////////////////////
		// start Linux 
		if(write(hSerial, input.c_str(), input.size())<1){

			addError("error in SerialCom::_send( string input )");
			addError("----------");
			
			return -1;
		}
		// end linux
		//////////////////////////////////////////////////////////////////////////
#endif

	
		return 0;
	}


	addError("error in SerialCom::_send( string input )");
	addError("not connected");
	addError("----------");
	return -1;
}

std::string SerialCom::_read( void )
{

	if (con_stat>=0)
	{

		const int n=(SizeOfReadString);
		//char szBuffer[n +1]={0};
		char *szBuffer=new char[n+1];		 // 
		memset(szBuffer,0,n+1);				 // this sets the string to 0. this is important to see the end of the recieved string.
#ifdef WIN32
		//////////////////////////////////////////////////////////////////////////
		// Windows Part

		DWORD dwByteRead=0;
		if(!ReadFile(hSerial,szBuffer,n,&dwByteRead,NULL)){

			addError("error in SerialCom::_read( void )");
			addError("----------");
			delete[] szBuffer;
			return SERIAL_COM_ERROR_NO_BYTES_READ;
		}
		// end Windows Part
		//////////////////////////////////////////////////////////////////////////

#else //Linux

		//////////////////////////////////////////////////////////////////////////
		// start Linux 
		int bytes=0;
		if (ttyACM_mode)
		{
			bytes = read(hSerial,szBuffer,n);  	
		}
		else
		{
		//$$ this should be improved. possible answerer why we need two read commands is maybe due to some 13 10 problems 
			bytes = read(hSerial,szBuffer,n);  
			bytes = read(hSerial,szBuffer,n);
		}
		if(bytes < 1){

			addError("error in SerialCom::_read( void )");
			addError("----------");
			delete[] szBuffer;
			return SERIAL_COM_ERROR_NO_BYTES_READ;
		}
		// end linux
		//////////////////////////////////////////////////////////////////////////
#endif


		string s2(szBuffer);
		delete[] szBuffer;

		if (s2.size()<1)
		{
			s2=SERIAL_COM_ERROR_BUFFER_IS_EMPTY;
			addError("error in std::string SerialCom::_read( void )");
			addError("Buffer is Empty");
			addError("----------");
		}
		return s2;
	}
	addError("error in SerialCom::_read( void )");
	addError("not connected");
	addError("----------");
	
	return SERIAL_COM_ERROR_NOT_CONNECTED;


}

int SerialCom::set_Port( int port ) /* no checking if the port exists */
{
	
_port=SComHelper::convertInt2PortString(port);
 std::size_t found =_port.find("error");
 if (found<_port.size())
 {
	 addError("error in int SerialCom::set_Port( int port )");
	 addError("wrong port");
	 addError(SComHelper::val2string(port));
	 addError(_port);
	 addError("----------");
 }
	return 0;
}

bool SerialCom::isErrorValue( std::string& returnString )
{ 

if (returnString.find(SERIAL_COM_ERROR)!=std::string::npos)
{
  return true;
}
return false;
}

bool SerialCom::isErrorValueNotConnected( std::string& returnString )
{
  if (returnString.compare(SERIAL_COM_ERROR_NOT_CONNECTED)==0)
  {
    return true;
  }
  return false;
}

bool SerialCom::isErrorValueBufferIsEmpty( std::string& returnString )
{
  if (returnString.compare(SERIAL_COM_ERROR_BUFFER_IS_EMPTY)==0)
  {
    return true;
  }
  return false;
}

bool SerialCom::isErrorValueNoBytesRead( std::string& returnString )
{
  if (returnString.compare(SERIAL_COM_ERROR_NO_BYTES_READ)==0)
  {
    return true;
  }
  return false;
}
