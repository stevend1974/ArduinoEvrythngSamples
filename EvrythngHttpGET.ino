/*
  Sketch Demonstrating getting a thng object from the Evrythng platform
  
  Variables to modify
  const char *APIToken = " please set your evrythng api token value"
  const char *thngid=" please set an existing thng identifier value"
  
  Hardware needed
  - arduino + NIC interface
  - DHCP enabled on your network
  
  This sketch will 
  - setup and initialize the ethernet connection
  - connect to the evryhng api server
  - HTTP GET of the thng based on the provided id
      - return the HTTP response
      - if the HTTP response is not OK (200) -> end test
      - if the HTTP response is OK (200) -> get properties of thng
      - properties obtained will be written to serial
      
  output on the serial port will be a simile of

        connecting...
        connected... http response stream.... 
          checking out the response
          HTTP/1.1 result code is :<200>
        connected... http response stream.... 
          checking out properties
           ---> "activity0":"no activity","activity1":"development","activity2":"training","activity3":"consulting","currentactivity":"no activity"
         got what we needed -> sketch stops working
  
  
  Steven D. version 1.0
 */
#include <SPI.h>
#include <Ethernet.h>
  

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
char serverName[] = "api.evrythng.com";
const char *APIToken = "replace_by_your_evrythng_api_token";
const char *thngid="replace_by_your_evrythng_thng";

boolean endtest;

String propertiesstring;

EthernetClient client;

boolean gethttpresponsecode;

int result; // the result of the calculation

void setup()
{
  Serial.begin(9600);
  if(Ethernet.begin(mac) == 0) { // start ethernet using mac & IP address
    Serial.println("Failed to configure Ethernet using DHCP");  
    while(true)   // no point in carrying on, so stay in endless loop:
      ;
  }
  delay(1000); // give the Ethernet shield a second to initialize

  endtest = false;
  gethttpresponsecode=true;

  Serial.println("connecting...");
}

void loop()
{
  if (client.connect(serverName, 80)>0) {
    Serial.print("connected... ");
    client.print("GET /thngs/");
    client.print(thngid);
    client.print("?access_token=");
    client.print(APIToken);
    client.println(" HTTP/1.1");
    client.println("Host: evrythng.net");
    client.println("Connection: close");
    client.println();
  } else {
    Serial.println("connection failed");
  }
  if (client.connected()) {
    // check for http response code
    Serial.println("http response stream.... ");
    
    if (gethttpresponsecode){
    
    if(client.find("HTTP/1.1 ")){
        Serial.println("  checking out the response");
               result = client.parseInt();
               Serial.print("HTTP/1.1 result code is :<" );
               Serial.print(result);
               Serial.println(">"); 
               
               if (result!=200){
                 // http response code is NOT OK  (200) so we stop
                 endtest = true;
                 Serial.println (" http response is not ok...stopping test");
                 
                 }
               // we are not looking anymore for the httpresponse code
               gethttpresponsecode = false;
   }

  } else { // no need for httpresponse code just the properties
     if(client.find ("properties\":{")){
        Serial.println("  checking out properties");
       
        propertiesstring = client.readStringUntil('}}') ; 
        Serial.println (" ---> "+propertiesstring);
        propertiesstring="";
        endtest = true;
       }
     
  } // end not httpresponse code
       
    client.stop();
    client.flush();
    delay(10000); // check again in 10 seconds
  }
  else {
    Serial.println();
    Serial.println("not connected");
    client.stop();
    client.flush();
    delay(1000);
  }
  
  if (endtest){
      // found objective stop working
      Serial.println(" got what we needed -> sketch stops working");
      delay (500);
      while(true);
    }
}
