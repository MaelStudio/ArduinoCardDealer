#include <Stepper.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// pins
const byte buttonPin = 2;
const byte potPin = A3;

// steppers
const int stepsPerRevolution = 2038;
const int stepsPerCard = -1700;
const int stepsResetCard = 300;
Stepper spinStepper = Stepper(stepsPerRevolution, 8, 10, 9, 11);
Stepper cardStepper = Stepper(stepsPerRevolution, 4, 6, 5, 7);

// oled
const int displayAddress = 0x3C;
Adafruit_SSD1306 display(128, 64, &Wire, -1);

// vars
int players;
const int playersMin = 1;
const int playersMax = 15;

int cardsPerPlayer;
const int cardsMin = 1;
const int cardsMax = 30;

const int potMin = 1;
const int potMax = 30;

void setup() {
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(potPin, INPUT);

  spinStepper.setSpeed(15);
  cardStepper.setSpeed(15);

  display.begin(SSD1306_SWITCHCAPVCC, displayAddress);
  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
}

void loop() {

  
  // select players count
	while(digitalRead(buttonPin)) {
    
    players = 1023-analogRead(potPin);
    players = map(players, 0, 1023, potMin, potMax);
    if(players > playersMax) {
      players = playersMax;
    }
    if(players < playersMin) {
      players = playersMin;
    }

    display.clearDisplay();
    display.setCursor(0, 0);
    display.print("Select\nplayers:");
    display.setCursor(52, 40);
    display.print(players);
    display.drawRect(49, 37, 30, 20, SSD1306_WHITE);
    display.fillTriangle(40, 37, 30, 47, 40, 57, SSD1306_WHITE);
    display.fillTriangle(87, 37, 97, 47, 87, 57, SSD1306_WHITE);
    display.display();
  }

  while(!digitalRead(buttonPin)) {} // wait for button release
  delay(50);

  // select cards per player count
  while(digitalRead(buttonPin)) {
    
    cardsPerPlayer = 1023-analogRead(potPin);
    cardsPerPlayer = map(cardsPerPlayer, 0, 1023, potMin, potMax);
    if(cardsPerPlayer > cardsMax) {
      cardsPerPlayer = cardsMax;
    }
    if(cardsPerPlayer < cardsMin) {
      cardsPerPlayer = cardsMin;
    }

    display.clearDisplay();
    display.setCursor(0, 0);
    display.print("Select\ncards:");
    display.setCursor(52, 40);
    display.print(cardsPerPlayer);
    display.drawRect(49, 37, 30, 20, SSD1306_WHITE);
    display.fillTriangle(40, 37, 30, 47, 40, 57, SSD1306_WHITE);
    display.fillTriangle(87, 37, 97, 47, 87, 57, SSD1306_WHITE);
    display.display();

  }
  unsigned long start = millis();
  while(millis()-start < 3000) {
    display.clearDisplay();
    display.setCursor(0, 0);
    display.print("Starting!");
    displayProgressBar(float(millis()-start)/3000, 2, 30, 124, 10);
    display.display();
  }

  start = millis();

  for(int p=0; p < players; p++) {
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("Dealing\ncards to\nplayer #");
    display.print(p);
    display.print(" / ");
    display.print(players);
    display.display();

    for(int c=0;c<cardsPerPlayer;c++) {
      cardStepper.step(stepsPerCard); // deal cards
      cardStepper.step(stepsResetCard);
    }
    if(p<players-1) {
      spinStepper.step(stepsPerRevolution*1.1 / players); // spin for next player
    }
    
  }

  display.clearDisplay();
  display.setCursor(0, 0);
  display.print("Done!\n\nFinished\nin ");
  display.print((millis()-start)/1000);
  display.print(" s");
  display.display();

  while(digitalRead(buttonPin)) {} // wait for button press
  while(!digitalRead(buttonPin)) {} // wait for button release
  delay(50);
}

void displayProgressBar(float progress, int x, int y, int w, int h) {
  display.drawRect(x, y, w, h, SSD1306_WHITE);
  display.fillRect(x+3, y+3, ceil(w*progress)-6, h-6, SSD1306_WHITE);
}