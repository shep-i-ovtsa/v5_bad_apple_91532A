#include "bad_apple.hpp"
#include <array>
#include <string>
#include "pros/screen.hpp"
#include <fstream>
void bad_apple::link(const std::string& file_path){
    std::ifstream file(file_path); 
    if (!file.is_open()) {
        pros::screen::set_pen(pros::Color::red);
        pros::screen::fill_circle(100, 100, 50);
        return;
    }
    std::string line;

    std::getline(file, line); 
    parseHeader(line);

    std::getline(file, line); 

    while (std::getline(file, line)) {
        uint32_t frame_start = pros::millis();
        paintFrame(line);
        uint32_t elapsed = pros::millis() - frame_start;
        int budget = frame_time / speed;
        if ((int)elapsed < budget)
            pros::delay(budget - elapsed);
    }
    file.close();
}

void bad_apple::play(void *param){
    bad_apple* self = static_cast<bad_apple*>(param);
    self -> link(self -> path);
    self -> end();
}
