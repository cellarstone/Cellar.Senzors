
const int inputPin = 7;

void setup() {

	Serial.begin(9600); // Serial port begin
	pinMode(inputPin, INPUT);

}

void loop() {
	int value = digitalRead(inputPin);

	Serial.println(value);
}