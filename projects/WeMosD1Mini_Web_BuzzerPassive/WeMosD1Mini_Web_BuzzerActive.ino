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

// Ma bzucet
bool sensorIsOn = false;
int sensorPin = D8; // relay turns trigger signal - active high;


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

	pinMode(sensorPin, OUTPUT); // set pin 8 as output
	//digitalWrite(sensorPin, LOW);





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
}



/*******************************************************/
/*******************************************************/
/*******************************************************/
/*		LOOP		*/
/*******************************************************/
/*******************************************************/
/*******************************************************/

void loop() {
	// Pokud ma promenna svetlo hodnotu pravda, posli na pin D1 napeti a rozsvit LED diodu
	if (sensorIsOn == true)
	{
		//Serial.println(svetlo);
		//digitalWrite(sensorPin, HIGH);
		analogWrite(sensorPin, 800);
	}
	else
	{
		//Serial.println(svetlo);
		//digitalWrite(sensorPin, LOW);
		analogWrite(sensorPin, 0);
	}

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

	// Pokud je v radku vyraz ZapnoutSvetlo, nastav promennou svetlo na pravdu
	if (request.indexOf("On") > -1) sensorIsOn = true;
	// Pokud je v radku vyraz VypnoutSvetlo, nastav promennou svetlo na nepravdu
	if (request.indexOf("Off") > -1) sensorIsOn = false;

	// Posli pripojenemu klientu HTML kod stranky s tlacitky
	client.println("HTTP/1.1 200 OK");
	client.println("Content-Type: text/html");
	client.println("");
	client.println("<!DOCTYPE html>");
	client.println("<html><body>");
	client.print("<h1>WM005 - Bzucak</h2>");
	// Tlacitko pro zapnuti lampicky vede na adresu /ZapnoutSvetlo
	client.println("<a href=\"/On\"><button>Zapni bzuèení</button></a>");
	// Tlacitko pro vypnuti lampicky vede na adresu /VypnoutSvetlo
	client.println("<a href=\"/Off\"><button>Vypni bzuèení</button></a><br />");
	client.println("</body></html>");

	// To je vse, ted jeste 1 ms pockam a cela smycka se muze zopakovat
	delay(1);
	Serial.println("Klient odpojen");
	Serial.println("");
}