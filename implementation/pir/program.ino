#include <EEPROM.h>

#include <ESP8266WebServer.h>

#include <ESP8266WiFi.h>
#include <ESP8266WiFiAP.h>
#include <ESP8266WiFiGeneric.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266WiFiScan.h>
#include <ESP8266WiFiSTA.h>
#include <ESP8266WiFiType.h>
#include <WiFiClient.h>
#include <WiFiClientSecure.h>
#include <WiFiServer.h>
#include <WiFiUdp.h>

#include <PubSubClient.h>
#include <Timer.h>
/*
 Name:		WeMosD1Mini_PIR.ino
 Created:	3/5/2017 4:43:17 PM
 Author:	lukas
*/

//nadefinovani EEPROM adres a velikosti
int EE_WIFI_SSID_ADDR = 10;
int EE_WIFI_SSID_SIZE = 32;

int EE_WIFI_PASS_ADDR = 42;
int EE_WIFI_PASS_SIZE = 32;

int EE_DEVICE_NAME_ADDR = 74;
int EE_DEVICE_NAME_SIZE = 32;

int EE_MQTT_SERVER_ADDR = 106;
int EE_MQTT_SERVER_SIZE = 32;

int EE_MQTT_TOPIC_ADDR = 138;
int EE_MQTT_TOPIC_SIZE = 32;

int EE_MQTT_USER_NAME_ADDR = 170;
int EE_MQTT_USER_NAME_SIZE = 20;

int EE_MQTT_USER_PASS_ADDR = 190;
int EE_MQTT_USER_PASS_SIZE = 32;

int EE_MQTT_PORT_ADDR = 222;

int ledPin = LED_BUILTIN; // choose the pin for the LED
int inputPin = D5;		  // choose the input pin (for PIR sensor)
int pirState = LOW;		  // we start, assuming no motion detected
int val = 0;
int oldval = 1; // variable for reading the pin status

bool IS_AP_MODE = false;
long lastMillis = 0;

//nazev zarizeni - bude pouzito i pro pojmenovani zarizeni v siti a vychoziho access pointu
String DeviceName = "";
const char *DEFAULT_DEVICE_NAME = "s3106";
const char *DEFAULT_AP_PASS = "11235813";

//IP adresa MQTT serveru
String MqttHostName = "192.168.1.19"; //"192.168.1.173"

//Port MQTT serveru
int MqttPort; //1883
int DEFAULT_MQTT_PORT = 1883;

//uzivatel MQTT serveru
String MqttUserName = "test"; //test

//heslo MQTT uzivatele
String MqttUserPass = "test"; //test

//nazev MQTT fronty
String MqttTopicName_pir = DeviceName + "/pir";
String MqttTopicName_status = DeviceName + "/status";

//nazev wifi site
String WifiSsid = "CellarstoneDevicesWifi"; //CellarstoneDevicesWifi

//wifi heslo
String WifiPass = "Cllrs123IoT456"; //Cllrs123IoT456

//#define wifi_ssid "CellarstoneDevicesWifi"
//#define wifi_password "Cllrs123IoT456"

//#define mqtt_server "192.168.1.173"
// #define mqtt_user "test"
// #define mqtt_password "test"

//#define pir_topic "s3100/pir"

ESP8266WebServer webServer(80);
WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

// PROGMEM const char html_hlavicka[] = "<!DOCTYPE html><html lang=\"en\" xmlns=\"http://www.w3.org/1999/xhtml\"><head><meta charset=\"utf-8\" /><title>ESP8266 - Control panel</title><style type=\"text/css\">html,body{font-family:'Segoe UI';margin:0;padding:0;width:100%;height:100%;overflow:auto;background-color:black}.item{color:limegreen;font-weight:bold;text-align:right}.value{color:grey;font-weight:normal;text-align:left}</style></head><body><table style=\"margin: 100px auto;\"><tr><td class=\"item\"> IP Address:</td><td class=\"value\">{ip}</td></tr><tr><td class=\"item\"> Device name:</td><td class=\"value\"> PirSensor_001</td></tr><tr><td class=\"item\"> Firmware version:</td><td class=\"value\"> 1.0.1</td></tr><tr><td class=\"item\"> Uptime:</td><td class=\"value\">{uptime} ms</td></tr></table></body></html>";
// PROGMEM const char html_paticka[] = "</div></body>";

