#include <Arduino.h>
#include <ads1292.h>
#include "ESP32Port.h"

extern "C" {
  #include "ad5940.h"
  #include "AD5940.h"
  #include "AD5940Main.h"
  #include "BodyImpedance-HiZ_Electrodes.h"
}


// Shared SPI bus - both the ADS1292 and the AD5940 sit on these three
// lines. Each device gets its own CS pin below; do NOT create a second
// SPIClass instance for the AD5940 - one .begin() call on this bus,
// shared by both drivers.
#define FSPI_CLK  12
#define FSPI_MISO 13
#define FSPI_MOSI 11

// ADS1292
#define ADS1292_CS_PIN 6
#define ADS1292_DRDY_PIN 5

// AD5940
#define AD5940_CS_PIN  10
#define AD5940_INT_PIN  8

#define APPBUFF_SIZE 512
uint32_t AppBuff[APPBUFF_SIZE];

SPIClass SharedSPI(FSPI);
ads1292 sEMG(ADS1292_CS_PIN);

volatile bool sEMGDataReady = false;

void IRAM_ATTR sEMGDataReadyISR() {
  sEMGDataReady = true;
}

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    delay(10);
  }

  // -1 for the .begin() CS argument: both devices manage their own CS
  // manually via digitalWrite (inside each driver), so there's no single
  // "default" CS pin for the shared bus itself.
  SharedSPI.begin(FSPI_CLK, FSPI_MISO, FSPI_MOSI, -1);

  Serial.println("Initializing ADS1292...");
  sEMG.begin(SharedSPI);

  Serial.println("Initializing AD5940...");
  AD5940(SharedSPI, AD5940_CS_PIN, AD5940_INT_PIN);
  AD5940PlatformCfg();
  AD5940BIAStructInit();
  AppBIAInit(AppBuff, APPBUFF_SIZE);
  AppBIACtrl(BIACTRL_START, 0);

  Serial.println("Reading ADS1292 Chip ID...");
  uint8_t chipId = sEMG.readReg(ADS_REG_ID);
  Serial.printf("ADS1292 Chip ID read: 0x%02X\n", chipId);

  if ((chipId & 0xFC) == 0x50) { 
  Serial.println("Success: Connected to ADS1292 family chip cleanly!");
  } else {
    Serial.println("Warning: Chip ID does not match the ADS1292 family signature.");
  }

  sEMG.writeReg(ADS_REG_CONFIG1, 0x02);
  sEMG.writeReg(ADS_REG_CONFIG2, 0xA0);
  sEMG.beginChannel2();

  pinMode(ADS1292_DRDY_PIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(ADS1292_DRDY_PIN), sEMGDataReadyISR, FALLING);
}

void loop() {
  if (AD5940_GetMCUIntFlag()) {
    AD5940_ClrMCUIntFlag();
    uint32_t count = APPBUFF_SIZE;
    AppBIAISR(AppBuff, &count);
    BIAShowResult(AppBuff, count);
  }

  if (sEMGDataReady) {
    sEMGDataReady = false;

    uint32_t status;
    int32_t ch1, ch2;
    if (sEMG.readECG(&status, &ch1, &ch2)) {
      Serial.printf("Status: 0x%06X, Channel 1: %d, Channel 2: %d\n", status, ch1, ch2);
    } else {
      Serial.println("Error reading data from ADS1292.");
    }
  }
}