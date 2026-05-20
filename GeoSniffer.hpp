// GeoSniffer.hpp
#pragma once

// Main libs
#include <iostream>
#include <utility>
#include <thread>
#include <cstring>
#include <atomic>

// Libs for ParseWifi
#include <cstdio>
#include <string>
#include <array>
#include <regex>
#include <sstream>

// Libs for curl POST to Google Cloud Geolocation API
#include <curl/curl.h>
#include <nlohmann/json.hpp>

// Libs for Telegram Bot
#include <tgbot/tgbot.h>

// Libs for GPIO PIN management
#include <gpiod.hpp>
#include <chrono>

namespace GeoSnifferLib {

    namespace Wifi {
		// Used variables
		inline std::map<std::string, int> networks;
		inline std::string currentMac;

		// Execute iwlist (shell command) and save the output
        std::string exec(const char* cmd);

        // Parse the output from exec()
        void parseWifi(const std::string& input);

        // Return scanned networks
        std::map<std::string, int> getNetworks();
    }

    namespace PostGC {
		// Get Google API key from file
		std::string getGoogleAPI();

		// Get the output from HTTP curl response
		size_t writeCallback(char* ptr, size_t size, size_t nmemb, std::string* data);

		// Call Google Cloud Geolocation API to get an approximate location
        std::string postGCG(const std::map<std::string, int>& networks);

        // Parse the JSON output from postGCG()
        std::tuple<double, double, double> parseCoords(const std::string& StrJSON);
    }

    namespace TGBot {
		// Get Bot token
		std::string getBotToken();

		// Thread for autoLocation
		inline std::thread autoLocationd;

		// Used to stop the autoLocationd thread
		inline std::atomic<bool> stopThread(false);

    	// Get the message with location + Google Maps Link
    	std::string locateMsg();

		// Start and run the Telegram Bot
		bool RunBot();
	}

    namespace Gpiod {
		// Number of the GPIO PIN for the active buzzer
		inline unsigned int numPinBuzzer = 17;

    	// Beep the active buzzer 5 times
    	void beepBuzzer();
	}
}