const char htmlHeader[] = "<!DOCTYPE html><html lang=\"en\" xmlns=\"http://www.w3.org/1999/xhtml\"><head> <meta charset=\"utf-8\"/> <meta name=\"viewport\" content=\"width=device-width\" /> <title>WeMos AP - Config</title> <style type=\"text/css\"> html, body { margin: 0px; padding: 0px; width: 100%; height: 100%; font-family: Segoe UI; background-color: #cccccc; background: -moz-linear-gradient(top, #eeeeee 0%, #cccccc 100%); background: -webkit-linear-gradient(top, #eeeeee 0%, #cccccc 100%); background: linear-gradient(to bottom, #eeeeee 0%, #cccccc 100%); filter: progid:DXImageTransform.Microsoft.gradient(startColorstr='#eeeeee', endColorstr='#cccccc', GradientType=0); background-repeat: no-repeat; overflow: hidden; } .header { position: absolute; left: 0; top: 0; right: 0; width: 100%; height: 65px; margin: 0px; padding: 0px; background: #eeeeee; background: -moz-linear-gradient(top, #eeeeee 0%, #cccccc 99%, #595959 100%); background: -webkit-linear-gradient(top, #eeeeee 0%, #cccccc 99%, #595959 100%); background: linear-gradient(to bottom, #eeeeee 0%, #cccccc 99%, #595959 100%); filter: progid:DXImageTransform.Microsoft.gradient(startColorstr='#eeeeee', endColorstr='#595959', GradientType=0); } .wifi-strength-0 { list-style-image: url(\"data:image/svg+xml;utf8,<svg height='24' viewBox='0 0 48 48' width='24' xmlns='http://www.w3.org/2000/svg'><path d='M24.02 42.98l23.26-28.98c-.9-.68-9.85-8-23.28-8-13.43 0-22.38 7.32-23.28 8l23.26 28.98.02.02.02-.02z' fill-opacity='.3'/><path d='M0 0h48v48h-48z' fill='none'/></svg>\"); } .wifi-strength-1 { list-style-image: url(\"data:image/svg+xml;utf8,<svg height='24' viewBox='0 0 48 48' width='24' xmlns='http://www.w3.org/2000/svg'><path d='M24.02 42.98l23.26-28.98c-.9-.68-9.85-8-23.28-8s-22.38 7.32-23.28 8l23.26 28.98.02.02.02-.02z' fill-opacity='.3'/><path d='M0 0h48v48h-48z' fill='none'/><path d='M13.34 29.72l10.65 13.27.01.01.01-.01 10.65-13.27c-.53-.41-4.6-3.72-10.66-3.72s-10.13 3.31-10.66 3.72z'/></svg>\"); } .wifi-strength-2 { list-style-image: url(\"data:image/svg+xml;utf8,<svg height='24' viewBox='0 0 48 48' width='24' xmlns='http://www.w3.org/2000/svg'><path d='M24.02 42.98l23.26-28.98c-.9-.68-9.85-8-23.28-8s-22.38 7.32-23.28 8l23.26 28.98.02.02.02-.02z' fill-opacity='.3'/><path d='M0 0h48v48h-48z' fill='none'/><path d='M9.58 25.03l14.41 17.95.01.02.01-.02 14.41-17.95c-.72-.56-6.22-5.03-14.42-5.03s-13.7 4.47-14.42 5.03z'/></svg>\"); } .wifi-strength-3 { list-style-image: url(\"data:image/svg+xml;utf8,<svg height='24' viewBox='0 0 48 48' width='24' xmlns='http://www.w3.org/2000/svg'><path d='M24.02 42.98l23.26-28.98c-.9-.68-9.85-8-23.28-8s-22.38 7.32-23.28 8l23.26 28.98.02.02.02-.02z' fill-opacity='.3'/><path d='M0 0h48v48h-48z' fill='none'/><path d='M7.07 21.91l16.92 21.07.01.02.02-.02 16.92-21.07c-.86-.66-7.32-5.91-16.94-5.91-9.63 0-16.08 5.25-16.93 5.91z'/></svg>\"); } .wifi-strength-4 { list-style-image: url(\"data:image/svg+xml;utf8,<svg height='24' viewBox='0 0 48 48' width='24' xmlns='http://www.w3.org/2000/svg'><path d='M24.02 42.98l23.26-28.98c-.9-.68-9.85-8-23.28-8s-22.38 7.32-23.28 8l23.26 28.98.02.02.02-.02z'/><path d='M0 0h48v48h-48z' fill='none'/></svg>\"); } .header-container { width: 350px; height: 65px; margin: 0 auto; } .logo { height: 60px; margin-top: 3px; width: 70px; float: left; } .title-container { width: 280px; } .title { width: 100%; font-size: 1.5em; font-weight: bold; padding-left: 4.4em; color: #888888; } .subtitle { width: 100%; font-size: 1.1em; padding-left: 6.5em; color: #888888; } .main-container { width: 350px; margin: 0 auto; } .controls { width: 350px; margin: 70px auto 5px auto; text-align: center; } .button { height: 35px; width: 110px; font-size: 1.1em; color: #595959; } .config-form { } .form-line { margin-top: 10px; } .form-label { float: left; width: 140px; text-align: right; font-size: 1.1em; color: #595959; } .form-value { margin-left: 145px; } .form-control { width: 190px; font-size: 1.1em; color: #595959; } </style></head>";
const char htmlConfigForm[] = "<body> <input type=\"hidden\" id=\"hdnIsApMode\" value=\"{IsApModeValue}\" /> <input type=\"hidden\" id=\"hdnDeviceIp\" value=\"{DeviceIpValue}\" /><div class=\"header\"> <div class=\"header-container\"> <div class=\"logo\"> <svg version=\"1.1\" xmlns=\"http://www.w3.org/2000/svg\" xmlns:xlink=\"http://www.w3.org/1999/xlink\" x=\"0px\" y=\"0px\" viewBox=\"0 0 107.7 110.6\" style=\"enable-background: new 0 0 107.7 110.6; height: 58px; width: 70px;\" xml:space=\"preserve\"> <style type=\"text/css\"> .st0 { display: none; } .st1 { fill: #2B66B1; } .st2 { fill: #478FCC; } </style> <g id=\"Layer_1\" class=\"st0\"></g> <g id=\"Layer_2\"> <g> <path class=\"st1\" d=\"M52.3,110.5h-41c-7.6,0-11.7-3.6-11.7-10.1V10.1C-0.3,3.6,3.8,0,11.4,0h41c7.7,0,12,3.6,12,10.1V36H44.7V14.2 H19.3v82.1h25.5V73h19.6v27.3C64.4,106.9,60.1,110.5,52.3,110.5z\"/> </g> <g> <path class=\"st2\" d=\"M95.9,110.6H56.5c-7.5,0-11.8-3.6-11.8-10.1V73h19.5v23.6h24V75L49,43.6c-3.3-2.5-4.3-4.6-4.3-8.8V10.2 c0-6.5,4.3-10.1,11.8-10.1h39.4c7.5,0,11.8,3.6,11.8,10v24.7H88.2V14h-24v20l39.2,31.2c3.3,2.6,4.3,4.8,4.3,9v26.3 C107.7,106.9,103.4,110.6,95.9,110.6z\"/> </g> </g> </svg> </div> <div class=\"title-container\"> <div class=\"title\"> C e l l a r s t o n e </div> <div class=\"subtitle\"> ESP8266 configuration </div> </div> </div></div><div class=\"main-container\"> <div class=\"controls\"> <button class=\"button\" onclick=\"saveValues();\">Save</button> <button class=\"button\" onclick=\"scanWifi();\">Scan</button> <button class=\"button\" onclick=\"reset();\">Reset</button> </div> <div class=\"config-form\"> <div class=\"form-line\"> <div class=\"form-label\"> <label for=\"txtWifiSsid\">WiFi SSID:</label> </div> <div class=\"form-value\"> <input type=\"text\" class=\"form-control\" id=\"txtWifiSsid\" value=\"{WifiSsidValue}\"/> </div> </div> <div class=\"form-line\"> <div class=\"form-label\"> <label for=\"txtWifiPass\">WiFi pass:</label> </div> <div class=\"form-value\"> <input type=\"password\" class=\"form-control\" id=\"txtWifiPass\" value=\"{WifiPassValue}\"/> </div> </div> <div class=\"form-line\"> <div class=\"form-label\"> <label for=\"txtDeviceName\">Device name:</label> </div> <div class=\"form-value\"> <input type=\"text\" class=\"form-control\" id=\"txtDeviceName\" value=\"{DeviceNameValue}\" /> </div> </div> <div class=\"form-line\"> <div class=\"form-label\"> <label for=\"txtMqttServer\">MQTT server:</label> </div> <div class=\"form-value\"> <input type=\"text\" class=\"form-control\" id=\"txtMqttServer\" value=\"{MqttServerValue}\" /> </div> </div>  <div class=\"form-line\"> <div class=\"form-label\"> <label for=\"txtMqttUser\">MQTT user:</label> </div> <div class=\"form-value\"> <input type=\"text\" class=\"form-control\" id=\"txtMqttUser\" value=\"{MqttUserValue}\" /> </div> </div> <div class=\"form-line\"> <div class=\"form-label\"> <label for=\"txtMqttPass\">MQTT pass:</label> </div> <div class=\"form-value\"> <input type=\"password\" class=\"form-control\" id=\"txtMqttPass\" value=\"{MqttPassValue}\" /> </div> </div> </div></div>";
const char htmlFooter[] = "<script type=\"text/javascript\"> var DeviceIpAddress = document.getElementById('hdnDeviceIp').value; function saveValues() { var url = \"http://\" + DeviceIpAddress + \"/?action=save&wifiSsid=\" + document.getElementById('txtWifiSsid').value + \"&wifiPass=\" + document.getElementById('txtWifiPass').value + \"&deviceName=\" + document.getElementById('txtDeviceName').value + \"&mqttServer=\" + document.getElementById('txtMqttServer').value + \"&mqttUserName=\" + document.getElementById('txtMqttUser').value + \"&mqttUserPass=\" + document.getElementById('txtMqttPass').value; /*httpGetAsync(url, onSaveValuesSuccess, onSaveValuesError);*/ location.href = url; }; function scanWifi() { /*var url = location.href + \"?action=scan\"; httpGetAsync(url, onScanSuccess, onScanError);*/ alert(DeviceIpAddress); }; function reset() { location.href = \"http://\" + DeviceIpAddress + \"/?action=reset\"; }; function onSaveValuesSuccess(responseText) { alert('Configuration saved! The device will reset.'); }; function onSaveValuesError() { alert('Save configuration failed!'); }; function onScanSuccess(responseText) { document.getElementById('wifiList').innerHTML = responseText; }; function onScanError() { }; function httpGetAsync(url, successCallback, errorCallback) { var xmlHttp = new XMLHttpRequest(); xmlHttp.onreadystatechange = function() { if (xmlHttp.readyState == 4 && xmlHttp.status == 200) successCallback(xmlHttp.responseXml); if (xmlHttp.readyState == 4 && xmlHttp.status == 404) errorCallback(); }; xmlHttp.open('GET', url, true); xmlHttp.send(null); };</script></body></html>";
const char htmlResetForm[] = "<body> <input type=\"hidden\" id=\"hdnIsApMode\" value=\"{IsApModeValue}\" /> <input type=\"hidden\" id=\"hdnDeviceIp\" value=\"{DeviceIpValue}\" /> <div class=\"header\"> <div class=\"header-container\"> <div class=\"logo\"> <svg version=\"1.1\" xmlns=\"http://www.w3.org/2000/svg\" xmlns:xlink=\"http://www.w3.org/1999/xlink\" x=\"0px\" y=\"0px\" viewBox=\"0 0 107.7 110.6\" style=\"enable-background: new 0 0 107.7 110.6; height: 58px; width: 70px;\" xml:space=\"preserve\"> <style type=\"text/css\"> .st0 { display: none; } .st1 { fill: #2B66B1; } .st2 { fill: #478FCC; }</style> <g id=\"Layer_1\" class=\"st0\"></g> <g id=\"Layer_2\"> <g> <path class=\"st1\" d=\"M52.3,110.5h-41c-7.6,0-11.7-3.6-11.7-10.1V10.1C-0.3,3.6,3.8,0,11.4,0h41c7.7,0,12,3.6,12,10.1V36H44.7V14.2 H19.3v82.1h25.5V73h19.6v27.3C64.4,106.9,60.1,110.5,52.3,110.5z\" /> </g> <g> <path class=\"st2\" d=\"M95.9,110.6H56.5c-7.5,0-11.8-3.6-11.8-10.1V73h19.5v23.6h24V75L49,43.6c-3.3-2.5-4.3-4.6-4.3-8.8V10.2 c0-6.5,4.3-10.1,11.8-10.1h39.4c7.5,0,11.8,3.6,11.8,10v24.7H88.2V14h-24v20l39.2,31.2c3.3,2.6,4.3,4.8,4.3,9v26.3 C107.7,106.9,103.4,110.6,95.9,110.6z\" /> </g> </g> </svg> </div> <div class=\"title-container\"> <div class=\"title\"> C e l l a r s t o n e </div> <div class=\"subtitle\"> ESP8266 configuration </div> </div> </div> </div> <div class=\"main-container\"> <div class=\"controls\"> <button class=\"button\" onclick=\"return false;\">Save</button> <button class=\"button\" onclick=\"return false;\">Scan</button> <button class=\"button\" onclick=\"return false;\">Reset</button> </div> <div style=\"margin: 0 auto; text-align: center;\"> R E S T A R T I N G&nbsp;&nbsp;D E V I C E<br/> P L E A S E&nbsp;&nbsp;W A I T </div> </div>";
const char htmlResetFooter[] = "<script type=\"text/javascript\"> var DeviceIpAddress = document.getElementById('hdnDeviceIp').value; setTimeout(function() { location.href = \"http://\" + DeviceIpAddress; }, 9000); </script></body></html>";

