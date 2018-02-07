/* Home RF Web Server  */

#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <ESP8266mDNS.h>
#include <DNSServer.h>            //Local DNS Server used for redirecting all requests to the configuration portal
#include <ESP8266WebServer.h>    
#include <RCSwitch.h>
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager 
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

// for WeMos Mini D1  
const int RF433_PINOUT = D1;
const int IR_PINOUT = D2;
// Pins WEMOS D1 Mini for Status LEDs
const int RedLED = D6;
const int BlueLED=D7;
const String Version_text="IR433mHzWSmDNSOTA+Working!";

IRsend irsend(IR_PINOUT);  // an IR emitter led is connected to GPIO pin 4
RCSwitch mySwitch = RCSwitch();
WiFiClient espClient;
PubSubClient MQTTclient(espClient);
long lastMsg = 0;
char msg[50];
const char* mqtt_server = "192.168.0.251";

// TCP server at port 80 will respond to HTTP requests
WiFiServer server(80);
String http_response="";
void setup(void)
{  
  irsend.begin();
  Serial.begin(9600);
  
  mySwitch.enableTransmit(RF433_PINOUT);
  pinMode(RedLED, OUTPUT);
  pinMode(BlueLED, OUTPUT);
  digitalWrite(RedLED, LOW);
  digitalWrite(BlueLED, LOW);  
 
  // Connect to WiFi network
  WiFiManager wifiManager;
  //first parameter is name of access point, second is the password
  wifiManager.autoConnect("RF-Hub");
  wifiManager.setConfigPortalTimeout(180); 
  WiFi.mode(WIFI_STA); 
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  ArduinoOTA.setPassword("5294");
  digitalWrite(RedLED, LOW); delay(200); digitalWrite(RedLED, HIGH); delay(200);digitalWrite(RedLED, LOW); delay(200); digitalWrite(RedLED, HIGH); delay(200);
  digitalWrite(BlueLED, LOW); delay(150); digitalWrite(BlueLED, HIGH); delay(150);digitalWrite(BlueLED, LOW); delay(150); digitalWrite(BlueLED, HIGH); delay(150);
  digitalWrite(RedLED, LOW); delay(100); digitalWrite(RedLED, HIGH); delay(100);digitalWrite(RedLED, LOW); delay(100); digitalWrite(RedLED, HIGH); delay(100);
  digitalWrite(BlueLED, LOW); delay(100); digitalWrite(BlueLED, HIGH); delay(100);digitalWrite(BlueLED, LOW); delay(100); digitalWrite(BlueLED, HIGH); delay(100);
  Serial.println("Connecting to MQTT..");
  MQTTclient.setServer(mqtt_server, 1883);
  MQTTclient.setCallback(callback);   
  Serial.println("Connected");
  // Spin up the Web server framework
  if (!MDNS.begin("AVRFHUB")) {
    Serial.println("Error setting up MDNS responder!");
    while(1) { 
      delay(1000);
    }
  }
  Serial.println("Remote 433mHz and IR Web Server");
  // Start TCP (HTTP) server
  server.begin();
  Serial.println("TCP server started");
  
  // Add service to MDNS-SD
  MDNS.addService("http", "tcp", 80);
  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH)
      type = "sketch";
    else // U_SPIFFS
      type = "filesystem";

    // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
    Serial.println("Start updating " + type);
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });
  ArduinoOTA.begin();
}

