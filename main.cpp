#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define OLED_ADDRESS 0x3C

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define TOUCH_PIN 2
#define BUZZER_PIN 8

bool lastTouchState = LOW;
unsigned long lastTouchTime = 0;
unsigned long lastBlinkTime = 0;
unsigned long lastIdleTime = 0;
bool isSleeping = false;
int reactionIndex = 0; // cycles through reactions

void setup() {
  Serial.begin(9600);

  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);
  noTone(BUZZER_PIN);

  pinMode(TOUCH_PIN, INPUT);

  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDRESS)) {
    Serial.println("OLED not found! Try 0x3D");
    while (true);
  }

  display.clearDisplay();
  display.display();
  startupAnimation();

  lastTouchTime = millis();
  lastBlinkTime = millis();
  lastIdleTime = millis();
  drawNormal();
}

void loop() {
  bool touched = digitalRead(TOUCH_PIN);

  if (touched == HIGH && lastTouchState == LOW) {
    isSleeping = false;
    lastTouchTime = millis();

    // Cycle through reactions so you see all of them
    reactionIndex = (reactionIndex + 1) % 4;

    switch (reactionIndex) {
      case 0:
        drawHappy();
        playMelody_happy();
        delay(1800);
        break;
      case 1:
        drawHeart();
        playMelody_giggle();
        delay(1800);
        break;
      case 2:
        drawWink();
        playMelody_wink();
        delay(1500);
        break;
      case 3:
        drawDizzy();
        playMelody_dizzy();
        delay(1500);
        break;
    }

    drawNormal();
  }

  lastTouchState = touched;

  // Blink every 3.5 seconds
  if (!isSleeping && millis() - lastBlinkTime > 3500) {
    blinkEyes();
    lastBlinkTime = millis();
  }

  // Look around every 6 seconds
  if (!isSleeping && millis() - lastIdleTime > 6000) {
    lookAround();
    lastIdleTime = millis();
  }

  // Sleep after 12 seconds
  if (!isSleeping && millis() - lastTouchTime > 12000) {
    isSleeping = true;
    sleepTransition();
  }

  delay(50);
}

// =====================
// 🎵 MELODIES
// =====================

void playMelody_happy() {
  int notes[]    = {523, 659, 784, 1047};
  int durations[] = {80,  80,  80,  180};
  for (int i = 0; i < 4; i++) {
    tone(BUZZER_PIN, notes[i], durations[i]);
    delay(durations[i] + 20);
  }
  noTone(BUZZER_PIN);
  digitalWrite(BUZZER_PIN, LOW);
}

void playMelody_giggle() {
  int notes[]    = {784, 523, 784, 1047, 1318};
  int durations[] = {70,  70,  70,  70,   180};
  for (int i = 0; i < 5; i++) {
    tone(BUZZER_PIN, notes[i], durations[i]);
    delay(durations[i] + 15);
  }
  noTone(BUZZER_PIN);
  digitalWrite(BUZZER_PIN, LOW);
}

void playMelody_wink() {
  int notes[]    = {1047, 784, 1047};
  int durations[] = {80,   80,  160};
  for (int i = 0; i < 3; i++) {
    tone(BUZZER_PIN, notes[i], durations[i]);
    delay(durations[i] + 20);
  }
  noTone(BUZZER_PIN);
  digitalWrite(BUZZER_PIN, LOW);
}

void playMelody_dizzy() {
  int notes[]    = {600, 400, 300, 200};
  int durations[] = {80,  80,  80,  200};
  for (int i = 0; i < 4; i++) {
    tone(BUZZER_PIN, notes[i], durations[i]);
    delay(durations[i] + 20);
  }
  noTone(BUZZER_PIN);
  digitalWrite(BUZZER_PIN, LOW);
}

void playStartupChime() {
  int notes[]    = {523, 659, 784, 1047};
  int durations[] = {90,  90,  90,  200};
  for (int i = 0; i < 4; i++) {
    tone(BUZZER_PIN, notes[i], durations[i]);
    delay(durations[i] + 20);
  }
  noTone(BUZZER_PIN);
  digitalWrite(BUZZER_PIN, LOW);
}

// =====================
// 🤖 ROBOT EYES HELPER
// =====================
// Left eye box:  x=20, y=14, w=34, h=28
// Right eye box: x=74, y=14, w=34, h=28

void drawEyeBoxes() {
  display.fillRoundRect(20, 14, 34, 28, 4, WHITE);
  display.fillRoundRect(74, 14, 34, 28, 4, WHITE);
}

void drawPupils(int lx, int ly, int rx, int ry) {
  display.fillRect(lx, ly, 10, 10, BLACK);
  display.fillRect(rx, ry, 10, 10, BLACK);
}

// =====================
// 😐 NORMAL FACE
// =====================
void drawNormal() {
  display.clearDisplay();
  drawEyeBoxes();
  // Centered pupils
  drawPupils(32, 23, 86, 23);
  // Straight mouth
  display.drawLine(44, 54, 84, 54, WHITE);
  display.drawLine(44, 55, 84, 55, WHITE);
  display.display();
}

