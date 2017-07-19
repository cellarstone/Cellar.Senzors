﻿int Led = 13; // define LED pin
int buttonpin = 3; // define photo interrupter signal pin
int val; //define a numeric variable

void setup()
{
	Serial.begin(9600);
	pinMode(Led, OUTPUT); // LED pin as output
	pinMode(buttonpin, INPUT); //photo interrupter pin as input
}

void loop()
{
	val = digitalRead(buttonpin); //read the value of the sensor 
	Serial.print(val);

	if (val == HIGH) // turn on LED when sensor is blocked 
	{
		digitalWrite(Led, HIGH);
	}
	else
	{
		digitalWrite(Led, LOW);
	}
}