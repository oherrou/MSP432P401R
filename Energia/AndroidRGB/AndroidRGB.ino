/*!
* \file    AndroidRGB.ino
* \author  Olivier Herrou
* \version 1.0
* \date    January 2016
* \brief   Read serial port and wait for the following message :
           #RGB, where 'R', 'G' and 'B' are byte corresponding to the value of each color (0-255)
*/

#define LED1 78
#define Push2 74
char incoming;
int buttonState = 0;
int lastButtonState = 0;
int i =0;
void setup()  
{
  Serial.begin(9600);
  pinMode(Push2, INPUT_PULLUP);  
}
 
  
void loop() 
{
  buttonState = digitalRead(Push2); 
  if (buttonState != lastButtonState)
  {
    if(buttonState == HIGH)
    {
      Serial.println("Appuie");
    }
  }  
  lastButtonState = buttonState;
   
  if(Serial.available())
  {
    incoming= Serial.read();// read incoming char
    if(i==1)
    { 
      analogWrite(RED_LED,incoming);
      i++;
    }
    else if(i==2)
    {
      analogWrite(GREEN_LED,incoming);
      i++;
    }
    else if(i==3)
    {
      analogWrite(BLUE_LED,incoming);
      i = 0;
    }
    if(incoming == '#')
    {
      i = 1;
    }
  }
  
}