Timer t;

void setup()
{

	Serial.begin(9600);

	pinMode(ledPin, OUTPUT);  // declare LED as output
	pinMode(inputPin, INPUT); // declare sensor as input

	//nastaveni zarizeni - nacteni konfigurace z eeprom apod.
	setupDevice();

	//nastaveni wifi
	setup_wifi();

	//nastaveni webserveru
	setupWebServer();

	//nastaveni mqtt
	if (!IS_AP_MODE)
	{
		setupMqttClient();
		reconnect();
	}

	lastMillis = millis();

	//casovane akce
	t.every(60000, sendStatus);
}

void setupDevice()
{
	//naplnime globalni konfiguracni promenne nactenim z EEPROM
	//loadConfigFromEeprom();

	if (WifiSsid == "")
	{
		WifiSsid = DEFAULT_WIFI_SSID;
	}

	if (WifiPass == "")
	{
		WifiPass = DEFAULT_WIFI_PSSWD;
	}

	if (DeviceName == "")
	{
		DeviceName = DEFAULT_DEVICE_NAME;
	}

	if (MqttHostName == "")
	{
		MqttHostName = DEFAULT_MQTT_URL;
	}

	if (MqttPort == 0)
	{
		MqttPort = DEFAULT_MQTT_PORT;
	}

	MqttTopicName_pir = DeviceName + "/pir";
	MqttTopicName_status = DeviceName + "/status";
	MqttTopicName_ip = DeviceName + "/ip";

	logConfig();
	//saveConfigToEeprom(WifiSsid.c_str(), WifiPass.c_str(), DeviceName.c_str(), MqttHostName.c_str(), MqttUserName.c_str(), MqttUserPass.c_str());
}

