#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>


// ========================
// OLED
// ========================

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 display(
  SCREEN_WIDTH,
  SCREEN_HEIGHT,
  &Wire,
  -1
);


// ========================
// PINS
// ========================

// Road A
int redA = 2;
int yellowA = 3;
int greenA = 4;

// Road B
int redB = 5;
int yellowB = 6;
int greenB = 7;

// Ultrasonic - Road B
int trigPin = 8;
int echoPin = 9;

// Pedestrian
int buttonPin = 10;
int buzzerPin = 11;
int pedestrianRed = 12;
int pedestrianGreen = 13;


// ========================
// STATES
// ========================

enum State {
  A_GREEN,
  A_YELLOW,
  ALL_RED_1,

  B_GREEN,
  B_YELLOW,
  ALL_RED_2,

  PEDESTRIAN
};

State currentState = A_GREEN;
State nextStateAfterPedestrian;


// ========================
// REQUESTS
// ========================

bool pedestrianRequest = false;
bool vehicleRequestB = false;

bool lastButtonState = HIGH;


// ========================
// TIME
// ========================

unsigned long previousTime = 0;


// ========================
// ULTRASONIC
// ========================

long duration;
float distance;


// ========================
// SETUP
// ========================

void setup() {

  // Road A
  pinMode(redA, OUTPUT);
  pinMode(yellowA, OUTPUT);
  pinMode(greenA, OUTPUT);

  // Road B
  pinMode(redB, OUTPUT);
  pinMode(yellowB, OUTPUT);
  pinMode(greenB, OUTPUT);

  // Ultrasonic
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  // Pedestrian
  pinMode(pedestrianRed, OUTPUT);
  pinMode(pedestrianGreen, OUTPUT);

  // Button
  pinMode(buttonPin, INPUT_PULLUP);

  // Buzzer
  pinMode(buzzerPin, OUTPUT);

  Serial.begin(9600);


  // ========================
  // OLED START
  // ========================

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {

    Serial.println("OLED not found");

    while (true);
  }

  display.clearDisplay();

  display.setTextColor(SSD1306_WHITE);

  display.setTextSize(1);

  display.setCursor(15, 15);
  display.println("SMART TRAFFIC");

  display.setCursor(20, 35);
  display.println("SYSTEM READY");

  display.display();

  delay(1500);


  previousTime = millis();
}


// ========================
// LOOP
// ========================

void loop() {

  unsigned long currentTime = millis();


  // ========================
  // PEDESTRIAN BUTTON
  // ========================

  bool buttonState = digitalRead(buttonPin);

  if (
    buttonState == LOW &&
    lastButtonState == HIGH
  ) {

    pedestrianRequest = true;

    Serial.println("Pedestrian Request");
  }

  lastButtonState = buttonState;


  // ========================
  // VEHICLE DETECTION
  // ========================

  if (currentState != B_GREEN) {

    distance = readDistance();

    if (distance < 20) {

      vehicleRequestB = true;

      Serial.println("Vehicle Request - Road B");
    }
  }


  // ========================
  // STATE MACHINE
  // ========================

  switch (currentState) {


    // =====================
    // ROAD A GREEN
    // =====================

    case A_GREEN:

      setLights(
        LOW, LOW, HIGH,
        HIGH, LOW, LOW
      );

      pedestrianStop();

      showTraffic(
        "A: GREEN",
        "B: RED"
      );


      // بعد 5 ثواني
      if (
        currentTime - previousTime >= 5000
      ) {

        // لا نغير إلا إذا أحد ينتظر
        if (
          vehicleRequestB ||
          pedestrianRequest
        ) {

          currentState = A_YELLOW;

          previousTime = currentTime;
        }
      }

      break;



    // =====================
    // ROAD A YELLOW
    // =====================

    case A_YELLOW:

      setLights(
        LOW, HIGH, LOW,
        HIGH, LOW, LOW
      );

      pedestrianStop();

      showTraffic(
        "A: YELLOW",
        "B: RED"
      );


      if (
        currentTime - previousTime >= 2000
      ) {

        currentState = ALL_RED_1;

        previousTime = currentTime;
      }

      break;



    // =====================
    // ALL RED 1
    // =====================

    case ALL_RED_1:

      setLights(
        HIGH, LOW, LOW,
        HIGH, LOW, LOW
      );

      pedestrianStop();

      showTraffic(
        "A: RED",
        "B: RED"
      );


      if (
        currentTime - previousTime >= 1000
      ) {


        // المشاة أولاً
        if (pedestrianRequest) {

          if (vehicleRequestB) {

            nextStateAfterPedestrian =
              B_GREEN;
          }

          else {

            nextStateAfterPedestrian =
              A_GREEN;
          }

          currentState = PEDESTRIAN;

          pedestrianRequest = false;
        }


        // سيارة تنتظر
        else if (vehicleRequestB) {

          currentState = B_GREEN;

          vehicleRequestB = false;
        }


        else {

          currentState = A_GREEN;
        }


        previousTime = currentTime;
      }

      break;



    // =====================
    // ROAD B GREEN
    // =====================

    case B_GREEN:

      setLights(
        HIGH, LOW, LOW,
        LOW, LOW, HIGH
      );

      pedestrianStop();

      showTraffic(
        "A: RED",
        "B: GREEN"
      );


      if (
        currentTime - previousTime >= 8000
      ) {

        currentState = B_YELLOW;

        previousTime = currentTime;
      }

      break;



    // =====================
    // ROAD B YELLOW
    // =====================

    case B_YELLOW:

      setLights(
        HIGH, LOW, LOW,
        LOW, HIGH, LOW
      );

      pedestrianStop();

      showTraffic(
        "A: RED",
        "B: YELLOW"
      );


      if (
        currentTime - previousTime >= 2000
      ) {

        currentState = ALL_RED_2;

        previousTime = currentTime;
      }

      break;



    // =====================
    // ALL RED 2
    // =====================

    case ALL_RED_2:

      setLights(
        HIGH, LOW, LOW,
        HIGH, LOW, LOW
      );

      pedestrianStop();

      showTraffic(
        "A: RED",
        "B: RED"
      );


      if (
        currentTime - previousTime >= 1000
      ) {


        if (pedestrianRequest) {

          nextStateAfterPedestrian =
            A_GREEN;

          currentState =
            PEDESTRIAN;

          pedestrianRequest = false;
        }

        else {

          currentState =
            A_GREEN;
        }


        previousTime = currentTime;
      }

      break;



    // =====================
    // PEDESTRIAN
    // =====================

    case PEDESTRIAN:

      setLights(
        HIGH, LOW, LOW,
        HIGH, LOW, LOW
      );

      pedestrianGo();

      showPedestrian();


      if (
        currentTime - previousTime >= 5000
      ) {

        pedestrianStop();

        currentState =
          nextStateAfterPedestrian;


        if (
          nextStateAfterPedestrian ==
          B_GREEN
        ) {

          vehicleRequestB = false;
        }


        previousTime = currentTime;
      }

      break;
  }
}


