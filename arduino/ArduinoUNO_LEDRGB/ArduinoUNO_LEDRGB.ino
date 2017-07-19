int cervenaPin = 11;
int zelenaPin = 10;
int modraPin = 9;

void setup()
{
	pinMode(cervenaPin, OUTPUT);
	pinMode(zelenaPin, OUTPUT);
	pinMode(modraPin, OUTPUT);
}

void loop()
{
	setColor(255, 0, 0);  //�erven�
	delay(1000);
	setColor(0, 255, 0);  //zelen�
	delay(1000);
	setColor(0, 0, 255);  //modr�
	delay(1000);
	setColor(255, 255, 0);  //�lut�
	delay(1000);
	setColor(80, 0, 80);  //fialov�
	delay(1000);
	setColor(0, 255, 255);  //tyrkysov�
	delay(1000);
}

void setColor(int cervena, int zelena, int modra)
{
	analogWrite(cervenaPin, cervena);
	analogWrite(zelenaPin, zelena);
	analogWrite(modraPin, modra);
}