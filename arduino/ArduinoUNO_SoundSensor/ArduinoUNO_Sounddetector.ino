
int volume;
int soundDetectedPin = 10;

void setup() {
	Serial.begin(9600); // Serial port begin
	pinMode(soundDetectedPin, INPUT);
}

void loop() {

	volume = digitalRead(soundDetectedPin); // Reads the value from the Analog PIN A0

							 //Serial print level

	Serial.println(volume);
	

}
