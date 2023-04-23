/*
 * -----10--------20--------30--------40--------50--------60--------70-------80
 *
 * P1X DEMO TOOL V2 - MUSIC DATA
 * by w84death^P1X
 *
 * https://github.com/w84death/p1x-demo-tool
 * (c) 2023.04
 *
 * -----10--------20--------30--------40--------50--------60--------70-------80
 */

#pragma once
#include <vector>

struct Note {
    int pitch;      // MIDI note number
    float duration; // duration in seconds
};
static std::vector<std::vector<Note>> tracks = {
    {
        {60, 1.0f},
        {62, 1.0f}
    },
    {
        {67, 0.5f},
        {69, 0.5f},
        {71, 0.5f},
        {72, 0.5f},
    },
    {
        {},
    },
    {
        {}
    },
};
