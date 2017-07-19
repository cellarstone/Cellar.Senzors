/*
 Name:		ArduinoUNO_MQ2SmokeDetector.ino
 Created:	3/6/2017 4:55:49 PM
 Author:	lukas
*/

int smokeA0 = A5;

// the setup function runs once when you press reset or power the board
void setup() {
	pinMode(smokeA0, INPUT);
	Serial.begin(9600);
}

// the loop function runs over and over again until power down or reset
void loop() {
	int analogSensor = analogRead(smokeA0);
	Serial.print("Pin A0: ");
	Serial.println(analogSensor);
	delay(100);
}
