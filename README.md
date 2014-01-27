ArduinoEvrythngSamples
======================

Sample Sketches on Arduino with Ethernet Shield using the Evrythng API

ArduinoEvryThngSample
=====================

Arduino EvryThng Sample Sketches

This is a sample Arduino application that uses the official Ethernet shield. 
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
  - connect to the evryhng api server
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
  - connect to the evryhng api server
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
  - connect to the evryhng api server
  - HTTP PUT to update a property of the thng based on the provided thng id and values
      - return the HTTP response
      - if the HTTP response is not OK (200) -> end test
      - if the HTTP response is OK (200) -> get properties of thng  
      

Evrythng Setup
==============

You will need an evrythng account and API token. 
For more information see https://dev.evrythng.com/
      
Licence
=======
      
Copyright 2014 Steven D.

The ArduinoEvrythngSample software is licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. 
You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions and limitations under the License.

