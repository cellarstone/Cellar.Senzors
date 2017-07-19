/*
 Name:		ArduinoUNO_DHT11.ino
 Created:	3/5/2017 8:35:11 PM
 Author:	lukas
*/

// Teplomìr a vlhkomìr DHT11/22

// pøipojení knihovny DHT
#include <DHT-sensor-library-master\DHT.h>
// nastavení èísla pinu s pøipojeným DHT senzorem
#define pinDHT D5
// odkomentování správného typu èidla
#define typDHT11 DHT11     // DHT 11
//#define typDHT22 DHT22   // DHT 22 (AM2302)

// inicializace DHT senzoru s nastaveným pinem a typem senzoru
DHT mojeDHT(pinDHT, typDHT11);

void setup() {
	// komunikace pøes sériovou linku rychlostí 9600 baud
	Serial.begin(9600);
	// zapnutí komunikace s teplomìrem DHT
	mojeDHT.begin();
}

void loop() {
	// pomocí funkcí readTemperature a readHumidity naèteme
	// do promìnných tep a vlh informace o teplotì a vlhkosti,
	// ètení trvá cca 250 ms
	float tep = mojeDHT.readTemperature();
	float vlh = mojeDHT.readHumidity();
	// kontrola, jestli jsou naètené hodnoty èísla pomocí funkce isnan
	if (isnan(tep) || isnan(vlh)) {
		// pøi chybném ètení vypiš hlášku
		Serial.println("Chyba pøi ètení z DHT senzoru!");
	}
	else {
		// pokud jsou hodnoty v poøádku,
		// vypiš je po sériové lince
		Serial.print("Teplota: ");
		Serial.print(tep);
		Serial.print(" stupnu Celsia, ");
		Serial.print("vlhkost: ");
		Serial.print(vlh);
		Serial.println("  %");
	}
	// pauza pro pøehlednìjší výpis
	delay(2000);
}
