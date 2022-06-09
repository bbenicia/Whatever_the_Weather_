//light instrument test #2// delete this file after completion ! 




// NEOPIXELS !!! 

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
 #include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif

#define LED_PIN    6

// How many NeoPixels are attached to the Arduino?
#define LED_COUNT 15

// Declare our NeoPixel strip object:
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

struct MyColor {
  long hue;
  int sat;
  int val;
};

int loopCount; // tracks what group loop is on
//Word words[26]; // all words
//Word groups[10]; // groups of words
MyColor colors[12]; // all colors
int waitLengthSlow = 500;
int waitLengthFast= 250;
int glowLengthSlow = 5;
int glowLengthFast= 25;
//int glowDuration = 5000;
int glowDuration = 3000;
int fadeLengthSlow = 10;
int fadeLengthFast= 3;
int fadeDuration = 400;



const int butt0 = 13;
const int butt1 = 12;
const int butt2 = 11;
const int butt3 = 10;

int buttstate0 = 0 ; 
int buttstate1 = 0 ; 
int buttstate2 = 0 ; 
int buttstate3 = 0 ; 


//fsr 
int fsr = A1 ; //change once determine anolog pin 
int fsrread ; 
int fsrstate = 0 ; 


//pott
#define pott A0 //change once you determine the pin 
int readpott ; 
int pottval ; 
int pottstate = 0 ; 

int lastSensors[6]; // keeps track of whether new input is possible
int actionArray[100][3];
int actionPos = 0;

unsigned long timeArray[100];

// ledcol0 = [ 0 , 7 , 8 ] ; 
// ledcol1 = [ 1 , 6, 9 ] ; 
// ledcol2 = [ 2 , 5 , 10 ] ; 
// ledcol3 = [3 , 4 , 11] ; 
int leds[4][4] = {
  {0, 7, 8 ,15},
  {1, 6, 9  ,14},
  {2, 5, 10 ,13},
  {3, 4, 11 , 12},
};


void setup() {



  
  // put your setup code here, to run once:
 // These lines are specifically to support the Adafruit Trinket 5V 16 MHz.
  // Any other board, you can remove this part (but no harm leaving it):
#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
  clock_prescale_set(clock_div_1);
#endif
  // END of Trinket-specific code.

  strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();            // Turn OFF all pixels ASAP
  strip.setBrightness(50); // Set BRIGHTNESS to about 1/5 (max = 255)

  //Serial.begin(9600);
 // Serial.println("hello world");




  // colors HSV 
  colors[0] = {(int) 249 * 65535/360, 255, (int) 255 * .95}; // heavy rain 
  colors[1] = {(int) 186 * 65535/360, 255 *.71, (int) 255}; //snow
  colors[2] = {(int) 208 * 65535/360, 255*.93, (int) 255}; //light rain 
  colors[3] = {(int) 171 * 65535/360, 255, (int) 255};  // light rain 
  colors[4] = {(int) 215 * 65535/360, 255* .9, (int) 255 * .98};  //light rain 
  colors[5] = {(int) 199 * 65535/360, 255* .84, (int) 255 * .98};  //light rain 
  colors[6] = {(int) 267 * 65535/360, 255, (int) 255 * .73};  // thunder
  colors[7] = {(int) 281 * 65535/360, 255, (int) 255 * .81};  //thunder
  colors[8] = {(int) 292 * 65535/360, 255, (int) 255 * .74}; //thunder
  colors[9] = {(int) 58 * 65535/360, 255, (int) 255 * .89};  //thunder
  colors[10] = {(int) 144 * 65535/360, 255 *.31, (int) 255 * .98};  //air 

//  int len1 = sizeof(words) / sizeof(words[0]);
//  int len2 = sizeof(groups) / sizeof(groups[0]);

//  Serial.println("searching");
//  int *arr = getWordsByGroup(2);
//  Serial.println(sizeof(arr));
//  for (int i = 0; i < sizeof(arr); i++) {
//    Word m = words[arr[i]];
//    Serial.println(m.name);
//    Serial.println(m.start);
//  }


 pinMode(butt0, INPUT);
 pinMode(butt1, INPUT);
 pinMode(butt2, INPUT);
 pinMode(butt3, INPUT);

 // initialize action array
 for (int i = 0; i < 100; i++) {
    actionArray[i][0] = -1;
    actionArray[i][1] = 0;
 }


}

