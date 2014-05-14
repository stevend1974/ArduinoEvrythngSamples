ArduinoEvrythngSamples
======================

Sample Sketches on Arduino with Ethernet Shield using the Evrythng API


These are sample Arduino Sketches that use the official Ethernet shield. 
It has been tested with Arduino Uno (v1.0) but it should also work with other boards.

Arduino Sketches
================

EvrythngHttpGET
---------------
Sketch Demonstrating getting a thng object from the Evrythng platform
  
Variables to modify
  const char *APIToken = " please set your evrythng api token value"
  const char *thngid=" please set an existing thng identifier value"
  
Hardware needed
  - arduino + NIC interface
  - DHCP enabled on your network
  
This sketch will 
  - setup and initialize the ethernet connection
  - connect to the evrythng api server
  - HTTP GET of the thng based on the provided id
      - return the HTTP response
      - if the HTTP response is not OK (200) -> end test
      - if the HTTP response is OK (200) -> get properties of thng
      - properties obtained will be written to serial


      
EvrythngHttpPOST
----------------

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

EvrythngHttpPUT
---------------
  
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
  

EvrythngCreateAction
--------------------

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
  
      

Evrythng Setup
==============

You will need an evrythng account and API token. 
For more information see https://dev.evrythng.com/


ActivityManagementArduinoEvrythng
=================================
ActivityMgmntArduinoEvryThng.ino is a sketch which demonstrates an activity management integration
between Arduino and Evrythng.
The needed hardware, schema setup and configuration are described in ActivityManagementArduinoEvrythng.pdf

This is a sample to simulate an activity registration component.

The user can register the time he spent on an activity.

This sample will demonstrate :
obtaining thng properties from the Evrythng platform to the Arduino platform
registering activities from the Arduino platform to the Evrythng platform

The setup is a simple user interface with push buttons.
LEDs are used to show the interaction.

The user has 5 switches available :
1 button will trigger a configuration update (updating activity description code)
3 buttons will activate a specific activity code (activity1, activity2 or activity3)
1 button will stop all activities
  
The current activity in progress will be indicated by the corresponding LED.
The user can switch from one activity to another and hence stopping the previous activity
The user can decide to do no activity at all
     
The duration of each activity will be sent to the Evrythng API.    

The configuration settings (the activity code for each button) will be obtained from the Evrythng platform. During the update the red configuration LED will flash. 
When the configuration is not being updated, the green configuration LED will permanently be on.
      
      
Licence
=======
      
Copyright 2014 Steven D.

The ArduinoEvrythngSamples software is licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. 
You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions and limitations under the License.

