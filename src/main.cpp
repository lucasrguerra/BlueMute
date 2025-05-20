#include <Arduino.h>
#include <Bluetooth.h>
#include <Display.h>
#include <Wi-Fi.h>

#define MODE_BUTTON 27

Bluetooth bluetooth;
Display display;
WiFi wifi;
byte mode = 0;

void handleButtonPress();

void setup() {
	Serial.begin(115200);
	delay(1000);

	pinMode(MODE_BUTTON, INPUT);

	bluetooth.begin();
	display.begin();
	wifi.begin();

	Serial.println("Setup complete");
	display.showLoadingScreen();
	display.showMessage("Parado");
}

void loop() {
	if (digitalRead(MODE_BUTTON) == HIGH) {
		while (digitalRead(MODE_BUTTON) == HIGH) { delay(10); }
		handleButtonPress();
	}

	switch (mode) {
	case 0:
		delay(100);
		break;

	case 1:
		bluetooth.executeJamming();
		break;

	case 2:
		wifi.executeJamming();
		break;

	default:
		break;
	}
}

void handleButtonPress() {
	mode = (mode + 1) % 3;
	switch (mode) {
	case 0:
		Serial.println("Parado");
		display.showMessage("Parado");

		bluetooth.stopJamming();
		wifi.stopJamming();
		break;

	case 1:
		Serial.println("Jamming Bluetooth");
		display.showMessage("Jamming Bluetooth");

		wifi.stopJamming();
		bluetooth.startJamming();
		break;

	case 2: {
		Serial.println("Jamming Wi-Fi");
		display.showMessage("Aguarde");
		
		bluetooth.stopJamming();
		
		wifi.performScan();
		int number_of_aps = wifi.findedAPs();
		char* message = (char*)malloc(50);
		sprintf(message, "Jamming Wi-Fi\n%d APs encontrados", number_of_aps);
		display.showMessage(message);

		wifi.startJamming();
		break;
	}

	default:
		break;
	}

	delay(300);
}