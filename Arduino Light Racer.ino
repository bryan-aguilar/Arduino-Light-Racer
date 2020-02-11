/*
  How to change difficulty.
  Level One: interval should be 800 done
  Level Two: interval should be 600  done
  Level Three: interval should be 400  done
  Level Four: Interval should be 400 and new win pos every two bounces done
  Level Five: Interval should be 400 and new win pos every bounce  done


*/
#include <FastLED.h>
#include<LiquidCrystal.h>
//initialize LCD
LiquidCrystal lcd(23, 25, 27, 29, 31, 33);
bool refreshLCD = false; //will let LCD function know if a refresh is required so it is not constantly refreshing

//initialize delay variables.
unsigned long currentMillis;
unsigned long p1PreviousMillis = millis();
unsigned long p2PreviousMillis = millis();


//will be changed to alter difficulty
//represents interval time in milliseconds
int p1Interval = 650;
int p2Interval = 650;
int diffIncrement = 200; // what the difficulty will be incremented by when a player moves up a level.

//level difficulty flag
bool p1lvl4Diff = true;
bool p2lvl4Diff = true;

bool p1Direction, p2Direction; //True = LEDs move left to right. False = LEDs move right to left

int p1Loc = 0; //player 1(left side) starting location. Also acts as loop index
int p2Loc = 0; //player 2(right side) starting location. Also acts as loop index

//Head and tail boundaries that allow you to change these. You should only be changing these to even numbers and they should never overlap
//note that the left boundary will light up left boundary +1
//I beleive these numbers must be even numbers also for the loops to work (have not tested though)
int p1LeftBoundary = 32;
int p1RightBoundary = 0;
int p2LeftBoundary = 32;
int p2RightBoundary = 0;



//button setup
const int redButt = 2; //player 1
const int redButtLED = 4;
const int blueButt = 3; //player 2
const int blueButtLED = 5;

//interrupt flags
volatile bool redButtPressed = false;
volatile bool blueButtPressed = false;

//dead time variables
//if a player hits a button and its not the right location there button will become "dead" for 3 seconds;
unsigned long p1DeadTime = millis();
unsigned long p2DeadTime = millis();
int deadTimeInterval = 3000;
bool p1Dead;
bool p2Dead;

//initialize LED Strips
//green line
#define p1DATA_PIN  51
#define p2DATA_PIN 50
//yellow line
#define p1CLK_PIN  53
#define p2CLK_PIN 52

#define LED_TYPE  APA102
#define COLOR_ORDER  BGR
//always use an even number of LEDs. This is important to the loop functionality.
#define NUM_LEDS  34
CRGB p1leds[NUM_LEDS];
CRGB p2leds[NUM_LEDS];

//will tell winning position generating function if a new position is required
bool p1NewWinPosReqd = true;
bool p2NewWinPosReqd = true;


int p1WinPos, p2WinPos; //storage for winning position
int p1WinCheck, p2WinCheck;//variable used to store the current lit leds. The way the led strip indexing works reqs these var's to exist

CRGB p1PrimColor, p1WinColor, p2PrimColor, p2WinColor;


//level counters
int p1Level = 1;
int p2Level = 1;
int maxLevel = 5;

void setup() {
  // put your setup code here, to run once:
  FastLED.addLeds<LED_TYPE, p1DATA_PIN, p1CLK_PIN, COLOR_ORDER>(p1leds, NUM_LEDS);
  FastLED.addLeds<LED_TYPE, p2DATA_PIN, p2CLK_PIN, COLOR_ORDER>(p2leds, NUM_LEDS);
  FastLED.setBrightness(75);
  //uses arduinos built in resistor. LOW = Button Pressed High = Button Not pressed
  pinMode(blueButt, INPUT_PULLUP);
  pinMode(redButt, INPUT_PULLUP);
  //led setup
  pinMode(redButtLED, OUTPUT);
  pinMode(blueButtLED, OUTPUT);
  //  Interrupt will enable when button goes from HIGH to LOW
  attachInterrupt(digitalPinToInterrupt(redButt), redButton, FALLING);
  attachInterrupt(digitalPinToInterrupt(blueButt), blueButton, FALLING);
  //initital LCD Setup.
  lcd.begin(16, 2);
  lcd.print("Player1 Player2");
  lcd.setCursor(0, 1);
  lcd.print("Level ");
  lcd.print(p1Level);
  lcd.print(" Level ");
  lcd.print(p2Level);
  randomSeed(analogRead(0));
}