void loop() {

  fsrread = analogRead(fsr); 
  if (fsrread < 100) {
    Serial.println(" - No pressure");
    fsrstate = 0 ; 
  } else if (fsrread < 435) {
    Serial.println(" - Light touch");
    fsrstate = 1 ; 
  } else if (fsrread < 800) {
    Serial.println(" - Light squeeze");
    fsrstate = 2 ; 
  }

  readpott = analogRead ( pott) ; 
  pottval = map(readpott , 0 , 1023 , 0 ,100) ; 

  if (pottval < 15) {
    pottstate = 0 ; 
  } else if (pottval < 30) {
    pottstate = 1 ; 
  } else if (pottval < 45) {
    pottstate = 2 ; 
  } else if (pottval< 60) {
    pottstate= 3 ; 
  }

  buttstate0 = digitalRead(butt0);
  buttstate1 = digitalRead(butt1);
  buttstate2 = digitalRead(butt2);
  buttstate3 = digitalRead(butt3) ; 

int buttonStates[] = {buttstate0, buttstate1, buttstate2, buttstate3 ,fsrstate , pottstate};
//int buttonStates[] = {1, 0, 0, 0};

startAction(buttonStates);
  // evaluate all actions in action array to see what should be lit up
  processActions();

  
  
}


//light functions 


void clearRange(int start, int end) {
  if (start < end) {
      for(int i = start; i <= end; i++) { // For each pixel in strip...
        strip.setPixelColor(i, strip.Color(0, 0, 0));        //  Set pixel's color (in RAM)
      }
    } else {
      for(int i= start; i >= end; i--) { // For each pixel in strip...
        strip.setPixelColor(i, strip.Color(0, 0, 0));        //  Set pixel's color (in RAM)
      }
    }
  strip.show();
}

void colorRange(int start, int end, MyColor color) {
  if (start < end) {
      for(int i = start; i <= end; i++) { // For each pixel in strip...
        strip.setPixelColor(i, strip.ColorHSV(color.hue, color.sat, color.val));        //  Set pixel's color (in RAM)
      }
    } else {
      for(int i= start; i >= end; i--) { // For each pixel in strip...
        strip.setPixelColor(i, strip.ColorHSV(color.hue, color.sat, color.val));        //  Set pixel's color (in RAM)
      }
    }
  strip.show();
}



void colorWipeRange(int start, int end, MyColor color, int wait) {
  if (start < end) {
      for(int i = start; i <= end; i++) { // For each pixel in strip...
        strip.setPixelColor(i, strip.ColorHSV(color.hue, color.sat, color.val));         //  Set pixel's color (in RAM)
        strip.show();                          //  Update strip to match
        delay(wait);                           //  Pause for a moment
      }
    } else {
      int range = (start == end) ? 1 : start - end;
      for(int i= start; i >= end; i--) { // For each pixel in strip...
        strip.setPixelColor(i, strip.ColorHSV(color.hue, color.sat, color.val));         //  Set pixel's color (in RAM)
        strip.show();                          //  Update strip to match
        delay(wait);                           //  Pause for a moment
      }
    }
}

void theaterChase(uint32_t color, int wait) {
  for(int a=0; a<10; a++) {  // Repeat 10 times...
    for(int b=0; b<3; b++) { //  'b' counts from 0 to 2...
      strip.clear();         //   Set all pixels in RAM to 0 (off)
      // 'c' counts up from 'b' to end of strip in steps of 3...
      for(int c=b; c<strip.numPixels(); c += 3) {
        strip.setPixelColor(c, color); // Set pixel 'c' to value 'color'
      }
      strip.show(); // Update strip with new contents
      delay(wait);  // Pause for a moment
    }
  }
}


void fadeRange(int start, int end, MyColor color, int duration, int wait) {
  int valStep = color.val / duration;
  valStep = (valStep < 1) ? 1 : valStep;
  //color.sat= map(color.sat , 0 , color.val , 0 , color.sat);
  int startVal = color.val;
  int startSat = color.sat;
  for (int i = 0; i < duration; i++) {
    color.val = map(i , 0, duration, startVal, 0);
    color.sat= map(i , 0, duration, startSat, 0);
   // color.val = color.val - valStep;
    if (color.val <= 0) break;
    if (start < end) {
      for(int i = start; i <= end; i++) { // For each pixel in strip...
        strip.setPixelColor(i, strip.ColorHSV(color.hue, color.sat, color.val));         //  Set pixel's color (in RAM)
      }
    } else {
      int range = (start == end) ? 1 : start - end;
      for(int i= start; i >= end; i--) { // For each pixel in strip...
        strip.setPixelColor(i, strip.ColorHSV(color.hue, color.sat, color.val));        //  Set pixel's color (in RAM)
      }
    }
    strip.show();
    delay(wait);
  }
}


