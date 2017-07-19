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
#include <math.h>

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

double Thermistor(int RawADC) {
	double Temp;
	Temp = log(10000.0*((1024.0 / RawADC - 1)));
	Temp = 1 / (0.001129148 + (0.000234125 + (0.0000000876741 * Temp * Temp))* Temp);
	Temp = Temp - 273.15;            // Convert Kelvin to Celcius
									 //Temp = (Temp * 9.0)/ 5.0 + 32.0; // Convert Celcius to Fahrenheit
	return Temp;
}




/*******************************************************/
/*******************************************************/
/*******************************************************/
/*		SETUP		*/
/*******************************************************/
/*******************************************************/
/*******************************************************/
void setup() {
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
	//nothing
}


/*******************************************************/
/*******************************************************/
/*******************************************************/
/*		LOOP		*/
/*******************************************************/
/*******************************************************/
/*******************************************************/

void loop() {
	int readVal = analogRead(A0);
	double temp = Thermistor(readVal);

	//Serial.println(readVal);
	Serial.println(temp);  // display tempature
						   //Serial.println(readVal);  // display tempature





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
	client.print("<h1>WM006 - Temperature</h2>");

	// TEXT --------------------------------------------
	client.print("<div>");
	client.print(temp);
	client.print("</div>");
	// -------------------------------------------------

	client.println("</body></html>");

	// To je vse, ted jeste 1 ms pockam a cela smycka se muze zopakovat
	delay(1);
	Serial.println("Klient odpojen");
	Serial.println("");
}