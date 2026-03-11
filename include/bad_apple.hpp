/*
  Bad apple
  modified and made into a class from this guys work:https://github.com/MrG4meLover/vex-v5-bad-apple/tree/main
  oh and its a video player now ofc
*/

/*
! video file structure
[<width>$<height>$<encoding>$<scale>$<fps>$0$]  header
[<author>$<title>$<frame_count>]    metadata
0#0#0#1#...         //actual frame
*/

#pragma once
#include "main.h"
#include "pros/misc.hpp"
#include <array>
#include <string>
#include <vector>
#include <fstream>

class bad_apple {

enum encoding_type : uint8_t {
    BINARY_64     = 1,
    HALF_COLOR_64 = 2,
    EXPANDED_64   = 3
};

std::array<pros::Color, 64> pallete = {
    pros::Color::black,// 0
    pros::Color::white,// 1

    // 2-15: core colors
    pros::Color::red,// 2
    pros::Color::lime,// 3
    pros::Color::blue,// 4
    pros::Color::yellow,// 5
    pros::Color::cyan,// 6
    pros::Color::magenta,// 7
    pros::Color::orange,// 8
    pros::Color::purple,// 9
    pros::Color::pink,// 10
    pros::Color::maroon,// 11
    pros::Color::green, // 12
    pros::Color::navy,// 13
    pros::Color::teal,// 14
    pros::Color::olive, // 15

    // 16-31
    pros::Color::coral, // 16
    pros::Color::salmon,// 17
    pros::Color::gold,// 18
    pros::Color::yellow_green,// 19
    pros::Color::medium_aquamarine,// 20
    pros::Color::sky_blue,// 21
    pros::Color::steel_blue,// 22
    pros::Color::violet,// 23
    pros::Color::indigo,// 24
    pros::Color::orchid,// 25
    pros::Color::hot_pink,// 26
    pros::Color::crimson,// 27
    pros::Color::tomato,// 28
    pros::Color::sienna,// 29
    pros::Color::tan,// 30
    pros::Color::silver,// 31

    // 32-63
    pros::Color::dark_red,// 32
    pros::Color::dark_orange,// 33
    pros::Color::dark_green,// 34
    pros::Color::dark_blue,// 35
    pros::Color::dark_cyan,// 36
    pros::Color::dark_magenta,// 37
    pros::Color::dark_violet,// 38
    pros::Color::dark_orchid,// 39
    pros::Color::dark_salmon,// 40
    pros::Color::dark_khaki,// 41
    pros::Color::dark_gray,// 42
    pros::Color::medium_slate_blue,// 43
    pros::Color::dark_slate_gray,// 44
    pros::Color::dark_turquoise,// 45
    pros::Color::dark_goldenrod,// 46
    pros::Color::dark_sea_green,// 47
    pros::Color::medium_blue,// 48
    pros::Color::medium_purple,// 49
    pros::Color::medium_orchid,// 50
    pros::Color::medium_violet_red,// 51
    pros::Color::medium_sea_green,// 52
    pros::Color::medium_slate_blue,// 53
    pros::Color::medium_spring_green, // 54
    pros::Color::medium_turquoise,// 55
    pros::Color::light_blue,// 56
    pros::Color::light_coral,// 57
    pros::Color::light_cyan,// 58
    pros::Color::light_green,// 59
    pros::Color::light_gray,// 60
    pros::Color::light_pink,// 61
    pros::Color::light_salmon,// 62
    pros::Color::light_yellow,// 63
};

int  IMG_SCALE   = 2;
int  FRAME_WIDTH = 0;
int  FRAME_HEIGHT= 0;
int  FRAME_X     = 0;
int  FRAME_Y     = 0;
int  speed       = 1;
int  fps         = 30;
int  frame_time  = 33;
encoding_type encoding = BINARY_64;
int  frameNum    = 0;
bool is_pause    = false;
bool ended       = false;
std::string path;

std::vector<uint8_t> front_buf;
std::vector<uint8_t> back_buf;

void initBuffers() {
    int total = FRAME_WIDTH * FRAME_HEIGHT;
    front_buf.assign(total, 255);
    back_buf.assign(total, 0);
}

inline void writePixel(int idx, uint8_t color_idx) {
    if (idx < (int)back_buf.size())
        back_buf[idx] = color_idx;
}

void flushDirtyBinary() {
    for (int y = 0; y < FRAME_HEIGHT; y++) {
        int row_start = y * FRAME_WIDTH;
        for (int x = 0; x < FRAME_WIDTH; x++) {
            int idx = row_start + x;
            if (back_buf[idx] != front_buf[idx]) {
                pros::screen::set_pen(pallete[back_buf[idx]]);
                pros::screen::fill_rect(
                    x * IMG_SCALE + FRAME_X,
                    y * IMG_SCALE + FRAME_Y,
                    (x + 1) * IMG_SCALE + FRAME_X,
                    (y + 1) * IMG_SCALE + FRAME_Y
                );
                front_buf[idx] = back_buf[idx];
            }
        }
    }
}

void flushDirtyRLE() {
    for (int y = 0; y < FRAME_HEIGHT; y++) {
        int row_start = y * FRAME_WIDTH;
        uint8_t run_color = back_buf[row_start];
        int run_start = 0;
        front_buf[row_start] = back_buf[row_start];

        for (int x = 1; x < FRAME_WIDTH; x++) {
            int idx = row_start + x;
            if (back_buf[idx] != run_color) {
                pros::screen::set_pen(pallete[run_color]);
                pros::screen::fill_rect(
                    run_start * IMG_SCALE + FRAME_X,
                    y         * IMG_SCALE + FRAME_Y,
                    x         * IMG_SCALE + FRAME_X,
                    (y + 1)   * IMG_SCALE + FRAME_Y
                );
                run_color = back_buf[idx];
                run_start = x;
            }
            front_buf[idx] = back_buf[idx];
        }
        pros::screen::set_pen(pallete[run_color]);
        pros::screen::fill_rect(
            run_start    * IMG_SCALE + FRAME_X,
            y            * IMG_SCALE + FRAME_Y,
            FRAME_WIDTH  * IMG_SCALE + FRAME_X,
            (y + 1)      * IMG_SCALE + FRAME_Y
        );
    }
}

void flushDirty() {
    if (encoding == BINARY_64)
        flushDirtyBinary();
    else
        flushDirtyRLE();
}

uint8_t decodeChar(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'A' && c <= 'Z') return (c - 'A') + 10;
    if (c >= 'a' && c <= 'z') return (c - 'a') + 36;
    if (c == '$') return 62;
    if (c == '#') return 63;
    return 0;
}

