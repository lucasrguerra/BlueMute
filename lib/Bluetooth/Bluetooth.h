#ifndef BLUETOOTH_H
#define BLUETOOTH_H

#include <Arduino.h>
#include <esp_bt.h>
#include <RF24.h>
#include <SPI.h>

#define HSPI_CE 26
#define HSPI_CSN 15
#define HSPI_SCK 14
#define HSPI_MISO 12
#define HSPI_MOSI 13

#define VSPI_CE 22
#define VSPI_CSN 5
#define VSPI_SCK 18
#define VSPI_MISO 19
#define VSPI_MOSI 23

#define SPI_SPEED 16000000

class Bluetooth {
    private:
        SPIClass hspi;
        SPIClass vspi;
        RF24 radio_hspi;
        RF24 radio_vspi;

    public:
        Bluetooth();
        void begin();
        void startJamming();
        void stopJamming();
        void executeJamming();
};

#endif