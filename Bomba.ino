#include <LiquidCrystal.h>
#include <Keypad.h>

LiquidCrystal lcd(A0, A1, A2, A3, A4, A5);

const byte ROWS = 4;
const byte COLS = 4;

char keys[ROWS][COLS] = {
  { '1', '2', '3', 'A' },


  { '4', '5', '6', 'B' },


  { '7', '8', '9', 'C' },


  { '*', '0', '#', 'D' }

};
byte rowPins[ROWS] = { 6, 5, 4, 3 };
byte colPins[COLS] = { 7, 8, 9, 10 };
Keypad kpd = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);
char key;

int count = 2;
int phase = 0;
int hr = 0, min = 0, sec = 0;
int passhr, passmin, passsec;
int code = 0;
unsigned long int currentTime = 0;
unsigned long int startTime = 0;
unsigned long int buzzTime=0;
int timeSec;  //time in seconds

void setup() {
  pinMode(2, OUTPUT);
  pinMode(11, OUTPUT);
  lcd.begin(16, 2);
  lcd.setCursor(4, 0);
  lcd.print("SET TIME");
  lcd.setCursor(2, 1);
  lcd.print("00 : 00 : 00");
  lcd.setCursor(2, 1);
  lcd.blink();
}

void getTime() {
  if (key) {
    analogWrite(11, 50);
    lcd.print(key);
    int value = key - '0';
    if (count < 4) { hr = hr * 10 + value; }
    if (count > 4 && count < 9) { min = min * 10 + value; }
    if (count > 9) { sec = sec * 10 + value; }
    count++;
    delay(100);
    analogWrite(11, 0);
    if (count == 4 || count == 9) {
      count += 3;
      lcd.setCursor(count, 1);
    }
  }
  checkTime();
}

int checkTime() {
  if (count == 14) {
    if (hr <= 24 && min <= 60 && sec <= 60) {
      phase = 1;
      return 0;
    } else {
      lcd.setCursor(2, 1);
      lcd.print("INVALID TIME");
      count = 2;
      hr = 0;
      min = 0;
      sec = 0;
      delay(2000);
      lcd.setCursor(2, 1);
      lcd.print("00 : 00 : 00");
      lcd.setCursor(2, 1);
    }
  }
}

void printTime() {
  //hours
  if (hr < 10) {
    lcd.print("0");
    lcd.print(hr);
  } else lcd.print(hr);
  lcd.print(":");
  //minutes
  if (min < 10) {
    lcd.print("0");
    lcd.print(min);
  } else lcd.print(min);
  lcd.print(":");
  //seconds
  if (sec < 10) {
    lcd.print("0");
    lcd.print(sec);
  } else lcd.print(sec);
}

void menuTwo() {
  lcd.clear();
  printTime();
  lcd.print("-->25641");
  lcd.setCursor(0, 1);
  lcd.print("ENTER CODE:");
  passhr=hr;passmin=min;passsec=sec;
  timeSec = passhr * 3600 + passmin * 60 + passsec;
  phase = 2;
}

void getCode() {
  if (key) {
    analogWrite(11, 50);
    lcd.print(key);
    code = code * 10 + (key - '0');
    delay(100);
    analogWrite(11, 0);
  }
}

int confirmCode() {
  if (code / 10000 != 0) {
    if (code == 25641) {
      switch (phase) {
        case 2:
          phase = 3;
          break;
        case 4:
          phase = 5;
          break;
      }
    } else {
      lcd.setCursor(11, 1);
      lcd.print("     ");
      lcd.setCursor(11, 1);
      code = 0;
    }
  }
}

void menuThree() {
  lcd.clear();
  lcd.setCursor(4, 0);
  printTime();
  lcd.setCursor(0, 1);
  lcd.print("25641----->");
  currentTime = millis();
  startTime = millis();
  code = 0;
  phase = 4;
}

void updateHMS() {
  lcd.setCursor(2, 0);
  if (hr < 10) {
    lcd.print("0");
    lcd.print(hr);
  } else lcd.print(hr);
  lcd.print(" : ");
  if (min < 10) {
    lcd.print("0");
    lcd.print(min);
  } else lcd.print(min);
  lcd.print(" : ");
  if (sec < 10) {
    lcd.print("0");
    lcd.print(sec);
  } else lcd.print(sec);
  lcd.setCursor(11, 1);
  if (code != 0)
    lcd.print(code);
}

void updateTime() {
  currentTime = millis();
  sec--;
  if (sec <= -1) {
    min--;
    if (min <= -1) {
      hr--;
      min = 60;
    }
    sec = 60;
  }
  updateHMS();
}



void toggleSL() {
  int timeNow = (hr * 3600) + (min * 60) + sec;
  int dela = map(timeNow, 0, timeSec, 0, 5000);
  if (millis() - buzzTime >= dela) {
    buzzTime = millis();
    analogWrite(11, 50);
    digitalWrite(2, HIGH);
  }
  if (dela >= 50 && millis() - buzzTime >= 50 && digitalRead(3)==HIGH) {
    analogWrite(11, 0);
    digitalWrite(2, LOW);
  }
}

void loop() {
  key = kpd.getKey();
  switch (phase) {
    case 0:
      getTime();
      break;
    case 1:
      menuTwo();
      break;
    case 2:
      getCode();
      confirmCode();
      break;
    case 3:
      menuThree();
      break;
    case 4:
      if (millis() - currentTime >= 1000)
        updateTime();
      getCode();
      confirmCode();
      toggleSL();
      break;
    case 5:
      lcd.clear();
      digitalWrite(2, LOW);
      analogWrite(11, 0);
      hr=passhr;min=passmin;sec=passsec;
      code = 0;
      phase = 1;
      break;
  }
}