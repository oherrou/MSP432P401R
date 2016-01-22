/*!
* \file    xxxxxx.pde
* \author  Olivier Herrou
* \version 1.0
* \date    January 2016
* \brief   This program is the interface between your MSP432 and your Android phone.
           Running an application, the orientation of your smartphone will be send to your PC which will display a clone of your phone.
           The data received will be computed by the PC and send to the MSP432p401r to light its RGB led.
           The Azimuth is the Blue component.
           The Roll is the Red component.
           The Pitch is the Green component.
           
      MSP432p401r                 PC                      Android
        Energia               Processing                    AI2
       ---------            ---------------              ---------    
      |      USB|----------|      Bluetooth|->        <-|Bluetooth|
      |         |          |               |            |         |
      |         |          |               |            |         |
      |         |          |               |            |         |
      |         |          |               |            |         |
           
* \pre     All devices should run their respective programs
           MSP432p401r : xxxxx.ino
           PC          : xxxxx.pde
           Android     : xxxxx.apk
* \warning Work in progress 
*/


/*

Communication entre Android <-> PC :
 - Gyroscope envoie les informations pour guider le repos sous formes de trames
  R,value|P,value|A,value|

Communication entre MSP <-> PC
 Le PC reçoit une trame depuis Android S,Value| qui indique la valeur de la slidebar
 Le PC transmet alors cette information au MSP
 Le MSP recoit l'info et renvoie la trame M,1| si tout s'est bien passé.
 
Responses data structure : Opcode,value 1,value 2,...,value n|
 * Opcode :
  -> 3 bytes, these bytes describe the instruction. 
  -> Check the opcode table.
  -> 0x20 - 0x3F : Destination is PC
  -> 0x40 - 0x5F : Destination is Android
  -> 0x60 - 0x7F : Destination is MSP
  
 * Value :
  -> The bytes following the opcode are part of the instruction
  -> Check the opcode table for more informations
   
  Opcode Table :
  -> 0x20 : Android sending texts to PC
     "100" for now
  -> 0x21 : Android sending Roll,Pitch,Azimuth values to PC
     "101" for now
  -> 0x22 : Android sending Slider value to PC
     "102" for now
  -> 120 : MSP sending ultrason datas
  
  
*/


import processing.serial.*;

Serial portAndroid;
Serial portMSP;
PImage imgRobot;
PImage imgArrowUp;
PImage imgArrowDown;
PImage imgArrowLeft;
PImage imgArrowRight;
PImage imgArrowUp2;
PImage imgArrowDown2;
PImage imgArrowLeft2;
PImage imgArrowRight2;
int iPitch = 0; // -90 90
int iRoll = 0;   // -90 90
int iAzimuth = 0; // 0 360
int iSlider = 0; // 0 255
int iTolerance = 5;
int iSizeAxes = 150;
String szDirectionHor = "";
String szDirectionVert = "";
boolean bAndroidMod = true;

void setup()
{
  size(800, 600, OPENGL); // Don't change size !
  portAndroid = new Serial(this, "COM9", 9600);
  portAndroid.bufferUntil('|');
 // portMSP = new Serial(this, "COM5", 9600);
  //portMSP.bufferUntil('|');
  imgRobot       = loadImage("robot.png");
  imgArrowRight  = loadImage("arrow_right.png");
  imgArrowLeft   = loadImage("arrow_left.png");
  imgArrowUp     = loadImage("arrow_up.png");
  imgArrowDown   = loadImage("arrow_down.png");
  imgArrowRight2 = loadImage("arrow_right2.png");
  imgArrowLeft2  = loadImage("arrow_left2.png");
  imgArrowUp2    = loadImage("arrow_up2.png");
  imgArrowDown2  = loadImage("arrow_down2.png");
}

void draw()
{
  background(5,45,89);
  drawInterface();
  
  /* Compute the direction */
  // Android landscape Roll => directionVert, Pitch directionHor
  if(bAndroidMod) // Android controls the robot
  {
    if(iRoll > 15) szDirectionHor = "Right"; 
    else if(iRoll < -15) szDirectionHor = "Left"; 
    else szDirectionHor = "";
    if(iPitch < -50) szDirectionVert = "Backward";
    else if(iPitch > -20) szDirectionVert = "Forward";
    else szDirectionVert ="";
    hint(ENABLE_DEPTH_TEST); 
    drawPhoneOrientation();
    hint(DISABLE_DEPTH_TEST);
  }
  
  //updateMSPLeds();
}