void loop() {
  currentMillis = millis();
  checkDeadState();
  generateWinningPos();
  p1Colors();
  p2Colors();
  leftStrip();
  rightStrip();
  verifyWinLoss();
  updateLCD();
  gameOver();
}

void checkDeadState() {
  //checks if a button should be dead or not
  //should set dead varibale to true and turn off the led.

  if (currentMillis - p1DeadTime >= deadTimeInterval) {
    p1Dead = false;
    digitalWrite(redButtLED, HIGH);
  }
  else {
    p1Dead = true;
    digitalWrite(redButtLED, LOW);
  }

  if (currentMillis - p2DeadTime >= deadTimeInterval) {
    p2Dead = false;
    digitalWrite(blueButtLED, HIGH);

  }
  else {
    p2Dead = true;
    digitalWrite(blueButtLED, LOW);
  }
}

void generateWinningPos() {
  //checks to see if new win position is required.
  //if it is it generates random number between boundaries and then sets variable back to false
  //will also set LCD refresh flag back to false since
  if (p1NewWinPosReqd == true) {
    //p1WinPos = random8(p1RightBoundary, (p1LeftBoundary + 1));
    p1WinPos = random(p1RightBoundary, (p1LeftBoundary+1));
    p1NewWinPosReqd = false;
    refreshLCD = false;
  }
  if (p2NewWinPosReqd == true) {
    //p2WinPos = random8(p2RightBoundary, (p2LeftBoundary + 1));
    p2WinPos = random(p2RightBoundary, (p2LeftBoundary+1));
    p2NewWinPosReqd = false;
    refreshLCD = false;
  }


}

void p1Colors() {
  //sets color based on player level
  switch (p1Level) {
    case 1:
      p1PrimColor = CRGB :: Orange;
      p1WinColor = CRGB:: Blue;
      break;
    case 2:
      p1PrimColor = CRGB :: Red;
      p1WinColor = CRGB::Green;
      break;
    case 3:
      p1PrimColor = CRGB::Yellow;
      p1WinColor = CRGB::Violet;
      break;
    case 4:
      p1PrimColor = CRGB:: Blue;
      p1WinColor = CRGB::Red;
      break;
    case 5:
      p1PrimColor = CRGB::Silver;
      p1WinColor = CRGB :: Blue;
      break;
  }
}


void p2Colors() {
  //sets color based on player level
  switch (p2Level) {
    case 1:
      p2PrimColor = CRGB :: Orange;
      p2WinColor = CRGB:: Blue;
      break;
    case 2:
      p2PrimColor = CRGB :: Red;
      p2WinColor = CRGB::Green;
      break;
    case 3:
      p2PrimColor = CRGB::Yellow;
      p2WinColor = CRGB::Violet;
      break;
    case 4:
      p2PrimColor = CRGB:: Blue;
      p2WinColor = CRGB::Red;
      break;
    case 5:
      p2PrimColor = CRGB::Silver;
      p2WinColor = CRGB :: Blue;
      break;
  }
}

