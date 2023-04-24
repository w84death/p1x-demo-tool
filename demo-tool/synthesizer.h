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
#include <iostream>
#include <map>
#include <alsa/asoundlib.h>
#include <iostream>
#include <map>
#include "music.h"

/*
 * -----10--------20--------30--------40--------50--------60--------70-------80
 */

std::atomic<bool> pause_playback{false};
std::atomic<bool> quit_playback{false};
std::vector<std::thread> player_threads;
int sample_rate = 44100;
int playing_track;
unsigned int delay_samples = static_cast<unsigned int>(delay_time * sample_rate);
std::vector<float> delay_buffer(delay_samples, 0.0f);
unsigned int write_ptr = 0;
unsigned int read_ptr = 0;
struct ADSR {
    float attack;
    float decay;
    float sustain;
    float release;
    float sustain_level;
};

/*
 * -----10--------20--------30--------40--------50--------60--------70-------80
 */

float sine_wave(float frequency, float time) {
    const float PI = 3.14159265;
    return sin(2 * PI * frequency * time);
}

float saw_wave(float frequency, float time) {
    float x = fmod(frequency * time, 1.0f);
    return (2.0f * x) - 1.0f;
}

float square_wave(float frequency, float time) {
    return (sinf(2 * M_PI * frequency * time) >= 0.0f) ? 1.0f : -1.0f;
}

float midi_note_to_frequency(int note) {
    return 440.0f * pow(2.0f, (note - 69) / 12.0f);
}

float white_noise() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<float> dist(-1.0, 1.0);
    return dist(gen);
}

float lfo(float time, float frequency, float amplitude) {
    return amplitude * std::sin(2 * M_PI * frequency * time);
}

float adsr_envelope(const ADSR &adsr, float time, float key_release_time) {
    if (time < adsr.attack) {
        return time / adsr.attack;
    } else if (time < adsr.attack + adsr.decay) {
        return 1.0f - (time - adsr.attack) / adsr.decay * (1.0f - adsr.sustain_level);
    } else if (key_release_time < 0 || time < key_release_time) {
        return adsr.sustain_level;
    } else if (time < key_release_time + adsr.release) {
        return adsr.sustain_level * (1.0f - (time - key_release_time) / adsr.release);
    } else {
        return 0.0f;
    }
}
/*
 * -----10--------20--------30--------40--------50--------60--------70-------80
 */
float kick(float frequency, float time, float volume) {
    float base_freq = 70.0f; // Base frequency for the kick
    float envelope = std::exp(-4.0f * time); // Exponential decay envelope
    float sine_wave1 = std::sin(2.0f * M_PI * base_freq * time);
    float sine_wave2 = std::sin(2.0f * M_PI * frequency * time);

    return envelope * (sine_wave1 + sine_wave2) * volume;
}

float hi_hat(float frequency, float time, float volume) {
    float envelope = std::exp(-14.0f * time); // Exponential decay envelope
    return envelope * white_noise()*volume;
}

float synth(float frequency, float time, float key_release_time, float volume) {
    float sine = sine_wave(frequency, time);
    float saw = saw_wave(frequency, time) * 0.5f;
    float square = square_wave(frequency, time) * 0.5f;

    float lfo_signal = lfo(time, 0.5f, 0.5f);
    float modulated_frequency = frequency + lfo_signal * 5.0f;
    float sine_modulated = sine_wave(modulated_frequency, time) * 0.25f;

    ADSR envelope = {0.01f, 0.1f, 0.5f, 0.2f, 1.0f};
    float amplitude_envelope = adsr_envelope(envelope, time, key_release_time);

    return (sine + saw + square + sine_modulated) * amplitude_envelope * volume;
}

float electric_piano(float frequency, float time, float key_release_time, float volume) {
    ADSR envelope = {0.01f, 0.3f, 0.6f, 1.0f, 0.7f};
    float amplitude_envelope = adsr_envelope(envelope, time, key_release_time);

    float sine_wave1 = std::sin(2.0f * M_PI * frequency * time);
    float sine_wave2 = 0.5f * std::sin(2.0f * M_PI * 2 * frequency * time);
    float sine_wave3 = 0.25f * std::sin(2.0f * M_PI * 4 * frequency * time);

    return amplitude_envelope * (sine_wave1 + sine_wave2 + sine_wave3)*volume;
}


/*
 * -----10--------20--------30--------40--------50--------60--------70-------80
 */
void toggle_track_muted(int track_index, bool muted) {
    tracks[track_index].muted = muted;
}

void play_note(Note note, Track track, snd_pcm_t *handle, int sample_rate) {
    int buffer_size = sample_rate * note.duration;
    float buffer[buffer_size];

    float lfo_frequency = 10.0f; // LFO frequency in Hz
    float lfo_amplitude = 0.25f; // LFO amplitude (0.0 to 1.0)
    float volume = track.volume;
    if (track.muted) volume = 0.0f;

    for (int i = 0; i < buffer_size; ++i) {
        float time = static_cast<float>(i) / sample_rate;
        float lfo_signal = lfo(time, lfo_frequency, lfo_amplitude);
        float modulated_amplitude = (1.0f + lfo_signal);
        float dry_signal, wet_signal;

        switch (track.instrument) {
            case 0: // KICK
                buffer[i] = kick(midi_note_to_frequency(note.pitch), time, volume);
                break;
            case 1: // HI-HAT
                buffer[i] = hi_hat(midi_note_to_frequency(note.pitch), time, volume);
                break;
            case 2: // SYNTH
                buffer[i] = synth(midi_note_to_frequency(note.pitch), time, note.release, volume);
                break;
            case 3: // ELECTRIC PIANO WITH ECHO
                dry_signal = modulated_amplitude * electric_piano(midi_note_to_frequency(note.pitch), time, note.release, volume);
                wet_signal = delay_buffer[read_ptr];
                delay_buffer[write_ptr] = dry_signal + wet_signal * feedback;
                buffer[i] = dry_signal + wet_signal;
                write_ptr = (write_ptr + 1) % delay_samples;
                read_ptr = (read_ptr + 1) % delay_samples;
                break;
            default:
                buffer[i] = sine_wave(midi_note_to_frequency(note.pitch), time);
        }
    }

    snd_pcm_writei(handle, buffer, buffer_size);
}

/*
 * -----10--------20--------30--------40--------50--------60--------70-------80
 */
void playback_thread(Track &track, std::vector<Pattern> &patterns, int sample_rate) {
    snd_pcm_t *handle;
    snd_pcm_open(&handle, "default", SND_PCM_STREAM_PLAYBACK, 0);
    snd_pcm_set_params(handle, SND_PCM_FORMAT_FLOAT, SND_PCM_ACCESS_RW_INTERLEAVED, 1, sample_rate, 1, 100000);
    bool is_paused = false;

    while (!quit_playback) {
        if (!pause_playback) {
            if (is_paused) {
                snd_pcm_pause(handle, 0);
                is_paused = false;
            }
            for (int pattern_id : track.pattern_ids) {
                Pattern &pattern = patterns[pattern_id];
                for (Note &note : pattern.notes) {
                    play_note(note, track, handle, sample_rate);
                }
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
        player_threads.emplace_back(playback_thread, std::ref(tracks[i]), std::ref(patterns), sample_rate);
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
