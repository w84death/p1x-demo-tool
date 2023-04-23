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
    int instrument; // instrument type (0 - kick, 1 - hi-hat, 2 - arpeggiator synth, 3 - electric piano)
    float release;  // key release time in seconds
};


std::vector<Note> generate_arpeggiator_sequence(int root_note, float duration, int instrument) {
    std::vector<Note> arpeggiator_sequence;
    std::vector<int> intervals = {0, 4, 7}; // Major chord intervals

    for (int i = 0; i < intervals.size(); ++i) {
        arpeggiator_sequence.push_back({root_note + intervals[i], duration, instrument});
    }

    return arpeggiator_sequence;
}

static std::vector<std::vector<Note>> tracks = {
    // Kick drum track
    {
        {36, 0.5f, 0, 0.0f},
        {0, 0.5f, 0, 0.0f},
        {36, 0.5f, 0, 0.0f},
        {0, 0.5f, 0, 0.0f},
    },
    // Hi-hat track
    {
        {42, 0.25f, 1, 0.1f},
        {0, 0.25f, 1, 0.0f},
        {42, 0.25f, 1, 0.1f},
        {0, 0.25f, 1, 0.0f},
    },
    // Arpeggiator synth track
    generate_arpeggiator_sequence(60, 0.5f, 2), // C Major chord arpeggio
    // Electric piano track
    {
        {72, 1.0f, 3, 0.9f},
        {76, 1.0f, 3, 0.9f},
        {79, 1.0f, 3, 0.9f},
        {72, 1.0f, 3, 0.9f},
    }
};