void leftStrip() {
  if (currentMillis - p1PreviousMillis >= p1Interval) {
    //make sure direction is set properly.
    //it location is equal to left boundary direction should be true(move right) if equal to left boundary direction should be false(move left)
    //will loop between boundaries
    if (p1Loc == p1LeftBoundary) {

      p1UpperTierDiff();//check for level 4 and 5 difficulties
      p1Direction = true;
      p1leds[p1Loc - 1] = CRGB::Black;
      p1leds[p1Loc - 2] = CRGB::Black;
    }
    else if (p1Loc == p1RightBoundary) {

      p1UpperTierDiff();//check for level 4 and 5 difficulties
      p1Direction = false;
      p1leds[p1Loc + 2] = CRGB::Black;
      p1leds[p1Loc + 3] = CRGB::Black;


    }
    //if moving left to right
    if (p1Direction == true) {
      //this clearing happens earlier in the function
      //this stops the loop from referencing a location outside of the arrary
      /*
        For example, if left boundary was set to 34(the farthest left it can go) it would
        try to clear location led 36 and 37. Which do not exist
        when p1loc is at a boundary mark, the clearing of the previous LEDs happens above
      */
      if (p1Loc != p1LeftBoundary) {
        p1leds[p1Loc + 2] = CRGB::Black;
        p1leds[p1Loc + 3] = CRGB::Black;
      }
      //checks to see if win pos corressponds to location or location + 1.
      //if it does that means it a winning pair and will change to winning color
      if (p1WinPos == p1Loc || p1WinPos == p1Loc + 1) {
        p1leds[p1Loc] = p1WinColor;
        p1leds[p1Loc + 1] = p1WinColor;
      }
      else {
        p1leds[p1Loc] = p1PrimColor;
        p1leds[p1Loc + 1] = p1PrimColor;

      }
      FastLED.show();


      p1WinCheck = p1Loc;
      p1Loc -= 2;
      p1PreviousMillis = currentMillis;
    }
    //if moving right to left
    if (p1Direction == false) {
      //ensures that the loop does not attempt to reference an LED that does not exist.
      //Clearing of previous LEDs when p1Loc is at boundary happens earlier
      if (p1Loc != p1RightBoundary) {
        p1leds[p1Loc - 1] = CRGB::Black;
        p1leds[p1Loc - 2] = CRGB::Black;
      }
      if (p1WinPos == p1Loc || p1WinPos == p1Loc + 1) {
        p1leds[p1Loc] = p1WinColor;
        p1leds[p1Loc + 1] = p1WinColor;
      }
      else {
        p1leds[p1Loc] = p1PrimColor;
        p1leds[p1Loc + 1] = p1PrimColor;
      }
      FastLED.show();

      p1WinCheck = p1Loc;
      p1Loc += 2;
      p1PreviousMillis = currentMillis;

    }
  }

}

void rightStrip() {
  if (currentMillis - p2PreviousMillis >= p2Interval) {
    //make sure direction is set properly.
    //same logic as leftsrip()
    //will loop between boundaries
    //also clears the last used leds in the chain when p2loc is on boundary position

    if (p2Loc == p2LeftBoundary) {
      p2UpperTierDiff();//check for level 4 and 5 difficulties
      p2Direction = true;
      p2leds[p2Loc - 1] = CRGB::Black;
      p2leds[p2Loc - 2] = CRGB::Black;

    }
    else if (p2Loc == p2RightBoundary) {

      p2UpperTierDiff();//check for level 4 and 5 difficulties
      p2Direction = false;
      p2leds[p2Loc + 2] = CRGB::Black;
      p2leds[p2Loc + 3] = CRGB::Black;

    }



    //if moving left to right
    if (p2Direction == true) {

      if (p2Loc != p2LeftBoundary) {
        p2leds[p2Loc + 2] = CRGB::Black;
        p2leds[p2Loc + 3] = CRGB::Black;
      }
      if (p2WinPos == p2Loc || p2WinPos == p2Loc + 1) {
        p2leds[p2Loc] = p2WinColor;
        p2leds[p2Loc + 1] = p2WinColor;
      }
      else {
        p2leds[p2Loc] = p2PrimColor;
        p2leds[p2Loc + 1] = p2PrimColor;
      }
      FastLED.show();
      p2WinCheck = p2Loc;
      p2Loc -= 2;
      p2PreviousMillis = currentMillis;
    }
    //if moving right to left
    if (p2Direction == false) {
      if (p2Loc != p2RightBoundary) {
        p2leds[p2Loc - 1] = CRGB::Black;
        p2leds[p2Loc - 2] = CRGB::Black;
      }
      if (p2WinPos == p2Loc || p2WinPos == p2Loc + 1) {
        p2leds[p2Loc] = p2WinColor;
        p2leds[p2Loc + 1] = p2WinColor;
      }
      else {
        p2leds[p2Loc] = p2PrimColor;
        p2leds[p2Loc + 1] = p2PrimColor;
      }
      FastLED.show();
      p2WinCheck = p2Loc;
      p2Loc += 2;
      p2PreviousMillis = currentMillis;

    }
  }

}

