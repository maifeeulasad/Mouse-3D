#include<Wire.h>
const int MPU = 0x68;
int t = 0, dt = 1;
int AcX, AcY, AcZ, GyX, GyY, GyZ, tmp;
int AcXo, AcYo, AcZo, GyXo, GyYo, GyZo;
float roll = 0, pitch = 0, rollgy = 0, pitchgy = 0, rollac = 0, pitchac = 0, Ax, Ay, Az, Gx, Gy, Gz, gain = 0.95;
void MPUconfig(int Addr, int data)
{
  Wire.beginTransmission(MPU);
  Wire.write(Addr);
  Wire.write(data);
  Wire.endTransmission();
}
void MPUread()
{
  Wire.beginTransmission(MPU);
  Wire.write(0x3B);  // starting with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission();
  Wire.requestFrom(MPU, 14); // request a total of 14 registers
  AcX = Wire.read() << 8 | Wire.read(); // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)
  AcY = Wire.read() << 8 | Wire.read(); // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
  AcZ = Wire.read() << 8 | Wire.read(); // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)
  tmp = Wire.read() << 8 | Wire.read(); // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)
  GyX = Wire.read() << 8 | Wire.read(); // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
  GyY = Wire.read() << 8 | Wire.read(); // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)
  GyZ = Wire.read() << 8 | Wire.read();
}
void offset()
{
  MPUconfig(0x1A, 0b00000000);
  int ax = 0, ay = 0, az = 0, gx = 0, gy = 0, gz = 0, i;
  MPUread();
  MPUread();
  ax = AcX;
  ay = AcY;
  az = AcZ;
  gx = GyX;
  gy = GyY;
  gz = GyZ;
  for (i = 0; i < 1100; i++)
  {
    MPUread();
    if (i > 100)
    {
      ax = (ax + AcX) / 2;
      ay = (ay + AcY) / 2;
      az = (az + AcZ) / 2;
      gx = (gx + GyX) / 2;
      gy = (gy + GyY) / 2;
      gz = (gz + GyZ) / 2;
    }
    delay(2);
  }
  AcXo = ax;
  AcYo = ay;
  AcZo = 16384 - az;
  GyXo = gx;
  GyYo = gy;
  GyZo = gz;
  MPUconfig(0x1A, 0b00000110);
}
void setup()
{
  Serial.begin(115200);
  Wire.begin();
  MPUconfig(0x1A, 0b00000110);
  MPUconfig(0x1B, 0b00000000);
  MPUconfig(0x1C, 0b00000000);
  MPUconfig(0x6B, 0b00000000);
  offset();
}

void loop()
{
  t = millis();
  MPUread();
  Ax = (float)(AcX - AcXo) / 16384;
  Ay = (float)(AcY - AcYo) / 16384;
  Az = (float)(AcZ - AcZo) / 16384;
  Gx = (float)(GyX - GyXo) / 131;
  Gy = (float)(GyY - GyYo) / 131;
  Gz = (float)(GyZ - GyZo) / 131;
  pitchgy = (Gy * ((float) dt / 1000)) + pitch;
  rollgy = (Gx * ((float) dt / 1000)) + roll;
  pitchac = atan2(Ax, Az) * (float)(180 / PI);
  rollac =  atan2(Ay, Az) * (float) (180 / PI);
  roll = gain * rollgy + (1 - gain) * rollac;
  pitch = gain * pitchgy + (1 - gain) * pitchac;
  Serial.print(roll);
  Serial.print(" "); 
  Serial.print(pitch);
  Serial.println(" ");
  dt = millis() - t;
}
