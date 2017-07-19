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
	// Spo��t� vzd�lenost 
	float distance = pulseIn(echoPin, HIGH);
	distance = distance*0.017315f;

	// ode�le informace na s�riv� port 
	Serial.print(distance);
	Serial.print("cm\n");
	//po�k� 1 sekundu 
	delay(1000);

}