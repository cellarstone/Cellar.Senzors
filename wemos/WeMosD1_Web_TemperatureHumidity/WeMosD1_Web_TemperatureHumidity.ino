// Teplom�r a vlhkom�r DHT11/22






// p�ipojen� knihovny DHT
#include "DHT.h"

//Wifi
#include <ESP8266WiFi.h>
#include <WiFiServer.h>
#include <WiFiClientSecure.h>
#include <WiFiClient.h>
#include <ESP8266WiFiType.h>
#include <ESP8266WiFiSTA.h>
#include <ESP8266WiFiScan.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266WiFiGeneric.h>
#include <ESP8266WiFiAP.h>
#include <WiFiUdp.h>



// nastaven� ��sla pinu s p�ipojen�m DHT senzorem
#define pinDHT D6

// odkomentov�n� spr�vn�ho typu �idla
#define typDHT11 DHT11     // DHT 11
//#define typDHT22 DHT22   // DHT 22 (AM2302)








// Nazev Wi-Fi site, do ktere se mam pripojit
const char* ssid = "CellarstoneDevicesWifi";

// Heslo Wi-Fi site, do ktere se mam pripojit
const char* password = "Cllrs123IoT456";




// Webovy server pobezi na portu 80 (HTTP komunikace)
WiFiServer server(80);




// inicializace DHT senzoru s nastaven�m pinem a typem senzoru
DHT mojeDHT(pinDHT, typDHT11);








void setup() {
	// komunikace p�es s�riovou linku rychlost� 9600 baud
	Serial.begin(9600);
	// zapnut� komunikace s teplom�rem DHT
	mojeDHT.begin();




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







void loop() {
	// pomoc� funkc� readTemperature a readHumidity na�teme
	// do prom�nn�ch tep a vlh informace o teplot� a vlhkosti,
	// �ten� trv� cca 250 ms
	float tep = mojeDHT.readTemperature();
	float vlh = mojeDHT.readHumidity();



	// kontrola, jestli jsou na�ten� hodnoty ��sla pomoc� funkce isnan
	if (isnan(tep) || isnan(vlh)) {
		// p�i chybn�m �ten� vypi� hl�ku
		Serial.println("Chyba p�i �ten� z DHT senzoru!");
	}
	else {
		// pokud jsou hodnoty v po��dku,
		// vypi� je po s�riov� lince
		Serial.print("Teplota: ");
		Serial.print(tep);
		Serial.print(" stupnu Celsia, ");
		Serial.print("vlhkost: ");
		Serial.print(vlh);
		Serial.println("  %");
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




	// Posli pripojenemu klientu HTML kod stranky s tlacitky
	client.println("HTTP/1.1 200 OK");
	client.println("Content-Type: text/html");
	client.println("");
	client.println("<!DOCTYPE html>");
	client.println("<html><body>");
	client.print("<h1>WM004 - Senzor Teploty</h2>");







	// TEXT --------------------------------------------

	// pokud jsou hodnoty v po��dku,
	// vypi� je po s�riov� lince
	client.print("<div>");
	client.print("Teplota: ");
	client.print(tep);
	client.print(" stupnu Celsia ");
	client.print("</div>");
	client.print("<div>");
	client.print("vlhkost: ");
	client.print(vlh);
	client.println("  %");
	client.print("</div>");
	// -------------------------------------------------


	client.println("</body></html>");

	// To je vse, ted jeste 1 ms pockam a cela smycka se muze zopakovat
	delay(1);
	Serial.println("Klient odpojen");
	Serial.println("");









	// pauza pro p�ehledn�j�� v�pis
	//delay(2000);
}