/*!
*  \fn      void serialEvent(Serial whichPort)
*  \brief   Compute the data received on the serial port
*  \return  none
*/
void serialEvent(Serial whichPort)
{
  
 if(whichPort == portMSP)
 {
   String data2 = portMSP.readStringUntil('|');
   data2 = data2.substring(0,data2.length()-1);
   println(data2);
 }
 
 if(whichPort == portAndroid)
 {
   String szData = portAndroid.readStringUntil('|');
   szData = szData.substring(0,szData.length()-1); 
   int iIndex1 = szData.indexOf(",");
   String szOpcode = szData.substring(0, iIndex1); // Get the opcode
   String szValues = szData.substring(iIndex1+1, szData.length()); // Get the values
   
   if(szOpcode.equals("101") && bAndroidMod) // Receiving Orientation from Android
   {
     updateOrientation(szValues);
   }
   else if(szOpcode.equals("110"))
   {
     iSlider = int(szValues);
     portMSP.write(iSlider);
   }
  }
}

/*!
*  \fn      void updateOrientation(String szValues)
*  \brief   Compute the data received on the serial port
*  \param   szValues the data from Android giving information about the orientation of the phone
*  \return  none
*/
void updateOrientation(String szValues)
{
   int iIndex1 = szValues.indexOf(",");
   String szCurrentValue = szValues.substring(0, iIndex1);
   int iCurrentValue;
   
   // Roll
   iCurrentValue = -1*int(szCurrentValue);
   if( ((iRoll - iTolerance) > (-1*iCurrentValue)) || ((-1*iCurrentValue) > (iRoll + iTolerance)) )
   {
      iRoll = -iCurrentValue;
   }
   
   //Pitch
   szValues = szValues.substring(iIndex1+1, szValues.length());
   iIndex1 = szValues.indexOf(",");
   szCurrentValue = szValues.substring(0, iIndex1);
   iCurrentValue = int(szCurrentValue);
   if( ((iPitch - iTolerance) > iCurrentValue) || (iCurrentValue > (iPitch + iTolerance)) )
   {
      iPitch = iCurrentValue;
   }
   
   //Azimuth
   szValues = szValues.substring(iIndex1+1, szValues.length());
   iCurrentValue = int(szValues);
   if( ((iAzimuth - iTolerance) > iCurrentValue) || (iCurrentValue > (iAzimuth + iTolerance)) )
   {
     iAzimuth = iCurrentValue;
   }
}

/*!
*  \fn      void keyPressed()
*  \brief   Check if keys are pressed 
*  \return  none
*/
void keyPressed()
{
  if(key == ' ')
  {
    bAndroidMod = !bAndroidMod;
    if(bAndroidMod == false) 
    {
      iRoll = iPitch = iAzimuth = 0;
    }
  }
  
  if(bAndroidMod == false) // PC mod
  {
    if(key == CODED)
    {
      if(keyCode == UP)
      {
        println("UP");
        szDirectionVert = "Forward";
      }
      else if(keyCode == DOWN)
      {
        println("DOWN");
        szDirectionVert = "Backward";
      }
      else szDirectionVert = "";
      
      if(keyCode == LEFT)
      {
        println("LEFT");
        szDirectionHor = "Left";
      }
      else if(keyCode == RIGHT)
      {
        println("RIGHT");
        szDirectionHor = "Right";
      }
      else szDirectionHor = "";
    }
  }  
}



void updateMSPLeds()
{
     portMSP.write('#');
     byte iValue = byte(map(iRoll,-90,90,0,255)); // Processing are signed but not on Energia so it's fine
     text("Red  : "+iValue, 10,10);
     portMSP.write(iValue); //R
     iValue = byte(map(iPitch,-90,90,0,255));
     text("Green : "+iValue, 10,30);
     portMSP.write(iValue); //G
     iValue = byte(map(iAzimuth,0,360,0,255));
     text("Blue : "+iValue, 10,50);
     portMSP.write(iValue); //B
}



