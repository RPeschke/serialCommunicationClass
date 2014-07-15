serialCommunicationClass
========================

a set of classes to communicate with devices like power suplies over Com port


#1 Syntax

##1.1 Constructor

SerialCom(void);
SerialCom(const char* port,int baudRate=9600,int ByteSize=8,int StopBits=0,int Parity=0);
One can create an instant of this class without setting any value, But in the most cases one wants to have a constructor which directly connects to the device. So it is recommended to use the second constructor. It has some reasonable default if you don’t know it exactly try to use them. "Port" must be a port on your computer
1.2 I/O Methods
int send(std::string command);

Send the command string to the device. Terminates each line with \r\n command. Returns 0 if ever is ok. Returns -1 if an error occurred.
-----------------------------------------------------------------------------------------------------------------------------------
std::string read(void);
Reads one line from the port buffer. Returns "error" if a general error occurred. Returns "error not connected" if the instant is not connected to a device. If one wants to use this Method in another class make sure you check the string for the "error" substring.
Example:
size_t foundError=device_answer.find("error");
if(foundError!=string::npos){
// an error occured
}
-----------------------------------------------------------------------------------------------------------------------------------
std::string query(std::string command);
Send the command string to the device and reads the answer. It can have the same outputs as the read function has, so make sure you check for an "error" substring. It is also possible that the device simply does not respond correctly in this case it is very likely that the string is just empty. For that reason it is recommended that one checks that the string is not empty. If the string is empty just try the command again.
1.3 Other methods
void show_errors(void);
This is a record of all the errors that occurred (and are known). Use it whenever you don't get a connection.
int disconnect(void);
Disconnect from the current port. If one wants to change any of the values one has to disconnect and reconnect the object.
int connect(void); // connects if all variables are set
int connect(const char* port);
int connect(const char* port,int baudRate);
int connect(const char* port,int baudRate,int ByteSize);
int connect(const char* port,int baudRate,int ByteSize,int StopBits);
int connect(const char* port,int baudRate,int ByteSize,int StopBits,int Parity);
If one has disconnected the object one can reconnect it to a different port with different properties
int set_baudRate(int baudRate);
int set_ByteSize(int byteSize);
int set_StopBits(int stopBits);
int set_Parity(int);
Setter functions to safely change the value. One can only set the values to supported numbers. If the value is not accepted it will return -1 and the value remains unchanged.
2 Description
This Is a basic serial communication class. It is designed to work with your class in an ROOT envierment. For now you cannot use it directly with CINT so you have to write your own class surounding it. It contains the entirer platform dependencies.
3 Links
You can find further informations and the actual files on this page:
https://twiki.cern.ch/twiki/bin/view/Sandbox/TTiCPX400PowerSupply