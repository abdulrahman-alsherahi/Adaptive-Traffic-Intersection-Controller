# 🚦 Adaptive Traffic Intersection Controller

An Arduino-based **smart traffic intersection system** that adapts to vehicle detection and pedestrian requests using a **Finite State Machine (FSM)**.

The project combines traffic-light control, ultrasonic vehicle detection, pedestrian crossing, buzzer feedback, and an OLED status display into one embedded system.

---

## 📸 Project Overview

![System Overview](images/system-overview.png)

### Detailed System Diagram

![Project Infographic](images/project-infographic.png)

---

## ✨ Features

- 🚦 Two-way traffic light control
- 🚗 Vehicle detection using an HC-SR04 ultrasonic sensor
- 🧠 Adaptive traffic control based on vehicle requests
- 🚶 Pedestrian crossing request using a push button
- 🔴🟢 Dedicated pedestrian traffic light
- 🔊 Buzzer feedback during pedestrian crossing
- 🖥️ SSD1306 OLED display for real-time system status
- ⚠️ Safe `ALL_RED` transition between traffic directions
- 🔄 Finite State Machine architecture
- ⏱️ Non-blocking timing using `millis()`
- 📡 Event-based vehicle and pedestrian requests

---

## 🧠 System Concept

The system controls two traffic directions:

- **Road A**
- **Road B**

Road A has the default priority.

Road B contains an ultrasonic sensor that detects a waiting vehicle.

If no vehicle or pedestrian is waiting, Road A remains green.

When a vehicle is detected on Road B, the system stores a vehicle request and safely transitions through:

```text
A_GREEN
   ↓
A_YELLOW
   ↓
ALL_RED
   ↓
B_GREEN
```

After Road B finishes its cycle, control returns safely to Road A.

---

## 🚶 Pedestrian System

A pedestrian can request to cross by pressing the push button.

The request is stored instead of changing the traffic lights immediately.

The controller first completes the current traffic phase safely.

Then:

```text
Road A → RED
Road B → RED
        ↓
Pedestrian → GREEN
        ↓
Buzzer → ON
        ↓
Crossing period
        ↓
Pedestrian → RED
```

This prevents unsafe immediate changes when the button is pressed.

---

## 🚗 Adaptive Vehicle Detection

An **HC-SR04 ultrasonic sensor** monitors Road B.

When an object is detected within approximately:

```text
20 cm
```

the controller creates:

```cpp
vehicleRequestB = true;
```

The traffic controller then gives Road B its green phase when it is safe to do so.

This means Road B does not receive a green light unnecessarily when no vehicle is waiting.

---

## 🖥️ OLED Interface

The SSD1306 OLED provides real-time information about the system.

Example normal state:

```text
SMART TRAFFIC

A: GREEN
B: RED
```

When a vehicle is detected:

```text
SMART TRAFFIC

A: GREEN
B: RED

CAR B DETECTED
```

During pedestrian crossing:

```text
PEDESTRIAN

CROSS

CARS STOP
```

---

## 🔄 Finite State Machine

The controller is implemented using a **Finite State Machine (FSM)**.

Main states:

```text
A_GREEN
A_YELLOW
ALL_RED_1
B_GREEN
B_YELLOW
ALL_RED_2
PEDESTRIAN
```

Typical traffic sequence:

```text
        ┌─────────┐
        │ A_GREEN │
        └────┬────┘
             ↓
       ┌──────────┐
       │ A_YELLOW │
       └────┬─────┘
            ↓
       ┌───────────┐
       │ ALL_RED_1 │
       └─────┬─────┘
             ↓
        ┌─────────┐
        │ B_GREEN │
        └────┬────┘
             ↓
       ┌──────────┐
       │ B_YELLOW │
       └────┬─────┘
            ↓
       ┌───────────┐
       │ ALL_RED_2 │
       └─────┬─────┘
             ↓
          A_GREEN
```

The `PEDESTRIAN` state can be inserted when a pedestrian request is waiting.

---

## 🔌 Hardware

