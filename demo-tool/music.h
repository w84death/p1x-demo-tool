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
    float release;  // key release time in seconds
};

struct Pattern {
    std::vector<Note> notes;
    Pattern(std::initializer_list<Note> note_list) : notes(note_list) {}
};

struct Track {
    std::vector<int> pattern_ids;
    int instrument;
    float volume;
    bool muted;
};

float delay_time = 0.1f; // Delay time in seconds
float feedback = 0.5f; // Feedback amount (0.0 to 1.0)

std::vector<Pattern> patterns = {
    {   // Pattern 0: Kick pattern
        {0, 0.25f, 1.0f}, {0, 0.25f, 0.0f}
    },
    {   // Pattern 1: Hi-hat pattern
        {42, 0.125f, 0.8f}, {42, 0.125f, 0.0f}
    },
    {   // 2: synth pattern
        {60, 0.5f, 0.8f}, {64, 0.5f, 0.8f}, {67, 0.5f, 0.8f}, {72, 0.5f, 0.8f},
        {67, 0.5f, 0.8f}, {64, 0.5f, 0.8f}, {60, 0.5f, 0.8f}, {64, 0.5f, 0.8f}
    },
    {   // 3:Chord progression: I-IV-V-I (C major - F major - G major - C major)
        {72, 0.5f, 0.8f}, {76, 0.5f, 0.8f}, {79, 0.5f, 0.8f}, {77, 0.5f, 0.8f},
        {81, 0.5f, 0.8f}, {84, 0.5f, 0.8f}, {79, 0.5f, 0.8f}, {83, 0.5f, 0.8f},
        {86, 0.5f, 0.8f}, {72, 0.5f, 0.8f}, {76, 0.5f, 0.8f}, {79, 0.5f, 0.8f}
    }
};

std::vector<Track> tracks = {
    {   // Track 0: Kick track
        {0, 0, 0, 0}, 0, 1.0f, false
    },
    {   // Track 1: Hi-hat track
        {1, 1, 1, 1}, 1, 0.1f, false
    },
    {   // Track 2: Synth
        {2}, 2, 0.2f, false
    },
    {   // Track 3: Electric Piano
        {3}, 3, 0.3f, false
    }
};
