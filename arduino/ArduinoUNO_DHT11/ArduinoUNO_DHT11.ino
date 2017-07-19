/*
 Name:		ArduinoUNO_DHT11.ino
 Created:	3/5/2017 8:35:11 PM
 Author:	lukas
*/

// Teplom�r a vlhkom�r DHT11/22

// p�ipojen� knihovny DHT
#include <DHT-sensor-library-master\DHT.h>
// nastaven� ��sla pinu s p�ipojen�m DHT senzorem
#define pinDHT D5
// odkomentov�n� spr�vn�ho typu �idla
#define typDHT11 DHT11     // DHT 11
//#define typDHT22 DHT22   // DHT 22 (AM2302)

// inicializace DHT senzoru s nastaven�m pinem a typem senzoru
DHT mojeDHT(pinDHT, typDHT11);

void setup() {
	// komunikace p�es s�riovou linku rychlost� 9600 baud
	Serial.begin(9600);
	// zapnut� komunikace s teplom�rem DHT
	mojeDHT.begin();
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
	// pauza pro p�ehledn�j�� v�pis
	delay(2000);
}
