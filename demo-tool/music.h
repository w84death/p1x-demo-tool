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
