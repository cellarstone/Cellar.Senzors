


#include <Keypad.h>



int cervenaPin = 12;
int zelenaPin = 11;
int modraPin = 10;
const byte numRows = 4; //number of rows on the keypad
const byte numCols = 4; //number of columns on the keypad


char keymap[numRows][numCols] =
{
	{ '1', '2', '3', 'A' },
	{ '4', '5', '6', 'B' },
	{ '7', '8', '9', 'C' },
	{ '*', '0', '#', 'D' }
};


//Code that shows the the keypad connections to the arduino terminals
byte rowPins[numRows] = { 9,8,7,6 }; //Rows 0 to 3
byte colPins[numCols] = { 5,4,3,2 }; //Columns 0 to 3


//initializes an instance of the Keypad class
Keypad myKeypad = Keypad(makeKeymap(keymap), rowPins, colPins, numRows, numCols);




void setup()
{
	Serial.begin(9600);


	pinMode(cervenaPin, OUTPUT);
	pinMode(zelenaPin, OUTPUT);
	pinMode(modraPin, OUTPUT);
}

void loop()
{
	//setColor(255, 0, 0);  //èervená
	//delay(1000);
	//setColor(0, 255, 0);  //zelená
	//delay(1000);
	//setColor(0, 0, 255);  //modrá
	//delay(1000);
	//setColor(255, 255, 0);  //žlutá
	//delay(1000);
	//setColor(80, 0, 80);  //fialová
	//delay(1000);
	//setColor(0, 255, 255);  //tyrkysová
	//delay(1000);


	char keypressed = myKeypad.getKey();
	if (keypressed != NO_KEY)
	{
		Serial.print(keypressed);


		if (keypressed == '1')
		{
			setColor(255, 255, 255);  //bila
		}
		else if (keypressed == '2')
		{
			setColor(0, 128, 128);  //teal
		}
		else if (keypressed == '3')
		{
			setColor(128, 128, 0);  //olive
		}
		else if (keypressed == '4')
		{
			setColor(0, 0, 128);  //navy
		}
		else if (keypressed == '5')
		{
			setColor(184, 134, 11);  //dark gold
		}
		else if (keypressed == '6')
		{
			setColor(124, 252, 0);  //lawn green
		}
		else if (keypressed == '7')
		{
			setColor(139, 0, 139);  //dark magneta
		}
		else if (keypressed == '8')
		{
			setColor(139, 69, 19);  //saddle brown
		}
		else if (keypressed == '9')
		{
			setColor(0, 0, 0);  //zelená
		}



	}

}

void setColor(int cervena, int zelena, int modra)
{
	analogWrite(cervenaPin, cervena);
	analogWrite(zelenaPin, zelena);
	analogWrite(modraPin, modra);
}