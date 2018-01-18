#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>

#include <libxml/xmlmemory.h>
#include <libxml/parser.h>

#include "conf.h"
#include "readConf.h"

channelArrayStruct channels_struct;

channelArrayStruct * readChannels() {
	FILE *conf_file;

	char *path = getConfPath();

	conf_file = fopen(path,"r");
	free(path);
	if(conf_file == NULL) {

	fprintf(stderr,"no such file\n");
	return NULL;
	}
	//count lines vv
	int lines = 0;
	int ch;
	while((ch = fgetc(conf_file)) != EOF)
	{
		if(ch == '\n')
		{
			lines++;
		}
	}
	//count lines ^^
	fseek(conf_file, 0, SEEK_SET);

	channel *channels = malloc(sizeof(channel)*lines);
	if (channels == NULL) {
		fprintf(stderr,"malloc for channels failed :(");
		exit(1);
	}

	char buffer[200];
	int i;
	for(i=0; i<lines; i++) {
		memset(buffer, '\0', sizeof(buffer));
		if(fgets(buffer, sizeof(buffer), conf_file) == NULL) {
			break;
		}
		char * split = strchr(buffer, ',');
		if(split == NULL) {
			fprintf(stderr,"config file broken, risk of memory corruption - exitting");
			exit(1);
		}
		channels[i].id = malloc(strlen(split)+1-2);
		strncpy(channels[i].id, split+1,strlen(split)-2);
		*(channels[i].id + strlen(split)-2) = '\0';
		*split = '\0';
		channels[i].friendly_name = malloc(strlen(buffer)+1);
		strcpy(channels[i].friendly_name, buffer);
		channels[i].video_entries = NULL;
	}
	fclose(conf_file);

	channels_struct.data = channels;
	channels_struct.count = lines;

	return &channels_struct;
}
