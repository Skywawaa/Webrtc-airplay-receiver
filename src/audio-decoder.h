#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

struct audio_decoder;

struct decoded_audio {
	uint8_t *data;          /* Interleaved float32 PCM data */
	size_t data_size;       /* Size in bytes */
	int samples;            /* Number of samples per channel */
	int channels;           /* Number of channels */
	int sample_rate;        /* Sample rate in Hz */
	uint64_t pts;
};

/* Create a new AAC audio decoder */
struct audio_decoder *audio_decoder_create(void);

/* Destroy the decoder */
void audio_decoder_destroy(struct audio_decoder *dec);

/* Decode AAC data into PCM float32.
 * Returns true if audio was produced. */
bool audio_decoder_decode(struct audio_decoder *dec, const uint8_t *aac_data,
			  size_t aac_size, uint64_t pts,
			  struct decoded_audio *out);

/* Configure the decoder with AudioSpecificConfig (from AirPlay stream setup) */
bool audio_decoder_configure(struct audio_decoder *dec,
			     const uint8_t *asc_data, size_t asc_size);
