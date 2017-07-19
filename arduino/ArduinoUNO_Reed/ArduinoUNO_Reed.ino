// Arduino pin numbers
//D2 and A0 used on board and connected to D0 and A0 on the module
//G ground
//+ 5V
//open arduino console - upload the code and watch the result

const int digital = 2;
const int analog = 0;

void setup()
{
	pinMode(digital, INPUT);
	pinMode(analog, INPUT);
	Serial.begin(9600);
}

void loop()
{
	Serial.print(digitalRead(digital));
	Serial.print("-");
	Serial.println(analogRead(analog));
	delay(250);
}