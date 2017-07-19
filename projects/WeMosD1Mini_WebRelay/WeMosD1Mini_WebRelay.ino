#include <ESP8266WiFi.h>

// Nazev Wi-Fi site, do ktere se mam pripojit
const char* ssid = "CellarstoneDevicesWifi";

// Heslo Wi-Fi site, do ktere se mam pripojit
const char* password = "Cllrs123IoT456";

// Ma svitit lampicka?
bool svetlo = false;

// Webovy server pobezi na portu 80 (HTTP komunikace)
WiFiServer server(80);

int relay = D7; // relay turns trigger signal - active high;

void setup() {
	// Nastartuj seriovou linku, do ktere budu vypisovat provozni informace
	Serial.begin(9600);

	// Nastav pin D1 na zapis a nastav nizkou hodnotu (LED dioda nesviti)
	pinMode(relay, OUTPUT);
	digitalWrite(relay, LOW);

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
	// Pokud ma promenna svetlo hodnotu pravda, posli na pin D1 napeti a rozsvit LED diodu
	if (svetlo == true)
	{
		//Serial.println(svetlo);
		digitalWrite(relay, HIGH);
	}
	else
	{
		//Serial.println(svetlo);
		digitalWrite(relay, LOW);
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
	if (request.indexOf("ZapnoutSvetlo") > -1) svetlo = true;
	// Pokud je v radku vyraz VypnoutSvetlo, nastav promennou svetlo na nepravdu
	if (request.indexOf("VypnoutSvetlo") > -1) svetlo = false;

	// Posli pripojenemu klientu HTML kod stranky s tlacitky
	client.println("HTTP/1.1 200 OK");
	client.println("Content-Type: text/html");
	client.println("");
	client.println("<!DOCTYPE html>");
	client.println("<html><body>");
	client.print("<h1>WM003 - Lampicka</h2>");
	// Tlacitko pro zapnuti lampicky vede na adresu /ZapnoutSvetlo
	client.println("<a href=\"/ZapnoutSvetlo\"><button>Zapni svetlo</button></a>");
	// Tlacitko pro vypnuti lampicky vede na adresu /VypnoutSvetlo
	client.println("<a href=\"/VypnoutSvetlo\"><button>Vypni svetlo</button></a><br />");
	client.println("</body></html>");

	// To je vse, ted jeste 1 ms pockam a cela smycka se muze zopakovat
	delay(1);
	Serial.println("Klient odpojen");
	Serial.println("");
} 