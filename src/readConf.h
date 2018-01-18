#ifndef READCONF_H
#define READCONF_H

#include "parse.h"

typedef struct _channel {
	char *id;
	char *friendly_name;
	videoEntry **video_entries;
} channel;

typedef struct _channelArrayStruct {
	channel *data;
	int count;
} channelArrayStruct;

channelArrayStruct * readChannels();
#endif
