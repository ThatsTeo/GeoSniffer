#include "GeoSniffer.hpp"

// BEEPING THE BUZZER 5 TIMES

namespace GeoSnifferLib::Gpiod {
    unsigned int numPinBuzzer = 17;

    void beepBuzzer() {
        try {
            gpiod::chip chip("/dev/gpiochip4");

            auto request = chip.prepare_request()
            .set_consumer("beep active buzzer")
            .add_line_settings(numPinBuzzer,
                gpiod::line_settings().set_direction(gpiod::line::direction::OUTPUT))
            .do_request();

            unsigned int i = 0;
            while (i < 5) {
                request.set_value(numPinBuzzer, gpiod::line::value::ACTIVE);
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
                request.set_value(numPinBuzzer, gpiod::line::value::INACTIVE);
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
                ++i;
            }
        } catch (const std::exception& e) {
            std::cerr << "[ERROR] Error during beepBuzzer, exception: " << e.what() << std::endl;
        }
    }
}
