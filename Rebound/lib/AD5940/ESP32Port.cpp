/*
 * ESP32Port.cpp
 *
 * Port layer for the AD5940 driver library (ad5940lib), targeting an
 * ESP32-S3 running under the Arduino framework (PlatformIO).
 *
 * This is the ESP32 equivalent of ADI's ADICUP3029Port.c /
 * NUCLEOF411Port.c reference files. It implements the low-level
 * hardware functions that the generic ad5940lib driver calls into.
 * The driver code (ad5940.c, BodyImpedance...c, AD5940Main.c) is
 * unmodified - only this file and main.cpp are new.
 *
 * No hardware RESET pin is wired for this board, so AD5940_RstClr()
 * and AD5940_RstSet() are intentionally no-ops. AD5940_HWReset()
 * (called from AD5940PlatformCfg()) will therefore do nothing -
 * initialization relies entirely on AD5940_Initialize()'s register
 * writes and AD5940_WakeUp()'s SPI-read wakeup, with no way to force
 * the chip back to a known state if it ends up wedged. That's a
 * known tradeoff of skipping the reset line, not a bug.
 */

#include <Arduino.h>
#include <SPI.h>
#include "ESP32Port.h"

extern "C" {
  #include "ad5940.h"   // pulls in ad5940lib's type defs + function declarations
}

/* ---------------------------------------------------------------------
 * Pin assignments - set via the AD5940() init call below, not #define,
 * so they can be reconfigured at runtime if needed.
 * ------------------------------------------------------------------- */
static int AD5940_CS_PIN;    // chip select (active low) - unique to the AD5940
static int AD5940_INT_PIN;   // AD5940 interrupt-out pin -> ESP32 input

// AD5940 datasheet allows SCLK up to ~16 MHz; start conservative (1-4 MHz)
// until wiring/signal integrity is confirmed on a scope.
#define AD5940_SPI_CLOCK_HZ   4000000

// TODO: confirm SPI mode against the AD5940 datasheet's clock
// polarity/phase spec.
#define AD5940_SPI_MODE       SPI_MODE0

// Bus is now owned by main.cpp and shared with any other SPI device
// (e.g. the ADS1292) on the same physical lines. We only keep a pointer
// to it - .begin() is called exactly once, by main.cpp, not here.
static SPIClass *AD5940_SPI = nullptr;

/* Set by the ISR, polled/cleared by the driver via
 * AD5940_GetMCUIntFlag() / AD5940_ClrMCUIntFlag() */
static volatile bool ad5940_int_flag = false;

static void IRAM_ATTR AD5940_ISR() {
  ad5940_int_flag = true;
}

/* ---------------------------------------------------------------------
 * Custom hardware init - call this once from setup(), AFTER spiBus.begin()
 * has already been called (main.cpp owns that, since the bus is shared
 * with the ADS1292). This only sets up what's unique to the AD5940: its
 * own CS pin and its interrupt pin.
 * ------------------------------------------------------------------- */
void AD5940(SPIClass &spiBus, uint8_t cs, uint8_t int_pin) {
  AD5940_SPI      = &spiBus;
  AD5940_CS_PIN   = cs;
  AD5940_INT_PIN  = int_pin;

  // Call standard ADI resource init
  AD5940_MCUResourceInit(NULL);
}

/* ---------------------------------------------------------------------
 * Hardware init - called by AD5940() above. Do not call directly
 * unless you've already set AD5940_SPI/AD5940_CS_PIN/AD5940_INT_PIN
 * yourself.
 * ------------------------------------------------------------------- */
extern "C" uint32_t AD5940_MCUResourceInit(void *pCfg) {
  (void)pCfg;

  pinMode(AD5940_CS_PIN, OUTPUT);
  digitalWrite(AD5940_CS_PIN, HIGH);

  // TODO: confirm INPUT vs INPUT_PULLUP/PULLDOWN based on whether the
  // AD5940's interrupt GPIO is configured push-pull or open-drain
  pinMode(AD5940_INT_PIN, INPUT);

  // NOTE: spiBus.begin() is NOT called here. It's called once in
  // main.cpp before AD5940() runs, since the bus is shared with the
  // ADS1292. Calling .begin() twice on the same SPIClass/host is what
  // was causing the two devices to fight over the peripheral.

  // TODO: confirm edge polarity (FALLING vs RISING) against how the
  // AD5940's interrupt pin is configured (active-low vs active-high)
  attachInterrupt(digitalPinToInterrupt(AD5940_INT_PIN), AD5940_ISR, FALLING);

  return 0; // AD5940ERR_OK
}

/* ---------------------------------------------------------------------
 * Chip select control
 * ------------------------------------------------------------------- */
extern "C" void AD5940_CsClr(void) {
  digitalWrite(AD5940_CS_PIN, LOW);
}

extern "C" void AD5940_CsSet(void) {
  digitalWrite(AD5940_CS_PIN, HIGH);
}

/* ---------------------------------------------------------------------
 * Reset control - INTENTIONALLY A NO-OP. No hardware RESET pin is
 * wired on this board. AD5940_HWReset() will call these and do
 * nothing; the chip is only ever soft-configured via
 * AD5940_Initialize()'s register writes.
 * ------------------------------------------------------------------- */
extern "C" void AD5940_RstClr(void) {
  // No HW reset pin wired.
}

extern "C" void AD5940_RstSet(void) {
  // No HW reset pin wired.
}

/* ---------------------------------------------------------------------
 * Delay - driver calls this with a count in 10us units
 * ------------------------------------------------------------------- */
extern "C" void AD5940_Delay10us(uint32_t time) {
  delayMicroseconds(time * 10);
}

/* ---------------------------------------------------------------------
 * SPI transfer - the core communication function the driver relies on
 * for every register read/write and FIFO pull. CS is NOT toggled here
 * - the driver's own SPIWriteReg/SPIReadReg/FIFORd functions bracket
 * multiple calls to this with their own AD5940_CsClr()/AD5940_CsSet(),
 * since a single register access is often more than one byte-transfer
 * call and must stay inside one continuous CS-low window.
 * ------------------------------------------------------------------- */
extern "C" void AD5940_ReadWriteNBytes(unsigned char *pSendBuffer,
                                        unsigned char *pRecvBuff,
                                        unsigned long length) {
  AD5940_SPI->beginTransaction(SPISettings(AD5940_SPI_CLOCK_HZ, MSBFIRST, AD5940_SPI_MODE));

  for (unsigned long i = 0; i < length; i++) {
    uint8_t outByte = pSendBuffer ? pSendBuffer[i] : 0x00;
    uint8_t inByte = AD5940_SPI->transfer(outByte);
    if (pRecvBuff) {
      pRecvBuff[i] = inByte;
    }
  }

  AD5940_SPI->endTransaction();
}

/* ---------------------------------------------------------------------
 * Interrupt flag helpers - the generic driver polls these instead of
 * touching the ISR or pin directly.
 * ------------------------------------------------------------------- */
extern "C" uint32_t AD5940_GetMCUIntFlag(void) {
  return ad5940_int_flag ? 1 : 0;
}

extern "C" uint32_t AD5940_ClrMCUIntFlag(void) {
  ad5940_int_flag = false;
  return 1;
}