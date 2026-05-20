#include "GeoSniffer.hpp"
#include <atomic>

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

		bot.getEvents().onAnyMessage([](TgBot::Message::Ptr message) {
            std::cout << "[DEBUG] User message: " << message->text.c_str() << std::endl;
        });

		// /locate command
		bot.getEvents().onCommand("locate", [&bot](const TgBot::Message::Ptr& message) {
			bot.getApi().sendMessage(message->chat->id, "Processing information (this operation usually takes a few seconds)...");
			std::string msgToSend = locateMsg();
			std::cout << "[BOT MESSAGE] Message sent: " << msgToSend << std::endl;

			bot.getApi().sendMessage(message->chat->id, msgToSend);
		});

		// /beep command
		bot.getEvents().onCommand("beep", [&bot](const TgBot::Message::Ptr& message) {
			std::string beepMsg = "Beeping the buzzer 5 times...";
			std::cout << "[BOT MESSAGE] Message sent: " << beepMsg << std::endl;

			bot.getApi().sendMessage(message->chat->id, beepMsg);
			Gpiod::beepBuzzer();
		});

		// start auto locate
		bot.getEvents().onCommand("startAutoLocate", [&bot](const TgBot::Message::Ptr& message) {
			if(!stopThread) {
			    bot.getApi().sendMessage(message->chat->id, "You already have auto location active!");
				return;
			}
			bot.getApi().sendMessage(message->chat->id, "Starting automatic scanning thread...");

			// Start auto scan
			bot.getApi().sendMessage(message->chat->id, "Auto scan started. You will recive a new position every 2 minutes from now.\nDigit '/stopAutoLocate' to stop.");
			stopThread.store(false);
			std::thread autoLocationd([&bot, message]() {
			    while(!stopThread) {
					bot.getApi().sendMessage(message->chat->id, locateMsg());
					for(int i = 0; i < 120 && !stopThread; ++i) std::this_thread::sleep_for(std::chrono::seconds(1));
				}
			});
			autoLocationd.detach();
			std::cout << "[DEBUG] autoLocationd thread detached!" << std::endl;
		});

		// stop auto locate
		bot.getEvents().onCommand("stopAutoLocate", [&bot](const TgBot::Message::Ptr& message) {
			if(stopThread) {
			    bot.getApi().sendMessage(message->chat->id, "You must have auto locating on to use this command.");
				return;
			}
			bot.getApi().sendMessage(message->chat->id, "Stopping automatic scanning thread...");

			// Stop auto scan
			stopThread.store(true);
			bot.getApi().sendMessage(message->chat->id, "Automatic scanning stopped.");
			std::cout << "[DEBUG] autoLocationd stopped" << std::endl;
		});

		try {
		    std::cout << "Bot username: " << bot.getApi().getMe()->username.c_str() << std::endl;
			TgBot::TgLongPoll longPoll(bot);
			while (true) {
			    std::cout << "[DEBUG] Message longPoll" << std::endl;
				longPoll.start();
			}
		} catch (TgBot::TgException& e) {
			std::cout << "[ERROR]: " << e.what() << std::endl;
		}
		return true;
	}
}
