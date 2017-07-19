#include <boarddefs.h>
#include <IRremote.h>
#include <IRremoteInt.h>
#include <ir_Lego_PF_BitStreamEncoder.h>

#include <IRremote.h>
const unsigned long LG_POWER = 0x20DF10EF;
unsigned int irSignal[] = { 8950, -4400, 550, -550, 600, -500, 600, -1650, 550, -550, 600, -500, 600, -550, 600, -500, 600, -550, 550, -1650, 600, -1600, 600, -550, 550, -1650, 600, -1600, 600, -1600, 600, -1650, 600, -1600, 600, -500, 600, -550, 600, -500, 600, -1600, 600, -550, 600, -500, 600, -550, 600, -500, 600, -1600, 600, -1650, 600, -1600, 600, -500, 600, -1650, 550, -1650, 600, -1600, 600, -1600, 600 };

IRsend irsend;

void setup()
{
	Serial.begin(9600);
}

void loop()
{
	irsend.sendNEC(LG_POWER, 32);
	irsend.sendRaw(irSignal, sizeof(irSignal) / sizeof(irSignal[0]), 38);
	delay(100);
}