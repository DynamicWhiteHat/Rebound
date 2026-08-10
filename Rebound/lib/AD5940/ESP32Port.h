#ifndef ESP32PORT_H
#define ESP32PORT_H

#include <Arduino.h>
#include <SPI.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * AD5940 port-layer init.
 *
 * spiBus:  a SPIClass instance that has ALREADY had .begin() called on it
 *          (shared with any other SPI device on the same physical bus,
 *          e.g. the ADS1292). This function does NOT call spiBus.begin()
 *          itself - that must happen exactly once, in main.cpp, before
 *          this is called.
 * cs:      AD5940 chip-select pin (unique to the AD5940, NOT shared)
 * int_pin: AD5940 GP0/INT pin
 */
void AD5940(SPIClass &spiBus, uint8_t cs, uint8_t int_pin);

#ifdef __cplusplus
}
#endif

#endif // ESP32PORT_H