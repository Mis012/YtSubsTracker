#ifndef PARSE_H
#define PARSE_H

#include <libxml/xmlmemory.h>
#include <libxml/parser.h>

typedef struct _videoEntry {
		xmlChar *id;
		xmlChar *title;
		xmlChar *date;
		xmlChar *thumbnail;
	} videoEntry;

videoEntry ** parseFeed(const char *buffer);

#endif
