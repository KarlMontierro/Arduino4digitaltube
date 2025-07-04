// === Segment Pins ===
int pinA = 11;
int pinB = 7;
int pinC = 4;
int pinD = 2;
int pinE = A0;   // was pin 1 — moved to avoid Serial conflict
int pinF = 10;
int pinG = 5;
int pinDP = 3;   // Decimal point (dot)

int segmentPins[] = {pinA, pinB, pinC, pinD, pinE, pinF, pinG};

// === Digit Control Pins (Common Cathode) ===
int digitPins[] = {12, 9, 8, 6};  // D1, D2, D3, D4

// Segment map for digits 0–9
byte numbers[10][7] = {
  {HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, LOW},  // 0
  {LOW, HIGH, HIGH, LOW, LOW, LOW, LOW},      // 1
  {HIGH, HIGH, LOW, HIGH, HIGH, LOW, HIGH},   // 2
  {HIGH, HIGH, HIGH, HIGH, LOW, LOW, HIGH},   // 3
  {LOW, HIGH, HIGH, LOW, LOW, HIGH, HIGH},    // 4
  {HIGH, LOW, HIGH, HIGH, LOW, HIGH, HIGH},   // 5
  {HIGH, LOW, HIGH, HIGH, HIGH, HIGH, HIGH},  // 6
  {HIGH, HIGH, HIGH, LOW, LOW, LOW, LOW},     // 7
  {HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH}, // 8
  {HIGH, HIGH, HIGH, HIGH, LOW, HIGH, HIGH}   // 9
};

// Time tracking
unsigned long previousMillis = 0;
int seconds = 0;
int minutes = 0;
int hours = 0;

void setup() {
  Serial.begin(9600);

  for (int i = 0; i < 7; i++) pinMode(segmentPins[i], OUTPUT);
  pinMode(pinDP, OUTPUT);
  digitalWrite(pinDP, LOW);

  for (int i = 0; i < 4; i++) {
    pinMode(digitPins[i], OUTPUT);
    digitalWrite(digitPins[i], HIGH);  // Turn OFF initially
  }

  // Read time from Serial (format "HH:MM")
  while (!Serial.available()) delay(100);
  String input = Serial.readStringUntil('\n');
  int hh = input.substring(0, 2).toInt();
  int mm = input.substring(3, 5).toInt();

  if (hh >= 0 && hh < 24 && mm >= 0 && mm < 60) {
    hours = hh;
    minutes = mm;
    seconds = 0;
  }
}

void clearSegments() {
  for (int i = 0; i < 7; i++) digitalWrite(segmentPins[i], LOW);
  digitalWrite(pinDP, LOW);
}

void displayDigit(int digitIndex, int number, bool enableDot) {
  clearSegments();
  for (int s = 0; s < 7; s++) {
    digitalWrite(segmentPins[s], numbers[number][s]);
  }

  // Blinking dot every even second
  if (enableDot && (seconds % 2 == 0)) {
    digitalWrite(pinDP, HIGH);
  }

  digitalWrite(digitPins[digitIndex], LOW);   // Turn on digit
  delay(3);
  digitalWrite(digitPins[digitIndex], HIGH);  // Turn off digit
  digitalWrite(pinDP, LOW);                   // Reset dot
}

void loop() {
  // Update time every second
  if (millis() - previousMillis >= 1000) {
    previousMillis = millis();
    seconds++;
    if (seconds >= 60) {
      seconds = 0;
      minutes++;
      if (minutes >= 60) {
        minutes = 0;
        hours++;
        if (hours >= 24) hours = 0;
      }
    }
  }

  // Convert to 12-hour format
  int h = hours % 12;
  if (h == 0) h = 12;

  // Break into digits
  bool hideFirstDigit = (h < 10);
  int digits[4];
  digits[0] = hideFirstDigit ? -1 : h / 10;
  digits[1] = h % 10;
  digits[2] = minutes / 10;
  digits[3] = minutes % 10;

  
  for (int i = 0; i < 30; i++) {
    // Digit 0
    if (digits[0] >= 0) {
      displayDigit(0, digits[0], false);
    } else {
      clearSegments();
      digitalWrite(digitPins[0], LOW);
      delay(3);
      digitalWrite(digitPins[0], HIGH);
    }

    // Digit 1 with dot
    displayDigit(1, digits[1], true);
    displayDigit(2, digits[2], false);
    displayDigit(3, digits[3], false);
  }
}
