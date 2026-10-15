# WeatherStation PRO ☀️

**Aurora Build 0009.2 — ESP32 touchscreen weather station**

WeatherStation PRO is a DIY touchscreen weather station for Arduino IDE and a compatible ESP32 3.2-inch display. Designed for makers and 3D printing enthusiasts.

## Firmware download

Click **Code → Download ZIP** to download the repository. **Aurora Build 0009.2 is available now:** [Browse the Arduino source](./WeatherStation_PRO_Aurora_Build_0009_2/) · [Download the complete repository as ZIP](https://github.com/Simch1/WeatherStation_PRO/archive/refs/heads/main.zip).

## Features

- Current weather, humidity, feels-like temperature and wind
- Five-day forecast and hourly weather information
- Open-Meteo weather data
- Wi-Fi and location configuration
- Italian and English interface
- Display settings and diagnostics

## Requirements

- ESP32 with compatible 3.2-inch TFT touchscreen
- Arduino IDE with Espressif ESP32 board support
- TFT_eSPI configured for your specific display
- ArduinoJson 7.x
- USB data cable

## Installation

1. Download the complete source code using the ZIP link above and extract it.
2. Keep the `.ino`, `.h`, and `.cpp` files together in the Arduino sketch directory.
3. Open the main `.ino` file with Arduino IDE.
4. Configure the display driver, touchscreen and pin mappings for your board.
5. Select your ESP32 board and USB port, then compile and upload.
6. Configure Wi-Fi and location on first startup.

**Hardware note:** Aurora Build 0009.2 uses GPIO 32 for the optional push button. Check the actual configuration before wiring.


## Video tutorials

Watch the project in action and learn how to set up Wi-Fi using your smartphone.

| Tutorial | Watch on GitHub | Download MP4 |
|---|---|---|
| **WeatherStation PRO – Device Overview** | [Watch video](https://github.com/Simch1/WeatherStation_PRO/blob/main/Video_Tutorial/WeatherStation_PRO_View.mp4) | [Download MP4](https://github.com/Simch1/WeatherStation_PRO/raw/refs/heads/main/Video_Tutorial/WeatherStation_PRO_View.mp4) |
| **WeatherStation PRO – Wi-Fi Setup** | [Watch video](https://github.com/Simch1/WeatherStation_PRO/blob/main/Video_Tutorial/WeatherStation_PRO_WiFi.mp4) | [Download MP4](https://github.com/Simch1/WeatherStation_PRO/raw/refs/heads/main/Video_Tutorial/WeatherStation_PRO_WiFi.mp4) |

Both video tutorials are included in the repository ZIP download.

## PDF guides and documentation

| Guide | View PDF | Download PDF |
|---|---|---|
| Installation guide | [View](./PDF_Guides/Installation_guide_EN.pdf) | [Download](https://github.com/Simch1/WeatherStation_PRO/raw/refs/heads/main/PDF_Guides/Installation_guide_EN.pdf) |
| Quick assembly guide | [View](./PDF_Guides/Quick%20Assembly%20Guide.pdf) | [Download](https://github.com/Simch1/WeatherStation_PRO/raw/refs/heads/main/PDF_Guides/Quick%20Assembly%20Guide.pdf) |
| Quick Wi-Fi guide | [View](./PDF_Guides/Quick%20WiFi%20Guide.pdf) | [Download](https://github.com/Simch1/WeatherStation_PRO/raw/refs/heads/main/PDF_Guides/Quick%20WiFi%20Guide.pdf) |

## MakerWorld

Firmware repository for the WeatherStation PRO 3D-printable project. The MakerWorld link will be added after publication.

## Status

**Firmware uploaded:** Aurora Build 0009.2 (34 source and documentation files). Compilation and hardware compatibility have not yet been independently verified.
