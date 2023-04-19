int main()
{
    int err;
    snd_pcm_t *handle;
    snd_pcm_hw_params_t *params;
    unsigned int sample_rate = 44100;
    int buffer_frames = 1024;
    int dir;

    TinySoundFont = tsf_load_memory(MinimalSoundFont, sizeof(MinimalSoundFont));

snd_pcm_open(&handle, "default", SND_PCM_STREAM_PLAYBACK, 0);

    // Allocate and initialize hardware parameters object
    snd_pcm_hw_params_alloca(&params);
    snd_pcm_hw_params_any(handle, params);
    snd_pcm_hw_params_set_access(handle, params, SND_PCM_ACCESS_RW_INTERLEAVED);
    snd_pcm_hw_params_set_format(handle, params, SND_PCM_FORMAT_FLOAT_LE);
    snd_pcm_hw_params_set_channels(handle, params, 1);
    snd_pcm_hw_params_set_rate_near(handle, params, &sample_rate, &dir);
    snd_pcm_hw_params_set_period_size(handle, params, buffer_frames, 0);

    snd_pcm_hw_params(handle, params);
snd_pcm_prepare(handle);


    tsf_set_output(TinySoundFont, TSF_STEREO_INTERLEAVED, sample_rate, 0);
tsf_note_on(TinySoundFont, 0, 60, 1.0f); //preset 0, middle C
tsf_note_on(TinySoundFont, 0, 48, 1.0f); //C2
tsf_note_on(TinySoundFont, 0, 52, 1.0f); //E2

// play stream

 // Start the PCM device
    snd_pcm_start(handle);

    // Wait for the audio to finish playing
    snd_pcm_drain(handle);

// Close PCM device
tsf_close(TinySoundFont);
snd_pcm_close(handle);

return 0;

=======
#include <alsa/asoundlib.h>
//g++ alsa.cpp -o alsa -lasound -ldl -lm
long loops;
int rc;
int size;
snd_pcm_t *handle;
snd_pcm_hw_params_t *params;
unsigned int val;
int dir;
snd_pcm_uframes_t frames;
char *buffer;

int main() {

    rc = snd_pcm_open(&handle, "default", SND_PCM_STREAM_PLAYBACK, 0);
    snd_pcm_hw_params_alloca(&params);
    snd_pcm_hw_params_any(handle, params);
    snd_pcm_hw_params_set_access(handle, params,
    SND_PCM_ACCESS_RW_INTERLEAVED);
    snd_pcm_hw_params_set_format(handle, params,
    SND_PCM_FORMAT_S16_LE);
    snd_pcm_hw_params_set_channels(handle, params, 2);
    val = 44100;
    snd_pcm_hw_params_set_rate_near(handle, params,&val, &dir);
    frames = 32;
    snd_pcm_hw_params_set_period_size_near(handle,params, &frames, &dir);
    rc = snd_pcm_hw_params(handle, params);
    snd_pcm_hw_params_get_period_size(params, &frames,&dir);
    size = frames * 4; /* 2 bytes/sample, 2 channels */
    buffer = (char *) malloc(size);
    snd_pcm_hw_params_get_period_time(params,&val, &dir);
    loops = 5000000 / val;
    while (loops > 0) {
        loops--;
        rc = read(0, buffer, size);
        rc = snd_pcm_writei(handle, buffer, frames);
    }
    snd_pcm_drain(handle);
    snd_pcm_close(handle);
    free(buffer);
}
