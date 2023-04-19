#include <iostream>
#include <thread>
#include <chrono>
#include "tsf.h"
#include <alsa/asoundlib.h>

// Define the name of the SoundFont file to load
#define SOUNDFONT_FILE "soundfont.sf2"

int main() {
    // Load the SoundFont file
    TSF* tsf = tsf_load_filename(SOUNDFONT_FILE);
    if (!tsf) {
        std::cerr << "Failed to load SoundFont file" << std::endl;
        return 1;
    }

    // Open the ALSA PCM device for playback
    snd_pcm_t* pcm_handle;
    if (snd_pcm_open(&pcm_handle, "default", SND_PCM_STREAM_PLAYBACK, 0) < 0) {
        std::cerr << "Failed to open ALSA PCM device" << std::endl;
        return 1;
    }

    // Set the ALSA PCM parameters
    snd_pcm_hw_params_t* pcm_params;
    snd_pcm_hw_params_malloc(&pcm_params);
    snd_pcm_hw_params_any(pcm_handle, pcm_params);
    snd_pcm_hw_params_set_access(pcm_handle, pcm_params, SND_PCM_ACCESS_RW_INTERLEAVED);
    snd_pcm_hw_params_set_format(pcm_handle, pcm_params, SND_PCM_FORMAT_S16_LE);
    snd_pcm_hw_params_set_channels(pcm_handle, pcm_params, 2);
    unsigned int rate = tsf_get_output_rate(tsf);
    snd_pcm_hw_params_set_rate_near(pcm_handle, pcm_params, &rate, nullptr);
    snd_pcm_hw_params(pcm_handle, pcm_params);
    snd_pcm_hw_params_free(pcm_params);

    // Start playing the music
    tsf_reset(tsf);
    while (true) {
        // Generate the audio buffer for the next block of music
        const int block_size = 2048;
        short audio_buffer[block_size * 2];
        tsf_render_short(tsf, audio_buffer, block_size, 0);

        // Write the audio buffer to the ALSA PCM device
        snd_pcm_sframes_t frames_written = snd_pcm_writei(pcm_handle, audio_buffer, block_size);
        if (frames_written < 0) {
            std::cerr << "Failed to write audio to ALSA PCM device" << std::endl;
            return 1;
        }
        if (frames_written < block_size) {
            std::cerr << "Audio buffer underrun detected" << std::endl;
        }

        // Sleep for a short time to allow the audio buffer to be filled
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    // Clean up and exit
    tsf_close(tsf);
    snd_pcm_close(pcm_handle);
    return 0;
}
