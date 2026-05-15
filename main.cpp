#include "GeoSniffer.hpp"

namespace GSL = GeoSnifferLib;


int main(void) {
	
	std::cout << "[DEBUG] Starting test" << std::endl;
	
	// iwlist command to get MAC address
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
	std::cout << std::endl << "Latitude: " << lat << std::endl << "Longitude: " << lng << std::endl << "Accuracy: " << accuracy << std::endl;
	
	GSL::TGBot::RunBot();
	return 0;
}