void setup_wifi()
{
	// delay(10);
	// // We start by connecting to a WiFi network
	// Serial.println();
	// Serial.print("Connecting to ");
	// Serial.println(WifiSsid);

	// WiFi.begin(WifiSsid.c_str(), WifiPass.c_str());

	// while (WiFi.status() != WL_CONNECTED) {
	//   delay(500);
	//   Serial.print(".");
	// }

	// Serial.println("");
	// Serial.println("WiFi connected");
	// Serial.println("IP address: ");
	// Serial.println(WiFi.localIP());

	//pokud existuje posledni pouzita wifi sit, zkusime se pripojit
	if (WifiSsid != "")
	{
		//WiFi.config(IPAddress(10, 10, 0, 2), IPAddress(10, 10, 0, 1), IPAddress(255, 255, 255, 0));

		//rezim clienta (stanice)
		WiFi.mode(WIFI_STA);

		Serial.println("");
		Serial.print("Connecting to ");
		Serial.print(WifiSsid);

		//pripojime se k wifi
		WiFi.begin(WifiSsid.c_str(), WifiPass.c_str());

		lastMillis = millis();
		//behem pripojovani budem blikat diodou - po pripojeni zustane sviti
		while (WiFi.status() != WL_CONNECTED)
		{
			digitalWrite(BUILTIN_LED, HIGH);
			delay(50);
			digitalWrite(BUILTIN_LED, LOW);
			delay(50);

			//pokus o pripojeni nechame bezet 15 sekund
			if ((millis() - lastMillis) > 15000)
			{
				//pokud se nepripojime do 15 sekund, zhasnem diodu a nastartujem AccesPoint
				digitalWrite(BUILTIN_LED, HIGH);
				Serial.println("");
				Serial.print("Can not connect to ");
				Serial.print(WifiSsid);
				createAP();
				return;
			}
		}
		Serial.println("");
		Serial.println("Connected!");
		Serial.println("");
		Serial.print("IP address: ");
		Serial.print(WiFi.localIP());
		IS_AP_MODE = false;
	}
	else
	{
		//pokud jsme jeste nebyli k zadne wifi pripojeni, nastartujem rovnou AccessPoint
		createAP();
	}
}

