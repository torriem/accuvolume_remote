/* 
 Serial 7 Segment Display : System Functions

 See main file for license and information.

 These are all the lower level system functions that allow Serial 7 Segment to run
 
 */

//Sets up the hardware pins to control the 7 segments and display type
void setupDisplay()
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

  //This pinout is for OpenSegment PCB layout
  //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
#if DISPLAY_TYPE == OPENSEGMENT
  int digit1 = 9; //Pin 12 on my 4 digit display
  int digit2 = 16; //Pin 9 on my 4 digit display
  int digit3 = 17; //Pin 8 on my 4 digit display
  int digit4 = 3; //Pin 6 on my 4 digit display

  //Declare what pins are connected to the segments
  int segA = 14; //Pin 11 on my 4 digit display
  int segB = 2; //Pin 7 on my 4 digit display
  int segC = 8; //Pin 4 on my 4 digit display
  int segD = 6; //Pin 2 on my 4 digit display
  int segE = 7; //Pin 1 on my 4 digit display
  int segF = 15; //Pin 10 on my 4 digit display
  int segG = 4; //Pin 5 on my 4 digit display
  int segDP= 5; //Pin 3 on my 4 digit display

  int numberOfDigits = 4; //Do you have a 2 or 4 digit display?

  //The 1" SparkFun displays are common cathode but because of the PNP and NPN
  //transistor configuration we are using, we need inverted signals:
  //1 to turn a digit on, 0 to turn a segment on
  //This is the same as a common anode setup.
  int displayType = COMMON_ANODE; 

  //Initialize the SevSeg library with all the pins needed for this type of display
  myDisplay.Begin(displayType, numberOfDigits, 
  digit1, digit2, digit3, digit4, 
  segA, segB, segC, segD, segE, segF, segG, 
  segDP);

#endif
  //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

  //This pinout is for the original Serial7Segment layout
  //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
#if DISPLAY_TYPE == S7S
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
#endif
  //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
  
  //This pinout is for the original Serial 7 Segment Shield
  //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
#if DISPLAY_TYPE == S7SHIELD
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

  int numberOfDigits = 4; //Do you have a 2 or 4 digit display?

  int displayType = COMMON_CATHODE; 

  //Initialize the SevSeg library with all the pins needed for this type of display
  myDisplay.Begin(displayType, numberOfDigits, 
  digit1, digit2, digit3, digit4, 
  segA, segB, segC, segD, segE, segF, segG, 
  segDP);
#endif
  //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
}


//Check to see if we need an emergency system reset
//Scan the RX pin for 2 seconds
//If it's low the entire time, then reset the system settings
void checkEmergencyReset(void)
{
  pinMode(0, INPUT); //Turn the RX pin into an input
  digitalWrite(0, HIGH); //Push a 1 onto RX pin to enable internal pull-up

  //Quick pin check
  if(digitalRead(0) == HIGH) return;

  myDisplay.SetBrightness(100); //Set display to 100% brightness during emergency reset so we can see it

  //Wait 2 seconds, displaying reset-ish things while we wait
  for(uint8_t i = 0 ; i < 10 ; i++)
  {
    constantDisplay("____", 200);
    if(digitalRead(0) == HIGH) return; //Check to see if RX is not low anymore

    constantDisplay("----", 200);
    if(digitalRead(0) == HIGH) return; //Check to see if RX is not low anymore
  }

  //If we make it here, then RX pin stayed low the whole time

    //Now sit in a loop indicating system is now at 9600bps
  while(digitalRead(0) == LOW)
  {
    constantDisplay("000-", 500);
    constantDisplay("00-0", 500);
    constantDisplay("0-00", 500);
    constantDisplay("-000", 500);
  }

  //Once we breakout of this loop (pin on RX is removed), system will init with new default settings
}
//Given a string, displays it costantly for a given amount of time
void constantDisplay (char *theString, long amountOfTime)
{
  long startTime = millis();
  while( (millis() - startTime) < amountOfTime)
    myDisplay.DisplayString(theString, 0); //(numberToDisplay, decimal point location)
}