// ========================
// TRAFFIC LIGHTS
// ========================

void setLights(
  int rA,
  int yA,
  int gA,

  int rB,
  int yB,
  int gB
) {

  digitalWrite(redA, rA);
  digitalWrite(yellowA, yA);
  digitalWrite(greenA, gA);

  digitalWrite(redB, rB);
  digitalWrite(yellowB, yB);
  digitalWrite(greenB, gB);
}


// ========================
// PEDESTRIAN STOP
// ========================

void pedestrianStop() {

  digitalWrite(
    pedestrianRed,
    HIGH
  );

  digitalWrite(
    pedestrianGreen,
    LOW
  );

  noTone(buzzerPin);
}


// ========================
// PEDESTRIAN GO
// ========================

void pedestrianGo() {

  digitalWrite(
    pedestrianRed,
    LOW
  );

  digitalWrite(
    pedestrianGreen,
    HIGH
  );

  tone(
    buzzerPin,
    1000
  );
}


// ========================
// ULTRASONIC
// ========================

float readDistance() {

  digitalWrite(
    trigPin,
    LOW
  );

  delayMicroseconds(2);


  digitalWrite(
    trigPin,
    HIGH
  );

  delayMicroseconds(10);


  digitalWrite(
    trigPin,
    LOW
  );


  duration =
    pulseIn(
      echoPin,
      HIGH,
      30000
    );


  if (duration == 0) {

    return 999;
  }


  return
    duration *
    0.0343 /
    2;
}


// ========================
// OLED - NORMAL TRAFFIC
// ========================

void showTraffic(
  String roadA,
  String roadB
) {

  display.clearDisplay();

  display.setTextSize(1);

  display.setCursor(20, 0);

  display.println(
    "SMART TRAFFIC"
  );


  display.setCursor(0, 20);

  display.println(roadA);


  display.setCursor(0, 35);

  display.println(roadB);


  // إذا توجد سيارة تنتظر
  if (vehicleRequestB) {

    display.setCursor(0, 50);

    display.println(
      "CAR B DETECTED"
    );
  }


  // إذا المشاة ينتظرون
  else if (pedestrianRequest) {

    display.setCursor(0, 50);

    display.println(
      "PED WAITING"
    );
  }


  display.display();
}


// ========================
// OLED - PEDESTRIAN
// ========================

void showPedestrian() {

  display.clearDisplay();

  display.setTextSize(1);

  display.setCursor(20, 5);

  display.println(
    "PEDESTRIAN"
  );


  display.setTextSize(2);

  display.setCursor(15, 25);

  display.println(
    "CROSS"
  );


  display.setTextSize(1);

  display.setCursor(25, 50);

  display.println(
    "CARS STOP"
  );


  display.display();
}
