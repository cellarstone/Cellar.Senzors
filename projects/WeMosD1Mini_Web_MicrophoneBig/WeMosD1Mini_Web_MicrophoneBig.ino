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




/*******************************************************/
/*		WIFI		*/
/*******************************************************/

// Nazev Wi-Fi site, do ktere se mam pripojit

const char* ssid = "CellarstoneDevicesWifi";

// Heslo Wi-Fi site, do ktere se mam pripojit
const char* password = "Cllrs123IoT456";

// Webovy server pobezi na portu 80 (HTTP komunikace)
WiFiServer server(80);


/*******************************************************/
/*		Senzor		*/
/*******************************************************/

int sensorPin = A0; // select the input pin for the potentiometer
int sensorValue = 0; // variable to store the value coming from the sensor



/*******************************************************/
/*******************************************************/
/*******************************************************/
/*		SETUP		*/
/*******************************************************/
/*******************************************************/
/*******************************************************/


void setup()
{
	// Nastartuj seriovou linku, do ktere budu vypisovat provozni informace
	Serial.begin(9600);


	/*	----------------	WIFI	----------------	*/

	// Pripojeni k Wi-Fi
	Serial.println();
	Serial.print("Pripojuji k ");
	Serial.println(ssid);
	WiFi.begin(ssid, password);

	// Dokud nejsem pripojeny k Wi-Fi,zapisuj do seriove linky tecky progressbaru
	while (WiFi.status() != WL_CONNECTED) {
		delay(100);
		Serial.print(".");
	}

	// Jsem pripojeny k Wi-Fi a mohu pokracovat
	Serial.println();
	Serial.println("WiFi pripojena!");

	// Spusteni serveru
	server.begin();
	Serial.println("Server spusten");

	// Napis IP adresu, kterou mikropocitac dostal
	Serial.print("Pouzij k pripojeni tuto adresu: ");
	Serial.print("http://");
	Serial.println(WiFi.localIP());



	/*	----------------	SENZOR	----------------	*/

	pinMode(sensorPin, INPUT);


}

/*******************************************************/
/*******************************************************/
/*******************************************************/
/*		LOOP		*/
/*******************************************************/
/*******************************************************/
/*******************************************************/

void loop()
{

	/*	----------------	SENZOR	----------------	*/

	sensorValue = analogRead(sensorPin);
	
	Serial.println(sensorValue);


	/*	----------------	WIFI	----------------	*/

	// Cekej, dokud se nepripoji nejaky klient
	WiFiClient client = server.available();
	if (!client) {
		return;
	}

	// Cekej, dokud klient neposle nejaka data
	Serial.println("Novy klient!");
	while (!client.available()) {
		delay(1);
	}

	// Precti a vypis prvni radek od klienta s URL dotazem
	String request = client.readStringUntil('\r');
	Serial.println(request);
	client.flush();

	
	// Posli pripojenemu klientu HTML kod stranky s tlacitky
	client.println("HTTP/1.1 200 OK");
	client.println("Content-Type: text/html");
	client.println("");
	client.println("<!DOCTYPE html>");
	client.println("<html><body>");
	client.print("<h1>WM007 - Microphone</h2>");

	// TEXT --------------------------------------------
	client.print("<div>");
	client.print(sensorValue);
	client.print("</div>");
	// -------------------------------------------------

	client.println("</body></html>");

	// To je vse, ted jeste 1 ms pockam a cela smycka se muze zopakovat
	delay(1);
	Serial.println("Klient odpojen");
	Serial.println("");
}