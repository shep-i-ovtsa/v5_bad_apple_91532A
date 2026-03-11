#include "main.h"
#include "bad_apple.hpp" //btw tiny jpeg library is here because i was trying to use jpeg files as frames before finding this dude. os dw about it
#include "pros/colors.hpp"
bad_apple apple;
void initalise(){
 
}
void opcontrol(){   
    pros::Task bad(bad_apple::play, &apple, TASK_PRIORITY_DEFAULT, 32384, "BadApple");
    //VERY IMPORTANT!!! YOU MUST CALL IT LIKE THIS TO ALLOCATE 16kbs to stack or more. if not you will RUN OUT OF MEMORY! (im too tired to push it so just copy paste this ok)
    apple.set_path("/usd/apple2.txt");
    pros::Controller master(pros::E_CONTROLLER_MASTER);    
    pros::screen::set_pen(pros::Color::white);
    pros::screen::fill_rect(0,0,480,300);
    while(true){

        if(master.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_B)){
            apple.pause(); //some media controls
        } else if (master.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_A)){
            apple.unpause();
        }
        pros::delay(20);
    }
}
