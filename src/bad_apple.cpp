#include "bad_apple.hpp"
#include <array>
#include <string>
#include "pros/screen.hpp"
void bad_apple::link(const std::string& file_path){
    std::ifstream file(file_path); 
    if (!file.is_open()) {
        pros::screen::set_pen(pros::Color::red);
        pros::screen::fill_circle(100, 100, 50);
    }
    std::string line;
    int counter = 0;
    while (std::getline(file, line)) {
        paintFrame(line);
    }
    file.close();
}
void bad_apple::play(void *param){
    bad_apple* self = static_cast<bad_apple*>(param);
    self -> link(self -> path);
self -> end();
}