void loop(void)
{
  // Check if a client has connected
  WiFiClient http_client = server.available();
  if (!http_client) {
    ArduinoOTA.handle();
    return;
  }
  
  //Maintain connection the MQTT Endpoint (Blocking)
  if (!MQTTclient.connected()) {
    reconnect();
  }
  
  // Read the first line of HTTP request
  String req = http_client.readStringUntil('\r');
  
  // First line of HTTP request looks like "GET /path HTTP/1.1"
  // Retrieve the "/path" part by finding the spaces
  int addr_start = req.indexOf(' ');
  int addr_end = req.indexOf(' ', addr_start + 1);
  if (addr_start == -1 || addr_end == -1) {
    Serial.print("Invalid request: ");
    Serial.println(req);
    return;
  }
  req = req.substring(addr_start + 1, addr_end);
  Serial.print("Request: ");
  Serial.println(req);
  http_client.flush();
  digitalWrite(RedLED, HIGH);
  //Index Request
  if (req == "/")                 {WebResponse("Root/index Page Request.");}
  //AV Room
  if (req == "/AVSPEAKERSON")     {AVSPEAKERSON();}
  if (req == "/AVSPEAKERSOFF")    {AVSPEAKERSOFF();}
  if (req == "/AVONLYON")         {AVONLYON();}
  if (req == "/KODI")             {KODI();}
  if (req == "/SAT")              {SAT();}
  if (req == "/VOLUP")            {VOLUP();}
  if (req == "/VOLDOWN")          {VOLDOWN();}
  //Hall
  if (req == "/HALLON")           {HALLON();}
  if (req == "/HALLOFF")          {HALLOFF();}
  //Loft
  if (req == "/LOFTON")           {LOFTON();}
  if (req == "/LOFTOFF")          {LOFTOFF();} 
  //Gym
  if (req == "/GYMON")            {GYMON();}
  if (req == "/GYMOFF")           {GYMOFF();} 
  if (req == "/RUNON")            {RUNON();}
  //Lights Conservatory
  if (req == "/TABLEON")          {TABLEON();}
  if (req == "/TABLEOFF")         {TABLEOFF();}
  if (req == "/WALLON")           {WALLON();}
  if (req == "/WALLOFF")          {WALLOFF();}
  if (req == "/PRIYALIGHTON")     {PRIYALIGHTON();}
  if (req == "/PRIYALIGHTOFF")    {PRIYALIGHTOFF();}
  // Conservatory AV
  if (req == "/CONSPEAKERSON")    {CONSPEAKERSON();} 
  if (req == "/CONSPEAKERSOFF")   {CONSPEAKERSOFF();}
  if (req == "/SHARPTVON")        {SHARPTVON();}
  if (req == "/SHARPTVOFF")       {SHARPTVOFF();}
  //Empty request?
  if (http_response=="") // No response as the string in the incoming URL was not recognised and so no response set so 404
      {
        http_response = "HTTP/1.1 404 Not Found\r\n\r\n";
        Serial.println("Sending 404");
      }
  http_client.print(http_response);
  char *s;
  s = TimeToString(millis()/1000);
  Serial.println(s);
  http_client.print(" Time in millis :");
  http_client.print(TimeToString(millis()/1000));
  http_client.print("</html>\r\n\r\n");
  Serial.print("HTTP Server responded:");
  Serial.print(http_response);
  Serial.print(" Time in millis :");
  Serial.print(TimeToString(millis()/1000));
  Serial.println("</html>\r\n\r\n");
  http_response="";
  Serial.print("Done with wifi client, request was ");
  Serial.println(req);
  char charBuf[50];
  req.toCharArray(charBuf, 50);
  MQTTclient.publish("RFWebRequest", charBuf );
  MQTTclient.publish("Register", "WebRF Ping");
  //if (flip_OTA){
  //  Process_OTA(); // Spend the next 5 minutes waiting for OTA
  //}
  digitalWrite(RedLED, LOW);
}

void AVSPEAKERSON(){
    WebResponse("Powering on AV Speakers, Button 2 ON = Amp and TV then Button 3 ON = Speakers");
    StatusBrand();
    mySwitch.send("001010011001111101011011"); delay(200); mySwitch.send("001010011001111101011011"); delay(200);    // Button 2 On 
    mySwitch.send("001010011001111101011011"); delay(200); mySwitch.send("001010011001111101011011"); delay(200);  
    mySwitch.send("001010011001111101011101"); delay(200); mySwitch.send("001010011001111101011101"); delay(200);    // Button 3 On
    mySwitch.send("001010011001111101011101"); delay(200); mySwitch.send("001010011001111101011101"); delay(200);  
  }
  
void AVSPEAKERSOFF() {
    WebResponse("Powering off AVSPEAKERS 1) Speakers off button 3, 2) AMP and TV Off Button 2");
    StatusBrand(); 
    mySwitch.send("001010011001111101010101"); delay(200); // Button 3 Off = Speakers off 
    mySwitch.send("001010011001111101010011"); delay(200); // Button 2 Off = Amp and TV
    mySwitch.send("001010011001111101010101"); delay(200); mySwitch.send("001010011001111101010101"); delay(200); //Repeat pressing button 3 off 
    mySwitch.send("001010011001111101010011"); delay(200); mySwitch.send("001010011001111101010011"); delay(200); //Repeat pressing button 2 off                             
  }

