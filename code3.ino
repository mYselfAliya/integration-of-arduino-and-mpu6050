// Define MPU6050 I2C address
#define MPU6050_ADDRESS 0x68

// Define SDA and SCL pins
#define SDA_PIN A4
#define SCL_PIN A5

void i2cDelay() {
  delayMicroseconds(50);
}

void setSDA(bool state) {
  if (state)
    pinMode(SDA_PIN, INPUT_PULLUP);
  else {
    pinMode(SDA_PIN, OUTPUT);
    digitalWrite(SDA_PIN, LOW);
  }
}

void setSCL(bool state) {
  if (state)
    pinMode(SCL_PIN, INPUT_PULLUP);
  else {
    pinMode(SCL_PIN, OUTPUT);
    digitalWrite(SCL_PIN, LOW);
  }
}

bool readSDA() {
  pinMode(SDA_PIN, INPUT);
  return digitalRead(SDA_PIN);
}

void i2cStart() {
  setSDA(true);
  setSCL(true);
  i2cDelay();
  setSDA(false);
  i2cDelay();
  setSCL(false);
}

void i2cStop() {
  setSDA(false);
  setSCL(true);
  i2cDelay();
  setSDA(true);
  i2cDelay();
}

bool i2cWrite(byte data) {
  for (int i = 0; i < 8; i++) {
    setSDA((data & 0x80) != 0);
    setSCL(true);
    i2cDelay();
    setSCL(false);
    data <<= 1;
  }
  // ACK bit
  setSDA(true); // Release SDA
  setSCL(true);
  i2cDelay();
  bool ack = !readSDA();
  setSCL(false);
  return ack;
}

byte i2cRead(bool ack) {
  byte data = 0;
  setSDA(true); // Release SDA for input
  for (int i = 0; i < 8; i++) {
    data <<= 1;
    setSCL(true);
    i2cDelay();
    if (readSDA()) data |= 1;
    setSCL(false);
    i2cDelay();
  }
  // Send ACK or NACK
  setSDA(!ack);
  setSCL(true);
  i2cDelay();
  setSCL(false);
  setSDA(true);
  return data;
}

void setup() {
  Serial.begin(9600);
  pinMode(SDA_PIN, INPUT_PULLUP);
  pinMode(SCL_PIN, INPUT_PULLUP);

  // Wake up MPU6050 (write 0x00 to PWR_MGMT_1)
  i2cStart();
  i2cWrite(MPU6050_ADDRESS << 1);
  i2cWrite(0x6B);
  i2cWrite(0x00);
  i2cStop();
}
// Global state
float velX = 0, velY = 0, posX = 0, posY = 0;
unsigned long lastTime = 0;

void loop() {
  // Request ACCEL_XOUT_H
  i2cStart();
  i2cWrite(MPU6050_ADDRESS << 1);
  i2cWrite(0x3B);
  i2cStop();

  i2cStart();
  i2cWrite((MPU6050_ADDRESS << 1) | 0x01);
  int16_t ax = (i2cRead(true) << 8) | i2cRead(true);  // X
  int16_t ay = (i2cRead(true) << 8) | i2cRead(true);  // Y
  int16_t az = (i2cRead(true) << 8) | i2cRead(false); // Z
  i2cStop();

  // Convert to g
  float xg = ax / 16384.0;
  float yg = ay / 16384.0;
  float zg = az / 16384.0 - 1.0; // remove gravity

  float mag = sqrt(xg*xg + yg*yg + zg*zg);

  unsigned long now = millis();
  float dt = (now - lastTime) / 1000.0;
  lastTime = now;

  if (mag > 0.15) {
    float xm = xg * 9.81;
    float ym = yg * 9.81;

    velX += xm * dt;
    velY += ym * dt;

    posX += velX * dt;
    posY += velY * dt;
  } else {
    velX = velY = 0; // Stop drift when still
  }

  Serial.print("X: "); Serial.print(posX, 2); Serial.print(" m  ");
  Serial.print("Y: "); Serial.println(posY, 2);
  delay(50);
}
