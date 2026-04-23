#pragma once

/*
 * webrtc-output.h
 * WebRTC output server for ultra-low-latency (< 100 ms) browser playback.
 *
 * Opens a minimal HTTP signalling endpoint on the configured port.
 * Browsers connect by navigating to http://localhost:<port>/ which serves
 * a self-contained WebRTC player page.  The page exchanges SDP via a
 * WHEP-style POST /offer endpoint.
 *
 * Video : H.264 Annex-B NAL units are packetised into RTP per RFC 6184
 *         and sent directly over WebRTC without decode/re-encode.
 * Audio : float32 interleaved PCM is resampled to 48 kHz, encoded to
 *         Opus (libopus / FFmpeg native), and packetised per RFC 7587.
 *
 * Requires libdatachannel (https://github.com/paullouisageneau/libdatachannel)
 * for ICE / DTLS / SRTP transport, and FFmpeg for Opus encoding + SWR.
 */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

struct webrtc_output;

/*
 * Create a WebRTC output server that listens for HTTP signalling on
 * http_port (e.g. 8889).  Navigate to http://localhost:<http_port>/ in a
 * browser to open the built-in player.
 * Returns NULL on failure.
 */
struct webrtc_output *webrtc_output_create(int http_port);

/*
 * Shut down the server and free all resources.
 */
void webrtc_output_destroy(struct webrtc_output *out);

/*
 * Write one H.264 Annex-B video frame.
 *   data   : raw Annex-B bytes (start codes 00 00 00 01 or 00 00 01)
 *   size   : byte count
 *   pts_us : presentation timestamp in microseconds
 *
 * Silently dropped when no WebRTC peer is connected.
 */
void webrtc_output_write_video(struct webrtc_output *out,
                               const uint8_t *data, size_t size,
                               int64_t pts_us);

/*
 * Write decoded PCM audio (float32 interleaved, e.g. L R L R …).
 *   pcm         : sample buffer
 *   samples     : number of samples PER CHANNEL
 *   channels    : channel count (typically 2)
 *   sample_rate : input sample rate (typically 44100 from AirPlay)
 *   pts_us      : presentation timestamp in microseconds (unused internally)
 *
 * Audio is resampled to 48 kHz and encoded to Opus in 20 ms frames.
 */
void webrtc_output_write_audio(struct webrtc_output *out,
                               const float *pcm, int samples,
                               int channels, int sample_rate,
                               int64_t pts_us);
