//Anthony Zhelnakov
//Power Meter
//Dec 22 2022

//MASSIVE props to mo_thunderz on youtube for putting out a great tutorial series on 
//setting up a webserver with the ESP32.
//Github: https://github.com/mo-thunderz

#include "WIFI_fns.h"

#include <WiFi.h>                                     // needed to connect to WiFi
#include <ESPAsyncWebServer.h>                        // needed to create a simple webserver (make sure tools -> board is set to ESP32, otherwise you will get a "WebServer.h: No such file or directory" error)
#include <WebSocketsServer.h>                         // needed for instant communication between client and server through Websockets
#include <ArduinoJson.h>                              // needed for JSON encapsulation (send multiple variables with one string)
//Library allowing us to access data file
#include <SPIFFS.h>


// SSID and password of Wifi connection:

//Access point code
const char* ssid = "Power Meter";
const char* password = "password"; //There is a min character requirement for the password (I think 8)

//IPAddress local_IP(192,168,1,22);
IPAddress local_IP(192,168,77,77);
IPAddress gateway(192,168,1,5);
IPAddress subnet(255,255,255,0);

// global variables of the LED selected and the intensity of that LED
int LED_selected = 0;
int LED_intensity = 50;

//User-settable variables:
void (*setDataMode)(String); 
void (*setCurrentRange)(String); 
void (*setDomain)(int); 
void (*removeWarning)(); 
void (*getAllConfig)();
void (*setLeadResistance)(float);

void setCallbacks(  void (*dm)(String), 
                    void (*cr)(String), 
                    void (*domain)(int),
                    void (*rw)(),
                    void (*cnfg)(),
                    void (*lr)(float) ) {
  setDataMode = dm;
  setCurrentRange = cr;
  setDomain = domain;
  removeWarning = rw;
  getAllConfig = cnfg;
  setLeadResistance = lr;
}

// Initialization of webserver and websocket
AsyncWebServer server(80);                                  // the server uses port 80 (standard port for websites
WebSocketsServer webSocket = WebSocketsServer(81);          // the websocket uses port 81 (standard port for websockets)

void wifiSetup() {                                   
  if (!SPIFFS.begin()) {
    Serial.println("SPIFFS could not initialize");
  }

//Hosting on network code
//  WiFi.begin(ssid, password);                         // start WiFi interface
//  Serial.println("Establishing connection to WiFi with SSID: " + String(ssid));     // print SSID to the serial interface for debugging
// 
//  while (WiFi.status() != WL_CONNECTED) {             // wait until WiFi is connected
//    delay(1000);
//    Serial.print(".");
//  }
//  Serial.print("Connected to network with IP address: ");
//  Serial.println(WiFi.localIP());                     // show IP address that the ESP32 has received from router
//End of network code

//Access point code:
  Serial.println("setting up access point ... ");
  Serial.println(WiFi.softAPConfig(local_IP, gateway, subnet) ? "Ready" : "Failed!");

  Serial.print("starting access point ... ");
  Serial.println(WiFi.softAP(ssid, password) ? "Ready" : "Failed!");
//End of access point code

  Serial.print("IP address = ");
  Serial.println(WiFi.softAPIP());
  
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {                               // define here wat the webserver needs to do
    request->send(SPIFFS, "/Webpage.html", "text/html");           //    -> it needs to send out html text file contents from the memory
  });

  //Notify user of specific errors when server is being loaded
  server.onNotFound([] (AsyncWebServerRequest *request) {
    request->send(404, "text/plain", "File not found");
  });
  
  server.serveStatic("/", SPIFFS, "/");
  
  webSocket.begin();                                  // start websocket
  webSocket.onEvent(webSocketEvent);                  // define a callback function -> what does the ESP32 need to do when an event from the websocket is received? -> run function "webSocketEvent()"

  server.begin();                                     // start server
}

void wifiLoop() {
  webSocket.loop();                                   // Update function for the webSockets 
}

