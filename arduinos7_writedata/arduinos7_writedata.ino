
// Wifi  -> #define S7WIFI
// Cable -> #define S7WIRED

#include <SPI.h>
#include <Ethernet.h>
#include "Settimino.h"

// Uncomment next line to perform small and fast data access
#define DO_IT_SMALL

// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte mac[] = { 
  0x28, 0x63, 0x36, 0x8E, 0x76, 0xE3 }; 

IPAddress Local(192,168,0,90); // Local Address
IPAddress PLC(192,168,0,124);   // PLC Address

// Following constants are needed if you are connecting via WIFI
// The ssid is the name of my WIFI network (the password obviously is wrong)
/*char ssid[] = "SKYNET-AIR";    // Your network SSID (name)
char pass[] = "yourpassword";  // Your network password (if any)*/
IPAddress Gateway(192, 168, 0, 1);
IPAddress Subnet(255, 255, 255, 0);

int DBNum = 1; // 
int Buffer[1024]; // 

S7Client Client(_S7WIRED);

const int  button1 = 22;
const int  button2 = 24;  

int button1State = 0;
int button2State = 0;

unsigned long Elapsed; // To calc the execution time
//----------------------------------------------------------------------
// Setup : Init Ethernet and Serial port
//----------------------------------------------------------------------
void setup() {
    // Open serial communications and wait for port to open:
    Serial.begin(9600);

    pinMode(button1, INPUT);
    pinMode(button2, INPUT);
    
     while (!Serial) {
      ; // wait for serial port to connect. Needed for Leonardo only
    }

    // Start the Ethernet Library
    Ethernet.begin(mac, Local);
    // Setup Time, someone said me to leave 2000 because some 
    // rubbish compatible boards are a bit deaf.
    delay(2000); 
    Serial.println("");
    Serial.println("Cable connected");  
    Serial.print("Local IP address : ");
    Serial.println(Ethernet.localIP());

     
}
//----------------------------------------------------------------------
// Connects to the PLC
//----------------------------------------------------------------------
bool Connect()
{
    int Result=Client.ConnectTo(PLC, 
                                  0,  // Rack (see the doc.)
                                  1); // Slot (see the doc.)
    Serial.print("Connecting to ");Serial.println(PLC);  
    if (Result==0) 
    {
      Serial.print("Connected ! PDU Length = ");Serial.println(Client.GetPDULength());
    }
    else
      Serial.println("Connection error");
    return Result==0;
}
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Prints the Error number
//----------------------------------------------------------------------
void CheckError(int ErrNo)
{
  Serial.print("Error No. 0x");
  Serial.println(ErrNo, HEX);

  // Checks if it's a Severe Error => we need to disconnect
  if (ErrNo & 0x00FF)
  {
    Serial.println("SEVERE ERROR, disconnecting.");
    Client.Disconnect(); 
  }
}
//----------------------------------------------------------------------
// Profiling routines
//----------------------------------------------------------------------
void MarkTime()
{
  Elapsed=millis();
}
//----------------------------------------------------------------------
void ShowTime()
{
  // Calcs the time
  Elapsed=millis()-Elapsed;
  Serial.print("Job time (ms) : ");
  Serial.println(Elapsed);   
}
//----------------------------------------------------------------------
// Main Loop
//----------------------------------------------------------------------
void loop() 
{
  int Size, Result;
  //void Target;

  
  button1State = digitalRead(button1);
  button2State = digitalRead(button2);

  // Connection
  while (!Client.Connected)
  {
    if (!Connect())
      delay(500);
  }
  
  MarkTime();

  if (button1State == 1 && button2State == 0)
  {   Buffer[0]=1;
      Client.WriteArea(S7AreaDB, // We are requesting DB access
                         DBNum,    // DB Number
                         0,        // Start from byte N.0
                         1,     // We need "Size" bytes
                         &Buffer);  // Put them into our target (Buffer or PDU)
  }
  else if(button1State == 0 && button2State ==1)
  {
       Buffer[0]=0;
       Client.WriteArea(S7AreaDB, // We are requesting DB access
                         DBNum,    // DB Number
                         0,        // Start from byte N.0
                         1,     // We need "Size" bytes
                         &Buffer);  // Put them into our target (Buffer or PDU)

  }
  
}
