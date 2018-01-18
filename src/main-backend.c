#include <stdio.h>
#include <stdlib.h>

#include <libxml/xmlmemory.h>
#include <libxml/parser.h>

#include <libsoup/soup.h>

#include "parse.h"

#include "readConf.h"

channelArrayStruct * getDataFromBackend() {
	channelArrayStruct *channels_struct;
	SoupSession * session = soup_session_new();

	channels_struct = readChannels();
	if(channels_struct != NULL) {
		int i;
		for (i = 0; i < channels_struct->count; i++) {
			SoupMessage *msg = soup_message_new ("GET", channels_struct->data[i].id);
			if(msg == NULL) {
				fprintf(stderr, "msg is NULL. request: %s", channels_struct->data[i].id);
				continue;
			}
			soup_session_send_message (session, msg);
			channels_struct->data[i].video_entries = parseFeed(msg->response_body->data);
			g_object_unref(msg);
		}
		return channels_struct;
	}
	else {
		return NULL;
	}

}
