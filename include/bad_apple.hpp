/*
  Bad apple
  modified and made into a class from this guys work:https://github.com/MrG4meLover/vex-v5-bad-apple/tree/main
*/

#include "main.h"
#include <string>
enum speed_multipliers{
    ONE = 100,
    TWO = 50,
    FIVE = 20,
    TEN = 10
};
class bad_apple{
const double FRAME_LENGTH = 4000.0 / 30.0; 
const int IMG_SCALE = 5;            
const int FRAME_WIDTH = 480 / 8;     
const int FRAME_X = 90;    
const int FRAME_Y = 0;
speed_multipliers speed;
int frameNum = 0;
bool is_pause = false;
bool ended;
uint8_t decodeChar(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'A' && c <= 'Z') return (c - 'A')+10;
    if (c >= 'a' && c <= 'z') return (c - 'a')+ 36;
    if (c == '$') return 62;
    if (c == '#') return 63;
    return 0;
}
void end(){
    ended = true;
}
void paintFrame(const std::string& frame) { ///BTW FRAMES ARE SO HUGE IF WE LEFT IT AS A COPY WE WOULD LITTERALLY RUN OUT OF MEMORY
    int pixelx = 0;
    int pixely = 0;
    while(is_pause){
        pros::delay(500);
    }
    for (char c : frame) {
        uint8_t val = decodeChar(c);
        for (int bit = 5; bit >=0; bit--) { //take 6 bits not 5. if you take 5 it will do sone weird alignment artifact
            bool white = (val >> bit) & 1; //optimisation @.@
            if (white) {
                pros::screen::set_pen(pros::Color::white); //technically i could make this any color i want? but for now its the classics
            } else {
                pros::screen::set_pen(pros::Color::black);
            }
            pros::screen::fill_rect( //use draw_rect for grid style and fill for block style
                pixelx * IMG_SCALE + FRAME_X,
                pixely * IMG_SCALE + FRAME_Y,
                pixelx * IMG_SCALE + FRAME_X + IMG_SCALE,
                pixely * IMG_SCALE + FRAME_Y + IMG_SCALE
            ); //actually in retrospect you can techniclly make this any style for cool filters. but i personally like the grid style better
            pixelx++;
            if (pixelx >= FRAME_WIDTH) {
                pixelx = 0;
                pixely++;
            }
        }
    }
    frameNum++;
   uint32_t next_frame = frameNum * (uint32_t)FRAME_LENGTH;
   pros::delay(speed); //speed!!! kachow >.<
}
public:
bool is_done(){
    return ended;
}
void set_speed(speed_multipliers new_speed){
    this -> speed = new_speed;
}
void pause(){
    this -> is_pause = true;
}
void unpause(){
    this -> is_pause = false;
}
static void play(void* param);
};