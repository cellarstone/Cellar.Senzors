/*.
 Name:		PirMotionSensorWemos.ino
 Created:	6/16/2017 12:27:04 PM
 Author:	Ondrej Snop - WWW.CELLARSTONE.CZ
 Version:	1.0.0
*/


// pøipojení knihovny DHT
//#include "DHT_U.h"
//#include "DHT.h"

#include <DHT_U.h>
#include <DHT.h>
#include <sstream>

//#include <WiFiManager.h>
#include <ESP8266mDNS.h>
#include <ArduinoOTA.h>
#include <ESP8266WebServer.h>
#include <PubSubClient.h>
#include <WiFiUdp.h>
#include <WiFiServer.h>
#include <WiFiClientSecure.h>
#include <WiFiClient.h>
#include <ESP8266WiFiType.h>
#include <ESP8266WiFiSTA.h>
#include <ESP8266WiFiScan.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266WiFiGeneric.h>
#include <ESP8266WiFiAP.h>
#include <ESP8266WiFi.h>

int PIR_SENSOR_PIN = 4; //pin, na ktery je pripojen vystup PIR senzoru

int DEBUG_LED_PIN = 3; //pin diody simulujici odeslani zpravy do fronty

int REG_DATA_PIN = 5; //posuvny registr DATA pin
int REG_CLK_PIN = 6; //posuvny registr CLOCK pin
int REG_LATCH_PIN = 7; //posuvny registr LATCH pin

char* MqttHostName = "192.168.1.234";//"192.168.0.193"; //IP adresa MQTT serveru
//char* MqttUserName = "app"; //uzivatel MQTT serveru
//char* MqttUserPass = "app"; //heslo MQTT uzivatele
//char* MqttQueueName = "PirSensorQueue"; //{ 'P', 'i', 'r', 'S', 'e', 'n', 's', 'o', 'r', 'Q', 'u', 'e', 'u', 'e' }; //nazev MQTT fronty

char* DeviceNamePrefix = "PIRSensor_"; //nazev zarizeni
char* FullDeviceName = ""; //bude sestaveno z DeviceNamePrefix a ciselneho kodu z HW micro-prepinace

char* WifiSsid = "CellarstoneDevicesWifi"; //"NTBG"; //"MeetRoom"; //"Gibonovo"; //nazev wifi site
char* WifiPass = "Cllrs123IoT456"; //"11235813";//"Sokepans"; //"LePri5ere"; //wifi heslo

//int sensorCheckIntervalMillis = 5000;
long lastMillis = 0;

ESP8266WebServer server(80);

WiFiClient wifiClient;
//WiFiManager wifiManager;
PubSubClient mqttClient(MqttHostName, 1883, messageRecievedCallback, wifiClient);

PROGMEM const char html_hlavicka[] = "<!DOCTYPE html><html lang=\"en\" xmlns=\"http://www.w3.org/1999/xhtml\"><head><meta charset=\"utf-8\" /><title>ESP8266 - Control panel</title><style type=\"text/css\">html,body{font-family:'Segoe UI';margin:0;padding:0;width:100%;height:100%;overflow:auto;background-color:black}.item{color:limegreen;font-weight:bold;text-align:right}.value{color:grey;font-weight:normal;text-align:left}</style></head><body><table style=\"margin: 100px auto;\"><tr><td class=\"item\"> IP Address:</td><td class=\"value\">{ip}</td></tr><tr><td class=\"item\"> Device name:</td><td class=\"value\"> PirSensor_001</td></tr><tr><td class=\"item\"> Firmware version:</td><td class=\"value\"> 1.0.1</td></tr><tr><td class=\"item\"> Uptime:</td><td class=\"value\">{uptime} ms</td></tr></table></body></html>";
PROGMEM const char html_paticka[] = "</div></body>";




// nastavení èísla pinu s pøipojeným DHT senzorem
#define pinDHT D7

// odkomentování správného typu èidla
#define typDHT11 DHT11     // DHT 11
//#define typDHT22 DHT22   // DHT 22 (AM2302)

// inicializace DHT senzoru s nastaveným pinem a typem senzoru
DHT mojeDHT(pinDHT, typDHT11);


