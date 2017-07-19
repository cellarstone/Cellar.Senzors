/*
 Name:		WeMosD1Mini_LED.ino
 Created:	3/5/2017 4:43:17 PM
 Author:	lukas
*/

void setup() {
	pinMode(LED_BUILTIN, OUTPUT);     // Initialize the LED_BUILTIN pin as an output
}

// the loop function runs over and over again forever
void loop() {
	digitalWrite(LED_BUILTIN, LOW);   // Turn the LED on (Note that LOW is the voltage level
									  // but actually the LED is on; this is because 
									  // it is acive low on the ESP-01)
	delay(1000);                      // Wait for a second
	digitalWrite(LED_BUILTIN, HIGH);  // Turn the LED off by making the voltage HIGH
	delay(1000);                      // Wait for two seconds (to demonstrate the active low LED)
}