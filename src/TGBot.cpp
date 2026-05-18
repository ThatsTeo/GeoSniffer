#include "GeoSniffer.hpp"

namespace GSL = GeoSnifferLib;

// FUNCTION TO RUN BOT 

namespace GeoSnifferLib::TGBot {
	std::string getBotToken() {
		std::ifstream config("../config.txt");
		std::string line;
	
		while(std::getline(config, line)) {
			if(line.find("TELEGRAM_TOKEN=") != std::string::npos) {
				return line.substr(line.find('=') + 1);
			}
		}
		std::cerr << "[TOKEN ERROR] Telegram Bot token not found.. Returning empty string" << std::endl;
		return "";
	}

	std::string locateMsg() {
		const auto command = "sudo iwlist wlan0 scanning";
		const std::string outputWifi = Wifi::exec(command);

		if (outputWifi.empty()) {
			std::cerr << "[ERROR] No output from iwlist..." << std::endl;
		}

		Wifi::parseWifi(outputWifi);

		const std::string positionStr = PostGC::postGCG(Wifi::networks);
		// std::cout << "[DEBUG] Full JSON: " << positionStr << std::endl;

		auto [lat, lng, accuracy] = GSL::PostGC::parseCoords(positionStr);

		if (accuracy <= 100) {
			return "Position: " + std::to_string(lat) + ", " + std::to_string(lng)
			 + "\nAccuracy: " + std::to_string(static_cast<int>(accuracy)) + " meters"
			 + "\nhttps://www.google.com/maps/place/" + std::to_string(lat) + "," + std::to_string(lng);
		 } else {
			 return "Accuracy is more than 100 meters, it's not ideal to update the location now...";
		 }
	}

	bool RunBot() {
		std::string botToken = getBotToken();
		TgBot::Bot bot(botToken);

		// /locate command
		bot.getEvents().onCommand("locate", [&bot](const TgBot::Message::Ptr& message) {
			bot.getApi().sendMessage(message->chat->id, "Processing information (this operation usually takes a few seconds)...");
			bot.getApi().sendMessage(message->chat->id, locateMsg());
		});

		// /beep command
		bot.getEvents().onCommand("beep", [&bot](const TgBot::Message::Ptr& message) {
			std::string beepMsg = "Beeping the buzzer 5 times...";
			bot.getApi().sendMessage(message->chat->id, beepMsg);
			Gpiod::beepBuzzer();
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
