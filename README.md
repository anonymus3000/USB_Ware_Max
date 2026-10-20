<p align="center">
  <img src="img/USB_Ware.jpg" alt="USB Ware" width="1000"/>
</p>

**ESP32-S2 HID Automation and Control Firmware**

---

## 1. Introduction

**USB Ware Max** is a modular firmware developed for the **ESP32-S3 Dev kit** microcontroller, designed to transform the device into a **USB Human Interface Device (HID)**(i.e. Bad USB) capable of executing programmable keyboard and mouse automation tasks.

The firmware integrates an **embedded web server** allowing users to select and trigger multiple automation routines via a browser-based control panel.

---

## 2. Web Control Interface

Upon power-up, the ESP32-S2 initializes a Wi-Fi access point (AP) under default credentials:

* **SSID:** `USB Ware`
* **Password:** `HID1000#`
* **Hidden:** `false`
* **Local IP Address:** `10.0.0.1`

Users may connect to this network and access the control dashboard through:

```
http://10.0.0.1
```

The web interface provides a structured selection form, allowing execution of predefined operational modes. A dynamic status display provides feedback regarding current activity or response.

I also added a virtual mouse joystick/trackpad and direct type keyboard giving phone users more capabilities.
I also added couple more built in shortcuts (which are great for both phone and PC users

---

## 3. Requirements

<p align="center">
  <img src="img/ESP32-S2-Mini-V1.0.0-LOLIN-WIFI-IOT-Board.png" alt="ESP32-S2 Mini" width="200"/>
</p>


| Component                   | Specification                                                                        |
| --------------------------- | ------------------------------------------------------------------------------------ |
| **Microcontroller**         | ESP32-S2 & ESP32-S3 (native USB support required)                                    |
| **Development Environment** | Arduino IDE 1.8.19 or later                                                          |
| **Core Version**            | ESP32 Arduino Core ≥ 3.3.0                                                           |
| **Libraries**               | `WiFi.h`, `WebServer.h`, `DNSServer.h`, `USB.h`, `USBHIDKeyboard.h`, `USBHIDMouse.h` |

---

## #4. Note:
Everything is vibe coded with Gemini (Claude didnt want to do it at all).
Everything works on a ESP-32 S3 Developer Kit.
