#include <cstdio>
#include <math.h>
#include <alsa/asoundlib.h>

// g++ -o sine_wave alsa.cpp -lasound -lm

int main()
{
    int err;
    snd_pcm_t *handle;
    snd_pcm_hw_params_t *params;
    unsigned int sample_rate = 44100;
    int buffer_frames = 1024;

    float frequency = 440.0;
    float phase = 0.0;
    float phase_increment = 2.0 * M_PI * frequency / sample_rate;
    float amplitude = 0.5;
    int dir;

    // Open PCM device for playback
    err = snd_pcm_open(&handle, "default", SND_PCM_STREAM_PLAYBACK, 0);
    if (err < 0)
    {
        printf("Unable to open PCM device: %s\n", snd_strerror(err));
        return -1;
    }

    // Allocate and initialize hardware parameters object
    snd_pcm_hw_params_alloca(&params);
    snd_pcm_hw_params_any(handle, params);
    snd_pcm_hw_params_set_access(handle, params, SND_PCM_ACCESS_RW_INTERLEAVED);
    snd_pcm_hw_params_set_format(handle, params, SND_PCM_FORMAT_FLOAT_LE);
    snd_pcm_hw_params_set_channels(handle, params, 1);
    snd_pcm_hw_params_set_rate_near(handle, params, &sample_rate, &dir);
    snd_pcm_hw_params_set_period_size(handle, params, buffer_frames, 0);

    // Apply hardware parameters to PCM device
    err = snd_pcm_hw_params(handle, params);
    if (err < 0)
    {
        printf("Unable to set hardware parameters: %s\n", snd_strerror(err));
        return -1;
    }

    // Start PCM device
    err = snd_pcm_prepare(handle);
    if (err < 0)
    {
        printf("Unable to start PCM device: %s\n", snd_strerror(err));
        return -1;
    }

    // Generate sine wave and write to PCM device
    while (1)
    {
        float buffer[buffer_frames];
        for (int i = 0; i < buffer_frames; i++)
        {
            buffer[i] = amplitude * sin(phase);
            phase += phase_increment;
            if (phase > 2.0 * M_PI)
            {
                phase -= 2.0 * M_PI;
            }
        }

        int frames_written = snd_pcm_writei(handle, buffer, buffer_frames);
        if (frames_written < 0)
        {
            err = snd_pcm_recover(handle, frames_written, 0);
            if (err < 0)
            {
                printf("Unable to recover from write error: %s\n", snd_strerror(err));
                return -1;
            }
        }
        else if (frames_written != buffer_frames)
        {
            printf("Short write (expected %d, wrote %d)\n", buffer_frames, frames_written);
        }
    }


// Close PCM device
snd_pcm_close(handle);

return 0;

}