void AVONLYON()  {
    WebResponse("Powering On AVONLY: KURO and AMP 1) AMP and TV On Button 2");
    StatusBrand();  
    mySwitch.send("001010011001111101011011"); delay(200); mySwitch.send("001010011001111101011011"); delay(200);
    mySwitch.send("001010011001111101011011"); delay(200); mySwitch.send("001010011001111101011011"); delay(200); 
  }

void KODI()  {
    WebResponse("Send IR Code MPLAY (KODI) - Amp in AV Room to MPLAYER "); 
    irsend.sendRC5(0x17F,12); delay(100); irsend.sendRC5(0x17F,12); delay(100);              
  }
  
void SAT()  {
    WebResponse("Send IR Code MPLAY (KODI) - Amp in AV Room to MPLAYER "); 
    irsend.sendRC5(0x1BF,12); delay(100); irsend.sendRC5(0x9BF,12); delay(100);              
  }

void VOLUP()  {
    WebResponse("Send IR Code MPLAY (KODI) - Amp in AV Room to MPLAYER "); 
    irsend.sendRC5(0xC10,12); delay(100); irsend.sendRC5(0x410,12); delay(100);              
  }

void VOLDOWN()  {
    WebResponse("Send IR Code MPLAY (KODI) - Amp in AV Room to MPLAYER "); 
    irsend.sendRC5(0xC11,12); delay(100); irsend.sendRC5(0x411,12); delay(100);              
  }
  
void HALLON(){
    WebResponse("Powering on Hall Lights -Hall 1, then Hall 2");
    ProElec(); mySwitch.send("010101010001010100110011"); mySwitch.send("010101010001010111000011"); 
    delay(2000);                         
  } 
  
void HALLOFF()  {
    WebResponse("Powering off Hall Lights -Hall 1 then Hall 2"); 
    ProElec(); mySwitch.send("010101010001010100111100"); mySwitch.send("010101010001010111001100"); 
    delay(2000);                         
  }

void LOFTON() {
    WebResponse("Powering On Loft RP..");
    ProElec(); mySwitch.send("010101010001011100000011"); delay(2000); mySwitch.send("010101010001011100000011");                            
  }
void LOFTOFF() {
    WebResponse("Powering On Loft RP..");
    ProElec(); mySwitch.send("010101010001011100001100"); delay(2000); mySwitch.send("010101010001011100001100");                       
  }

/*
Codes for second grey STATUS brand RF remote used in GYM Only not in AV room

One ON    001110000001100011111111
          001110000001100011111111
One OFF   001110000001100011110111
          001110000001100011110111
Two ON    001110000001100011111011
          001110000001100011111011
Two OFF   001110000001100011110011
          001110000001100011110011
  
 */
void GYMON(){
    WebResponse("Powering on Gym Amp & TV");
    StatusBrand();
    mySwitch.send("001110000001100011111111"); delay(200); mySwitch.send("001110000001100011111111"); delay(200);   
    mySwitch.send("001110000001100011111111"); delay(200); mySwitch.send("001110000001100011111111"); delay(200);  
  }

void GYMOFF(){
    WebResponse("Powering off Gym Amp, TV & Running Machine");
    StatusBrand(); // Amp & TV then Running Machine..
    mySwitch.send("001110000001100011110011"); delay(500); mySwitch.send("001110000001100011110011"); delay(200);   
    mySwitch.send("001110000001100011110011"); delay(200); mySwitch.send("001110000001100011110011"); delay(200); 
    mySwitch.send("001110000001100011110111"); delay(200); mySwitch.send("001110000001100011110111"); delay(2000);   
    mySwitch.send("001110000001100011110111"); delay(200); mySwitch.send("001110000001100011110111"); delay(200);  
    mySwitch.send("001110000001100011110011"); delay(200); mySwitch.send("001110000001100011110011"); delay(200);   
    mySwitch.send("001110000001100011110011"); delay(200); mySwitch.send("001110000001100011110011"); delay(200);  
  }

void RUNON(){
    WebResponse("Powering on Amp & TV then Running Machine");
    StatusBrand(); // Amp & TV then Running Machine..
    mySwitch.send("001110000001100011111111"); delay(200); mySwitch.send("001110000001100011111111"); delay(200);   
    mySwitch.send("001110000001100011111111"); delay(200); mySwitch.send("001110000001100011111111"); delay(200);  
    mySwitch.send("001110000001100011111011"); delay(200); mySwitch.send("001110000001100011111011"); delay(200);   
    mySwitch.send("001110000001100011111011"); delay(200); mySwitch.send("001110000001100011111011"); delay(200);  
  }

