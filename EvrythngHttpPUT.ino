/*
   Copyright 2014 Steven D.

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
   
 */


/*
  Sketch Demonstrating updating a thng property on the Evrythng platform
  
  Variables to modify
  const char *APIToken = " please set your evrythng api token value"
  const char *thngid=" please set an existing thng identifier value"
  char propertystr[] = "{\"key\": \"property1\",\"value\": \"updatedvalue1\"}";
        replace property1 with an existing property on the thng
        replace updatevalue1 with the value you want to update

  
  Hardware needed
  - arduino + NIC interface
  - DHCP enabled on your network
  
  This sketch will 
  - setup and initialize the ethernet connection
  - connect to the evrythng api server
  - HTTP PUT to update a property of the thng based on the provided thng id and values
      - return the HTTP response
      - if the HTTP response is not OK (200) -> end test
      - if the HTTP response is OK (200) -> get properties of thng
      
  output on the serial port will be a simile of
  
  connecting...
  connected... 
       HTTP PUT      
        Object posted with length 47 :      [{"key": "property1","value": "updatedvalue1"}]
       HTTP PUT completed 
    http response stream.... 
      checking out the response
      HTTP/1.1 result code is :<200>
   testing finished -> sketch stops working
  
 */



#include <SPI.h>
#include <Ethernet.h>
  

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
char serverName[] = "api.evrythng.com";
//const char *APIToken = "replace_by_your_evrythng_api_token";
//const char *thngid="replace_by_your_evrythng_thng_id";
const char *APIToken = "Oz28gK1LijEoxWJKblXoHWGGrnbMaPUpjqAMnKRx2GoSFikMwig1Zcrb94GlZhiRBTtzYSWIeOTFzovI";
const char *thngid="52e411f7e4b0a7fd3090b31a";


boolean endtest;

String propertiesstring;

EthernetClient client;

char propertystr[] = "[{\"key\": \"property1\",\"value\": \"updatedvalue1\"}]";


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

  Serial.println("connecting...");
}

void loop()
{
  if (client.connect(serverName, 80)>0) {
    Serial.println("connected... ");
    
              
    // Make a HTTP request PUT:                            
                            
    Serial.println ("     HTTP PUT ");
    Serial.println ("     ");
    Serial.print ("      Object posted with length "+String(strlen(propertystr))+" : ");
    Serial.print("     ");Serial.println (propertystr);
    Serial.println (" ");
              
    // Make a HTTP request POST:
    client.print("PUT https://api.evrythng.com/thngs/");
    client.print(thngid);client.println("/properties HTTP/1.1");
    client.println("Content-Type: application/json");
    client.println("Accept: application/json");
    client.print("Authorization: ");client.println(APIToken);
    client.println("Connection: close");
    client.print("Content-Length: ");
    client.println(String(strlen(propertystr)));
    client.println();
    client.println(propertystr);  
    
     Serial.println ("     HTTP PUT completed ");
     Serial.println ("");

  } else {
    Serial.println("connection failed");
  }
  if (client.connected()) {
    // check for http response code
    Serial.println("  http response stream.... ");
   
   
          if(client.find("HTTP/1.1 ")){
              Serial.println("    checking out the response");
                     result = client.parseInt();
                     Serial.print("    HTTP/1.1 result code is :<" );
                     Serial.print(result);
                     Serial.println(">"); 
                     
                     if (result!=200){
                       // http response code is NOT OK  (200) so we stop
                       endtest = true;
                       Serial.println ("  http response is not ok...stopping test");
                       
                       }

         } 
         
    endtest = true;   
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
      Serial.println(" testing finished -> sketch stops working");
      delay (500);
      while(true);
    }
}