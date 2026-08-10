#include "ads1292.h"

ads1292::ads1292(uint8_t csPin) {
  _csPin = csPin;
  _spiSettings = SPISettings(1000000, MSBFIRST, SPI_MODE1);
}

void ads1292::begin(SPIClass &spiBus) {
  _spi = &spiBus;
  pinMode(_csPin, OUTPUT);
  digitalWrite(_csPin, HIGH);
  
  delay(500); 
  reset();
  
  delayMicroseconds(50); 
  
  sdatac();
  delayMicroseconds(10);
}

void ads1292::reset() {
  _spi->beginTransaction(_spiSettings);
  digitalWrite(_csPin, LOW);           
  _spi->transfer(ADS1292_CMD_RESET);
  delayMicroseconds(10);
  digitalWrite(_csPin, HIGH);          
  _spi->endTransaction(); 
}

void ads1292::wakeup() {
  _spi->beginTransaction(_spiSettings);
  digitalWrite(_csPin, LOW);
  _spi->transfer(ADS1292_CMD_WAKEUP);
  delayMicroseconds(10);
  digitalWrite(_csPin, HIGH);
  _spi->endTransaction();
}

void ads1292::standby() {
  _spi->beginTransaction(_spiSettings);
  digitalWrite(_csPin, LOW);
  _spi->transfer(ADS1292_CMD_STANDBY);
  delayMicroseconds(10);
  digitalWrite(_csPin, HIGH);
  _spi->endTransaction();
}

void ads1292::start() {
  _spi->beginTransaction(_spiSettings);
  digitalWrite(_csPin, LOW);
  _spi->transfer(ADS_CMD_START);
  delayMicroseconds(10);
  digitalWrite(_csPin, HIGH);
  _spi->endTransaction();
}

void ads1292::stop() {
  _spi->beginTransaction(_spiSettings);
  digitalWrite(_csPin, LOW);
  _spi->transfer(ADS_CMD_STOP);
  delayMicroseconds(10);
  digitalWrite(_csPin, HIGH);
  _spi->endTransaction();
}

void ads1292::sdatac() {
  _spi->beginTransaction(_spiSettings);
  digitalWrite(_csPin, LOW);
  _spi->transfer(ADS_CMD_SDATAC);
  delayMicroseconds(10);
  digitalWrite(_csPin, HIGH);
  _spi->endTransaction();
}

void ads1292::rdatac() {
  _spi->beginTransaction(_spiSettings);
  digitalWrite(_csPin, LOW);
  _spi->transfer(ADS_CMD_RDATAC);
  delayMicroseconds(10);
  digitalWrite(_csPin, HIGH);
  _spi->endTransaction();
}

void ads1292::rdata() {
  _spi->beginTransaction(_spiSettings);
  digitalWrite(_csPin, LOW);
  _spi->transfer(ADS_CMD_RDATA);
  delayMicroseconds(10);
  digitalWrite(_csPin, HIGH);
  _spi->endTransaction();
}

uint8_t ads1292::readReg(uint8_t reg) {
  _spi->beginTransaction(_spiSettings);
  digitalWrite(_csPin, LOW);
  
  _spi->transfer(ADS_CMD_RREG | (reg & 0x1F));
  _spi->transfer(0x00); // 0x00 means "Read 1 register"
  uint8_t value = _spi->transfer(0x00);
  
  digitalWrite(_csPin, HIGH);
  _spi->endTransaction();
  return value;
}

void ads1292::writeReg(uint8_t reg, uint8_t val) {
  _spi->beginTransaction(_spiSettings);
  digitalWrite(_csPin, LOW);
  
  _spi->transfer(ADS_CMD_WREG | (reg & 0x1F));
  _spi->transfer(0x00); // 0x00 means "Write 1 register"
  _spi->transfer(val);
  
  digitalWrite(_csPin, HIGH);
  _spi->endTransaction();
}

void ads1292::beginChannel1() {
  writeReg(ADS_REG_CH1SET, 0x00); 
}

void ads1292::beginChannel2() {
  writeReg(ADS_REG_CH2SET, 0x00); 
}

bool ads1292::readECG(uint32_t *status, int32_t *ch1, int32_t *ch2) {
  _spi->beginTransaction(_spiSettings);
  digitalWrite(_csPin, LOW);

  uint8_t s1 = _spi->transfer(0x00);
  uint8_t s2 = _spi->transfer(0x00);
  uint8_t s3 = _spi->transfer(0x00);
  *status = ((uint32_t)s1 << 16) | ((uint32_t)s2 << 8) | s3;

  uint8_t c1_1 = _spi->transfer(0x00);
  uint8_t c1_2 = _spi->transfer(0x00);
  uint8_t c1_3 = _spi->transfer(0x00);
  int32_t val1 = ((int32_t)c1_1 << 16) | ((int32_t)c1_2 << 8) | c1_3;
  if (val1 & 0x00800000) val1 |= 0xFF000000; 
  *ch1 = val1;

  uint8_t c2_1 = _spi->transfer(0x00);
  uint8_t c2_2 = _spi->transfer(0x00);
  uint8_t c2_3 = _spi->transfer(0x00);
  int32_t val2 = ((int32_t)c2_1 << 16) | ((int32_t)c2_2 << 8) | c2_3;
  if (val2 & 0x00800000) val2 |= 0xFF000000;
  *ch2 = val2;

  digitalWrite(_csPin, HIGH);
  _spi->endTransaction();
  return true;
}
