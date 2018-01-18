#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <libxml/xmlmemory.h>
#include <libxml/parser.h>

#include "parse.h"

xmlNode * getChildByName(xmlNode *parent, char *child) {
	xmlNode *cur = parent->children;
	while (cur != NULL) {
		if((!xmlStrcmp(cur->name, (const xmlChar *)child))) {
			return cur;
		}
		cur = cur->next;
	}
	return NULL;
}

videoEntry ** parseFeed(const char *buffer) {

	xmlDoc *doc;
	xmlNode *feed;

	doc = xmlParseMemory(buffer, strlen(buffer));

	if (doc == NULL ) {
		fprintf(stderr,"Document not parsed successfully. \n");
		return NULL;
	}

	feed = xmlDocGetRootElement(doc);

	if (feed == NULL) {
		fprintf(stderr,"empty document\n");
		xmlFreeDoc(doc);
		return NULL;
	}

	if (xmlStrcmp(feed->name, (const xmlChar *) "feed")) {
		fprintf(stderr,"document of the wrong type, root node != feed");
		xmlFreeDoc(doc);
		return NULL;
	}

	videoEntry tmp_video = {.id = NULL, .title = NULL, .date = NULL, .thumbnail = NULL};

	xmlNode *first_entry = getChildByName(feed, "entry");
	if (first_entry == NULL) {
		fprintf(stderr,"no entry\n");
		xmlFreeDoc(doc);
		return NULL;
	}
	int i;
	xmlNode * entry = first_entry;
	videoEntry **video_entries = malloc(15*sizeof(videoEntry*)); //15 - number of entries per rss (probably max at least)
	for (i = 0; i < 15; i++) {
		int should_break = 0;
		if(i>0) {
			do {
				entry = entry->next;
				if (entry == NULL) {
					video_entries[i] = NULL;
					should_break = 1;
					break;
				}
			}
			while (strcmp((char *)entry->name, "entry"));
			if (should_break == 1) {
				break;
			}
		}

		tmp_video.id = xmlNodeGetContent(getChildByName(entry, "videoId"));
		tmp_video.title = xmlNodeGetContent(getChildByName(entry, "title"));
		tmp_video.date = xmlNodeGetContent(getChildByName(entry, "published"));
		xmlNode *group = getChildByName(entry, "group");
		tmp_video.thumbnail = xmlNodeGetContent(getChildByName(group, "thumbnail")->properties->children);

		videoEntry *video = malloc(sizeof(videoEntry));
		if(video == NULL) {
			fprintf(stderr,"malloc failed :(");
			exit(1);
		}
		video->id = malloc(strlen((char *)tmp_video.id)+1);
		video->title = malloc(strlen((char *)tmp_video.title)+1);
		video->date = malloc(strlen((char *)tmp_video.date)+1);
		video->thumbnail = malloc(strlen((char *)tmp_video.thumbnail)+1);

		strcpy((char *)video->id, (char *)tmp_video.id);
		strcpy((char *)video->title, (char *)tmp_video.title);
		strcpy((char *)video->date, (char *)tmp_video.date);
		strcpy((char *)video->thumbnail, (char *)tmp_video.thumbnail);

		//medium quality
		*(video->thumbnail+strlen((char *)video->thumbnail)-13) = 'm';
		// -END-

		xmlFree(tmp_video.id);
		xmlFree(tmp_video.title);
		xmlFree(tmp_video.date);
		xmlFree(tmp_video.thumbnail);

		video_entries[i] = video;
	}

	xmlFreeDoc(doc);
	return video_entries;
}
