#include <EEPROM.h>

// Tastereingänge 1-5
const int in1 = 5;
const int in2 = 8;
const int in3 = 12;
const int in4 = 13;
const int in5 = A0;
// Schaltereingänge 1-2
const int in6 = A1;
const int in7 = A5;
// Push-Encodereingang
const int inA = A3;
const int inB = A4;
const int inP = A2;

// PWM-Ausgänge für Tasterbeleuchtung
const int out1 = 2;   //GND für Tasterbeleuchtung 1   //Magenta by default
const int out2 = 4;   //GND für Tasterbeleuchtung 2   //Magenta by default
const int out3 = 6;   //GND für Tasterbeleuchtung 3   //Rot by default
const int out4 = 7;   //GND für Tasterbeleuchtung 4   //Gelb by default
const int out5 = 9;   //GND für Tasterbeleuchtung 5   //Grün by default
const int outR = 3;   //Value für Rot  (Multiplex)
const int outG = 10;  //Value für Grün (Multiplex)
const int outB = 11;  //Value für Blau (Multiplex)

// Color-Definition der Taster
//                     R1   G1   B1   R2   G2   B2   R3   G3   B3   R4   G4   B4   R5   G5   B5
int colorArray[15] = {160, 000, 255, 160, 000, 255, 255, 000, 000, 230, 255, 000, 000, 255, 000}; //Array welches kontinuierlich ausgelesen und geschrieben wird
int colorArFix[15] = {160, 000, 255, 160, 000, 255, 255, 000, 000, 230, 255, 000, 000, 255, 000}; //Array zur Bereitstellung Standardwerte während des Betriebs
int colorArWth[15] = {255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255}; //Array alle LEDs Weiß
int colorArMen[15] = {255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 000, 000}; //Hauptmenü
int colorArSM2[15] = {255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 000, 255, 000}; //SubMenü 2
int colorArSM3[15] = {255, 255, 255, 255, 000, 000, 255, 255, 255, 255, 000, 000, 000, 255, 000}; //SubMenü 3
int darkVal = 127;
int highVal = 255;
int maxVal[15] = {darkVal, 0, 0, darkVal, 0, 0, darkVal, 0, 0, darkVal, 0, 0, darkVal, 0, 0};     //Maximalhelligkeit für die einzelnen Taster

boolean startAni = true;
boolean enterMenue = false;
boolean enterMenu1 = false;

int taste1 = LOW;
int taste1Alt = LOW;
int taste2 = LOW;
int taste2Alt = LOW;
int taste3 = LOW;
int taste3Alt = LOW;
int taste4 = LOW;
int taste4Alt = LOW;
int taste5 = LOW;
int taste5Alt = LOW;
int schalter1 = HIGH;
int schalter1Alt = HIGH;
int schalter2 = HIGH;
int schalter2Alt = HIGH;
int push1 = HIGH;
int push1Alt = HIGH;
int encoderPos = 0;
int encoderPinA = LOW;
int encoderPinAAlt = HIGH;
int encoderPinB = LOW;
int encoderPinBAlt = LOW;
long millisAlt = 0;
int debounceDelay = 10;

//MIDI-Informationen
int channelData = 144;  // 1001 0000 = Note On Kanal 1#

int note1 = 36;         //C2
int note2 = 37;         //C#2
int note3 = 38;         //D2
int note4 = 39;         //D#2
int note5 = 40;         //E2
int note6 = 41;         //F2
int note7 = 42;         //F#2
int note8 = 43;         //G2

int velHigh = 127;      //Velocity für HIGH
int velLow = 0;         //Velocity für LOW


void sendNote(int note, int velocity) {
  Serial.write(channelData);
  Serial.write(note);
  Serial.write(velocity);
}


