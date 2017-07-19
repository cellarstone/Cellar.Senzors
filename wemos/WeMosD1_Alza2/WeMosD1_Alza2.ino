/*
* PIR sensor tester
*/


#include <ESP8266WiFi.h>
#include <PubSubClient.h>
//#include <Bounce2.h>
#include <EEPROM.h>

const char* ssid = "CellarstoneDevicesWifi";
const char* password = "Cllrs123IoT456";
const char* mqtt_server = "192.168.1.227";

WiFiClient espClient;
PubSubClient client(espClient);




int ledPin = LED_BUILTIN;                // choose the pin for the LED
int pirPin = D5;               // choose the input pin (for PIR sensor)
int pirState = LOW;             // we start, assuming no motion detected
int val = 0;                    // variable for reading the pin status








void setup_wifi() {

	delay(10);
	// We start by connecting to a WiFi network
	Serial.println();
	Serial.print("Connecting to ");
	Serial.println(ssid);

	WiFi.begin(ssid, password);

	while (WiFi.status() != WL_CONNECTED) {
		/*extButton();
		for (int i = 0; i<500; i++) {
		extButton();
		delay(1);
		}*/
		Serial.print(".");
	}
	/*digitalWrite(ledPin, LOW);
	delay(500);
	digitalWrite(ledPin, HIGH);
	delay(500);
	digitalWrite(ledPin, LOW);
	delay(500);
	digitalWrite(ledPin, HIGH);*/
	Serial.println("");
	Serial.println("WiFi connected");
	Serial.println("IP address: ");
	Serial.println(WiFi.localIP());
}


void callback(char* topic, byte* payload, unsigned int length) {
	Serial.print("Message arrived [");
	Serial.print(topic);
	Serial.print("] ");
	for (int i = 0; i < length; i++) {
		Serial.print((char)payload[i]);
	}
	Serial.println();

	
}

void reconnect() {
	// Loop until we're reconnected
	while (!client.connected()) {
		Serial.print("Attempting MQTT connection...");
		// Attempt to connect
		if (client.connect("WeMos 654984 Client", "app", "app")) {
			Serial.println("connected");
			// Once connected, publish an announcement...
			client.publish("mqtt.topic", "Ahoj z wemose");
			//// ... and resubscribe
			//client.subscribe(inTopic);
		}
		else {
			Serial.print("failed, rc=");
			Serial.print(client.state());
			Serial.println(" try again in 5 seconds");
			// Wait 5 seconds before retrying
			/*for (int i = 0; i<5000; i++) {
			extButton();
			delay(1);
			}*/
		}
	}
}



void setup() {
	EEPROM.begin(512);              // Begin eeprom to store on/off state

	pinMode(ledPin, OUTPUT);      // declare LED as output
	pinMode(pirPin, INPUT);     // declare sensor as input


	pirState = EEPROM.read(0);
	digitalWrite(pirPin, pirState);



	Serial.begin(9600);



	setup_wifi();                   // Connect to wifi 
	client.setServer(mqtt_server, 1883);
	client.setCallback(callback);
}

void loop() {
	val = digitalRead(pirPin);  // read input value.


	Serial.println(val);

	if (val == HIGH) {            // check if the input is HIGH
		digitalWrite(ledPin, HIGH);  // turn LED ON
		if (pirState == LOW) {
			// we have just turned on
			Serial.println("Motion detected!");
			// We only want to print on the output change, not state
			pirState = HIGH;
		}
	}
	else {
		digitalWrite(ledPin, LOW); // turn LED OFF
		if (pirState == HIGH) {
			// we have just turned of
			Serial.println("Motion ended!");
			// We only want to print on the output change, not state
			pirState = LOW;
		}
	}


	if (!client.connected()) {
		reconnect();
	}
	client.loop();
}