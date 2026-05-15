#include "GeoSniffer.hpp"

namespace GSL = GeoSnifferLib;

// FUNCTION TO RUN BOT 

namespace GeoSnifferLib::TGBot {
	std::string getBotToken() {
		std::ifstream config("../config.txt");
		std::string line;
	
		while(std::getline(config, line)) {
			if(line.find("TELEGRAM_TOKEN=") != std::string::npos) {
				return line.substr(line.find("=") + 1);
			}
		}
		std::cout << "[DEBUG] Telegram Bot token not found.. Returning empty string" << std::endl;
		return "";
	}
	
	bool RunBot() {
		std::string botToken = getBotToken();
		TgBot::Bot bot(botToken);

		bot.getEvents().onCommand("locate", [&bot](TgBot::Message::Ptr message) {
			const char* command = "sudo iwlist wlan0 scanning";
			std::string output = GSL::Wifi::exec(command);
    
			if (output.empty()) {
				std::cout << "Nessun output" << std::endl;
			}
			GSL::Wifi::parseWifi(output);
	
			// output callback api 
			std::string positionStr = GSL::PostGC::postGCG(GSL::Wifi::networks);
			//std::cout << "[DEBUG] Full JSON: " << positionStr << std::endl;

			auto [lat, lng, accuracy] = GSL::PostGC::parseCoords(positionStr);
			
			std::string posMsg = "Posizione: " + std::to_string(lat) + ", " + std::to_string(lng)
			 + "\nPrecisione: " + std::to_string((int) accuracy) + " metri"
			 + "\nhttps://www.google.com/maps/place/" + std::to_string(lat) + "," + std::to_string(lng);
			bot.getApi().sendMessage(message->chat->id, posMsg);
		});
    
		try {
			printf("Bot username: %s\n", bot.getApi().getMe()->username.c_str());
			TgBot::TgLongPoll longPoll(bot);
			while (true) {
				printf("Message LongPoll;\n");
				longPoll.start();
			}
		} catch (TgBot::TgException& e) {
			printf("error: %s\n", e.what());
		}
		return true;
	}
}