/*
	Nastartuje zarizeni v rezimu Access Point
*/
void createAP()
{
	Serial.println("");
	Serial.println("Starting Access Point...");

	//spustime rezim Access Pointu
	WiFi.mode(WIFI_AP);

	//nastavime MAC adresu
	uint8_t mac[WL_MAC_ADDR_LENGTH];
	WiFi.softAPmacAddress(mac);

	//z MAC adresy udelame unikatni suffix pro nazev AP
	String macID = String(mac[WL_MAC_ADDR_LENGTH - 2], HEX) +
				   String(mac[WL_MAC_ADDR_LENGTH - 1], HEX);
	macID.toUpperCase();

	WiFi.softAP((DeviceName + macID).c_str(), DEFAULT_AP_PASS);

	Serial.println("");
	Serial.print("Access Point ");
	Serial.print((DeviceName + "-" + macID));
	Serial.print(" started!");
	Serial.println("");
	Serial.print("IP address: ");
	Serial.print(WiFi.softAPIP());

	IS_AP_MODE = true;
}

void reconnect()
{
	// Loop until we're reconnected
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// TADY JSEM TO ZMENIL Z WHILE NA If
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	if (!mqttClient.connected())
	{
		// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!1
		Serial.print("Attempting MQTT connection...");
		// Attempt to connect
		// If you do not want to use a username and password, change next line to
		// if (client.connect("ESP8266Client")) {
		if (mqttClient.connect(DeviceName.c_str(), MqttUserName.c_str(), MqttUserPass.c_str()))
		{
			Serial.println("connected");
		}
		else
		{
			Serial.print("failed, rc=");
			Serial.println(mqttClient.state());
			// Serial.println(" try again in 5 seconds");

			Serial.println(MqttHostName.c_str());
			Serial.println(MqttPort);
			Serial.println(MqttTopicName_pir.c_str());
			Serial.println(MqttTopicName_status.c_str());
			Serial.println(MqttUserName.c_str());
			Serial.println(MqttUserPass.c_str());

			// Wait 5 seconds before retrying
			//delay(5000);
		}
	}
}

