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
   
 /*
 
 
 /*
  Activity Management Registration Tool local to Arduino
 
   Created 14th of Feb 2014 by Steven Decruynaere
 
   This is sample code to simulate an activity registration.
   Activities can be enabled by the user interface on the Arduino
   
   The user has 5 buttons available :
     - 1 button will trigger a configuration update (updating activity descriptions)
     - 3 buttons will activate a specific activity (activity1, activity2 or activity3)
     - 1 button will stop any activity
     
     The user can switch from one activity to another and hence stopping the previous activity
     The user can decide to do no activity at all
     
     Te duration of each activity will be sent to the evrything API.
   
   When the Arduino is reset or setup the default activity is none.
   
   Variables to modify
    const char *APIToken = " please set your evrythng api token value"
    const char *thngid=" please set an existing thng identifier value"
      // set default activity codes
     activity0_code=0;
     activity1_code=11;
     activity2_code=22;
     activity3_code=33;
   
   
   Preparation on the Evrythng platform
   
   create a new thng
   
   curl -i -H "Content-Type: application/json" \
    -H "Authorization: *APIToken*" \
    -X POST "https://api.evrythng.com/thngs" \
    -d '{
      "name": "Arduisteven",
      "description": "Arduino Activity Management Interface", 
      "location": {
                "latitude": 43.772828,"longitude": 11.249488},
    "properties":{ 
      "activity0":"0",
      "activity1":"1000",
      "activity2":"2000",
      "activity3":"3000"
      },
      "tags":["Steven","Fribourg","Tinkering"]
      }'
   
   The properties are of importance !
   
   create a new action _recordactivity
        curl -i -H "Content-Type: application/json" \
      -H "Authorization: *APIToken*" \
      -X POST "https://api.evrythng.com/actions" \
      -d '{
        "name": "_recordactivity",
        "customFields":{ 
        "activityname":"name of activity",
        "duration":"duration in ms"
        },
        "tags":["Ardruino","Activity","Automatic"]
        }'
   
   To have output to the serial port replace the string "//Serial.print" with "Serial.print" in the entire sketch
   By default writing to the serial port is disabled to use less resources.
   
*/

#include <SPI.h>
#include <Ethernet.h>

// fixed parameters for the lifetime of Arduino hardwware related
  // serial port speed
  #define SERIAL_PORT_SPEED 9600
  
  //output pins to indicate leds for each activity
  #define activity1_LED_pin 2
  #define activity2_LED_pin 3
  #define activity3_LED_pin 4
  #define activity_stop_LED_pin 5
  
  // analog input pin, depending on voltage -> different activity
  #define analog_input_pin 2
  
  // pins related to configuration update
  #define config_state_pin 7 // green LED for configuration state ok
  #define config_updating_pin 8 // red LED for config update in progress
  
  // config update switch pin
  #define config_update_switch_pin 9
  
  // switchstate for get configuration update
  // when the switch is activated -> a config update request is sent 
  int getconfig_switchstate = 0;
    
// variable parameters of the activity management software related
  // code for each of the activitys 
  // updated via config update function
        int activity0_code;
        int activity1_code;
        int activity2_code;
        int activity3_code;

  
  // application related parameters
   
      // integer to define current activity  
      int currentactivity;
      // descriptions of activities
      int currentactivity_code;
      int previousactivity_code;
     
     // timer to know how long a task lasted
      long activitystart; 
      long activityduration;
      
      // variable to iterate through properties set on the thng
      int int_property=0; 
      
      //internet related variables
        // mac address hardcoded
        byte mac[6] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xFE };
        EthernetClient client;
      // EVRYTHNG variables
      char serverName[] = "api.evrythng.com";
      const char *APIToken = "*APIToken*";
      const char *thngid="52fb7719e4b0c55926866247";
      
      // boolean
      boolean b_do_some_http = false;
      // create a boolean to check if an update of the config is needed
      boolean b_getconfig = false; 
      // boolean to get thing properties
      boolean b_stopcurrenttask = false;
      // boolean to set a thng activity
      boolean b_thng_add_action = false;
      // boolean to set thng current activity
      boolean b_set_thng_currentactivity_property = false;
      // boolean to know if we are in initial setup
      boolean b_initial_loop = true;
      
      
      
// end of global variables      
  
