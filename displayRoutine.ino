/** Led 13 blink on known command received */
namespace ledBlink{
  
  const unsigned int duration = 250; //hold for x ms
  
  bool enabled;
  static long ledBlinkTimeout;
  
  void on(){
      enabled = true;
      ledBlinkTimeout = millis();
      digitalWrite(13, HIGH);
  }
  
  void ledBlinkRoutine(long currentMillis){
      if(enabled){
          if(currentMillis - ledBlinkTimeout > duration){
                enabled = false;
                digitalWrite(13, LOW);
          }
      }
  }
  
}

uint8_t currentViewMode;

typedef enum  {
    VIEWMODE_NORMAL,
    VIEWMODE_ANIM_MUTE,
    VIEWMODE_ANIM_VOLUP,
    VIEWMODE_ANIM_VOLDW,
    VIEWMODE_LOCKED,
    VIEWMODE_IDLE,
} ViewMode;

long animTimeoutLast;
const int animTimeout = 200; //stop animation after amount of ms (smaller than the remote code repeat interval for continuous animation)
const int frameInterval = 45; //speed of animation (duration of each frame in ms) 


const uint8_t framesLenght1 = 5;
const byte frames1[] = {
    B0001000,   // [0] => "1"
    B0001001,   // [1] => "2"
    B1001001,   // [2] => "3"
    B1000001,   // [3] => "4"
    B1000000,   // [4] => "5"
    B0000000,   // [5] => "6"
};

    
//rotating animation
const uint8_t framesLenght2 = 8;
const byte frames2[][2] = {
    {B0000000, B0001000}, //4
    {B0001000, B0000000}, //5
    {B0000100, B0000000}, //6
    {B0000010, B0000000}, //7
    {B1000000, B0000000}, //0
    {B0000000, B1000000}, //1
    {B0000000, B0100000}, //2
    {B0000000, B0010000}, //3
    
};


//filling bar animation
const uint8_t framesLenght3 = 7;
const byte frames3[][2] = {
    {B0000000, B0000000},
    {B0001000, B0001000},
    {B0001001, B0001001},
    {B1001001, B1001001},
    {B1000001, B1000001},
    {B1000000, B1000000},
    {B0000000, B0000000},
};



void viewModeNormal(){
    currentViewMode = VIEWMODE_NORMAL;
}

void viewModeAnimVolUp(){
    animTimeoutLast = millis();
    currentViewMode = VIEWMODE_ANIM_VOLUP;
}

void viewModeAnimVolDw(){
    animTimeoutLast = millis();
    currentViewMode = VIEWMODE_ANIM_VOLDW;
}

void viewModeAnimMute(){
    currentViewMode = VIEWMODE_ANIM_MUTE;
}

void viewModeLocked(){
    currentViewMode = VIEWMODE_LOCKED;
}


void viewModeSet(int a){
    currentViewMode = a;
}


const int muteBlinkInterval = 400;


void displayRoutine(){
    
    unsigned long currentMillis = millis();
    
    static long frameTimeout;
    static uint8_t currentFrame;
    
    
    static long muteBlink; //timing for the blink
    static bool muteBlinkBool;
    
    switch(currentViewMode){
        
        case VIEWMODE_NORMAL:
            sevenSeg_setNumber(79-dB+1);

            sevenSeg_setDecimalPoint(0, false);
            sevenSeg_setDecimalPoint(1, false);

            currentViewMode = VIEWMODE_IDLE;
        break;
    
        case VIEWMODE_ANIM_VOLUP:

            if(currentMillis - frameTimeout > frameInterval){
                frameTimeout = currentMillis;

                if(currentFrame<framesLenght2-1){
                    ++currentFrame;
                }
                else{
                    currentFrame = 0;
                    
                    if(currentMillis - animTimeoutLast > animTimeout){
                        currentViewMode = VIEWMODE_NORMAL;
                        currentFrame = 0;
                    }
                }

                sevenSeg_setDigit(0, frames2[currentFrame][1]<<1);
                sevenSeg_setDigit(1, frames2[currentFrame][0]<<1);
            } 
        break;
    
        case VIEWMODE_ANIM_VOLDW:
    
            if(currentMillis - frameTimeout > frameInterval){
                frameTimeout = currentMillis;
                
                sevenSeg_setDigit(0, frames2[(framesLenght2-1)-currentFrame][1]<<1);
                sevenSeg_setDigit(1, frames2[(framesLenght2-1)-currentFrame][0]<<1);
                
                if(currentFrame<framesLenght2-1){
                    ++currentFrame;
                }
                else{
                    currentFrame = 0;
                    if(currentMillis - animTimeoutLast > animTimeout){
                        currentViewMode = VIEWMODE_NORMAL;
                        currentFrame = 0;
                    }
                }
            }
        break;

        case VIEWMODE_LOCKED:
            sevenSeg_setDigit(0, B00000000);
            sevenSeg_setDigit(1, B00000000);

            sevenSeg_setDecimalPoint(0, true);
            sevenSeg_setDecimalPoint(1, true);

            currentViewMode = VIEWMODE_IDLE; 
        break;

        case VIEWMODE_ANIM_MUTE:
            
            muteBlinkBool = true;
            muteBlink = currentMillis;
            
            sevenSeg_setDigit(0, B00000010);
            sevenSeg_setDigit(1, B00000010);
            
            currentViewMode = VIEWMODE_IDLE; 
        break;
    
        default: //IDLE
        break;
    
    }
    

    
    if(muted){
        if(currentMillis - muteBlink > muteBlinkInterval){
            muteBlink = currentMillis;
            muteBlinkBool = !muteBlinkBool;
            
            if(muteBlinkBool){
                sevenSeg_setDigit(0, B00000010);
                sevenSeg_setDigit(1, B00000010);
            }
            else{
                sevenSeg_setNumber(79-dB+1);
            }
        }
    }
    
    
    ledBlink::ledBlinkRoutine(currentMillis);
    
    //draw segments
    sevenSeg_displayHold(830);
}