/*
	Nastaveni MQTT klienta
*/
void setupMqttClient()
{
	mqttClient.setServer(MqttHostName.c_str(), MqttPort);
	//mqttClient.setCallback(messageRecievedCallback);
}

/*
	Nastaveni web serveru
*/
void setupWebServer()
{
	//pokud client pristoupi na root
	webServer.on("/", []() {
		String action = webServer.arg("action");
		action.toLowerCase();

		//resetovat zarizeni
		if (action == "reset")
		{
			webServer.send(200, "text/html", getResetFormString());
			delay(500);
			Serial.println("Device will reset...");
			ESP.reset();
			return;
		}

		//zijeme?
		if (action == "isAlive")
		{
			webServer.send(200, "text/html", "I am alive!");
			delay(100);
			return;
		}

		//ulozit nastaveni
		if (action == "save")
		{
			String wifiSsid = webServer.arg("wifiSsid");
			String wifiPass = webServer.arg("wifiPass");
			String deviceName = webServer.arg("deviceName");
			String mqttServer = webServer.arg("mqttServer");
			// String mqttTopic = webServer.arg("mqttTopic");
			String mqttUserName = webServer.arg("mqttUserName");
			String mqttUserPass = webServer.arg("mqttUserPass");
			// int mqttPort = webServer.arg("mqttPort").toInt();

			saveConfigToEeprom(wifiSsid.c_str(), wifiPass.c_str(), deviceName.c_str(), mqttServer.c_str(), mqttUserName.c_str(), mqttUserPass.c_str());
			webServer.send(200, "text/html", getResetFormString());
			delay(500);
			ESP.reset();
			return;
		}

		//proskenovat okolni site a vratit jako xml
		if (action == "scan")
		{
			String wifiXml = getWifiNetworksXml();
			webServer.send(200, "text/xml", wifiXml);
			//String wifiHtml = getWifiNetworksHtml();
			//webServer.send(200, "text/html", wifiHtml);
			delay(500);
			return;
		}

		String str = String(htmlConfigForm);
		str.replace("{WifiSsidValue}", String(WifiSsid));
		str.replace("{WifiPassValue}", String(WifiPass));
		str.replace("{DeviceNameValue}", String(DeviceName));
		str.replace("{MqttServerValue}", String(MqttHostName));
		// str.replace("{MqttTopicValue}", String(MqttTopicName));
		str.replace("{MqttUserValue}", String(MqttUserName));
		str.replace("{MqttPassValue}", String(MqttUserPass));
		// str.replace("{MqttPortValue}", String(MqttPort));
		//str.replace("{IsApModeValue}", (IS_AP_MODE) ? "true" : "false");
		str.replace("{DeviceIpValue}", WiFi.localIP().toString());
		webServer.send(200, "text/html", String(htmlHeader) + str + String(htmlFooter));
		delay(100);
	});

	Serial.println("");
	Serial.println("Starting web server...");
	webServer.begin();
	Serial.println("Server started!");
}

