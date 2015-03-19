#define USESERIAL true

/* 
    Software I2C library
    http://playground.arduino.cc/Main/SoftwareI2CLibrary
    https://github.com/felias-fogg/SoftI2CMaster

    PORTB (digital pin 8 to 13)
    PORTC (analog input pins)
    PORTD (digital pins 0 to 7)
*/
#define SCL_PIN 0 
#define SCL_PORT PORTC 
#define SDA_PIN 1 
#define SDA_PORT PORTC 
#include <SoftI2CMaster.h>

/*
*   IRremote library
*/
#include <IRremote.h>
const uint8_t RECV_PIN = A3;
IRrecv irrecv(RECV_PIN);
decode_results results;

/*
*    sevenSeg
*/
const uint8_t displayCount = 2;
const uint8_t displayCommonPins[2] = {10, 11};
const uint8_t displayPins[8] = {
    5, //A
    3, //B
    4, //C
    6, //D
    8, //E
    7, //F
    9, //G
    2  //H (Point)
};

/*

*/
enum Cmd {
    CMD_NONE,
    CMD_VOLUP,
    CMD_VOLDW,
    CMD_MUTE
};

byte mode = CMD_NONE;
long modeTimout;

uint8_t dB; //current Attenuation
bool muted; //muted

const int muteCooldown = 500;
long muteRepeat; //prevent mute repeting

void setup() {
    
    //Initialize I2C
    delay(200);
    i2c_init();
    evc_mute(true);
    
    //Set Initial volume
    dB = 80; //minimum volume
    dB -= 25; //increase volume by 20dB 
    evc_2ch(dB);
    evc_mute(false);

    #if USESERIAL==true
    Serial.begin(9600);
    #endif

    //Initialize IR
    irrecv.enableIRIn();
    //irrecv.blink13(true);

    //Initialize display
    sevenSeg_setup();
    sevenSeg_setNumber(79-dB);

    //setupInterrupt();
    
    //led Blink
    pinMode(13, OUTPUT);
}




void loop() {
    
    unsigned long currentMillis = millis();

    byte action = processRemote();

    switch(action){
         
        case CMD_VOLUP:

            if(dB>0){
                evc_2ch(--dB); //decrease attenuation (vol up)

                //viewModeNormal();
                viewModeAnimVolUp();

                #if USESERIAL==true
                    Serial.print(F("\tVOL+"));
                    Serial.print(F("\t-"));
                    Serial.print(dB);
                    Serial.print(F("dB"));
                #endif
            }

            //when turning volume up, automatically unmute if muted.
            if(muted){
                muted = false;
                evc_mute(false);

                viewModeAnimVolUp();

                #if USESERIAL==true
                    Serial.print(F("\tUNMUTED"));
                #endif
            }    
        break;
         
        case CMD_VOLDW:
            if(dB<79){
                evc_2ch(++dB); //increase attenuation (vol down)

                //viewModeNormal();
                viewModeAnimVolDw();

                #if USESERIAL==true
                    Serial.print(F("\tVOL-"));
                    Serial.print(F("\t-"));
                    Serial.print(dB);
                    Serial.print(F("dB"));
                    if(muted) Serial.print(F("\tMUTED"));
                #endif
            }
            else if(dB==79){ //volume 0 is muted.
                dB=80;
                muted = true;
                evc_mute(muted);
                viewModeNormal();
                
                #if USESERIAL==true
                    Serial.print(F("\tMUTED"));
                #endif
            }
         break;
         
        case CMD_MUTE:
            if(currentMillis - muteRepeat > muteCooldown){ //500ms cooldown to prevent mute from toggling 

                muted = !muted;
                evc_mute(muted);

                if(muted){
                    if(dB>78){
                        dB=80;
                    }
                    viewModeAnimMute();
                }
                else{ //unmute
                    if(dB==80){
                        dB=79;
                    }
                    viewModeNormal();
                }
                
                #if USESERIAL==true
                    Serial.print(F("\tMUTE "));
                    (muted)?Serial.print(F("ON")):Serial.print(F("OFF"));
                #endif

            }
            muteRepeat = currentMillis;
        break;
         
        default: //idle if no command is received
        //do nothing
        break;
    }
    
    displayRoutine();
}

