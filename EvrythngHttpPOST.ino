/*
  Sketch Demonstrating posting a thng object on the Evrythng platform
  
  Variables to modify
  const char *APIToken = " please set your evrythng api token value"
  const char *thngid=" please set an existing thng identifier value"
  char thngjson[] = --> adapt json values in the string to your desired values
  
  Hardware needed
  - arduino + NIC interface
  - DHCP enabled on your network
  
  This sketch will 
  - setup and initialize the ethernet connection
  - connect to the evrythng api server
  - create the new thng
  - serialprintln will provide the new thing id
      - if an error occurs and id cannot be obtained
        the same object will be posted and the error will be printed to serial
  
  serial output may look as follows :

      ARDUINO creating a thng object via a HTTP POST to the EVRYTHNG api server.
       initializing ethernet...
        connecting...
        connected to server... 
           HTTP POST 
           {  "name": "ArduiStevAuto","description": "Automatic Thng Created by Mr Arduino", "location": {"latitude": 43.772828,"longitude": 11.249488},"properties":{ "property1":"value1","temperature":"24C","Humidity":"100%"},"tags":["Switzerland","Tinkering","Arduino"]}
           HTTP POST completed 
           reading http response stream.... 
             searching for id of created object in response stream ....
              created thng id: 52e411f7e4b0a7fd3090b31a"
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
const char *thngid="replace_by_your_evrythng_thng_id";

boolean endtest;
String responsestring;
boolean failedhttppost; 


EthernetClient client;

char thngjson[] = "{  \"name\": \"ArduiStevAuto\",\"description\": \"Automatic Thng Created by Mr Arduino\", \"location\": {\"latitude\": 43.772828,\"longitude\": 11.249488},\"properties\":{ \"property1\":\"value1\",\"temperature\":\"24C\",\"Humidity\":\"100%\"},\"tags\":[\"Switzerland\",\"Tinkering\",\"Arduino\"]}";


void setup()
{
  Serial.begin(9600);
  Serial.println("ARDUINO creating a thng object via a HTTP POST to the EVRYTHNG api server.");
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
    Serial.println ("      Object posted with length "+String(strlen(thngjson))+" : ");
    Serial.print("     ");Serial.println (thngjson);
    Serial.println (" ");
              
    // Make a HTTP request POST:
    client.println("POST http://api.evrythng.com/thngs HTTP/1.1");
    client.println("Content-Type: application/json");
    client.println("Accept: application/json");
    client.print("Authorization: ");client.println(APIToken);
    client.println("Connection: close");
    client.print("Content-Length: ");
    client.println(String(strlen(thngjson)));
    client.println();
    client.println(thngjson);  
    
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
            date: Fri, 24 Jan 2014 18:04:52 GMT
            location: https://api.evrythng.com/thngs/52e2ab44e4b0a7fd3090207f
            server: EVRYTHNG
            transfer-encoding: chunked
            Connection: keep-alive
            
            {"id":"52e2ab44e4b0a7fd3090207f","createdAt":1390586692568,"updatedAt":1390586692568,
            "tags":["tag1","tag2","tag3"],"name":"Arduisteven","description":"Arduino Activity Management Interface",
            "location":{"latitude":43.772828,"longitude":11.249488},
            "properties":{"prop1name":"prop1value","prop2name":"prop2value","prop3name":"prop3value"}}
            
            we will search for the first "id":" so we get the http response until the first comma  which will provide us with the entire thng details (including id)
            
             */
           
            responsestring = client.readStringUntil('\",') ; 
            Serial.println();
            Serial.print ("        created thng id: ");
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
                   Serial.print("HTTP/1.1 result code is :<<" );
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