| Component | Quantity |
|---|---:|
| Arduino UNO | 1 |
| Breadboard | 2 |
| Red LED | 3 |
| Yellow LED | 2 |
| Green LED | 3 |
| 220 Ω resistors | Multiple |
| HC-SR04 ultrasonic sensor | 1 |
| SSD1306 OLED I2C display | 1 |
| Push button | 1 |
| Buzzer | 1 |
| Jumper wires | Multiple |

---

## 📌 Pin Configuration

### Road A

| Function | Arduino Pin |
|---|---:|
| Red LED | D2 |
| Yellow LED | D3 |
| Green LED | D4 |

### Road B

| Function | Arduino Pin |
|---|---:|
| Red LED | D5 |
| Yellow LED | D6 |
| Green LED | D7 |

### Ultrasonic Sensor

| HC-SR04 | Arduino |
|---|---|
| TRIG | D8 |
| ECHO | D9 |
| VCC | 5V |
| GND | GND |

### Pedestrian System

| Function | Arduino Pin |
|---|---:|
| Push Button | D10 |
| Buzzer | D11 |
| Red LED | D12 |
| Green LED | D13 |

### OLED Display

| OLED | Arduino UNO |
|---|---|
| SDA | A4 |
| SCL | A5 |
| VCC | 5V |
| GND | GND |

---

## 💻 Software

The project was developed using:

- Arduino IDE
- C / C++
- Adafruit GFX Library
- Adafruit SSD1306 Library
- Git & GitHub

---

## 📚 Required Libraries

Install the following libraries using the Arduino IDE Library Manager:

```text
Adafruit GFX Library
Adafruit SSD1306
```

Dependencies such as `Adafruit BusIO` may also be installed automatically.

---

## ⏱️ Timing

Current traffic timing:

| State | Duration |
|---|---:|
| Road A Green | Minimum 5 s |
| Road A Yellow | 2 s |
| All Red | 1 s |
| Road B Green | 8 s |
| Road B Yellow | 2 s |
| Pedestrian Crossing | 5 s |

The timing is controlled using:

```cpp
millis()
```

instead of long blocking `delay()` calls.

This allows the system to continue monitoring buttons and sensors while the traffic controller is running.

---

## 🗂️ Project Structure

```text
Adaptive-Traffic-Intersection-Controller/
│
├── src/
│   └── SmartTrafficController.ino
│
├── images/
│   ├── system-overview.png
│   └── project-infographic.png
│
└── README.md
```

---

## 🎯 What I Learned

This project was built as part of my embedded systems learning journey.

It helped me practice:

- GPIO input/output
- Digital sensors
- I2C communication
- Ultrasonic distance measurement
- Embedded state machines
- Event-driven logic
- Non-blocking timing
- System-level thinking
- Hardware/software integration
- Modular embedded programming
- Debugging hardware connections

It also provides practical experience related to topics such as **digital systems, algorithms, electronics, and embedded software development**.

---

## 🚀 Future Improvements

Possible future versions include:

- 📡 Second ultrasonic sensor for Road A
- 🚶 Second pedestrian crossing
- 🌙 LDR-based night mode
- 🚑 Emergency vehicle priority
- 📊 Traffic statistics and event logging
- 💾 Data logging to a computer
- 🔧 Refactoring firmware into multiple modules
- ⚙️ STM32 implementation
- 🧠 VHDL / FPGA traffic controller
- 🖥️ PC monitoring application
- 🔌 Custom PCB design

---

## 📈 Development Roadmap

```text
Arduino Prototype
       ↓
Adaptive Traffic Logic
       ↓
OLED Interface
       ↓
Improved Software Architecture
       ↓
STM32 Version
       ↓
VHDL / FPGA Controller
       ↓
Custom PCB
```

---

## 👨‍💻 Author

**Abdulrahman Al-Sherahi**

Computer Engineering student interested in:

- Embedded Systems
- Embedded Software
- Digital Hardware
- FPGA
- Electronics
- Hardware/Software Integration

---

## ⭐ Project Status

**Version 1.0 — Working Prototype**

The current prototype successfully integrates traffic control, vehicle detection, pedestrian requests, audible feedback, and real-time OLED status information.
