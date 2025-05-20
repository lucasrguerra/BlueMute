#include <Bluetooth.h>

Bluetooth::Bluetooth():
    hspi(HSPI),
    vspi(VSPI),
    radio_hspi(HSPI_CE, HSPI_CSN, SPI_SPEED), 
    radio_vspi(VSPI_CE, VSPI_CSN, SPI_SPEED)
{}

void startRadio(RF24 &radio, SPIClass &spi, const char *name) {
	spi.begin(
		name == "HSPI" ? HSPI_SCK : VSPI_SCK,
		name == "HSPI" ? HSPI_MISO : VSPI_MISO,
		name == "HSPI" ? HSPI_MOSI : VSPI_MOSI,
		name == "HSPI" ? HSPI_CSN : VSPI_CSN
	);
	if (!radio.begin(&spi)) {
		Serial.printf("Failed to initialize NRF24 on %s\n", name);
		return;
	}

	delay(200);
	radio.setAutoAck(false);
	radio.stopListening();
	radio.setRetries(0, 0);
	radio.setPayloadSize(5);
	radio.setAddressWidth(3);
	radio.setPALevel(RF24_PA_MAX, true);
	radio.setDataRate(RF24_2MBPS);
	radio.setCRCLength(RF24_CRC_DISABLED);

	Serial.printf("NRF24 on %s initialized\n", name);
}

void Bluetooth::begin() {
	esp_bt_controller_deinit();

    startRadio(radio_hspi, hspi, "HSPI");
    startRadio(radio_vspi, vspi, "VSPI");

    Serial.println("Bluetooth initialized");
}

void Bluetooth::startJamming() {
    radio_hspi.startConstCarrier(RF24_PA_MAX, 45);
    radio_vspi.startConstCarrier(RF24_PA_MAX, 45);
}

void Bluetooth::stopJamming() {
    radio_hspi.stopConstCarrier();
    radio_vspi.stopConstCarrier();
}

void Bluetooth::executeJamming() {
    radio_hspi.setChannel(random(80));
    radio_vspi.setChannel(random(80));
}