void setup() {
  Serial.begin(31250);    //31250 Baud für MIDI / 9600 Baud für Hairless-MidiSerial

  TCCR1B = TCCR1B & 0b11111000 | 0x01;  //setze PWM-Frequenz für 9 und 10 auf 31372,55 Hz
  TCCR2B = TCCR2B & 0b11111000 | 0x01;  //setze PWM-Frequenz für 3 und 11 auf 31372,55 Hz

  pinMode(out1, OUTPUT);
  pinMode(out2, OUTPUT);
  pinMode(out3, OUTPUT);
  pinMode(out4, OUTPUT);
  pinMode(out5, OUTPUT);
  pinMode(outR, OUTPUT);
  pinMode(outG, OUTPUT);
  pinMode(outB, OUTPUT);

  pinMode(in1, INPUT);
  pinMode(in2, INPUT);
  pinMode(in3, INPUT);
  pinMode(in4, INPUT);
  pinMode(in5, INPUT);
  pinMode(in6, INPUT);
  pinMode(in7, INPUT);
  pinMode(inA, INPUT);
  pinMode(inB, INPUT);
  pinMode(inP, INPUT);

  digitalWrite(out1, HIGH);
  digitalWrite(out2, HIGH);
  digitalWrite(out3, HIGH);
  digitalWrite(out4, HIGH);
  digitalWrite(out5, HIGH);

  //digitalWrite(in1, HIGH);
  digitalWrite(in2, HIGH);
  digitalWrite(in3, HIGH);
  digitalWrite(in4, HIGH);
  digitalWrite(in5, HIGH);
  digitalWrite(in6, HIGH);
  digitalWrite(in7, HIGH);
  digitalWrite(inA, HIGH);
  digitalWrite(inB, HIGH);
  digitalWrite(inP, HIGH);

  // memcpy (colorArray, colorArFix, 17);
}