String getResetFormString()
{
	String str = String(htmlResetForm);
	//str.replace("{IsApModeValue}", (IS_AP_MODE) ? "true" : "false");
	str.replace("{DeviceIpValue}", WiFi.localIP().toString());

	return String(htmlHeader) + str + String(htmlResetFooter);
}

String getWifiNetworksHtml()
{
	String html = "<ul>\n";
	int8_t netCount = WiFi.scanNetworks(false, false);
	if (netCount > 0)
	{
		for (int i = 0; i < netCount; i++)
		{
			int rssi = WiFi.RSSI(i) * -1;
			String c;
			html += "<li class=\"wifi-icon wifi-icon-";
			if (rssi <= 10)
				c = "4";
			if (rssi > 10 && rssi <= 20)
				c = "3";
			if (rssi > 20 && rssi <= 40)
				c = "2";
			if (rssi > 40 && rssi <= 60)
				c = "1";
			if (rssi > 80)
				c = "0";
			html += c;
			html += "\">";

			html += "<span>";
			html += WiFi.SSID(i);
			html += "</span>";

			html += "</li>";
		}
	}
	html += "</ul>";

	return html;
}

String getWifiNetworksXml()
{
	String xml = "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n<WifiNetworks>";
	int8_t netCount = WiFi.scanNetworks(false, true);
	if (netCount > 0)
	{
		for (int i = 0; i < netCount; i++)
		{
			xml += "\t<WifiNetwork>\n";

			xml += "\t\t<Ssid>";
			xml += WiFi.SSID(i);
			xml += "</Ssid>\n";

			xml += "\t\t<Rssi>";
			xml += WiFi.RSSI(i);
			xml += "</Rssi>\n";

			xml += "\t\t<Secured>";
			xml += (WiFi.encryptionType(i) == ENC_TYPE_NONE) ? "false" : "true";
			xml += "</Secured>\n";

			xml += "\t</WifiNetwork>\n";
		}
	}
	xml += "</WifiNetworks>";

	return xml;
}

//************************************************************
long randNumber;
void sendStatus()
{

	if (!mqttClient.connected())
	{
		reconnect();
	}
	// mqttClient.loop();

	randNumber = random(0, 10);
	Serial.println(String(randNumber).c_str());

	//Get actual IP address
	String ip = WiFi.localIP().toString();
	Serial.println(ip.c_str());

	if (mqttClient.connected())
	{
		mqttClient.publish(MqttTopicName_status.c_str(), String(randNumber).c_str(), true);
		mqttClient.publish(MqttTopicName_ip.c_str(), ip.c_str(), true);
		Serial.println("STATUS OK");
	}
	else
	{
		Serial.println("STATUS BAD - MQTT is not reachable");
	}
}

//************************************************************

void loop()
{
	//update timer
	t.update();

	//WEBSERVER
	//obslouzit clienta pristupujici na web zarizeni
	webServer.handleClient();

	//MQTT
	val = digitalRead(inputPin); // read input value.


	

	if (val != oldval)
	{
		Serial.println(val);
		oldval = val;

		if (!mqttClient.connected())
		{
			reconnect();
		}
		mqttClient.loop();

		

		if (val == HIGH)
		{								// check if the input is HIGH
			digitalWrite(ledPin, HIGH); // turn LED ON
			if (pirState == LOW)
			{
				// we have just turned on
				Serial.println("Motion detected!");
				// We only want to print on the output change, not state
				pirState = HIGH;

				mqttClient.publish(MqttTopicName_pir.c_str(), String(1).c_str(), true);
			}
		}
		else
		{
			digitalWrite(ledPin, LOW); // turn LED OFF
			if (pirState == HIGH)
			{
				// we have just turned of
				Serial.println("Motion ended!");
				// We only want to print on the output change, not state
				pirState = LOW;

				mqttClient.publish(MqttTopicName_pir.c_str(), String(0).c_str(), true);
			}
		}

		
	}
}

/*
	ukaze konfiguraci
*/
void logConfig()
{

	Serial.println(WifiSsid.c_str());
	Serial.println(WifiPass.c_str());
	Serial.println(DeviceName.c_str());
	Serial.println(MqttHostName.c_str());
	Serial.println(MqttPort);
	Serial.println(MqttTopicName_pir.c_str());
	Serial.println(MqttTopicName_status.c_str());
	Serial.println(MqttUserName.c_str());
	Serial.println(MqttUserPass.c_str());
}

