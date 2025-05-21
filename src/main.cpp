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
		String message = String("Jamming Wi-Fi\n") + String(number_of_aps) + String(" APs encontrados");
		display.showMessage(message.c_str());

		delay(3000);
		bool is_selected = false;
		unsigned int ap_index = 0;
		int number_of_options = number_of_aps + 1;
		while (!is_selected) {
			int display_index = ap_index + 1;
			char* ssid = wifi.getSSID(ap_index);

			String message = String("Selecione o AP\n");
			if (ap_index < number_of_aps) {
				message += String(display_index) + String("/") + String(number_of_options) + String("\n") + String(ssid);
			} else {
				message += String(display_index) + String("/") + String(number_of_options) + String("\nTodos os APs");
			}
			display.showMessage(message.c_str());
			
			while (digitalRead(MODE_BUTTON) == LOW) { delay(10); }
			unsigned long press_duration = millis();
			while (digitalRead(MODE_BUTTON) == HIGH) { delay(10); }
			press_duration = millis() - press_duration;
			bool is_long_press = (press_duration > 1000);

			if (!is_long_press) {
				ap_index = display_index % number_of_options;
			} else {
				is_selected = true;
				if (ap_index < number_of_aps) {
					wifi_attack_mode = ap_index;
					Serial.printf("Jamming Wi-Fi\n%s", ssid);
					String msg = "Jamming Wi-Fi\n" + String(ssid);
					display.showMessage(msg.c_str());
				} else {
					wifi_attack_mode = -1;
					Serial.println("Jamming Wi-Fi\nTodos os APs");
					display.showMessage("Jamming Wi-Fi\nTodos os APs");
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