//-------SETUP-----------------------------------------------------------------------------------------
void setup() {
	Serial.begin(9600);
	delay(1000);	
	Serial.println("");

	// zapnutí komunikace s teplomìrem DHT
	mojeDHT.begin();


	/*pinMode(DEBUG_LED_PIN, OUTPUT);
	digitalWrite(DEBUG_LED_PIN, LOW);*/

	pinMode(BUILTIN_LED, OUTPUT);
	digitalWrite(BUILTIN_LED, HIGH); //nesviti - na Wemosu ma vestavena dioda obracene zapojeni

	//nastaveni naslouchani preruseni pro pin senzoru
	/*pinMode(PIR_SENSOR_PIN, INPUT_PULLUP);
	attachInterrupt(PIR_SENSOR_PIN, handlePirInterupt, RISING);*/

	//nastaveni pinu pro posuvny registr
	//pinMode(REG_CLK_PIN, OUTPUT);
	//pinMode(REG_LATCH_PIN, OUTPUT);
	//pinMode(REG_DATA_PIN, INPUT);

	setupDeviceFullName();
	Serial.println("");
	Serial.print("Device name: ");
	Serial.print(FullDeviceName);
	
	//wifiManager.autoConnect("ESP_AP", "12pass34");
	//wifiManager.startConfigPortal("ESP-AP");
	WiFi.mode(WIFI_STA);


	Serial.println("");
	Serial.print("Connecting to ");
	Serial.print(WifiSsid);
	//pripojime se k wifi
	WiFi.begin(WifiSsid, WifiPass);	
	
	//dokud se nepripojime, budem blikat diodou - po pripojeni zustane svitit
	checkWiFiStatus();
	Serial.println("");
	Serial.println("Connected!");
	Serial.println("");
	Serial.print("IP address: ");
	Serial.print(WiFi.localIP());

	initOTA();

	//nastaveni webserveru
	//pokud client prisoupi na root
	server.on("/", []() {
		String action = server.arg("action");
		action.toLowerCase();
		
		//resetovat zarizeni
		if (action == "reset") {
			ESP.restart();
		}
		String str = String(html_hlavicka);
		str.replace("{ip}", String(WiFi.localIP()));
		str.replace("{uptime}", String(millis()));
		server.send(200, "text/html", str);
	});

	Serial.println("");
	Serial.println("Starting web server...");
	server.begin();
	Serial.println("Server started!");
	
	lastMillis = millis();
}

//-------LOOP------------------------------------------------------------------------------------------
void loop() {
	//stav musime kontrolovat i v hlavni smycce, jelikoz bez wifi je nam senzor k nicemu
	checkWiFiStatus();

	//obslouzit pripadny pokus o update zarizeni
	ArduinoOTA.handle();

	//obslouzit clienta pristupujici na web zarizeni
	server.handleClient();

	//kazdych x milisekund zkontrolujeme stav senzoru
	if ((millis() - lastMillis) > 5000) {
		sendMessageIfSensorActive();
		lastMillis = millis();
	}
}






//-------METODY----------------------------------------------------------------------------------------

/*
	zpracuje signal z PIR senzoru
	odpoji naslouchani preruseni -> posle zpravu do fronty -> pripoji zpet naslouchani preruseni
*/
void handlePirInterupt() {
	detachInterrupt(PIR_SENSOR_PIN);

	sendMessageIfSensorActive();

	attachInterrupt(PIR_SENSOR_PIN, handlePirInterupt, RISING);
}

void messageRecievedCallback(char* topic, byte* payload, unsigned int length) {

	// handle message arrived

}

/*
	kontroluje stav pripojeni k wifi a dukud neni pripojeno, blika vestavenou diodou
	po pripojeni necha diodu svitit jako indikaci, ze je online
*/
void checkWiFiStatus() {
	while (WiFi.status() != WL_CONNECTED) {
		digitalWrite(BUILTIN_LED, HIGH);
		delay(50);
		digitalWrite(BUILTIN_LED, LOW);
		delay(50);
	}
}