// setup to initialize
void setup(){
  
  // open serial port
  Serial.begin(SERIAL_PORT_SPEED);
  //Serial.println ("Activity Management Registration Tool local to Arduino");
  //Serial.println(" arduino set up -> start");
  
  // setup the Arduino PINs  
  setup_pins();
  
  // set default activity codes
   activity0_code=0;
   activity1_code=11;
   activity2_code=22;
   activity3_code=33;
        
        
  // setup initial activity to 0 and its hardcoded description
  currentactivity = 0;
  currentactivity_code=activity0_code;         
  previousactivity_code= activity0_code; 
  activitystart =0;
  activityduration=0;
  
  
  // setup internet variables  
   b_thng_add_action = false;
   b_set_thng_currentactivity_property = false;
   b_initial_loop = true;
        
   // at setup get the configuration
   b_getconfig = false;
   b_do_some_http= false;
    
   // sstart ethernet
   if(Ethernet.begin(mac) == 0) { // start ethernet using mac & IP address
         ////Serial.println("Failed to configure Ethernet using DHCP");  
          while(true)   // no point in carrying on, so stay in endless loop:
                  ;
        }
     delay(1000); // give the Ethernet shield a second to initialize
   
               
  // giva all initialization some time to setup
  delay (1000);
  
  //Serial.println(" arduino setup -> end");
} 

    // setup related methods
    
    // method to initialize the pins of the arduino
    void setup_pins(){    
  
      //Serial.println("   setting up pins on arduino -> start");
      
      // declare the LED pins as outputs 
      pinMode(activity1_LED_pin,OUTPUT); // blue LED activity1
      pinMode(activity2_LED_pin,OUTPUT); // yellow LED activity2
      pinMode(activity3_LED_pin,OUTPUT); // green LED activity3
      pinMode(activity_stop_LED_pin,OUTPUT); // red LED stop  
      pinMode(config_state_pin,OUTPUT); // Green LED request update config
      pinMode(config_updating_pin,OUTPUT); // Red LED Update Config
      
      // define pin for update request
      pinMode(config_update_switch_pin,INPUT); // request config update
      
      // set default ligt settings, no activity being executed
      digitalWrite(activity1_LED_pin, LOW); // activity1
      digitalWrite(activity2_LED_pin, LOW);
      digitalWrite(activity3_LED_pin, LOW);
      digitalWrite(activity_stop_LED_pin, HIGH);
      
      //config is ok and no update in progress
      digitalWrite(config_state_pin, HIGH);
      digitalWrite(config_updating_pin, LOW); 
      
      //Serial.println("   setting up pins on arduino -> end");

    }