void glowRange(int start, int end, MyColor color, int duration, int wait, int valLow, int valHigh, int satLow, int satHigh) {
  bool up = false;
  int startVal = color.val;
  int startSat = color.sat;
  //color.sat= map(color.sat , 0 , color.val , 0 , color.sat);
  int c = 0;
  for (int i = 0; i < duration; i++) {
    if (up) {
      // color.val++
      Serial.println("up");
      //Serial.println(i);
      color.val = map(i % (duration/20), 0, (duration/20) - 1, valLow, startVal);
      color.sat= map(i % (duration/20), 0, (duration/20) - 1, satHigh, startSat);
      Serial.println(color.val);
      if (color.val >= startVal) {
        color.val = startVal;
        color.sat = startSat;
        //color.sat= glowHigh;
        Serial.println(i);
        Serial.println("switch down");
        c++;
        i = duration/10 * c;
        Serial.println(i);
        up = false; 
      }
    } else {
      // color.val--;
      Serial.println("down");
      //Serial.println(i);
      color.val = map(i % (duration/20) , 0,(duration/20) - 1, startVal, valLow);
      color.sat= map(i % (duration/20) , 0, (duration/20) - 1, startSat, satLow);
      Serial.println(color.val);
      if (color.val <= valLow) {
        color.val = valLow;
        color.sat= satLow;
        Serial.println(i);
        Serial.println("switch up");
        c++;
        i = duration/10 * c;
        Serial.println(i);
        up = true;
      }
    }
    if (start < end) {
      for(int j = start; j <= end; j++) { // For each pixel in strip...
        strip.setPixelColor(j, strip.ColorHSV(color.hue, color.sat, color.val));         //  Set pixel's color (in RAM)
      }
    } else {
      int range = (start == end) ? 1 : start - end;
      for(int j= start; j >= end; j--) { // For each pixel in strip...
        strip.setPixelColor(j, strip.ColorHSV(color.hue, color.sat, color.val));        //  Set pixel's color (in RAM)
      }
    }
    strip.show();
    delay(wait);
  }
}



// Some functions for creating weather -----------------

bool checkInput(int sensor, int state) {
  // check by sensor in special case
  Serial.println("checking input");
  Serial.println(sensor);
  Serial.println(state);
  switch (sensor) {
    case 4: // fsr sensor
      return state >= 1 && lastSensors[sensor] == 0;
    case 5 : //pott 
      return state >= 1 && lastSensors[sensor] == 0;
    default: 
      return state == HIGH && lastSensors[sensor] == 0;
      
  }
}

// check button state and push current action into action array
void startAction(int states[]) {
  // check state against last input, to see if it is a new action
  // add wait time for start action
  for (int i = 0; i < 6; i++) {
    // check if sensor at i is on and valid
    bool valid = checkInput(i, states[i]); // should be function
    if (valid) {
      Serial.println("action at button");
      Serial.println(i);
      // action was detected, snapshot state and add to action arr
      actionArray[actionPos % 100][0] = i;
      actionArray[actionPos % 100][1] = states[i];
      timeArray[actionPos % 100] = millis();
      Serial.println(actionArray[actionPos % 100][0]);
      Serial.println(timeArray[actionPos % 100]);
      Serial.println(actionArray[actionPos % 100][1]);
      actionPos++;
      Serial.println(actionPos); 
      lastSensors[i] = states[i];
    } else{
      lastSensors[i] = 0;
    }
  }  
}