void drawAxes()
{
  int iLengthAxes = 70;
  pushMatrix();   
  translate(width/2, height/2, 0);
  strokeWeight(1);
  stroke(255,0,0);
  line(0, 0, iLengthAxes, 0);
  stroke(0,255,0);
  line(0, -iLengthAxes, 0, 0);      
  stroke(0,0,255);
  line(0, 0, 0, 0, 0, -iLengthAxes);   
  popMatrix();
}

void drawPhoneOrientation()
{
  pushMatrix();
  translate(width/2, height/2, 0);
  rotateZ(radians(iRoll));
  rotateX(radians(iPitch));
  noStroke();
  fill(48,29,14);
  box(100,5,40); //Phone case
  fill(214,214,214);
  translate(0,-5,0);
  box(50,1,20); //screen
  translate(-35,0,0);
  box(5,1,5);
  popMatrix();
}

void drawInterface()
{
  /* Bottom Interface */
  hint(DISABLE_DEPTH_TEST); 
  int iSpace = 10;
  int iWidthSecondary = 180;
  int iHeightSecondary = 200;
  int iWidthPrimary = 400;
  int iHeightPrimary = 200;
  
  // Values rectangle
  int iLblSize = 15; //Size of the text
  int iLblSpace = 13; //Size of the space between texts
  noFill();
  strokeWeight(2);
  stroke(7,180,237); 
  rect(iSpace, height - iHeightSecondary - iSpace, iWidthSecondary, iHeightSecondary,10);
  textSize(iLblSize);
  fill(255);
  text("Speed      : ",iSpace + iLblSpace,height - iHeightSecondary + iLblSpace);
  text("Direction : "+szDirectionVert,iSpace + iLblSpace,height - iHeightSecondary + 2*iLblSpace + iLblSize);
  text("                "+szDirectionHor,iSpace + iLblSpace,height - iHeightSecondary + 3*iLblSpace + 2*iLblSize);
  text("Info X      : "+mouseX,iSpace + iLblSpace,height - iHeightSecondary + 4*iLblSpace + 3*iLblSize);
  text("Roll         : "+iRoll,iSpace + iLblSpace,height - iHeightSecondary + 5*iLblSpace + 4*iLblSize);
  text("Pitch       : "+iPitch,iSpace + iLblSpace,height - iHeightSecondary + 6*iLblSpace + 5*iLblSize);
  text("Azimuth  : "+iAzimuth,iSpace + iLblSpace,height - iHeightSecondary + 7*iLblSpace + 6*iLblSize);
  
  //Debugging
  // Ultrason
  noFill();
  stroke(17,105,49); 
  rect(200, iSpace , iWidthPrimary, iHeightPrimary,10);

  
  // Robot view rectangle
  noFill();
  stroke(235,138,28); 
  rect(200, height - iHeightSecondary - iSpace, iWidthPrimary, iHeightPrimary,10);
  pushMatrix();
  translate(width/2, height/2 + 190 );
  rotate(radians(iPitch));
  translate(-imgRobot.width/2, -imgRobot.height/2);
  image(imgRobot,0,0);
  popMatrix();
  
  // Key  
  noFill();
  stroke(7,180,237); 
  rect(610, height - iHeightSecondary - iSpace, iWidthSecondary, iHeightSecondary,10);
  if(szDirectionVert.equals("Forward")) image(imgArrowUp2,678,height-105-45,40,40);
  else image(imgArrowUp,678,height-105-45,40,40);
  
  if(szDirectionVert.equals("Backward")) image(imgArrowDown2,678,height-105,40,40);
  else image(imgArrowDown,678,height-105,40,40);
  
  if(szDirectionHor.equals("Left")) image(imgArrowLeft2,630,height-105,40,40);
  else image(imgArrowLeft,630,height-105,40,40);
  
  if(szDirectionHor.equals("Right")) image(imgArrowRight2,728,height-105,40,40);
  else image(imgArrowRight,728,height-105,40,40);
  
  // Android/PC switch
  stroke(42,133,24); 
  rect(630, height-50, 140, 30,10);
  if(bAndroidMod)
  {
    textSize(16);
    text("Android Remote",640,height-30);
  }
  else
  {
    textSize(17);
    text("PC Remote",655,height-30);
  }
  
  // Phone orientation
  noFill();
  stroke(7,180,237); 
  rect(320, height/2 - 80 , 160, 160,10);
  drawAxes();
}