#include <Wire.h>
#include <TimeLib.h>
#include <DS1307RTC.h>

int upPin = 3;
int upPinDown = false;

int downPin = 4;
int downPinDown = false;

int nextPin = 5;
int nextPinDown = false;

int currentFocus = 0;

int latchPin = 8;
int clockPin = 12;
int dataPin = 11;

int minutes = 1;
int minutesDataLength = 6;
char minutesData[] = {'0', '0', '0', '0', '0', '0'};

int hours = 1;
int hoursDataLength = 5;
char hoursData[] = {'0', '0', '0', '0', '0'};

int days = 1;
int daysDataLength = 5;
char daysData[] = {'0', '0', '0', '0', '0'};

int months = 1;
int monthsDataLength = 4;
char monthsData[] = {'0', '0', '0', '0'};

char data[] = {'0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0'};

const char *monthName[12] = {
  "Jan", "Feb", "Mar", "Apr", "May", "Jun",
  "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};

tmElements_t tm;

void setup() {
  Serial.begin(9600);

  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  pinMode(upPin, INPUT);
  // get the date and time the compiler was run
  if (getDate(__DATE__) && getTime(__TIME__)) {
    // and configure the RTC with this info
    RTC.write(tm);
  }
}

void loop() {
  int upPinState = digitalRead(upPin);
  if (upPinState == 0 && !upPinDown) {
    upPinDown = true;
  }

  if (upPinDown && upPinState == 1) {
    upPinDown = false;
    if (currentFocus == 0) {
      tm.Minute = minutes + 1;
      RTC.write(tm);
    } else if (currentFocus == 1) {
      tm.Hour = hours + 1;
      RTC.write(tm);
    } else if (currentFocus == 2) {
      tm.Day = days + 1;
      RTC.write(tm);
    } else if (currentFocus == 3) {
      tm.Month = months + 1;
      RTC.write(tm);
    }
  }

  int downPinState = digitalRead(downPin);
  if (downPinState == 0 && !downPinDown) {
    downPinDown = true;
  }

  if (downPinDown && downPinState == 1) {
    downPinDown = false;
    if (currentFocus == 0) {
      tm.Minute = minutes - 1;
      RTC.write(tm);
    } else if (currentFocus == 1) {
      tm.Hour = hours - 1;
      RTC.write(tm);
    } else if (currentFocus == 2) {
      tm.Day = days - 1;
      RTC.write(tm);
    } else if (currentFocus == 3) {
      tm.Month = months - 1;
      RTC.write(tm);
    }
  }

  int nextPinState = digitalRead(nextPin);
  if (nextPinState == 0 && !nextPinDown) {
    nextPinDown = true;
  }

  if (nextPinDown && nextPinState == 1) {
    nextPinDown = false;
    currentFocus++;
    if (currentFocus >= 4) {
      currentFocus = 0;
    }
  }

  if (RTC.read(tm)) {
    hours = tm.Hour;
    minutes = tm.Minute;
    days = tm.Day;
    months = tm.Month;
    Serial.print(minutes);
    Serial.print(":");
    Serial.print(hours);
    Serial.print(" - ");
    Serial.print(days);
    Serial.print("/");
    Serial.print(months);
    Serial.println();
  }

  closeLatch();

  convert(months, monthsData, monthsDataLength);
  convert(days, daysData, daysDataLength);
  convert(minutes, minutesData, minutesDataLength);
  convert(hours, hoursData, hoursDataLength);

  // months
  data[19] = monthsData[0];
  data[18] = monthsData[1];
  data[17] = monthsData[2];
  data[16] = monthsData[3];

  // days
  data[15] = daysData[0];
  data[14] = daysData[1];
  data[13] = daysData[2];
  data[12] = daysData[3];
  data[11] = daysData[4];

  // minutes
  data[10] = minutesData[0];
  data[9] = minutesData[1];
  data[8] = minutesData[2];
  data[7] = minutesData[3];
  data[6] = minutesData[4];
  data[5] = minutesData[5];

  // hours
  data[4] = hoursData[0];
  data[3] = hoursData[1];
  data[2] = hoursData[2];
  data[1] = hoursData[3];
  data[0] = hoursData[4];


//  // months
//  data[19] = 1;
//  data[18] = 1;
//  data[17] = 1;
//  data[16] = 1;
//
//  // days
//  data[15] = 1;
//  data[14] = 1;
//  data[13] = 1;
//  data[12] = 1;
//  data[11] = 1;
//
//  // minutes
//  data[10] = 1;
//  data[9] = 1;
//  data[8] = 1;
//  data[7] = 1;
//  data[6] = 1;
//  data[5] = 1;
//
//  // hours
//  data[4] = 1;
//  data[3] = 1;
//  data[2] = 1;
//  data[1] = 1;
//  data[0] = 1;

  writeData();
  openLatch();

  if (minutes >= 60) {
    minutes = 0;
  }

  if (hours >= 24) {
    hours = 0;
  }

  if (days >= 31) {
    days = 0;
  }

  if (months >= 12) {
    months = 0;
  }
}

void closeLatch() {
  digitalWrite(latchPin, LOW);
}

void openLatch() {
  digitalWrite(latchPin, HIGH);
}

void openClock() {
  digitalWrite(clockPin, HIGH);
}

void closeClock() {
  digitalWrite(clockPin, LOW);
}

void writeData() {
  for (int i = 0; i <= 20; i++) {
    openClock();
    if (data[i] == '0') {
      digitalWrite(dataPin, LOW);
    } else {
      digitalWrite(dataPin, HIGH);
    }
    closeClock();
  }
}

void convert(int n, char *out, int s) {
  int num = n;
  uint8_t bitsCount = s;
  char str[bitsCount + 1];
  uint8_t i = 0;

  while (bitsCount--) {
    str[i++] = bitRead(num, bitsCount) + '0';
  }

  str[i] = '\0';
  strncpy(out, str, s);
}


bool getTime(const char *str)
{
  int Hour, Min, Sec;

  if (sscanf(str, "%d:%d:%d", &Hour, &Min, &Sec) != 3) return false;
  tm.Hour = Hour;
  tm.Minute = Min;
  tm.Second = Sec;
  return true;
}

bool getDate(const char *str)
{
  char Month[12];
  int Day, Year;
  uint8_t monthIndex;

  if (sscanf(str, "%s %d %d", Month, &Day, &Year) != 3) return false;
  for (monthIndex = 0; monthIndex < 12; monthIndex++) {
    if (strcmp(Month, monthName[monthIndex]) == 0) break;
  }
  if (monthIndex >= 12) return false;
  tm.Day = Day;
  tm.Month = monthIndex + 1;
  tm.Year = CalendarYrToTm(Year);
  return true;
}
