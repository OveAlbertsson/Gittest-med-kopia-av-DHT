#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <DHT.h>

#include "WifiCredential.h"

#define DEBUG 0

// ifndef STASSID
// define STASSID "SSID"
// define STAPSK  "Password"
// endif

#define DHTPIN 2        // Digital pin connected to the DHT sensor; use pins 1, 2 or 3
#define DHTTYPE DHT11   // DHT11, DHT22, DHT21
#define ON 0
#define OFF 1

// Initialize DHT sensor.
DHT dht( DHTPIN, DHTTYPE );

const char* ssid = STASSID;
const char* password = STAPSK;

// TCP server at port 80 will respond to HTTP requests
WebServer server( 80 );

void sendText( String text ) {
  #ifdef DEBUG
  Serial.println( "Sending 200" );
  Serial.println( text );
  #endif
  server.send( 200, "text/html", text );
}

void sendError( String text ) {
  #ifdef DEBUG
  Serial.println( "Sending 404" );
  Serial.println( text );
  #endif
  server.send( 404, "text/plain", "ERROR: "+text );
}

void handleNotFound() {
  String message = "Page Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  sendError( message );
}

void handleRoot() {
  #ifndef DEBUG
  digitalWrite( LED, ON );
  #endif
  sendText( WiFi.localIP().toString() + ": Hej hej from ESP32-C3!" );
  #ifndef DEBUG
  digitalWrite( LED, OFF );
  #endif
}

#define TEMPERATURE 1
#define HUMIDITY 2

void handleGet( int type ) {
  #ifndef DEBUG
  digitalWrite( LED, ON );
  #endif
  float val;
  String var = "";
  String unit = "";
  if (type == TEMPERATURE) {
    var = "Temperature";
    val = dht.readTemperature();
    unit = "&deg;C";
    #ifdef DEBUG
    Serial.println( "getT inquiry" );
    #endif
  }
  else if (type == HUMIDITY) {
    var = "Humidity";
    val = dht.readHumidity();
    unit = "%";
    #ifdef DEBUG
    Serial.println( "getH inquiry" );
    #endif
  }
  if (var == "") {
    sendError( "Unknown data type!" );
  }
  else if (isnan(val)) {
    sendError( "Failed to read from DHT sensor!" );
  }
  else {
    String message = var+"("+unit+"): " + String(val);
    sendText( message );
  }
  #ifndef DEBUG
  digitalWrite( LED, OFF );
  #endif
}

void setup( void ) {
  dht.begin();
  #ifdef DEBUG
  Serial.begin(115200);
  #else
  // pinMode( LED, OUTPUT );
  // digitalWrite( LED, OFF );
  #endif
  
  // Connect to WiFi network
  WiFi.mode( WIFI_STA );
  WiFi.begin( ssid, password );
  #ifdef DEBUG
  Serial.println( "Uppkopplad" );
  #endif
  
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay( 500 );
    #ifdef DEBUG
    Serial.print( "." );
    #endif
  }
  #ifdef DEBUG
  Serial.println( "" );
  Serial.print( "Connected to " );
  Serial.println( ssid );
  Serial.print( "IP address: " );
  Serial.println( WiFi.localIP() );
  #endif
  
  server.onNotFound( handleNotFound );
  server.on( "/", handleRoot );
  server.on( "/getT", []() {handleGet( TEMPERATURE );} );
  server.on( "/getH", []() {handleGet( HUMIDITY );} );

  server.begin();
  #ifdef DEBUG
  Serial.println( "HTTP server started" );
  #endif
}

void loop( void ) {
  server.handleClient();
}