/*!
* \file    Ultrasound.ino
* \author  Olivier Herrou
* \version 1.0
* \date    December 2015
* \pre     Target : MSP432P401R Launchpad
* \brief   Using a HC-SR04 Ultrasonic Ranging Module, print the distance between the module and the obstacle in front of it.
           If the obstacle is in range the Red led will shine otherwise the Yellow one will.
* \remarks Connect pinRedLed and pinYellowLed to your own circuit with the Leds and 220ohm resistors
*/

/*! Trigger pin */
#define pinTrig 39
/*! Echo pin */
#define pinEcho 40
/*! Red Led */
#define pinRedLed 32
/*! Yellow Led */
#define pinYellowLed 31

boolean bIsInRange = 0;
boolean bPrevious = 0;

void setup() 
{
  Serial.begin (9600);
  pinMode(pinTrig, OUTPUT);
  pinMode(pinEcho, INPUT);
  pinMode(pinRedLed, OUTPUT);
  pinMode(pinYellowLed, OUTPUT);
}

void loop()
{
  int iDistance = ultrasonDistance();  
  
  if (iDistance >= 300 || iDistance <= 0)
  {
    bIsInRange = 0;
    Serial.println("#");
  }
  else 
  {
    bIsInRange = 1;
    Serial.println(iDistance);
  }
  
  if(bIsInRange == 1  && bPrevious == 0)
  {
    bPrevious = bIsInRange;
    digitalWrite(pinRedLed,LOW);
    digitalWrite(pinYellowLed,HIGH);
  }
  else if(bIsInRange == 0  && bPrevious == 1)
  {    
    bPrevious = bIsInRange;
    digitalWrite(pinRedLed,HIGH); 
    digitalWrite(pinYellowLed,LOW);
  }
  delay(300);  
}


/*!
*  \fn      int ultrasonDistance(void)
*  \brief   Function to get the distance of the obstacle send by the HC-SR04 sensor
*  \remarks More information on the HC-SR04 datasheet
*  \return  the distance
*/
int ultrasonDistance()
{
  int iDistance = 0, iDuration = 0;
  digitalWrite(pinTrig, LOW); 
  delayMicroseconds(3); 
  digitalWrite(pinTrig, HIGH);
  delayMicroseconds(15);
  digitalWrite(pinTrig, LOW);

  iDuration = pulseIn(pinEcho, HIGH);
  //iDistance = iDuration *0.034/2;
  iDistance = (iDuration/2) / 29.1;
  return iDistance;
}