/*
	Nacte konfigarici zarizeni z EEPROM
*/
void loadConfigFromEeprom()
{
	WifiSsid = readTextFromEEPROM(EE_WIFI_SSID_ADDR, EE_WIFI_SSID_SIZE);
	delay(5);
	WifiPass = readTextFromEEPROM(EE_WIFI_PASS_ADDR, EE_WIFI_PASS_SIZE);
	delay(5);
	DeviceName = readTextFromEEPROM(EE_DEVICE_NAME_ADDR, EE_DEVICE_NAME_SIZE);
	delay(5);
	MqttHostName = readTextFromEEPROM(EE_MQTT_SERVER_ADDR, EE_MQTT_SERVER_SIZE);
	delay(5);
	// MqttTopicName = readTextFromEEPROM(EE_MQTT_TOPIC_ADDR, EE_MQTT_TOPIC_SIZE);
	// delay(5);
	MqttUserName = readTextFromEEPROM(EE_MQTT_USER_NAME_ADDR, EE_MQTT_USER_NAME_SIZE);
	delay(5);
	MqttUserPass = readTextFromEEPROM(EE_MQTT_USER_PASS_ADDR, EE_MQTT_USER_PASS_SIZE);
	delay(5);
	// MqttPort = readIntFromEEPROM(EE_MQTT_PORT_ADDR);
	// delay(5);
}

/*
	Ulozi konfigaraci do EEPROM
*/
void saveConfigToEeprom(const char *wifiSsid, const char *wifiPass, const char *deviceName, const char *mqttHost, const char *mqttUserName, const char *mqttUserPass)
{
	writeTextToEEPROM(EE_WIFI_SSID_ADDR, EE_WIFI_SSID_SIZE, wifiSsid);
	delay(5);
	writeTextToEEPROM(EE_WIFI_PASS_ADDR, EE_WIFI_PASS_SIZE, wifiPass);
	delay(5);
	writeTextToEEPROM(EE_DEVICE_NAME_ADDR, EE_DEVICE_NAME_SIZE, deviceName);
	delay(5);
	writeTextToEEPROM(EE_MQTT_SERVER_ADDR, EE_MQTT_SERVER_SIZE, mqttHost);
	delay(5);
	// writeTextToEEPROM(EE_MQTT_TOPIC_ADDR, EE_MQTT_TOPIC_SIZE, mqttTopic);
	// delay(5);
	writeTextToEEPROM(EE_MQTT_USER_NAME_ADDR, EE_MQTT_USER_NAME_SIZE, mqttUserName);
	delay(5);
	writeTextToEEPROM(EE_MQTT_USER_PASS_ADDR, EE_MQTT_USER_PASS_SIZE, mqttUserPass);
	delay(5);
	// writeIntToEEPROM(EE_MQTT_PORT_ADDR, mqttPort);
	// delay(5);
}

/*
	Zapise predany string do EEPROM pameti
*/
void writeTextToEEPROM(int addr, int size, String s)
{
	EEPROM.begin(512);

	for (int i = 0; i < s.length(); i++)
	{
		EEPROM.write(addr + i, (byte)s[i]);
	}
	for (int i = s.length(); i < size; i++)
	{
		EEPROM.write(addr + i, ' ');
	}

	EEPROM.commit();
	EEPROM.end();
}

/*
	Zapise predany integer do EEPROM pameti
*/
void writeIntToEEPROM(int addr, int val)
{
	EEPROM.begin(512);

	byte byte1 = val;
	byte byte2 = val >> 8;
	EEPROM.write(addr, byte1);
	EEPROM.write(addr + 1, byte2);

	EEPROM.commit();
	EEPROM.end();
}

/*
	Precte string na predane adrese z EEPROM pameti
*/
String readTextFromEEPROM(int addr, int size)
{
	EEPROM.begin(512);

	String TextFromEEPROM = "";

	for (int i = 0; i < size; i++)
	{
		char znak = (char)EEPROM.read(addr + i);
		TextFromEEPROM = TextFromEEPROM + znak;
	}
	while (TextFromEEPROM[TextFromEEPROM.length() - 1] == ' ')
	{
		TextFromEEPROM = TextFromEEPROM.substring(0, TextFromEEPROM.length() - 1);
	}
	//if (TextFromEEPROM == "") return nullptr;

	EEPROM.end();

	return TextFromEEPROM;
}

/*
	Precte integer na predane adrese z EEPROM pameti
*/
int readIntFromEEPROM(int addr)
{
	EEPROM.begin(512);

	int val = EEPROM.read(addr) + ((EEPROM.read(addr + 1)) << 8);
	return val;

	EEPROM.end();
}