# GeoSniffBot 🛜

![License](https://img.shields.io/badge/license-GPL--3.0-blue)
![Language](https://img.shields.io/badge/language-C%2B%2B17-orange)
![Platform](https://img.shields.io/badge/platform-Raspberry%20Pi%205-red)

> A fast and smart way to locate a Raspberry Pi 5 without GPS

## 📑 Overview

**GeoSniffBot** was made as a school project, all developed by me in C++. The original idea was to "recreate" a sort of smart tracker with only DIY technology.
 
- To start, the main code relies on the `sudo iwlist wlan0 scanning` Linux command, which is used to obtain the MAC Address (physical network card address) from all near Access Points (aka Routers).
- After collecting the address, a function performs a curl POST using Google Cloud Geolocation API to triangulate the position of the Raspberry (works best with at least 2 nearby routers!).
- The output is then displayed through a Telegram Bot created to send the location directly to the users phone, with also a manual request via a `/locate` command.

### Demo on Telegram
| Command                     | Result           |
|-----------------------------|------------------|
| `/locate` (good accuracy)   | Location found   |
| `/locate` (accuracy > 100m) | Too imprecise    |
| `/beep`                     | Buzzer activated |

<img src="assets/exampleImg.png" width="300"/>
---

## How it works ⚙️

```
┌────────────────────────────────────┐
│     sudo iwlist wlan0 scanning     │  ← Obtain MACs
│        Parse Wi-Fi output          │
└─────────────────┬──────────────────┘
                  │ 
┌─────────────────▼───────────────────┐
│    Curl POST to Google Cloud API    │  ← Geolocation
│    Parse lat/lng/accuracy output    │
└─────────────────┬───────────────────┘
                  │ 
┌─────────────────▼───────────────────┐
│        Telegram BOT message         │  ← Output
└─────────────────────────────────────┘
```

## Used libs 📚

### Standard used: C++17

| Use               | Library                      |
|-------------------|------------------------------|
| Multithreading    | thread                       |
| Activating buzzer | libgpiod                     |
| Curl POST         | libcurl                      |
| JSON              | nlohmann/json                |
| Telegram Bot      | tgbot                        |
| Geolocation       | Google Cloud Geolocation API |

## Project Structure 🗂️

```
GeoSniff/                   # Root folder
├── src/
│   ├── ParseWifi.cpp           # Parse output & obtain MACs
│   ├── PostGC.cpp              # Curl POST & geolocalization
│   ├── GPIOdBuzzer.cpp         # Buzzer alarm with GPIO PINs
│   └── TGBot.cpp               # All Telegram Bot functions
├── config.txt     <-----   # Where you will place your API 
├── CMakeLists.txt          # Build program
├── GeoSniffer.hpp          # Custom header
├── main.cpp                # Main code
├── .gitignore
├── LICENSE                 # GNU GPL3 License
└── README.md               # This file
```

---

## How to compile and build 🏁

**First** you will ***need*** your Telegram Bot token and Google Cloud Geolocation API, then create a file named `config.txt`
in the root folder as showed in the structure above.

Once you have the file, copy this **template**:

```
GOOGLE_API_KEY=abcd
TELEGRAM_TOKEN=1234
```
then save the file and proced to the compile step.

To **compile** the executable, open terminal and got in the **root folder** (`GeoSniff/`) and run: 
- `mkdir build/` -> Used to create the build directory
- `cd build/` -> Enter the newly created folder
- `cmake ..` -> Generates the build files from CMakeLists.txt
- `make -j4` -> use to compile all files

### And you are done! Now to **execute** just run `./geo_sniff` and the program should start