/* Funry 4 button controllers Button Capture
 * Received 8989656 / 24bit Protocol: 1 = A
 * Received 8989652 / 24bit Protocol: 1 = B
 * Received 8989650 / 24bit Protocol: 1 = C
 * Received 8989649 / 24bit Protocol: 1 = D
 * Sythetic codes for Priya's Room On 8989640 = X
 * Sythetic codes for Priya's Room On 8989636 = Y
 */
 
void TABLEON() {
    WebResponse("Powering On Table Lights in conservatory..");
    Funry(); mySwitch.send(8989656 , 24); delay(2000);   // Button A                      
  }

void TABLEOFF() {
    WebResponse("Powering Off Table Lights in conservatory..");
    Funry(); mySwitch.send(8989650 , 24); delay(2000);   // Button C                      
  }

void WALLON() {
    WebResponse("Powering On Wall Lights in conservatory..");
    Funry(); mySwitch.send(8989652 , 24); delay(2000);   // Button B                      
  }

void WALLOFF() {
    WebResponse("Powering Off Wall Lights in conservatory..");
    Funry(); mySwitch.send(8989649 , 24); delay(2000);   // Button D                      
  }

void PRIYALIGHTON() {
    WebResponse("Powering On Table Lights in conservatory..");
    Funry(); mySwitch.send(8989640 , 24); delay(2000);   // Button X                      
  }

void PRIYALIGHTOFF() {
    WebResponse("Powering Off Table Lights in conservatory..");
    Funry(); mySwitch.send(8989636 , 24); delay(2000);   // Button Y                      
  }

void SHARPTVON(){
    WebResponse("Powering on Sharp TV");
    ProElec(); mySwitch.send("000001010001011100000011");delay(2000); 
  }  

void SHARPTVOFF(){
    WebResponse("Powering off Sharp TV");
    ProElec(); mySwitch.send("000001010001011100001100");delay(2000); 
  }  

void CONSPEAKERSON(){
    WebResponse("Powering on Conservatory Speakers");
    ProElec(); mySwitch.send("000001010001010100110011");delay(2000); 
  }  
  
void CONSPEAKERSOFF(){
    WebResponse("Powering off Conservatory Speakers");
    ProElec(); mySwitch.send("000001010001010100111100");delay(250);   
  }
 
void WebResponse(String m){
    IPAddress ip = WiFi.localIP();
    String ipStr = String(ip[0]) + '.' + String(ip[1]) + '.' + String(ip[2]) + '.' + String(ip[3]);
    http_response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html>";
    http_response += m+" from ESP8266 Version:"+Version_text+" at ";
    http_response += ipStr;
    //End of reponse is in the code at the base of Loop() where the processing completes - String output time
    }

 void ProElec(){
    mySwitch.setProtocol(1);
    mySwitch.setPulseLength(175);
    mySwitch.setRepeatTransmit(7);
    }

 void StatusBrand(){
    mySwitch.setProtocol(1);
    mySwitch.setPulseLength(306); 
    mySwitch.setRepeatTransmit(7);
  }

 void Funry(){
    mySwitch.setProtocol(1);
    mySwitch.setPulseLength(360);
    mySwitch.setRepeatTransmit(7);
  }

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == '1') {
    digitalWrite(BUILTIN_LED, LOW);   // Turn the LED on (Note that LOW is the voltage level
    // but actually the LED is on; this is because
    // it is acive low on the ESP-01)
  } else {
    digitalWrite(BUILTIN_LED, HIGH);  // Turn the LED off by making the voltage HIGH
  }

}


void publish()
{
   snprintf (msg, 75, "%ld", 1);
   Serial.print("Publish message: ");
   Serial.println(msg);
   MQTTclient.publish("RFWebRequest", msg);
}
void reconnect() {
  // Loop until we're reconnected
  while (!MQTTclient.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (MQTTclient.connect(clientId.c_str())) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      MQTTclient.publish("Register", "WebRF Controller Connected");
      // ... and resubscribe
      MQTTclient.subscribe("inTopic");
    } else {
      Serial.print("failed, rc=");
      Serial.print(MQTTclient.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}


// t is time in seconds = millis()/1000;
char * TimeToString(unsigned long t)
{
 static char str[12];
 long h = t / 3600;
 t = t % 3600;
 int m = t / 60;
 int s = t % 60;
 sprintf(str, "%04ld:%02d:%02d", h, m, s);
 return str;
}
