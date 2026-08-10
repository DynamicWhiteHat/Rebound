#ifndef __ADS1292_H__
#define __ADS1292_H__

#include <Arduino.h>
#include <SPI.h>

// SPI Commands
#define ADS1292_CMD_WAKEUP 0x02
#define ADS1292_CMD_STANDBY 0x04
#define ADS1292_CMD_RESET 0x06
#define ADS_CMD_START   0x08
#define ADS_CMD_STOP    0x0A
#define ADS_CMD_RDATAC  0x10
#define ADS_CMD_SDATAC  0x11
#define ADS_CMD_RDATA   0x12
#define ADS_CMD_RREG    0x20
#define ADS_CMD_WREG    0x40

#define ADS_REG_ID      0x00
#define ADS_REG_CONFIG1 0x01
#define ADS_REG_CONFIG2 0x02
#define ADS_REG_LOFF    0x03
#define ADS_REG_CH1SET  0x04
#define ADS_REG_CH2SET  0x05

class ads1292 {

public:
    ads1292(uint8_t csPin);
    void begin(SPIClass &spiBus);
    void wakeup();
    void standby();
    void reset();
    void start();
    void stop();
    void rdatac();
    void sdatac();
    void rdata();
    void beginChannel1();
    void beginChannel2();
    uint8_t readReg(uint8_t reg);
    void writeReg(uint8_t reg, uint8_t val);
    bool readECG(uint32_t *status, int32_t *ch1, int32_t *ch2);

private:
    uint8_t _csPin;
    SPIClass *_spi;
    SPISettings _spiSettings;

};

#endif // __ADS1292_H__