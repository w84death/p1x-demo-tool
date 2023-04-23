/*
 * -----10--------20--------30--------40--------50--------60--------70-------80
 *
 * P1X DEMO TOOL V2 - SYNTHESIZER
 * by w84death^P1X
 *
 * https://github.com/w84death/p1x-demo-tool
 * (c) 2023.04
 *
 * -----10--------20--------30--------40--------50--------60--------70-------80
 */
#include <random>
#include <vector>
#include <atomic>
#include <cmath>
#include <thread>
#include <chrono>
#include <alsa/asoundlib.h>
#include "music.h"
std::atomic<bool> pause_playback{false};
std::atomic<bool> quit_playback{false};

int sample_rate = 44100;
std::vector<std::thread> player_threads;

/*
 * -----10--------20--------30--------40--------50--------60--------70-------80
 */

float sine_wave(float frequency, float time) {
    const float PI = 3.14159265;
    return sin(2 * PI * frequency * time);
}

float midi_note_to_frequency(int note) {
    return 440.0f * pow(2.0f, (note - 69) / 12.0f);
}

void play_note(Note note, snd_pcm_t *handle, int sample_rate) {
    int buffer_size = sample_rate * note.duration;
    float buffer[buffer_size];

    for (int i = 0; i < buffer_size; ++i) {
        buffer[i] = sine_wave(midi_note_to_frequency(note.pitch), static_cast<float>(i) / sample_rate);
    }

    snd_pcm_writei(handle, buffer, buffer_size);
}

void playback_thread(int track_id, std::vector<Note> &notes, int sample_rate) {
    snd_pcm_t *handle;
    snd_pcm_open(&handle, "default", SND_PCM_STREAM_PLAYBACK, 0);
    snd_pcm_set_params(handle, SND_PCM_FORMAT_FLOAT, SND_PCM_ACCESS_RW_INTERLEAVED, 1, sample_rate, 1, 100000);

    size_t current_note = 0;
    bool is_paused = false;

    while (!quit_playback) {
        if (!pause_playback) {
            if (is_paused) {
                snd_pcm_pause(handle, 0);
                is_paused = false;
            }

            if (current_note < notes.size()) {
                play_note(notes[current_note], handle, sample_rate);
                current_note++;
            } else {
                current_note = 0;
            }
        } else {
            if (!is_paused) {
                snd_pcm_pause(handle, 1);
                is_paused = true;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }

    snd_pcm_close(handle);
}

/*
 * -----10--------20--------30--------40--------50--------60--------70-------80
 */
void synthStart(){
    for (int i = 0; i < tracks.size(); ++i) {
        player_threads.emplace_back(playback_thread, i, std::ref(tracks[i]), sample_rate);
    }
}

/*
 * -----10--------20--------30--------40--------50--------60--------70-------80
 */
void synthQuit(){
    for (auto &t : player_threads) {
        t.join();
    }
}
