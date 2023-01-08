//Anthony Zhelnakov
//Power Meter
//Dec 22 2022

#ifndef WIFI_FNS_H
#define WIFI_FNS_H

//#include <WiFi.h>                                   // needed to connect to WiFi
#include <ESPAsyncWebServer.h>                        // needed to create a simple webserver (make sure tools -> board is set to ESP32, otherwise you will get a "WebServer.h: No such file or directory" error)
#include <WebSocketsServer.h>                         // needed for instant communication between client and server through Websockets
#include <ArduinoJson.h>                              // needed for JSON encapsulation (send multiple variables with one string)
#include <Arduino.h>

void wifiSetup();

//void uploadBufs( int vals[100], int times[100]);

void wifiLoop();

void setCallbacks( void (*dm)(String), void (*cr)(String), void(*domain)(int), void(*rw)(), void (*cnfg)(), void(*lr)(float) );

void sendJson(String l_type, String l_value);

void sendJsonArrObj(float array_1[], int array_2[]);

void sendAllConfigJson( bool curr_range, short data_mode, short domain, float leadRes );

void webSocketEvent(byte num, WStype_t type, uint8_t * payload, size_t length);

#endif