void loop() {
  taste1 = digitalRead(in1);
  taste2 = digitalRead(in2);
  taste3 = digitalRead(in3);
  taste4 = digitalRead(in4);
  taste5 = digitalRead(in5);
  schalter1 = digitalRead(in6);
  schalter2 = digitalRead(in7);
  push1 = digitalRead(inP);
  encoderPinA = digitalRead(inA);

  // Startup Animation
  if (startAni == true) {
    for (int i = 0; i < 14; i++) {
      if (i < 5) {
        analogWrite(outR, 127);
      }
      if (i > 4 && i < 9) {
        analogWrite(outR, 0);
        analogWrite(outG, 127);
      }
      if (i > 8) {
        analogWrite(outG, 0);
        analogWrite(outB, 127);
      }

      if (i == 0 || i == 8 || i == 9) {
        digitalWrite(out2, HIGH);
        digitalWrite(out1, LOW);

      }
      else if (i == 1 || i == 7 || i == 10) {
        digitalWrite(out1, HIGH);
        digitalWrite(out2, LOW);
        digitalWrite(out3, HIGH);
      }
      else if (i == 2 || i == 6 || i == 11) {
        digitalWrite(out2, HIGH);
        digitalWrite(out3, LOW);
        digitalWrite(out4, HIGH);
      }
      else if (i == 3 || i == 5 || i == 12) {
        digitalWrite(out3, HIGH);
        digitalWrite(out4, LOW);
        digitalWrite(out5, HIGH);
      }
      else if (i == 4 || i == 13) {
        digitalWrite(out4, HIGH);
        digitalWrite(out5, LOW);
      }
      delay(75);
      if (i == 13) {
        delay(25);
        startAni = false;
      }
    }
  }


  // Helligkeitswahl der Taster mit schalter1
  if (schalter1 == LOW && schalter1Alt == HIGH) {
    darkVal = 50;
    highVal = 120;
    maxVal[0] = darkVal;
    maxVal[3] = darkVal;
    maxVal[6] = darkVal;
    maxVal[9] = darkVal;
    maxVal[12] = darkVal;
    schalter1Alt = schalter1;
  }
  if (schalter1 == HIGH && schalter1Alt == LOW) {
    darkVal = 150;
    highVal = 255;
    maxVal[0] = darkVal;
    maxVal[3] = darkVal;
    maxVal[6] = darkVal;
    maxVal[9] = darkVal;
    maxVal[12] = darkVal;
    schalter1Alt = schalter1;
  }


  // Tasten-Illumination
  for (int i = 0; i < 15; i = i + 3) {


    if (i == 0) {
      digitalWrite(out1, LOW);
      digitalWrite(out2, HIGH);
      digitalWrite(out3, HIGH);
      digitalWrite(out4, HIGH);
      digitalWrite(out5, HIGH);
    }
    else if (i == 3) {
      digitalWrite(out1, HIGH);
      digitalWrite(out2, LOW);
      digitalWrite(out3, HIGH);
      digitalWrite(out4, HIGH);
      digitalWrite(out5, HIGH);
    }
    else if (i == 6) {
      digitalWrite(out1, HIGH);
      digitalWrite(out2, HIGH);
      digitalWrite(out3, LOW);
      digitalWrite(out4, HIGH);
      digitalWrite(out5, HIGH);
    }
    else if (i == 9) {
      digitalWrite(out1, HIGH);
      digitalWrite(out2, HIGH);
      digitalWrite(out3, HIGH);
      digitalWrite(out4, LOW);
      digitalWrite(out5, HIGH);
    }
    else if (i == 12) {
      digitalWrite(out1, HIGH);
      digitalWrite(out2, HIGH);
      digitalWrite(out3, HIGH);
      digitalWrite(out4, HIGH);
      digitalWrite(out5, LOW);
    }

    analogWrite(outR, map(colorArray[i], 0, 255, 0, maxVal[i]));
    analogWrite(outG, map(colorArray[i + 1], 0, 255, 0, maxVal[i]));
    analogWrite(outB, map(colorArray[i + 2], 0, 255, 0, maxVal[i]));

    delayMicroseconds(500);

    analogWrite(outR, 0);
    analogWrite(outG, 0);
    analogWrite(outB, 0);


  }


  // Menü
  if (millis() <= 2000) {
    if (taste1 == HIGH && taste1Alt == LOW && taste5 == HIGH && taste5Alt == LOW) {
      enterMenue = true;
      taste1Alt = taste1;
      taste5Alt = taste5;
      memcpy (colorArray, colorArMen, 30);
    }
  }

  if (enterMenue == true) {

    if (taste1 == LOW && taste1Alt == HIGH) {
      taste1Alt = taste1;
    }
    if (taste2 == LOW && taste2Alt == HIGH) {
      taste2Alt = taste2;
    }
    if (taste3 == LOW && taste3Alt == HIGH) {
      taste3Alt = taste3;
    }
    if (taste4 == LOW && taste4Alt == HIGH) {
      taste4Alt = taste4;
    }
    if (taste5 == LOW && taste5Alt == HIGH) {
      taste5Alt = taste5;
    }



    //Color-Menü
    if (taste1 == HIGH && taste1Alt == LOW) {
      enterMenu1 = true;
      memcpy (colorArray, colorArFix, 30);
      taste1Alt = taste1;

      if (taste1 == HIGH) {
        encoderPos = 0;
        colorArray[0] = encoderPos;
      }
      if (taste2 == HIGH) {
        encoderPos = 0;
        colorArray[0] = encoderPos;
      }
      if (taste3 == HIGH) {
        encoderPos = 0;
        colorArray[0] = encoderPos;
      }
      if (taste4 == HIGH) {
        encoderPos = 0;
        colorArray[0] = encoderPos;
      }
      if (taste5 == HIGH) {
        encoderPos = 0;
        colorArray[0] = encoderPos;
      }
      if (taste1 == HIGH && taste5 == HIGH) {
        enterMenu1 = false;
      }
    }

    if (enterMenu1 == false) {

      //???-Menü
      if (taste2 == HIGH && taste2Alt == LOW) {
        memcpy (colorArray, colorArSM2, 30);
        taste2Alt = taste2;
        //...
      }

      //???-Menü
      if (taste3 == HIGH && taste3Alt == LOW) {
        memcpy (colorArray, colorArSM3, 30);
        taste3Alt = taste3;
        //...
      }

      //Exit Menü
      if (taste5 == HIGH && taste5Alt == LOW) {
        memcpy (colorArray, colorArFix, 30);
        taste5Alt = taste5;
        enterMenue = false;
      }
    }
  }

  if (enterMenue == false) {
    // Taste 1
    if (taste1 == LOW && taste1Alt == HIGH) {
      sendNote(note1, velLow);
      taste1Alt = taste1;
      maxVal[0] = darkVal;
    }
    if (taste1 == HIGH && taste1Alt == LOW) {
      sendNote(note1, velHigh);
      taste1Alt = taste1;
      maxVal[0] = highVal;
    }

    // Taste 2
    if (taste2 == LOW && taste2Alt == HIGH) {
      sendNote(note2, velLow);
      taste2Alt = taste2;
      maxVal[3] = darkVal;
    }
    if (taste2 == HIGH && taste2Alt == LOW) {
      sendNote(note2, velHigh);
      taste2Alt = taste2;
      maxVal[3] = highVal;
    }

    // Taste 3
    if (taste3 == LOW && taste3Alt == HIGH) {
      sendNote(note3, velLow);
      taste3Alt = taste3;
      maxVal[6] = darkVal;
    }
    if (taste3 == HIGH && taste3Alt == LOW) {
      sendNote(note3, velHigh);
      taste3Alt = taste3;
      maxVal[6] = highVal;
    }

    // Taste 4
    if (taste4 == LOW && taste4Alt == HIGH) {
      sendNote(note4, velLow);
      taste4Alt = taste4;
      maxVal[9] = darkVal;
    }
    if (taste4 == HIGH && taste4Alt == LOW) {
      sendNote(note4, velHigh);
      taste4Alt = taste4;
      maxVal[9] = highVal;
    }

    // Taste 5
    if (taste5 == LOW && taste5Alt == HIGH) {
      sendNote(note5, velLow);
      taste5Alt = taste5;
      maxVal[12] = darkVal;
    }
    if (taste5 == HIGH && taste5Alt == LOW) {
      sendNote(note5, velHigh);
      if (schalter2 == LOW) {                 // Bei Betätigung von schalter2 wird zu Note5 automatisch Note1 mitgesendet (Anwendugn z.B. für QLab um im Titel zu bleiben)
        sendNote(note1, velHigh);
      }
      taste5Alt = taste5;
      maxVal[12] = highVal;
    }

    // Encoder-PushButten
    if (push1 == HIGH && push1Alt == LOW) {
      sendNote(note6, velLow);
      push1Alt = push1;
    }
    if (push1 == LOW && push1Alt == HIGH) {
      sendNote(note6, velHigh);
      push1Alt = push1;
    }

    // Encoder
    if ((encoderPinAAlt == HIGH) && (encoderPinA == LOW)) {  // Check last State of Encoder pin A = 0  && n = 1
      if (millis() - debounceDelay >= millisAlt) {
        millisAlt = millis();
        if (digitalRead(inB) == HIGH) {                      // Check current Encoder pin B = 0
          encoderPinAAlt = encoderPinA;
          encoderPos--;                                      // Count down Encoder value -1
          sendNote(note7, velHigh);
        }
        if (digitalRead(inB) == LOW) {
          encoderPinAAlt = encoderPinA;
          encoderPos++;                                      // Count up Encoder value +1
          sendNote(note8, velHigh);
        }
      }
    }
    if (encoderPinA == HIGH) {
      encoderPinAAlt = HIGH;
    }
  }
}
