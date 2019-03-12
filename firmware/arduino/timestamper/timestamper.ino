#include <SPI.h>
#include <Wire.h>

#define PIN_INT       2
#define PIN_CE        3
#define PIN_RST_CAPT  4
#define PIN_MRST_N    5

#define CDCE925_ADDR  0x64

void write_reg(uint8_t addr, uint8_t reg, uint8_t data)
{
  Wire.beginTransmission(addr);
  Wire.write(reg);
  Wire.write(data);
  Wire.endTransmission();
}

void write_bulk(uint8_t addr, uint8_t startReg, uint8_t* data, uint8_t len)
{
  Wire.beginTransmission(addr);
  Wire.write(startReg);
  Wire.write(len);
  for (int i = 0; i < len; i++)
  {
    Wire.write(data[i]);
  }
  Wire.endTransmission();
}

uint8_t read_reg(uint8_t addr, uint8_t reg)
{
  Wire.beginTransmission(addr);
  Wire.write(reg);
  Wire.endTransmission(false);

  Wire.requestFrom(addr, (uint8_t) 1, true);

  return Wire.read();
}

void read_bulk(uint8_t addr, uint8_t startReg, uint8_t* data, uint8_t len)
{
  Wire.beginTransmission(addr);
  Wire.write(startReg);
  Wire.write(len);
  Wire.endTransmission();

  Wire.requestFrom(addr, len, true);

  for (int i = 0; i < len; i++)
  {
    data[i] = Wire.read();
  }
}

const uint8_t cdce925_regs[] = {
  //0x01, 0x00, 0xb4, 0x02, 0x02, 0x50, 0x60, 0x00,
  //0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  //0x00, 0x00, 0x00, 0x00, 0x6d, 0x02, 0x00, 0x05,
  //0xff, 0x00, 0x02, 0x83, 0xff, 0x00, 0x02, 0x80,
  //0x00, 0x00, 0x00, 0x00, 0x6d, 0x02, 0x00, 0x05,
  //0xfd, 0x25, 0x12, 0x02, 0xfd, 0x25, 0x12, 0x00,

  //0x01,
  //0x08, // LVCMOS
  //0x00, // XTAL
  //0xb4, 0x02, 0x02, 0x50, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x6d, 0x02, 0x00, 0x05, 0xff, 0x00, 0x02, 0x83, 0xff, 0x00, 0x02, 0x80, 0x00, 0x00, 0x00, 0x00, 0x6d, 0x02, 0x00, 0x05, 0xfd, 0x25, 0x12, 0x02, 0xfd, 0x25, 0x12, 0x00,


  // Wit h added 8MHz output
  0x01, 0x08, 0xb4, 0x02, 0x02, 0x50, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x6d, 0x02, 0x19, 0x05, 0xff, 0x00, 0x02, 0x83, 0xff, 0x00, 0x02, 0x80, 0x00, 0x00, 0x00, 0x00, 0x6d, 0x02, 0x00, 0x05, 0xfd, 0x25, 0x12, 0x02, 0xfd, 0x25, 0x12, 0x00,

  //0x01, 0x00, 0xb4, 0x02, 0x02, 0x50, 0x60, 0x00,
  //0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  //0x00, 0x00, 0x00, 0x00, 0x6d, 0x02, 0x00, 0x05,
  //0xff, 0x00, 0x02, 0x83, 0xff, 0x00, 0x02, 0x80,
  //0x00, 0x00, 0x00, 0x00, 0x6d, 0x02, 0x00, 0x05,
  //0xfd, 0x25, 0x12, 0x02, 0xfd, 0x25, 0x12, 0x00,
};


void cdce925_init()
{

  //write_bulk(CDCE925_ADDR, 0x00, cdce925_regs, sizeof(cdce925_regs));
  for (int i = 0; i < sizeof(cdce925_regs); i++)
  {
    write_reg(CDCE925_ADDR, 0x80 | i, cdce925_regs[i]);
  }
}

void burn()
{
  write_reg(CDCE925_ADDR, 0x86, 0); // Reset write bit
  write_reg(CDCE925_ADDR, 0x86, 1); // Set write bit

  // wait until done
  while (read_reg(CDCE925_ADDR, 0x81) & 0x40 == 0x40)
  {
    // Wait...
  }
}

void setup()
{
  Serial.begin(9600);
  delay(500);
  Serial.print("Initializing...");
  digitalWrite(SDA, 1);
  digitalWrite(SCL, 1);
  Wire.begin();
  digitalWrite(SDA, 1);
  digitalWrite(SCL, 1);

  SPI.begin();

  pinMode(PIN_MRST_N, OUTPUT);
  pinMode(PIN_INT, INPUT);
  pinMode(PIN_CE, OUTPUT);
  pinMode(PIN_RST_CAPT, OUTPUT);

  digitalWrite(PIN_CE, 1);

  /* Reset the CPLD */
  digitalWrite(PIN_MRST_N, 0);
  delayMicroseconds(1);
  digitalWrite(PIN_MRST_N, 1);

  //cdce925_init();

  //burn();
  
  //i2d_read_test();
  Serial.print("OK\r\n");
}


void i2d_read_test()
{
  uint8_t id = read_reg(CDCE925_ADDR, 0x80 | 46);
  Serial.print("id: ");
  Serial.print(id, HEX);
  Serial.print("\r\n");
}


void loop()
{
  if (digitalRead(PIN_INT))
  {
    /* Read the capture register */
    digitalWrite(PIN_CE, 0);
    uint8_t a = SPI.transfer(0x00);
    uint8_t b = SPI.transfer(0x00);
    uint8_t c = SPI.transfer(0x00);
    uint8_t d = SPI.transfer(0x00);
    digitalWrite(PIN_CE, 1);

    /* Re-enable capture */
    digitalWrite(PIN_RST_CAPT, 1);
    //delayMicroseconds(1);
    digitalWrite(PIN_RST_CAPT, 0);

    uint32_t ul = (uint32_t) a << 24 | (uint32_t) b << 16 | (uint32_t) c << 8 | (uint32_t) d;
    Serial.print("capture ");
    Serial.print(millis());
    Serial.print(": ");
    Serial.print(ul);
    Serial.print("\r\n");

    //i2d_read_test();
  }
}
