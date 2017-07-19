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
	setColor(255, 0, 0);  //èervená
	delay(1000);
	setColor(0, 255, 0);  //zelená
	delay(1000);
	setColor(0, 0, 255);  //modrá
	delay(1000);
	setColor(255, 255, 0);  //žlutá
	delay(1000);
	setColor(80, 0, 80);  //fialová
	delay(1000);
	setColor(0, 255, 255);  //tyrkysová
	delay(1000);
}

void setColor(int cervena, int zelena, int modra)
{
	analogWrite(cervenaPin, cervena);
	analogWrite(zelenaPin, zelena);
	analogWrite(modraPin, modra);
}