/*
	pokud bude senzor vysilat logickou 1, posle zpravu do MQTT message queue
*/
void sendMessageIfSensorActive() {
	/*if (WiFi.status() != WL_CONNECTED) return;
	digitalWrite(DEBUG_LED_PIN, HIGH);
	delay(2000);
	digitalWrite(DEBUG_LED_PIN, LOW);*/


	Serial.println("");
	if (!mqttClient.connected()) {
		Serial.println("Connecting to mqtt server.");
		
		mqttClient.connect("client004", "app", "app");
		delay(100);
		if (mqttClient.connected()) {
			Serial.println("Mqtt client connected!");
		}
		else {
			Serial.println("Mqtt client connection failed!");
			Serial.println("");
			Serial.println(mqttClient.state());
		}
	}
	else {


		// pomocí funkcí readTemperature a readHumidity naèteme
		// do promìnných tep a vlh informace o teplotì a vlhkosti,
		// ètení trvá cca 250 ms
		float tep = mojeDHT.readTemperature();
		float vlh = mojeDHT.readHumidity();









		//String pubString = "{"report":{"light": "" + String(lightRead) + ""}}";
		//pubString.toCharArray(message_buff, pubString.length() + 1);
		////Serial.println(pubString); 
		//client.publish("io.m2m/arduino/lightsensor", message_buff);




		//char array[10];
		//sprintf(array, "%f", tep);


		//char array2[10];
		//sprintf(array2, "%f", vlh);

		//String stringOne = String(tep);
		//String stringTwo = String(vlh);

		//Serial.println(stringOne);
		//Serial.println(stringTwo);


		//char* asdf = string2char(stringOne);
		//char* asdf2 = string2char(stringTwo);

		//Serial.println(asdf);
		//Serial.println(asdf2);



		

		float sensedTemperature = tep;
		char charTemperature[20];
		dtostrf(sensedTemperature, 5, 2, charTemperature);

		float sensedHumidity = vlh;
		char charHumidity[20];
		dtostrf(sensedHumidity, 5, 2, charHumidity);



		Serial.println(charTemperature);
		Serial.println(charHumidity);
		
		mqttClient.publish("alza/p1/z1/s2317/teplota", charTemperature);
		mqttClient.publish("alza/p1/z1/s2317/vlhkost", charHumidity);
		
		
		
		//mqttClient.publish("test/topic", "test message pico! - 2", false);
		/*mqttClient.publish_P("mqtt.topic", "test message pico! - 3", true);
		mqttClient.publish_P("mqtt.topic", "test message pico! - 4", false);
*/

		Serial.println("Message sent!");
	}
}

/*
	precte hodnotu z HW micro-prepinace a nastavi nazev zarizeni do DeviceFullName
*/
void setupDeviceFullName() {
	////probudime registr
	//digitalWrite(REG_LATCH_PIN, 1);
	//delayMicroseconds(20);
	//digitalWrite(REG_LATCH_PIN, 0);

	////precteme 1 byjt z registru
	//uint8_t deviceCode = (uint8_t)readByteFromShiftRegister();
	//int num = (int)deviceCode;

	//spojime prefix a kod z registru do nazvu zarizeni
	FullDeviceName = DeviceNamePrefix;
}

/*
	roblika diodu na desce podle predanych prametru
*/
void blinkOnBoradLed(int highTime, int lowTime, int cycles) {
	digitalWrite(BUILTIN_LED, HIGH);//vypneme led

	for (int i = 0; i < cycles; i++) {
		digitalWrite(BUILTIN_LED, LOW);
		delay(highTime);
		digitalWrite(BUILTIN_LED, HIGH);
		delay(lowTime);
	}
}

byte readByteFromShiftRegister() {
	int i;
	int temp = 0;
	int pinState;
	byte myDataIn = 0;

	//we will be holding the clock pin high 8 times (0,..,7) at the
	//end of each time through the for loop

	//at the begining of each loop when we set the clock low, it will
	//be doing the necessary low to high drop to cause the shift
	//register's DataPin to change state based on the value
	//of the next bit in its serial information flow.
	//The register transmits the information about the pins from pin 7 to pin 0
	//so that is why our function counts down
	for (i = 7; i >= 0; i--)
	{
		digitalWrite(REG_CLK_PIN, 0);
		delayMicroseconds(0.2);
		temp = digitalRead(REG_DATA_PIN);
		if (temp) {
			pinState = 1;
			//set the bit to 0 no matter what
			myDataIn = myDataIn | (1 << i);
		}
		else {
			//turn it off -- only necessary for debuging
			//print statement since myDataIn starts as 0
			pinState = 0;
		}

		//Debuging print statements
		//Serial.print(pinState);
		//Serial.print("     ");
		//Serial.println (dataIn, BIN);

		digitalWrite(REG_CLK_PIN, 1);

	}
	//debuging print statements whitespace
	//Serial.println();
	//Serial.println(myDataIn, BIN);
	return myDataIn;

}

/*
	inicializuje OTA - update Over The Air
*/
void initOTA() {
	ArduinoOTA.setHostname("Wemos D1 PIR");

	ArduinoOTA.onStart([]() {
		Serial.println("OTA Start");
	});
	ArduinoOTA.onEnd([]() {
		Serial.println("\nOTA End");
	});
	ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
		Serial.printf("OTA Progress: %u%%\r", (progress / (total / 100)));
	});
	ArduinoOTA.onError([](ota_error_t error) {
		Serial.printf("OTA Error[%u]: ", error);
		if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
		else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
		else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
		else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
		else if (error == OTA_END_ERROR) Serial.println("End Failed");
	});
	ArduinoOTA.begin();
}









char* string2char(String command) {
	if (command.length() != 0) {
		char *p = const_cast<char*>(command.c_str());
		return p;
	}
}