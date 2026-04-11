#pragma once

#include <obs-module.h>
#include <stdbool.h>
#include <stdint.h>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
typedef SOCKET socket_t;
#define INVALID_SOCK INVALID_SOCKET
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
typedef int socket_t;
#define INVALID_SOCK (-1)
#define closesocket close
#endif

#include <pthread.h>

/* Forward declarations */
struct airplay_server;
struct video_decoder;
struct audio_decoder;

/* Main source data structure */
struct airplay_source {
	obs_source_t *source;

	/* Settings */
	char server_name[256];
	uint16_t airplay_port;
	int width;
	int height;

	/* AirPlay server */
	struct airplay_server *server;

	/* Video decoder */
	struct video_decoder *decoder;

	/* Audio decoder */
	struct audio_decoder *audio_dec;

	/* Frame buffer */
	pthread_mutex_t frame_mutex;
	struct obs_source_frame current_frame;
	uint8_t *frame_data[MAX_AV_PLANES];
	bool frame_ready;

	/* State */
	bool running;
	bool connected;
};

void airplay_source_register(void);