void webSocketEvent(byte num, WStype_t type, uint8_t * payload, size_t length) {      // the parameters of this callback function are always the same -> num: id of the client who send the event, type: type of message, payload: actual data sent and length: length of payload
  switch (type) {                                     // switch on the type of information sent
    case WStype_DISCONNECTED:                         // if a client is disconnected, then type == WStype_DISCONNECTED
      Serial.println("Client " + String(num) + " disconnected");
      break;
    case WStype_CONNECTED:                            // if a client is connected, then type == WStype_CONNECTED
      Serial.println("Client " + String(num) + " connected");
      
      // send LED_intensity and LED_select to clients -> as optimization step one could send it just to the new client "num", but for simplicity I left that out here
      //sendJson("LED_intensity", String(LED_intensity));

      //This section runs whenever a client connects. Here, you can update the website for the new client so that it appears the same as for the other client(s).
      getAllConfig();
      break;
    case WStype_TEXT:                                 // if a client has sent data, then type == WStype_TEXT
      // try to decipher the JSON string received
      StaticJsonDocument<200> doc;                    // create JSON container 
      DeserializationError error = deserializeJson(doc, payload);
      if (error) {
        Serial.print(("deserializeJson() failed: "));
        Serial.println(error.f_str());
        return;
      }
      else {
        // JSON string was received correctly, so information can be retrieved:
        const char* l_type = doc["type"];
        const char* l_value = doc["value"];
        Serial.println("Type: " + String(l_type));
        Serial.println("Value: " + String(l_value));

        // if LED_intensity value is received -> update and write to LED
        if(String(l_type) == "LED_intensity") {
          LED_intensity = int(l_value);
          sendJson("LED_intensity", String(l_value));
        }
        else if (String(l_type) == "DataMode") {
          setDataMode(String(l_value));
        }
        else if (String(l_type) == "CurrentRange") {
          setCurrentRange(String(l_value));
        }
        else if (String(l_type) == "removeWarning") {
          removeWarning();
        }
        else if (String(l_type) == "LeadResistance") {
          float res_value = doc["res"];
          setLeadResistance(res_value);
        }
        else if (String(l_type) == "Domain") {
          String s = String(l_value);
          if (s == "1000") {  setDomain(1000); }
          else if (s == "2000") {  setDomain(2000); }
          else if (s == "3000") {  setDomain(3000); }
          else if (s == "4000") {  setDomain(4000); }
          else if (s == "5000") {  setDomain(5000); }
          else if (s == "6000") {  setDomain(6000); }
          else if (s == "7000") {  setDomain(7000); }
          else if (s == "8000") {  setDomain(8000); }
          else if (s == "9000") {  setDomain(9000); }
          else if (s == "10000") { setDomain(10000); }
        }
      }
      Serial.println("");
      break;
  }
}

// Send all kinds of info to web clients
void sendJson(String l_type, String l_value) {
    String jsonString = "";                           // create a JSON string for sending data to the client
    StaticJsonDocument<200> doc;                      // create JSON container
    JsonObject object = doc.to<JsonObject>();         // create a JSON Object
    object["type"] = l_type;                          // write data into the JSON object -> I used "type" to identify if LED_selected or LED_intensity is sent and "value" for the actual value
    object["value"] = l_value;
    serializeJson(doc, jsonString);                   // convert JSON object to string
    webSocket.broadcastTXT(jsonString);               // send JSON string to all clients
}

// Send the arrays filled with the measured data to the web clients
void sendJsonArrObj(float array_1[], int array_2[]) {
    String jsonString = "";                           // create a JSON string for sending data to the client
    const size_t CAPACITY = JSON_ARRAY_SIZE(100*2) + 200;   //Get the amount of bytes necessary to store an array of 1000 elements
    StaticJsonDocument<CAPACITY> doc;                      // create JSON container
    JsonObject object = doc.to<JsonObject>();         // create a JSON Object

    JsonArray samp_value = object.createNestedArray("value");    //Give the object a time and value array
    JsonArray samp_time = object.createNestedArray("time");    

    object["type"] = "graph_update";
    
    for(int i=0; i<100; i++) {
      samp_value.add(array_1[i]);
      samp_time.add(array_2[i]);
    }

    serializeJson(doc, jsonString);                   // convert JSON object to string
    webSocket.broadcastTXT(jsonString);               // send JSON string to all clients
}

// Send the current configurations of the firmware to the web clients 
void sendAllConfigJson( bool curr_range, short data_mode, short domain, float leadRes ) {
  String jsonString = "";
  StaticJsonDocument<200> doc;
  JsonObject object = doc.to<JsonObject>();
  object["type"] = "client_connected";
  object["current_range"] = curr_range;
  object["data_mode"] = data_mode;
  object["domain"] = domain;
  object["lead_res"] = leadRes;
  serializeJson(doc, jsonString);
  webSocket.broadcastTXT(jsonString);
}