void verifyWinLoss() {

  /*
  check dead state
    if it is:
    check if p1loc is equal to p1WinCheck or p1WinCheck +1
    if it is:
      increase level by 1
      incrememnt difficutly if level  2 or 3
      set button pressed to false
      set newwinposreqd to true
      set refreshlcd to true


    if its not:
      set dead time to current millis
      set dead variable to true so that led's can update


    reset butt pressed status
  */
  //if red button was pressed(player 1)
  if (redButtPressed == true) {
    if (p1Dead == false) {
      if (p1WinCheck == p1WinPos || p1WinCheck + 1 == p1WinPos) {
        p1Level++;
        if (p1Level < 4)
          p1Interval -= diffIncrement;
        p1NewWinPosReqd = true;
        refreshLCD = true;

      }
      else {
        p1DeadTime = currentMillis;
        p1Dead = true;
      }
    }
    redButtPressed = false;
  }
  // the blue button was pressed(player 2)
  if (blueButtPressed == true) {
    if (p2Dead == false) {
      if (p2WinCheck == p2WinPos || p2WinCheck + 1  == p2WinPos) {
        p2Level++;
        if (p2Level < 4)
          p2Interval -= diffIncrement;
        p2NewWinPosReqd = true;
        refreshLCD = true;

      }
      else {
        p2DeadTime = currentMillis;
        p2Dead = true;
      }
    }
    blueButtPressed = false;
  }

}

void updateLCD() {
  //will only update lcd screen if refreshLCD is flagged
  if (refreshLCD == true) {
    lcd.clear();
    lcd.home();
    lcd.print("Player1 Player2");
    lcd.setCursor(0, 1);
    lcd.print("Level ");
    lcd.print(p1Level);
    lcd.print(" Level ");
    lcd.print(p2Level);
  }
}

void gameOver() {
  //will throw program into infinite loop until reset button is pressed if someone completes all levels
  if (p1Level == (maxLevel + 1) || p2Level == (maxLevel + 1))
  {
    //if player one won
    if (p1Level == (maxLevel + 1)) {
      fill_rainbow(p2leds, NUM_LEDS, 0, 5);
      fill_rainbow(p1leds, NUM_LEDS, 0, 5);
      FastLED.show();


      do {
        lcd.clear();
        lcd.home();
        lcd.print("   Player One");
        lcd.setCursor(7, 1);
        lcd.print("Wins!");
        delay(3000);
        lcd.clear();
        lcd.home();
        lcd.print("Press reset");
        lcd.setCursor(0, 1);
        lcd.print("for new game");
      } while (true);
    }
    //if player two won
    if (p2Level == (maxLevel + 1)) {
      fill_rainbow(p2leds, NUM_LEDS, 0, 5);
      fill_rainbow(p1leds, NUM_LEDS, 0, 5);
      FastLED.show();

      do {
        lcd.clear();
        lcd.home();
        lcd.print("   Player Two");
        lcd.setCursor(7, 1);
        lcd.print("Wins!");
        delay(3000);
        lcd.clear();
        lcd.home();
        lcd.print("Press reset");
        lcd.setCursor(0, 1);
        lcd.print("for new game");
      } while (true);
    }
  }

}
void redButton() {

  static unsigned long last_interrupt_time = 0;
  unsigned long interrupt_time = millis();
  // If interrupts come faster than 200ms, assume it's a bounce and ignore
  if (interrupt_time - last_interrupt_time > 200)
  {
    redButtPressed = true;
  }
  last_interrupt_time = interrupt_time;

}

void blueButton() {
  static unsigned long last_interrupt_time = 0;
  unsigned long interrupt_time = millis();
  // If interrupts come faster than 200ms, assume it's a bounce and ignore
  if (interrupt_time - last_interrupt_time > 200)
  {
    blueButtPressed = true;
  }
  last_interrupt_time = interrupt_time;

}

void p1UpperTierDiff() {
  //this is for the level 4 Difficulty. will generate a new win position
  //after the light has gone down and back once
  if (p1Level == 4) {
    if (p1lvl4Diff) {
      p1NewWinPosReqd = true;
      generateWinningPos();
      p1lvl4Diff = !p1lvl4Diff;
    }
    else {
      p1lvl4Diff = !p1lvl4Diff;
    }
  }
  //Max level difficulty adjustment. Will generate a new win Position whenever the index hits a boundary
  if (p1Level == maxLevel) {
    p1NewWinPosReqd = true;
    generateWinningPos();
  }

}

void p2UpperTierDiff() {
  //level 4 difficulty adjustment
  //see leftstrip() for logic explanation
  if (p2Level == 4) {
    if (p2lvl4Diff) {
      p2NewWinPosReqd = true;
      generateWinningPos();
      p2lvl4Diff = !p2lvl4Diff;
    }
    else {
      p2lvl4Diff = !p2lvl4Diff;
    }
  }
  //Max level difficulty adjustment
  //see leftstrip() for logic
  if (p2Level == maxLevel) {
    p2NewWinPosReqd = true;
    generateWinningPos();
  }
}