void parseHeader(const std::string& line) {
    std::string content = line.substr(1, line.size() - 2);
    int field = 0;
    std::string token;
    for (char c : content) {
        if (c == '$') {
            if (field == 0) FRAME_WIDTH  = std::stoi(token);
            if (field == 1) FRAME_HEIGHT = std::stoi(token);
            if (field == 2) encoding     = (encoding_type)std::stoi(token);
            if (field == 3) IMG_SCALE    = std::stoi(token);
            if (field == 4) { fps = std::stoi(token); frame_time = (int)(1000.0 / fps); }
            field++;
            token.clear();
        } else {
            token += c;
        }
    }
    FRAME_WIDTH  /= IMG_SCALE;
    FRAME_HEIGHT /= IMG_SCALE;
    initBuffers();
}

void end() {
    ended   = true;
    frameNum = 0;
}

public:

void set_offset(int x, int y) { FRAME_X = x; FRAME_Y = y; }
void set_scale(int a)         { IMG_SCALE = a; }
void set_path(const std::string& new_path) { path = new_path; }
void set_speed(int new_speed)              { speed = new_speed; }
void set_encoding(encoding_type enc)       { encoding = enc; }
void set_pallete(std::array<pros::Color, 64>& pail) { pallete = pail; }
bool is_done() { return ended; }
void pause()   { is_pause = true; }
void unpause() { is_pause = false; }

void link(const std::string& path);

void paintFrame(const std::string& frame) {
    int idx = 0;

    while (is_pause) { pros::delay(500); }

    std::fill(back_buf.begin(), back_buf.end(), 0);

    if (encoding == BINARY_64) {
        for (char c : frame) {
            uint8_t val = decodeChar(c);
            for (int bit = 5; bit >= 0; bit--)
                writePixel(idx++, (val >> bit) & 1 ? 1 : 0);
        }
    }
    else if (encoding == HALF_COLOR_64) {
        for (char c : frame) {
            uint8_t val       = decodeChar(c);
            uint8_t color_idx = (val >> 2) & 0xF;
            uint8_t run       = (val & 0x3) + 1;
            for (int r = 0; r < run; r++)
                writePixel(idx++, color_idx);
        }
    }
    else if (encoding == EXPANDED_64) {
        for (int i = 0; i + 1 < (int)frame.size(); i += 2) {
            uint8_t color_idx = decodeChar(frame[i]);
            uint8_t run       = decodeChar(frame[i + 1]) + 1;
            if (color_idx > 63) color_idx = 0;
            for (int r = 0; r < run; r++)
                writePixel(idx++, color_idx);
        }
    }

    flushDirty();
    frameNum++;
}

static void play(void* param);

};