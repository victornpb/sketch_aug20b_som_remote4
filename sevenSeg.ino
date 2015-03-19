/*
 SevenSeg - 1.0
 
 created 20 August 2014
 by Victor N. Borges - www.vitim.us
 
 This arduino sketch provides software to drive 7-segments display with multiplexing.
 Only turns a single segment is turned on at time, this allows the display to be turned with just the pins current,
 and only requires a single resistor per display on the common pin.
 
 Displays are connected directly to the arduinos output pins, with one current limiting resistor on the common pin.
 For more than 1 display wire then in parallel, and connect the common pin to different I/O pins, so they can be selected.
 
 This code is assyncronous, and will block other pieces of code like using delay.
 
 call sevenSeg_setup() on your setup function, and call sevenSeg_displayHold(unsigned int us); inside the loop()
 the 'us' parameter is the interval between segments in microseconds, adjust this value depending on how long your
 whole program takes to execute.
 If you code alredy takes too much time to execute you can try to call sevenSeg_display() directly on your loop instead.
 
*/

//Change settings to match Common Cathode/Anode
#define SEGMENT_ON HIGH
#define COMMON_ON LOW

#define ALPHA 0 //change to 1 to include leters 

const byte sevenSeg_font[] = {
    B11111100,  // 0
    B01100000,  // 1
    B11011010,  // 2
    B11110010,  // 3
    B01100110,  // 4
    B10110110,  // 5
    B10111110,  // 6
    B11100000,  // 7
    B11111110,  // 8
    B11110110,  // 9
    
    //B00000010, // dash -
    //B00010000, // underscore _
  
  #if ALPHA==1
    B11101110, //A
    B00111110, //b
    B10011100, //C
    B00011010, //c
    B01111010, //d
    B10011110, //E
    B10001110, //F
    B01101110, //H
    B00101110, //h
    B00011100, //L
    B01100000, //l
    B11111100, //O
    B00111010, //o
    B11001110, //P
    B10110110, //S
  #endif
  
  B00000000,  // blank
};


byte displayData[2]; //hold current segments information

uint8_t currentSegment; //number of the current segment in the multiplexing
uint8_t currentDigit; //multiplexing for multiple digits

/** Call this on setup to set pinsMode */
void sevenSeg_setup(){
    uint8_t i;

    i=displayCount;
    while(i--)
        pinMode(displayCommonPins[i], INPUT);

    i=8;
    while(i--)
        pinMode(displayPins[i], OUTPUT);
}

/**  */
void sevenSeg_setNumber(unsigned int n){
    sevenSeg_setDigit(1, sevenSeg_font[n/10]);
    sevenSeg_setDigit(0, sevenSeg_font[n%10]);
}

/** Set a display digit
* @param uint8_t digit - Display Index
* @param byte character - 7bit 0 paded character
*/
void sevenSeg_setDigit(uint8_t digit, byte character){
    displayData[digit] = character | (displayData[digit] & B00000001); //set digit without changing the decimal point bit
}

/** Toggle the decimal point on/off
* @param uint8_t digit - Display Index
* @param bool on - state true/false
*/
void sevenSeg_setDecimalPoint(uint8_t digit, bool on){
    displayData[digit] = (displayData[digit]|1) & (on?B11111111:B11111110); //flip the decimalPoint bit without changing the rest
}


/**
* Function that perform the multiplexing to draw every digit with a assyncronous delay between segments 
* if your program have enough delay, call sevenSeg_display() directly instead.
* @param unsigned int us - adjust the interval between segments in microseconds
*/
void sevenSeg_displayHold(unsigned int us){
    
    static long previousMicros = 0; //variable to keep track of interval between segments
    unsigned long currentMicros = micros();
    //unsigned long currentMillis = millis();

    if(currentMicros - previousMicros > us){
        previousMicros = currentMicros;

        sevenSeg_display();
    }
}

/** function that perform the multiplexing to draw every digit */
void sevenSeg_display(){
    currentSegment = sevenSeg_scanSegments(displayData[currentDigit], currentSegment);

    //change displays after scaning every segment on a single digit
    if(currentSegment>7){
        currentSegment=-1;
        sevenSeg_switchDisplay();
    }
}

/** switch to next display */
void sevenSeg_switchDisplay(){

    //switch off current display
    pinMode(displayCommonPins[currentDigit], INPUT);

    currentDigit = (currentDigit+1) % displayCount; //next

    pinMode(displayCommonPins[currentDigit], OUTPUT);
    digitalWrite(displayCommonPins[currentDigit], COMMON_ON);
}


/** function that advance to each segment every time this is called */
int sevenSeg_scanSegments(byte data, uint8_t segmentNum){

    byte segment;
    bool state;

    digitalWrite(displayPins[segmentNum], !SEGMENT_ON); //turn previous segment OFF

    do{
        ++segmentNum; //cicle through the segments 0 to 7
        if(segmentNum>8) return segmentNum; //end, go to next digit

        segment = data & (0x1<<(7-segmentNum)); //apply a mask to select just the desired segment bit
        state = (segment>>(7-segmentNum)); //shift the bit to LSF to get the boolean value
    }
    while(!state); //skip the dead segments time

    digitalWrite(displayPins[segmentNum], SEGMENT_ON); //turn segment ON

    return segmentNum;
}


