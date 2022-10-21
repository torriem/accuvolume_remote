/* 
 Serial 7 Segment Display : System Functions

 See main file for license and information.

 These are all the lower level system functions that allow Serial 7 Segment to run
 
 */

//Sets up the hardware pins to control the 7 segments and display type
void setupDisplay(SevSeg &myDisplay)
{
  //Determine the display brightness
  byte settingBrightness = EEPROM.read(BRIGHTNESS_ADDRESS);
  if(settingBrightness > BRIGHTNESS_DEFAULT) {
    settingBrightness = BRIGHTNESS_DEFAULT; //By default, unit will be brightest
    EEPROM.write(BRIGHTNESS_ADDRESS, settingBrightness);
  }
  myDisplay.SetBrightness(settingBrightness); //Set the display to 100% bright

  // Set the initial state of displays and decimals 'x' =  off
  display.digits[0] = 'x';
  display.digits[1] = 'x';
  display.digits[2] = 'x';
  display.digits[3] = 'x';
  display.decimals = 0x00;  // Turn all decimals off
  display.cursor = 0;  // Set cursor to first (left-most) digit

  //Declare what pins are connected to the digits

  //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

  //This pinout is for the original Serial7Segment layout
  //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
  int digit1 = 16; // DIG1 = A2/16 (PC2)
  int digit2 = 17; // DIG2 = A3/17 (PC3)
  int digit3 = 3;  // DIG3 = D3 (PD3)
  int digit4 = 4;  // DIG4 = D4 (PD4)

  //Declare what pins are connected to the segments
  int segA = 8;  // A = D8 (PB0)
  int segB = 14; // B = A0 (PC0)
  int segC = 6;  // C = D6 (PD6), shares a pin with colon cathode
  int segD = A1; // D = A1 (PC1)
  int segE = 23; // E = PB7 (not a standard Arduino pin: Must add PB7 as digital pin 23 to pins_arduino.h)
  int segF = 7;  // F = D7 (PD6), shares a pin with apostrophe cathode
  int segG = 5;  // G = D5 (PD5)
  int segDP= 22; //DP = PB6 (not a standard Arduino pin: Must add PB6 as digital pin 22 to pins_arduino.h)

  int digitColon = 2; // COL-A = D2 (PD2) (anode of colon)
  int segmentColon = 6; // COL-C = D6 (PD6) (cathode of colon), shares a pin with C
  int digitApostrophe = 9; // APOS-A = D9 (PB1) (anode of apostrophe)
  int segmentApostrophe = 7; // APOS-C = D7 (PD7) (cathode of apostrophe), shares a pin with F

  int numberOfDigits = 4; //Do you have a 2 or 4 digit display?

  int displayType = COMMON_ANODE; //SparkFun 10mm height displays are common anode

  //Initialize the SevSeg library with all the pins needed for this type of display
  myDisplay.Begin(displayType, numberOfDigits, 
  digit1, digit2, digit3, digit4, 
  digitColon, digitApostrophe, 
  segA, segB, segC, segD, segE, segF, segG, 
  segDP,
  segmentColon, segmentApostrophe);
}

//Given a string, displays it costantly for a given amount of time
/*
void constantDisplay (char *theString, long amountOfTime)
{
  long startTime = millis();
  while( (millis() - startTime) < amountOfTime)
    myDisplay.DisplayString(theString, 0); //(numberToDisplay, decimal point location)
}
*/
