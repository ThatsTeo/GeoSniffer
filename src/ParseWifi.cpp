#include "GeoSniffer.hpp"

// COLLECT MAC ADDRESS FROM ALL NEAR AND DISCOVERABLE ACCESS POINT

namespace GeoSnifferLib::Wifi {
	std::string exec(const char* cmd) {
		std::array<char, 256> buffer{};
		std::string result;

		std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);

		if (!pipe) return "[PIPE ERROR] Error opening pipe";

		while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
			result += buffer.data();
		}
		return result;
	}

	void parseWifi(const std::string& input) {
		currentMac = "";

		// ESSID:"Name"
		std::regex essid_regex("ESSID:\"(.*)\"");
		// MAC address
		std::regex mac_regex("Address: ([0-9A-Fa-f:]{17})");
		// Signal level=-XX dBm
		std::regex signal_regex("Signal level=(-[0-9]+) dBm");

		std::istringstream stream(input);
		std::string line;

		while(std::getline(stream, line)) {
			std::smatch match;

			// if (std::regex_search(line, match, essid_regex)) {
				//std::cout << "Networks name: " << match[1] << std::endl;
			// }

			if (std::regex_search(line, match, mac_regex)) {
				//std::cout << "MAC Address: " << match[1] << " dBm" << std::endl;
				currentMac = match[1];
			}

			if (std::regex_search(line, match, signal_regex)) {
				//std::cout << "Signal strenght: " << match[1] << " dBm" << std::endl;
				networks[currentMac] = std::stoi(match[1]);
				currentMac = "";
			}
		}

		for(const auto& [mac, rssi] : networks) {
			std::cout <<"[DEBUG] MAC Address: " << mac << " | RSSI: " << rssi << " dBm" << std::endl;
		}
	}

	std::map<std::string, int> getNetworks() {
		return networks;
	}
}
