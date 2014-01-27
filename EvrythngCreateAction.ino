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
  Sketch Demonstrating posting an action on a thng object on the Evrythng platform
  
  Variables to modify
  const char *APIToken = " please set your evrythng api token value"
  const char *thngid=" please set an existing thng identifier value"
  const char actionjson = --> adapt json values in the string to your desired values
  for more information see https://dev.evrythng.com/documentation/api
  
  Hardware needed
  - arduino + NIC interface
  - DHCP enabled on your network
  
  This sketch will 
  - setup and initialize the ethernet connection
  - connect to the evrythng api server
  - add a scan action to an existing thng
  - serialprintln will provide the new action id
      - if an error occurs and id cannot be obtained
        the same object will be posted and the error will be printed to serial
  
  serial output may look as follows :
  
    ARDUINO creating an action scan on thingid 52e411f7e4b0a7fd3090b31a via a HTTP POST to the EVRYTHNG api server.
     initializing ethernet...
      connecting...
      connected to server... 
         HTTP POST 
          Object posted with length 83 : 
         { "thng":"52e411f7e4b0a7fd3090b31a","type":"scans","customFields":{"color":"blue"}}
         HTTP POST completed 
    
         reading http response stream.... 
           searching for id of created object in response stream ....
            created action id: 52e63cc0e4b0418f6a429ee2"
    
    ----------------------------------------------
     end of test requested -> sketch stops working
    ----------------------------------------------

  Steven D., 25 jan 2014
 */
 
 
#include <SPI.h>
#include <Ethernet.h>
#include <TextFinder.h>
  

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xFE };
char serverName[] = "api.evrythng.com";
const char *APIToken = "replace_by_your_evrythng_api_token";
const char *thngid="replace_by_your_evrythng_thng";
const char *actionjson  ="{ \"thng\":\"52e411f7e4b0a7fd3090b31a\",\"type\":\"scans\",\"customFields\":{\"color\":\"blue\"}}";


boolean endtest;
String responsestring;
boolean failedhttppost; 


EthernetClient client;


void setup()
{
  Serial.begin(9600);
  Serial.print("ARDUINO creating an action scan on thingid ");
  Serial.print(thngid);
  Serial.println(" via a HTTP POST to the EVRYTHNG api server.");
  Serial.println();
  
  Serial.println(" initializing ethernet...");
  if(Ethernet.begin(mac) == 0) { // start ethernet using mac & IP address
    Serial.println("Failed to configure Ethernet using DHCP");  
    while(true)   // no point in carrying on, so stay in endless loop:
      ;
  }
  delay(1000); // give the Ethernet shield a second to initialize

  endtest = false;
  responsestring="";
  failedhttppost= false; 
  
  Serial.println("");  
  Serial.println("  connecting...");
}

void loop()
{
  
  if (client.connect(serverName, 80)>0) {
    
    Serial.println("  connected to server... "); 
    
    Serial.println ("     HTTP POST ");
    Serial.print ("      Object posted with length ");Serial.print(strlen(actionjson));Serial.println (" : ");
    Serial.print   ("     ");Serial.println (actionjson);
    Serial.println (" ");
              
    // Make a HTTP request POST:
    client.print("POST http://api.evrythng.com/actions/"); client.print("scans");client.println(" HTTP/1.1");
    client.println("Content-Type: application/json");
    client.println("Accept: application/json");
    client.print("Authorization: ");client.println(APIToken);
    client.println("Connection: close");
    client.print("Content-Length: ");
    client.println(strlen(actionjson));
    client.println();
    client.println(actionjson);  
    
     Serial.println ("     HTTP POST completed ");
     Serial.println ("");
    
  } else {
    Serial.println(" connection failed");
  }
  
  
  if (client.connected()) {
    // check for http response code
    Serial.println("     reading http response stream.... ");
    Serial.println ();
    
    if (!failedhttppost){ // httppost to create thng
        
        if(client.find ("id\":\"")){
            Serial.println("       searching for id of created object in response stream ....");
            
            /*
            
            HTTP/1.1 201 Created
            Access-Control-Allow-Origin: *
            Access-Control-Expose-Headers: Link, X-Result-Count
            content-type: application/json
            date: Mon, 27 Jan 2014 10:35:30 GMT
            location: https://api.evrythng.com/actions/scans/52e63672e4b0a7fd309193c8
            server: EVRYTHNG
            transfer-encoding: chunked
            Connection: keep-alive

            {"id":"52e63672e4b0a7fd309193c8","createdAt":1390818930535,"customFields":{"color":"blue"},
            "timestamp":1390818930535,"type":"scans","user":"52e29c83e4b0418f6a412033",
            "location":{"latitude":46.7961,"longitude":7.123},
            "locationSource":...
            "thng":"52e411f7e4b0a7fd3090b31a"}

            
            we will search for the first "id":" so we get the http response until the first comma  which will provide us with the entire thng details (including id)
            
             */
           
            responsestring = client.readStringUntil('\",') ; 
            Serial.println();
            Serial.print ("        created action id: ");
            Serial.println (responsestring);
            Serial.println("" );
            responsestring="";
            endtest = true;
        
           } // couldnt find id, something went wrong during the creation
             else {
                 Serial.println ("  ERROR >> thing could not be created ");
                 failedhttppost = true;
                 Serial.println ("        >> trying a second post to catch the error ");
                 Serial.println();
           } // end checking for id details
    
    } else { // post failed so check the response to get more information
      
      endtest = true; // no need to go in endless cylces -> finish
      if(client.find("HTTP/1.1 ")){
                   Serial.println();
                   Serial.println("      checking out the response to get failure code");
                   Serial.print("        HTTP/1.1 result code is :<<" );
                   Serial.print( client.readString());
                   Serial.println(">>");
                   responsestring="";               
         }
      } // end http post failed catch error code
     
         client.stop();
         client.flush();
         delay(10000); // check again in 10 seconds
     
    } // end client connected
     else {
        Serial.println();
        Serial.println("   not connected");
        client.stop();
        client.flush();
        delay(10000);
    }
  
    if (endtest){
      // found objective stop working
      Serial.println();
      Serial.println("----------------------------------------------");
      Serial.println(" end of test requested -> sketch stops working");
      Serial.println("----------------------------------------------");
      delay (500);
      while(true);
    }
    
} // end of loop
