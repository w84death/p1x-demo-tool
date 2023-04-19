#include <cstdio>
#include <math.h>
#include <alsa/asoundlib.h>
#define TSF_IMPLEMENTATION
#include "tsf.h"

//This is a minimal SoundFont with a single loopin saw-wave sample/instrument/preset (484 bytes)
const static unsigned char MinimalSoundFont[] =
{
	#define TEN0 0,0,0,0,0,0,0,0,0,0
	'R','I','F','F',220,1,0,0,'s','f','b','k',
	'L','I','S','T',88,1,0,0,'p','d','t','a',
	'p','h','d','r',76,TEN0,TEN0,TEN0,TEN0,0,0,0,0,TEN0,0,0,0,0,0,0,0,255,0,255,0,1,TEN0,0,0,0,
	'p','b','a','g',8,0,0,0,0,0,0,0,1,0,0,0,'p','m','o','d',10,TEN0,0,0,0,'p','g','e','n',8,0,0,0,41,0,0,0,0,0,0,0,
	'i','n','s','t',44,TEN0,TEN0,0,0,0,0,0,0,0,0,TEN0,0,0,0,0,0,0,0,1,0,
	'i','b','a','g',8,0,0,0,0,0,0,0,2,0,0,0,'i','m','o','d',10,TEN0,0,0,0,
	'i','g','e','n',12,0,0,0,54,0,1,0,53,0,0,0,0,0,0,0,
	's','h','d','r',92,TEN0,TEN0,0,0,0,0,0,0,0,50,0,0,0,0,0,0,0,49,0,0,0,34,86,0,0,60,0,0,0,1,TEN0,TEN0,TEN0,TEN0,0,0,0,0,0,0,0,
	'L','I','S','T',112,0,0,0,'s','d','t','a','s','m','p','l',100,0,0,0,86,0,119,3,31,7,147,10,43,14,169,17,58,21,189,24,73,28,204,31,73,35,249,38,46,42,71,46,250,48,150,53,242,55,126,60,151,63,108,66,126,72,207,
		70,86,83,100,72,74,100,163,39,241,163,59,175,59,179,9,179,134,187,6,186,2,194,5,194,15,200,6,202,96,206,159,209,35,213,213,216,45,220,221,223,76,227,221,230,91,234,242,237,105,241,8,245,118,248,32,252
};
static tsf* TinySoundFont;

// g++ -o sine_wave alsa.cpp -lasound -lm
int audio_callback(snd_pcm_t *pcm, const snd_pcm_channel_area_t *areas, snd_pcm_uframes_t frames) {
    // SampleCount = (len / (2 * sizeof(short)));
    tsf_render_short(TinySoundFont, (short*)pcm, frames, 0);
    return 0;
}



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

}
