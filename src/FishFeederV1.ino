#define BLYNK_NO_BUILTIN // Disable built-in analog & digital pin operations (saves memory)

// Load Particle library: Blynk
#include "blynk.h"
#include "HC-SR04.h"

//#include <blynk.h>// Make sure you include this library or else it won't work!

Servo servo; /* create servo object to control a servo */

char auth[] = "ACCb2YSlpuo-quhxXIbNd9q3NtSP41Qo";
int FeederMode = 0;
int Spos1 = 0;
int Spos2 = 180;
int StartFeeder = 0;
int Mstate = 0;
int period = 1000;
unsigned long time_now = 0;
long time_pre1 = 0;
long time_pre2 = 0;

int Sstatus = 0;

// trigger / echo pins
const int triggerPin = A0;
const int echoPin = D8;
HC_SR04 rangefinder = HC_SR04(triggerPin, echoPin);

void setup()
{
  pinMode(D2, OUTPUT);                     // D2 is tied to the built-in LED.
  pinMode(D7, OUTPUT);                     // D7 is tied to the built-in LED.
  servo.attach(D3);                      /* attaches the servo on pin D0 to the servo object */
  rangefinder.init();
  //Particle.keepAlive(20min);
  //Particle.connect();
  Blynk.begin(auth, IPAddress(134, 209, 217, 102), 8080);
  delay(2000);                             // 2 second delay.
}

BLYNK_WRITE(V5)
{
  period = param.asInt();
}
BLYNK_WRITE(V4){
  FeederMode = param.asInt();
} 
BLYNK_WRITE(V3)
{
  if(FeederMode==0){
    servo.write(param.asInt());
  }
  
}

BLYNK_WRITE(V2)
{
  Mstate = (param.asInt());
  if (FeederMode == 0)
  {
    //digitalWrite(D2, Mstate);
  }
}
void AutomaticMode()
{
  time_now = millis();

  if (FeederMode == 1)
  {
    digitalWrite(D2, HIGH);
    if (time_now - time_pre1 >= period)
    {
      time_pre1 = time_now;
      if (Sstatus == 0)
      {
        servo.write(Spos1);
        Sstatus=1;
      }
    }
    if (time_now - time_pre2 >= (2.2*period))
    {
      time_pre2 = time_now;
      if (Sstatus == 1)
      {
        servo.write(Spos2);
        Sstatus = 0;
      }
    }
  }
  else
    digitalWrite(D2, Mstate);
}

void sendSensor()
{
  unsigned long start = micros();
  float CM = rangefinder.distCM();
  unsigned long calcTime = micros() - start;
  // You can send any value at any time.
  // Please don't send more that 10 values per second.
  Blynk.virtualWrite(V7, calcTime);
  Blynk.virtualWrite(V6, CM);
}

void loop()
{
  //time_now = millis();
  Blynk.run(); // Start Blynk running.
  AutomaticMode();
  sendSensor();
  
  /*
  if (digitalRead(D2) == HIGH) // If Blynk button for the LED is pressed then do the following...

  {
    Particle.publish("Motor", "On");  //  Publish to particle cloud so the LED turns on

  }

  else
  {
    Particle.publish("Motor", "Off"); // Publish to the cloud so LED turns off
  }
*/
}
