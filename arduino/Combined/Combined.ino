#include <Wire.h>
#include <MPU6050.h>
//read as rool:pitch:yaw:accX:accY:accZ
MPU6050 mpu;
unsigned long timer = 0;
float timeStep = 0.01;
float pitch = 0;
float roll = 0;
float yaw = 0;
float accX = 0;
float accY = 0;
float accZ = 0;
float rollRad = 0;
float pitchRad = 0;
float yawRad = 0;


float gX = 0;
float gY = 0;
float gZ = -9.8;

int count = 0;

Vector minusGravity(Vector vec)
{


  float x = gX, y = gY, z = gZ;
  vec.XAxis -= x * cos(yawRad) - y * sin(yawRad) ;
  vec.YAxis -= x * sin(yawRad) - y * cos(yawRad) ;

  vec.XAxis -=  x * cos(pitchRad) + z * sin(pitchRad) ;
  vec.ZAxis -= - x * sin(pitchRad) + z * cos(pitchRad) ;

  vec.YAxis -=  y * cos(rollRad) - z * sin(rollRad);
  vec.ZAxis -=  y * sin(rollRad) + z * cos(rollRad);


  return vec;
}

float accP(float value)
{
  return value;
  /*
    int x = (int)(value * 10);
    return ((float)(x)) / 10;
  */
}

void setup()
{
  Serial.begin(115200);
  while (!mpu.begin(MPU6050_SCALE_2000DPS, MPU6050_RANGE_2G))
  {

  }
  //mpu.calibrateGyro();
  mpu.setThreshold(3);
  for (int i = 0; i < 10; i++)
  {
    Vector normAccel = mpu.readNormalizeAccel();
    gX += normAccel.XAxis;
    gY += normAccel.YAxis;
    gZ += normAccel.ZAxis;
    Vector norm = mpu.readNormalizeGyro();
    pitch -=  norm.YAxis;
    roll -= norm.XAxis;
    yaw -= norm.ZAxis;
  }
  gX /= 10;
  gY /= 10;
  gZ /= 10;
  roll /= 10;
  pitch /= 10;
  yaw /= 10;
  rollRad = roll * 0.0174533;
  pitchRad = pitch * 0.0174533;
  yawRad = yaw * 0.0174533;
}
void loop()
{
  timer = millis();
  Vector norm = mpu.readNormalizeGyro();
  pitch +=  norm.YAxis * timeStep;
  roll += norm.XAxis * timeStep;
  yaw += norm.ZAxis * timeStep;
  //double them
  Vector normAccel = minusGravity(mpu.readNormalizeAccel());
  rollRad = roll * 0.0174533;
  pitchRad = pitch * 0.0174533;
  yawRad = yaw * 0.0174533;
  Serial.print(roll );
  Serial.print(":");
  Serial.print(pitch );
  Serial.print(":");
  Serial.print(yaw );
  Serial.print(":");
  accX = accP(normAccel.XAxis);
  accY = accP(normAccel.YAxis);
  accZ = accP(normAccel.ZAxis);
  Serial.print(accX);
  Serial.print(":");
  Serial.print(accY);
  Serial.print(":");
  Serial.println(accZ);
  delay((timeStep * 2500) - (millis() - timer));
}
