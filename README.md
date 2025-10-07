
# Battery Management & Charging Controller (BMS): Board: MKR WIFI 1010 

## 1. Quick Overview

This project is the intelligent brain (Controller) for a Lithium-ion battery system, designed to manage charging, ensure safety, and communicate key data. It's built on the **Arduino MKR WIFI 1010** board using the **FreeRTOS** operating system, which allows it to handle many critical tasks—like reading battery health, controlling the charger, and talking to a display—all at the same time without missing a beat.

The system connects to three main components:
1.  **Li-ion Battery Pack (via BMS):** To read its health (State of Charge, temperature, voltage).
2.  **On-Board Charger (OBC):** To command charging start/stop and set the correct voltage/current.
3.  **Human-Machine Interface (HMI):** A display or mobile app (via an intermediate module) for the user to monitor status and send commands.

## 2. Key Features

| Feature | Description | File Reference |
| :--- | :--- | :--- |
| **Smart Charging Control** | Automatically calculates the safest maximum charge current based on the battery's temperature and State of Charge (SoC), protecting the battery life. | `CHARGE_control.ino` |
| **Dual-Channel CAN Bus** | Uses two separate high-speed communication lines to manage different devices: **500 kbps** for the Battery's BMS and **250 kbps** for the On-Board Charger. | `CAN_COMM_SETUP.ino` |
| **Charging Plug Detection** | Monitors the **Proximity Pilot (Proxi)** signal to instantly detect when a charging cable is plugged in or disconnected. | `PROXI_task.ino` |
| **Max Current Negotiation** | Reads the **Control Pilot (CP)** signal from the charging station to determine the maximum current the external infrastructure can safely provide. | `CP_task.ino` |
| **Driver Interface (HMI)** | Transmits a stream of 28 key battery parameters (like SoC, Temp, Voltage) to a display (via a secondary serial connection, possibly for a BLE module) and accepts commands from the user (e.g., to turn on switches). | `HMI_task.ino` |
| **Power Management** | Controls physical relays/switches for essential functions like the DC 12V supply, the Inverter, and the main charging line. | `IO_task.ino` |
| **Safety and Diagnostics** | Constantly checks for errors like over/under voltage, current, and temperature, and manages communication timeouts for both CAN channels. | `CHARGE_control.ino`, `BMS_task.ino`, `OBC_task.ino` |

## 3. System Architecture (The Tasks)

The system relies on **FreeRTOS** to run several functions in parallel. Each major function is organized into its own dedicated task:

| Task File | Primary Component Controlled/Monitored |
| :--- | :--- |
| `BMS_task.ino` | Li-ion Battery Pack (via CAN) |
| `OBC_task.ino` | On-Board Charger (via CAN) |
| `PROXI_task.ino` | Charging Plug Connection Status |
| `CP_task.ino` | Charging Station Current Limit |
| `HMI_task.ino` | User Interface / Display |
| `IO_task.ino` | Physical Relays and Status LEDs |

## 4. Hardware and Dependencies

### Hardware
* **Microcontroller:** Arduino MKR WIFI 1010 (SAMD21-based board).
* **Communication:** Dual CAN Bus Interface (to support the 500kbps and 250kbps channels).

### Software/Libraries
This project requires several standard Arduino libraries, including:
* `FreeRTOS_SAMD21.h` (Critical for the multi-tasking structure)
* `CAN.h` and `CAN2.h` (For managing the two independent CAN channels)
* `Adafruit_SleepyDog.h` (For watchdog timer/reliability)

### Getting Started

1.  **Setup:** Place all `.ino` files in a single folder named after the main file (e.g., `BMS_MKR_1010`) in your Arduino sketchbook.
2.  **Install:** Ensure all required libraries are installed in your Arduino IDE.
3.  **Upload:** Compile and upload the sketch to your Arduino MKR WIFI 1010 board.
4.  **Monitor:** Use the Serial Monitor (115200 baud) to view the initialization process and task status messages.
