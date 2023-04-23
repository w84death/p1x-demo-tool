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

std::vector<float> track_volumes = {1.0f, 0.1f, .2f, 0.3f};

float delay_time = 0.1f; // Delay time in seconds
float feedback = 0.5f; // Feedback amount (0.0 to 1.0)

std::vector<std::vector<Note>> tracks = {
    // Kick track
    {
        // Intro
        {0, 0.25f, 0, 0.0f},

        // Main body (repeat 4 times)
        {0, 0.25f, 0, 1.0f}, {0, 0.25f, 0, 0.0f},
        {0, 0.25f, 0, 1.0f}, {0, 0.25f, 0, 0.0f},
        {0, 0.25f, 0, 1.0f}, {0, 0.25f, 0, 0.0f},
        {0, 0.25f, 0, 1.0f}, {0, 0.25f, 0, 0.0f},

        // Outro
        {0, 1.0f, 0, 0.0f},
    },
    // Hi-hat track
    {
        // Intro
        {42, 0.25f, 1, 0.0f},

        // Main body (repeat 4 times)
        {42, 0.125f, 1, 0.8f}, {42, 0.125f, 1, 0.0f},
        {42, 0.125f, 1, 0.8f}, {42, 0.125f, 1, 0.0f},
        {42, 0.125f, 1, 0.8f}, {42, 0.125f, 1, 0.0f},
        {42, 0.125f, 1, 0.8f}, {42, 0.125f, 1, 0.0f},

        // Outro
        {42, 1.0f, 1, 0.0f},
    },
    // Arpeggiator synth track
    {
        // Intro
        {60, 2.0f, 2, 0.8f},

        // Main body (repeat 4 times)
        {60, 0.5f, 2, 0.8f}, {64, 0.5f, 2, 0.8f},
        {67, 0.5f, 2, 0.8f}, {72, 0.5f, 2, 0.8f},
        {67, 0.5f, 2, 0.8f}, {64, 0.5f, 2, 0.8f},
        {60, 0.5f, 2, 0.8f}, {64, 0.5f, 2, 0.8f},

        // Outro
        {72, 2.0f, 2, 0.8f},
    },
    // Electric piano track
    {
        // Intro
        {72, 2.0f, 3, 0.8f},

        // Main body (repeat 4 times)
        // Chord progression: I-IV-V-I (C major - F major - G major - C major)
        {72, 0.5f, 3, 0.8f}, {76, 0.5f, 3, 0.8f}, {79, 0.5f, 3, 0.8f},
        {77, 0.5f, 3, 0.8f}, {81, 0.5f, 3, 0.8f}, {84, 0.5f, 3, 0.8f},
        {79, 0.5f, 3, 0.8f}, {83, 0.5f, 3, 0.8f}, {86, 0.5f, 3, 0.8f},
        {72, 0.5f, 3, 0.8f}, {76, 0.5f, 3, 0.8f}, {79, 0.5f, 3, 0.8f},

        // Outro
        {79, 2.0f, 3, 0.8f},
    },
};