// =====================
// 😄 HAPPY FACE  ^ ^
// =====================
void drawHappy() {
  display.clearDisplay();
  // Happy arc eyes (^) inside boxes
  display.fillRoundRect(20, 14, 34, 28, 4, WHITE);
  display.fillRoundRect(74, 14, 34, 28, 4, WHITE);
  // Black arcs drawn on top
  // Left eye - black fill bottom half = arc look
  display.fillRect(21, 26, 32, 15, BLACK);
  // Arc curve left
  for (int x = 0; x < 32; x++) {
    int y = 26 - (int)(6.0 * sin(x * 3.14159 / 32.0));
    display.drawPixel(21 + x, y, BLACK);
    display.drawPixel(21 + x, y + 1, BLACK);
  }
  // Right eye same
  display.fillRect(75, 26, 32, 15, BLACK);
  for (int x = 0; x < 32; x++) {
    int y = 26 - (int)(6.0 * sin(x * 3.14159 / 32.0));
    display.drawPixel(75 + x, y, BLACK);
    display.drawPixel(75 + x, y + 1, BLACK);
  }
  // Big smile
  for (int x = 34; x <= 94; x++) {
    int y = 48 + (int)(8.0 * sin((x - 34) * 3.14159 / 60.0));
    display.drawPixel(x, y, WHITE);
    display.drawPixel(x, y + 1, WHITE);
  }
  // Rosy cheeks
  display.drawCircle(14, 46, 5, WHITE);
  display.drawCircle(114, 46, 5, WHITE);
  display.display();
}

// =====================
// 😉 WINK FACE
// =====================
void drawWink() {
  display.clearDisplay();
  // Left eye normal box with pupil
  display.fillRoundRect(20, 14, 34, 28, 4, WHITE);
  drawPupils(32, 23, 86, 23);
  // Right eye wink — just a thick line
  display.fillRoundRect(74, 14, 34, 28, 4, WHITE);
  display.fillRect(75, 15, 32, 26, BLACK); // clear inside
  display.fillRoundRect(74, 25, 34, 8, 3, WHITE); // wink line
  // Smirk mouth
  display.drawLine(44, 52, 74, 50, WHITE);
  display.drawLine(44, 53, 74, 51, WHITE);
  display.display();
}

// =====================
// 💙 HEART EYES
// =====================
void drawHeart() {
  display.clearDisplay();
  // Left heart eye
  drawHeartAt(37, 22, 10);
  // Right heart eye
  drawHeartAt(91, 22, 10);
  // Smile
  for (int x = 34; x <= 94; x++) {
    int y = 48 + (int)(7.0 * sin((x - 34) * 3.14159 / 60.0));
    display.drawPixel(x, y, WHITE);
    display.drawPixel(x, y + 1, WHITE);
  }
  display.display();
}

void drawHeartAt(int cx, int cy, int size) {
  int s = size / 2;
  display.fillCircle(cx - s, cy, s, WHITE);
  display.fillCircle(cx + s, cy, s, WHITE);
  display.fillTriangle(cx - size, cy, cx + size, cy, cx, cy + size + 2, WHITE);
}

// =====================
// 😵 DIZZY / X EYES
// =====================
void drawDizzy() {
  display.clearDisplay();
  // Left X eye
  drawXEye(37, 28);
  // Right X eye
  drawXEye(91, 28);
  // Wavy/flat confused mouth
  display.drawLine(44, 54, 54, 50, WHITE);
  display.drawLine(54, 50, 64, 54, WHITE);
  display.drawLine(64, 54, 74, 50, WHITE);
  display.drawLine(74, 50, 84, 54, WHITE);
  display.display();
}

void drawXEye(int cx, int cy) {
  int s = 10;
  for (int i = -1; i <= 1; i++) {
    display.drawLine(cx - s, cy - s + i, cx + s, cy + s + i, WHITE);
    display.drawLine(cx + s, cy - s + i, cx - s, cy + s + i, WHITE);
  }
}

// =====================
// 💤 SLEEP
// =====================
void sleepTransition() {
  // Eyes closing slowly
  for (int h = 28; h >= 0; h -= 4) {
    display.clearDisplay();
    display.fillRoundRect(20, 28 - h/2, 34, h, 4, WHITE);
    display.fillRoundRect(74, 28 - h/2, 34, h, 4, WHITE);
    display.display();
    delay(60);
  }
  delay(200);
  drawSleep();
}

void drawSleep() {
  display.clearDisplay();
  // Closed eye lines
  display.fillRoundRect(20, 26, 34, 6, 3, WHITE);
  display.fillRoundRect(74, 26, 34, 6, 3, WHITE);
  // Zzz
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(98, 14); display.print("z");
  display.setCursor(105, 8); display.print("z");
  display.setCursor(112, 2); display.print("Z");
  // Sleepy mouth
  display.drawLine(50, 52, 78, 52, WHITE);
  display.display();
}

// =====================
// 👁️ BLINK
// =====================
void blinkEyes() {
  display.clearDisplay();
  display.fillRoundRect(20, 26, 34, 6, 3, WHITE);
  display.fillRoundRect(74, 26, 34, 6, 3, WHITE);
  display.drawLine(44, 54, 84, 54, WHITE);
  display.drawLine(44, 55, 84, 55, WHITE);
  display.display();
  delay(120);
  drawNormal();
}

// =====================
// 👀 LOOK AROUND
// =====================
void lookAround() {
  // Look right — pupils shift right
  display.clearDisplay();
  drawEyeBoxes();
  drawPupils(38, 23, 92, 23);
  display.drawLine(44, 54, 84, 54, WHITE);
  display.display();
  delay(700);

  // Look left — pupils shift left
  display.clearDisplay();
  drawEyeBoxes();
  drawPupils(22, 23, 76, 23);
  display.drawLine(44, 54, 84, 54, WHITE);
  display.display();
  delay(700);

  drawNormal();
}

// =====================
// ✨ STARTUP
// =====================
void startupAnimation() {
  // Boxes grow in
  for (int h = 4; h <= 28; h += 4) {
    display.clearDisplay();
    display.fillRoundRect(20, 28 - h/2, 34, h, 4, WHITE);
    display.fillRoundRect(74, 28 - h/2, 34, h, 4, WHITE);
    display.display();
    delay(50);
  }
  playStartupChime();
  delay(300);
}