void processAction(int pos) {
//  Serial.println("processing action at ");
  
  int sensor = actionArray[pos][0];
  unsigned long ts = timeArray[pos];
  int val = actionArray[pos][1];
  int col = 0;
  unsigned long diff = millis() - ts;
//  Serial.println(sensor);
//  Serial.println(diff);
  switch (sensor) {
    case 0:
    case 1:
    case 2:
    case 3:
      // handle light rain
      col = sensor % 4;
      if (diff < 2000) {
        colorRange(leds[col][0] , leds[col][0], colors[4] );
        actionArray[pos][2] = 0;
        //drawchar("rainy") ; 
      } else if (diff < 4000) {
        if (actionArray[pos][2] == 0) {
          actionArray[pos][2] = 1;
          clearRange(leds[col][0] , leds[col][0]);
        }
        colorRange(leds[col][1] , leds[col][1], colors[4] ); 
      } else if (diff < 6000) {
        if (actionArray[pos][2] == 1) {
          actionArray[pos][2] = 2;
          clearRange(leds[col][1] , leds[col][1]);
        }
        colorRange(leds[col][2] , leds[col][2], colors[4] );  
      } else if (diff < 8000) {
        if (actionArray[pos][2] == 2) {
          actionArray[pos][2] = 3;
          clearRange(leds[col][2] , leds[col][2]);
        }
        colorRange(leds[col][3] , leds[col][3], colors[4] );
      } else {
        clearRange(leds[col][3] , leds[col][3]);
        // zero out completed action
        Serial.println("zeroing out");
        Serial.println(pos);
        actionArray[pos][0] = -1;
        //display.clearDisplay();
        //display.display();
      }
      break;
    case 4:
     // fsr handling
    if (val == 1) {
       // handle light fsr press
       //strip.clear();
      // drawchar("thunder!") ; 
       theaterChase(strip.Color(238,355,0),25);
       theaterChase(strip.Color(101,0,224),35) ;  
       //strip.clear();
       actionArray[pos][1] = 0;
       clearRange(0 , 15);
       
    } else if (val == 2) {
      // handle heavy fsr press
    // drawchar("thunder!") ; 
     theaterChase(strip.Color(238,355,0), 25) ; 
     theaterChase(strip.Color(101,0,224), 35) ; 
     theaterChase(strip.Color(174,0,255), 35) ; 
     theaterChase(strip.Color(255,0,250), 35) ;
      actionArray[pos][1] = 0;
      clearRange(0 , 15);
      //strip.clear();
      
     } else {
      // zero out completed action
      //strip.clear();
      actionArray[pos][0] = -1;
      clearRange(0 , 15);
     // display.clearDisplay();
      //display.display(); 
     }  
     break;
    case 5:
    //pott handling 
    if (val == 1) {
       // pott 15 - 30 
      
       glowRange(0, 3, colors[1], 2000, 5, 60,colors[1].val, 155, colors[1].sat) ; 
       fadeRange(0, 3, colors[1], 400,10) ; 
       actionArray[pos][1] = 0;
       //clearRange(0 , 15);
       
    } else if (val == 2) {
      // pott 30 -45 
    
      glowRange(0, 11, colors[2], 2000, 5, 60,colors[2].val, 155, colors[2].sat) ;  
      fadeRange(0, 11, colors[2], 400,10) ;  
      actionArray[pos][1] = 0;
      //clearRange(0 , 15);
    
    
    } else if (val == 3) {
      // pott 45 -60
    
      glowRange(0, 15, colors[3], 2000, 5, 60,colors[3].val, 155, colors[3].sat) ;  
      fadeRange(0, 15, colors[3], 400,10) ; 
      actionArray[pos][1] = 0;
     // clearRange(0 , 15);
    
    
     } else {
      // zero out completed action
      //strip.clear();
      actionArray[pos][0] = -1;
      clearRange(0 , 15);
     }  
     break;
    
    case 6:
    case 7:
      // handle heavy rain
//      col = sensor % 4;
//      if (diff < 2000) {
//        colorRange(leds[col][0] , leds[col][0], colors[4] );
//      } else if (diff < 4000) {
//        colorRange(leds[col][1] , leds[col][1], colors[4] );  
//      } else if (diff < 6000) {
//        colorRange(leds[col][2] , leds[col][2], colors[4] );  
//      } else if (diff < 8000) {
//        colorRange(leds[col][3] , leds[col][3], colors[4] );
//      } else {
//        // zero out completed action
//        actionArray[pos][0] = -1;
//      }
      break;
    case 8:
      break;
    case 9:
      break;
  }
}

void processActions() {
//  Serial.println("processing actions");
  for (int i = 0; i < 100; i++) {
    // process valid actions
    if (actionArray[i][0] != -1) {
      
      processAction(i);
    }
  }
}
