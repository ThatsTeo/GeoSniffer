#include "GeoSniffer.hpp"

// CALLBACK TO GOOGLE CLOUD GEOLOCATION API FOR APPROXIMATE COORDINARTES

using json = nlohmann::json;

namespace GeoSnifferLib::PostGC {
	std::string getGoogleAPI() {
		std::ifstream config("../config.txt");
		std::string line;
	
		while(std::getline(config, line)) {
			if(line.find("GOOGLE_API_KEY=") != std::string::npos) {
				return line.substr(line.find('=') + 1);
			}
		}
		std::cerr << "[API ERROR] Google Cloud API not found.. Returning empty string" << std::endl;
		return "";
	}
	
	size_t writeCallback(char* ptr, size_t size, size_t nmemb, std::string* data) {
		data->append(ptr, size * nmemb);
		return size * nmemb;
	}
	
	std::string postGCG(const std::map<std::string, int>& networks) {
		// Google Geolocation API
		std::string geoAPI = getGoogleAPI();
		
		json payload;
		payload["wifiAccessPoints"] = json::array();
    
		for (const auto& [mac, rssi] : networks) {
			payload["wifiAccessPoints"].push_back({
				{"macAddress", mac},
				{"signalStrength", rssi}
			});
		}
    
		// curl response
		std::string response;
		CURL* curl = curl_easy_init();
    
		if (curl) {
			struct curl_slist* headers = nullptr;
			headers = curl_slist_append(headers, "Content-Type: application/json");
        
			std::string body = payload.dump();
        
			std::string url = "https://www.googleapis.com/geolocation/v1/geolocate?key=" + geoAPI;
			curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
			curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
			curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body.c_str());
			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
			curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
        
			curl_easy_perform(curl);
			curl_easy_cleanup(curl);
			curl_slist_free_all(headers);
		}
    
		return response;
	}

	std::tuple<double, double, double> parseCoords(const std::string& StrJSON) {
		auto locationJson = json::parse(StrJSON);
    
		double latitude = 0;
		double longitude = 0;
		double accuracy = 0;
    
		if (locationJson.contains("location")) {
			latitude = locationJson["location"]["lat"];
			longitude = locationJson["location"]["lng"];
		}
		if (locationJson.contains("accuracy")) {
			accuracy = locationJson["accuracy"];
		}
	
		return {latitude, longitude, accuracy};
	}
}
