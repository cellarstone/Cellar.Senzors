#include <ESP8266WiFi.h>




// Nazev Wi-Fi site, do ktere se mam pripojit
const char* ssid = "CellarstoneDevicesWifi";

// Heslo Wi-Fi site, do ktere se mam pripojit
const char* password = "Cllrs123IoT456";




// Webovy server pobezi na portu 80 (HTTP komunikace)
WiFiServer server(80);

int sensorPin = A0; // relay turns trigger signal - active high;
int sensorValue = 0;


void setup() {
	// Nastartuj seriovou linku, do ktere budu vypisovat provozni informace
	Serial.begin(9600);



	// Nastav pin D1 na zapis a nastav nizkou hodnotu (LED dioda nesviti)
	pinMode(sensorPin, INPUT);




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

// Smycka loop se opakuje stale dokola 
void loop() {

	sensorValue = analogRead(sensorPin);
	Serial.print("Analog reading = ");
	Serial.print(sensorValue);     // the raw analog reading
	String text = "";

	// We'll have a few threshholds, qualitatively determined
	if (sensorValue <= 0) {
		text = "???";
		Serial.println(" - " + text);
	}
	else if (sensorValue > 0 && sensorValue <= 100) {
		text = "DENNI SVETLO";
		Serial.println(" - " + text);
	}
	else if (sensorValue > 100 && sensorValue <= 500) {
		text = "SVETLO";
		Serial.println(" - " + text);
	}
	else if (sensorValue > 500 && sensorValue <= 800) {
		text = "STMIVANI";
		Serial.println(" - " + text);
	}
	else if(sensorValue > 800) {
		text = "TMA";
		Serial.println(" - " + text);
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
	client.print("<h1>Senzor intenzity svetla</h2>");
	
	





	// TEXT --------------------------------------------
	client.print("<div>");
	client.print(sensorValue);
	client.print("</div>");
	client.println("<div>" + text + "</div>");
	// -------------------------------------------------


	client.println("</body></html>");

	// To je vse, ted jeste 1 ms pockam a cela smycka se muze zopakovat
	delay(1);
	Serial.println("Klient odpojen");
	Serial.println("");
}





