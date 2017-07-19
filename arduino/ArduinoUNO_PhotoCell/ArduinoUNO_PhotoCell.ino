// This code is for the Photo resistor Module.

int sensorPin = A0;                      // Selecting A0 as the sensor input pin.

int ledPin = 13;                          // Selecting pin 13 as the LED pin

int sensorValue = 0;                     // variable to store the value coming from the sensor







void setup() {                          // code written in it will only run once.

	pinMode(ledPin, OUTPUT);          // Declaring pin 13 as output pin

	Serial.begin(9600);                // Setting baud rate at 9600

}





void loop() {                         // Code written in it will run repeatedly

	sensorValue = analogRead(sensorPin);          // Reading data from pin A0 and storing in Sensor value

	if (sensorValue < 500)

	{

		digitalWrite(ledPin, HIGH);                            // This will set the pin 13 as high

		Serial.println(" The value of LDR is = ");              // This will print " The value of LDR is = " on display

		Serial.println(sensorValue);                             // This will print the value of LDR on display

		delay(1000);                                            // This will give delay on 1 sec

	}

	else

	{

		digitalWrite(ledPin, LOW);                                    // This will set the pin 13 as low

		Serial.println(" The value of LDR is = ");

		Serial.println(sensorValue);                                    // This will print the sensor value on display

		delay(1000);                                                                   // This will give delay of 1 sec

	}







}