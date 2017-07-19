#define trigPin 13
#define echoPin 12


void setup() {
	Serial.begin(9600);
	pinMode(trigPin, OUTPUT);
	pinMode(echoPin, INPUT);
}

void loop() {
	digitalWrite(trigPin, LOW);
	delayMicroseconds(2);
	digitalWrite(trigPin, HIGH);
	delayMicroseconds(10);
	digitalWrite(trigPin, LOW);
	// Spoèítá vzdálenost 
	float distance = pulseIn(echoPin, HIGH);
	distance = distance*0.017315f;

	// odešle informace na sérivý port 
	Serial.print(distance);
	Serial.print("cm\n");
	//poèká 1 sekundu 
	delay(1000);

}