void loop(){
  
  // if in first loop get the config first
        if (b_initial_loop){
          // get first the thng properties to be in sync
            b_getconfig=true;
            b_do_some_http=true;
            b_initial_loop = false;
            
        // end if first loop
         }
  
        if(!b_do_some_http)  {
  
        // read the value of the switch
        int keyVal = analogRead(analog_input_pin);
        
        
        // //Serial.println (keyVal);
        getconfig_switchstate = digitalRead(config_update_switch_pin);



        if(getconfig_switchstate == HIGH)  {
                // call the updateconfig method
                //Serial.println(" update config button pressed...");
                //Serial.println(" stopping current task first");
                b_getconfig = true;
                digitalWrite(config_state_pin, LOW);
                digitalWrite(config_updating_pin, LOW);
                delay(200);
                digitalWrite(config_updating_pin, HIGH);
                delay(200);
                digitalWrite(config_state_pin, LOW);
                // implement code here to update teh configuration settings like activity code
                
                
          } else { // check analog input
          
               //Serial.print("The obtained analog input value is ");//Serial.println(keyVal);
              
              
              if (keyVal == 1023) {
                // call the updateconfig method
                //Serial.println(" activating task1...");
                setcurrentactivity(1);
              } else if (keyVal >= 990 && keyVal <= 1010) {
                // call the updateconfig method
                //Serial.println(" activating task2...");
                setcurrentactivity(2);
              } else if (keyVal >= 505 && keyVal <= 515) {
                // call the updateconfig method
                //Serial.println(" activating task3...");
                setcurrentactivity(3);
              } else if (keyVal >= 5 && keyVal <= 10) {
                // call the updateconfig method
                //Serial.println(" stopping activity...");
                setcurrentactivity(0);
              }
          } // end of else


          if (b_getconfig) {
              //Serial.println(" Update of Configuration -> start ");
              
              // implement code to obtain updated configuration code
              // setting the soft values of activity descriptions
              digitalWrite(config_state_pin, LOW);
              digitalWrite(config_updating_pin, HIGH);
              delay(500);
              digitalWrite(config_updating_pin, LOW);
              delay(500);
              digitalWrite(config_updating_pin, HIGH);
              delay(500);
              digitalWrite(config_state_pin, HIGH);
              digitalWrite(config_updating_pin, LOW);
              
              b_getconfig = true;
            
              //Serial.println(" Update of Configuration -> end ");
          }  
  
    // get a break of 1/4 of second before checking again
    delay(250);
  
  } // end of no http things to do..just listening to input
  
  
   if(b_do_some_http)  {
    
                  //Serial.println (" in HTTP world");
                  // if the current task has to be set to stop
                  // if set config properties obtained from thng
                  if (b_getconfig) {
                            //Serial.println(" Update of Configuration -> start ");
                            
                            // implement code to obtain updated configuration code
                            // setting the soft values of activity descriptions
                            digitalWrite(7, LOW);
                            digitalWrite(8, HIGH);
                            delay(250);
                            digitalWrite(8, LOW);
                            delay(250);
                            digitalWrite(8, HIGH);
                            delay(250);
  
                         //Serial.print(" checking property activity");Serial.println(int_property);
                         if (int_property <3){    
                            // start getting property
                            
                            
                            //https://api.evrythng.com/thngs/52fb7719e4b0c55926866247/properties/activity0?access_token=*APIToken*
                             if (client.connect("api.evrythng.com", 80)>0) {
                              //Serial.print("  connected... ");
                              client.print("GET /thngs/");
                              client.print(thngid);
                              client.print("/properties/");
                              
                              switch (int_property) {
                                  case 0:  // stop case
                                      client.print("activity0");
                                      break;
                                  case 1:  // stop case
                                      client.print("activity1");
                                      break;
                                  case 2:  // stop case
                                      client.print("activity2");
                                      break;  
                                  case 3:  // stop case
                                      client.print("activity3");
                                      break;                                         
                                  default:
                                      ;//Serial.println("not set");                                  
                               }
                              
                              client.print("?access_token=");
                              client.print(APIToken);
                              client.println(" HTTP/1.1");
                              client.println("Host: evrythng.net");
                              client.println("Connection: close");
                              client.println();
                              
                              //Serial.println (" -> http GET sent");
                              // --> increase property to check next one
                              int_property++;
                                    
                              delay(250);
                              
                            } else {
                              b_getconfig = false;
                              b_do_some_http = false;
                              //Serial.println("connection failed");
                            }
                                 
                            
                          if (client.connected()) {
                                
                                //Serial.println("  >> http response stream.... ");
                                
                                // uncomment following line to see entire http response
                                //Serial.print(client.readString());
                                if(client.find ("value")){
                                  
                                          //Serial.print("  obtaining property");Serial.println(int_property);
                                           //int activitycode = client.parseInt();
                                           //Serial.print (" code : ");Serial.println(activitycode);
                                           
                                           switch (int_property) {
                                                  case 0:
                                                      activity0_code=client.parseInt() ;
                                                      break;
                                                  case 1:  
                                                     activity1_code=client.parseInt();
                                                     break;
                                                  case 2:  
                                                     activity2_code=client.parseInt();
                                                     break; 
                                                  case 3:  
                                                      activity3_code=client.parseInt();
                                                      break; 
                                                  default:
                                                      ; //Serial.println(" nothing to set");
                                                   }; // end of switch
                                                            
                                    
                                } // got value up to next http stuff
                           
                        
                              }   else { // we are not connected 
                                    
                                    b_getconfig = false;
                                    b_do_some_http = false;
                                    int_property=3;
                                    digitalWrite(7, HIGH);
                                    digitalWrite(8, LOW);
                                    delay(1000);
                                  } 
                       
                       // end if int_property< 3
                         } else { // we cycled through all properties stop looping
                                              int_property=0; 
                                              b_getconfig = false;
                                              b_do_some_http = false;
                                              digitalWrite(7, HIGH);
                                              digitalWrite(8, LOW);
                                             } 
                              
                    client.stop();
                    client.flush();  
                    delay(250);
                    // end getting properties        
                    //Serial.println(" Update of Configuration -> end ");
                   
                 }    // end getting config of thng
  
  
  
                  // if PUT thng action
                  if(b_thng_add_action)  {
                    
                    // we do a fire and forget and do not care about the response to save resources...
                    
                    /*
                    -X POST "https://api.evrythng.com/actions/_logactivity" \
                      -d '{
                        "thng":"52e8f77de4b0498c09f12f2e",
                        "type":"_recordactivity",
                        "customFields":{
                          "activityname":"blue",
                          "duration":"432133432"
                          }
                      }'
                    */
                    

                    if (client.connect(serverName, 80)>0) {
    
                        //Serial.println("  connected to server... "); 
                        
                       int postsize = strlen("{\"name\":\"")+strlen(thngid)+strlen("\",\"type\":\"_recordactivity\",\"customFields\":{\"activityname\":\"")  + numdigits(currentactivity_code)+ strlen("\",\"duration\":\"") + String(activityduration).length()  + strlen("\"}}");
                         
                        //Serial.print(" posting data size: ");Serial.println(postsize);
                        //Serial.print ("activityduration is ");Serial.println(activityduration);                                
                        
                        // Make a HTTP request POST:
                        client.println("POST http://api.evrythng.com/actions/_recordactivity HTTP/1.1");
                        client.println("Content-Type: application/json");
                        client.println("Accept: application/json");
                        client.print("Authorization: ");client.println(APIToken);
                        client.println("Connection: close");
                        client.print("Content-Length: ");
                        client.println(postsize);
                        client.println();
                        client.print("{\"type\":\"_recordactivity\",\"thng\":\"");
                        client.print(thngid);
                        client.print("\",\"customFields\":{\"activityname\":\"");  
                        client.print(currentactivity_code);
                        client.print("\",\"duration\":\"");
                        client.print(activityduration);
                        client.println("\"}}");
                        
                        //Serial.println ("     HTTP POST completed ");
                        
                      } else {
                        //Serial.println(" connection failed");
                      }
                      
                      
                      if (client.connected()) {
                          if(client.find("HTTP/1.1 ")){
                                  //Serial.println("    checking out the response");
                                    Serial.print("    HTTP/1.1 result code is :<" );
                                    Serial.print(client.readString());
                                        //Serial.println(">"); 
                            
                             }
                        }

                      
                    // in all cases stop http and add action
                      b_thng_add_action = false;
                      b_do_some_http = false;
                      client.stop();
                      client.flush(); 

    
               } // end if http action
  
  
  
  
     } // end of http world
  
  } // end of main loop
  
  
  // methods used in main loop
  
  /* setcurrentactivity
     returns nothing
     parameter int newactivitynr
       parameter can be 0,1,2,3 for each of the activities being performed  
     the result of this action is setting current activity and corresponding LED  
  */
  void setcurrentactivity (int newactivitynr) {
          // checking if we changed activity (no need to update if continuing previous)
          if (newactivitynr!=currentactivity){
                // stop current activity & set new activity
                //Serial.println("  new activity to be set ...");
                //Serial.print("      previous activity :");//Serial.println(previousactivity_code);
                                
                switch (currentactivity) {
                    case 0:  // stop case
                        //Serial.print(" stopping activity ");//Serial.println(previousactivity_code);
                        digitalWrite(activity1_LED_pin, LOW);
                        digitalWrite(activity2_LED_pin, LOW);
                        digitalWrite(activity3_LED_pin, LOW);
                        digitalWrite(activity_stop_LED_pin, HIGH); // stop  
                        currentactivity_code=activity0_code; 
                        break;
                    case 1: 
                        //Serial.print(" starting activity1 : ");//Serial.println(activity1_code);
                        digitalWrite(activity1_LED_pin, HIGH); // activity1
                        digitalWrite(activity2_LED_pin, LOW);
                        digitalWrite(activity3_LED_pin, LOW);
                        digitalWrite(activity_stop_LED_pin, LOW);   
                        currentactivity_code=activity1_code; 
                        break;
                    case 2:  
                        //Serial.print(" starting activity2 : ");//Serial.println(activity2_code);
                        digitalWrite(activity1_LED_pin, LOW);
                        digitalWrite(activity2_LED_pin, HIGH); // activity2
                        digitalWrite(activity3_LED_pin, LOW);
                        digitalWrite(activity_stop_LED_pin, LOW);  
                        currentactivity_code=activity2_code; 
                        break;
                    case 3:
                        //Serial.print(" starting activity3 : ");//Serial.println(activity3_code);
                        digitalWrite(activity1_LED_pin, LOW);
                        digitalWrite(activity2_LED_pin, LOW);
                        digitalWrite(activity3_LED_pin, HIGH); // activity3
                        digitalWrite(activity_stop_LED_pin, LOW);    
                        currentactivity_code=activity3_code; 
                        break;
                    default:
                      currentactivity_code=activity0_code; ;
                  } // end of switch
                  
                  
                  // if the previous activity was nothing -> nothing to calculate
                  // currentactivity = activity up to now, newactivity will come after this one
                  if(currentactivity!=0){
                      // calculate time
                      activityduration = millis() - activitystart;
                      //Serial.print ("       The activity ");
                      //Serial.print (previousactivity_code);
                      //Serial.print (" took ");
                      //Serial.print (activityduration);
                      //Serial.println (" ms. ");
                       // go for some http post
                      b_thng_add_action = true;
                      b_do_some_http = true;
                  } // end I ended doing some activity (not doing nothing)
                  
                  
                  // reset timer 
                  activitystart = millis();
                  
                  // the current activity becomes the newly choosen one
                currentactivity = newactivitynr;
                previousactivity_code=currentactivity_code;     
                Serial.print(" new activity "); Serial.println(currentactivity_code);
                  
                //Serial.println("    new activity to be set finished");
                
             } // end of if newactivity             
             else {
                  //Serial.print(" no new activity started, still executing activity ");//Serial.println(currentactivity);
             } 
        
         } // end of method     
  
// function to know number of digits of an integer (the activitycode
int numdigits(int i)
{
        char str[16];

        sprintf(str,"%d",abs(i));
        return(strlen(str));
} 

 
