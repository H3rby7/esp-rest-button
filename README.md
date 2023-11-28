# ESP32 REST BUTTON

A physiacl button to the ESP32 will send out a REST request to a given server.

- [ESP32 REST BUTTON](#esp32-rest-button)
- [Setup](#setup)
  - [Drivers](#drivers)
- [Development and Flashing](#development-and-flashing)
  - [VS-Code with Espressif](#vs-code-with-espressif)
- [Run](#run)
  - [Setting up WIFI](#setting-up-wifi)

# Setup

## Drivers

To support UART flashing and debugging:

1. Download the [CP210x VCP](https://www.silabs.com/developers/usb-to-uart-bridge-vcp-drivers?tab=downloads) driver for your OS
2. Unpack, if necessary
3. Follow the steps for your Operating System to install the drivers.

# Development and Flashing

The code must be compiled and flashed onto the ESP32 microcontroller via USB.

Prerequisites:

1. Have the necessary drivers installed [as described above](#drivers)
2. Make sure your USB to micro-USB cable can transport data and not only charge!
3. [Visual Studio Code with the `espressif` extension](#vs-code-with-espressif) to support the build for the ESP with all required libraries.

## VS-Code with Espressif

1. Open this repository in its own VS-Code instance.
2. Install the [Espressif Extension](https://marketplace.visualstudio.com/items?itemName=espressif.esp-idf-extension) - i used the extension version 1.6.3
3. Follow the extension setup process
   1. Select an ESP-IDF version (i used 5.0.2) 
      * Note: This version differs from the extension versions, as it is two different things.
   2. Select an installation directory (i chose the workspace directory)
4. After the installation, restart VS-Code, just to be sure.
5. Make sure you followed the instructions for [installing the driver](#setup)
6. Select your ESP32 device by:
   1. Selecting `UART` as protocol (the star symbol at the bottom in VS Code)
   2. Selecting the right USB `COM` interface.

Here is an example `settings.json` for VSCode which can be put in the folder `.vscode.`.
I especially recommend the setting for `files.readonlyInclude` to prevent editing ESP32 library files by accident and `terminal.integrated.scrollback` as an ESP restart quickly floods the default terminal size.

```json
{
  "idf.flashType": "UART",
  "idf.adapterTargetName": "esp32",
  "idf.openOcdConfigs": [
    "board/esp32-bridge.cfg"
  ],
  "terminal.integrated.scrollback": 10000,
  "files.readonlyInclude": {
    "build/**": true,
    "**/esp/esp-idf/**": true,
    "**/.espressif/**": true
  }
}
```

You can then run `ESP-IDF` commands from VS-Code (CTRL+SHIFT+P)

* ESP-IDF: Build your Project
* ESP-IDF: Flash your Project
* ESP-IDF: Erase flash memory from device

or select them from the bottom tray menu.

# Run

1. Build your project (CTRL+SHIFT+P -> ESP-IDF: Build your project)
2. Flash the Code onto the ESP32 (CTRL+SHIFT+P -> ESP-IDF: Flash your Project)
3. Turn on the power

## Setting up WIFI

1. If no previously configured wifi can be reached, the esp32 opens its own wifi with SSID `ESP32-REST-BTN` and no password.
2. Connect to this wifi with a device and visit the default gateway (http://192.168.4.1) with your favorite browser. 
3. On the webpage insert the credentials of the network you want to provision.
