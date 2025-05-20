#include <Arduino.h>
#include <Bluetooth.h>
#include <Display.h>
#include <Wi-Fi.h>

#define MODE_BUTTON 27

Bluetooth bluetooth;
Display display;
WiFi wifi;
byte mode = 0;
int wifi_attack_mode = -1;

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
		wifi.executeJamming(wifi_attack_mode);
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
		int number_of_aps = wifi.foundAPs();
		char* message = (char*)malloc(50);
		sprintf(message, "Jamming Wi-Fi\n%d APs encontrados", number_of_aps);
		display.showMessage(message);

		delay(3000);
		bool is_selected = false;
		unsigned int ap_index = 0;
		while (!is_selected) {
			char* message = (char*)malloc(100);
			if (ap_index < number_of_aps) {
				sprintf(message, "Selecione o AP\n%d/%d\n%s", (ap_index + 1), (number_of_aps + 1), wifi.getSSID(ap_index));
			} else {
				sprintf(message, "Selecione o AP\n%d/%d\nTodos os APs", (ap_index + 1), (number_of_aps + 1));
			}
			display.showMessage(message);
			free(message);
			
			while (digitalRead(MODE_BUTTON) == LOW) { delay(10); }
			unsigned long press_duration = millis();
			while (digitalRead(MODE_BUTTON) == HIGH) { delay(10); }
			press_duration = millis() - press_duration;
			bool is_long_press = (press_duration > 1000);

			if (!is_long_press) {
				ap_index = (ap_index + 1) % (number_of_aps + 1);
			} else {
				is_selected = true;
				if (ap_index < number_of_aps) {
					wifi_attack_mode = ap_index;
					Serial.printf("Jamming Wi-Fi %d\n", wifi_attack_mode);
					display.showMessage("Jamming Wi-Fi");
				} else {
					wifi_attack_mode = -1;
					Serial.println("Jamming Wi-Fi (todos)");
					display.showMessage("Jamming Wi-Fi (todos)");
				}
			}
		}
		

		wifi.startJamming();
		break;
	}

	default:
		break;
	}

	delay(300);
}