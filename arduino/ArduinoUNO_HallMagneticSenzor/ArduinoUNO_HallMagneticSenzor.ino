int led = 13;//LED pin
int sensor = 3; //sensor pin
int val; //numeric variable

void setup()
{
	Serial.begin(9600);
	pinMode(led, OUTPUT); //set LED pin as output
	pinMode(sensor, INPUT); //set sensor pin as input
}

void loop()
{
	val = digitalRead(sensor); //Read the sensor
	Serial.print(val);

	if (val == HIGH) //when magnetic field is detected, turn led on
	{
		digitalWrite(led, HIGH);
	}
	else
	{
		digitalWrite(led, LOW);
	}
	delay(1000);
}