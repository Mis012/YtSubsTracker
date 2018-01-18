#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pwd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>

#include "conf.h"
#include "writeConf.h"

int getLineToRemove(char *channel_id) {

	FILE * fp;
	char * line = NULL;
	size_t len = 0;
	ssize_t read;

	char *path = getConfPath();
	fp = fopen(path, "r");
	free(path);
	if (fp == NULL) {
		fprintf(stderr, "ERROR removing channel - fopen() failed");
		return -1;
	}

	int n_lines = 0;
	while ((read = getline(&line, &len, fp)) != -1) {
		n_lines++;
		if(strstr(line, channel_id)) {
			fclose(fp);
			return n_lines;
		}
	}

	fclose(fp);

	fprintf(stderr, "ERROR removing channel - no such channel_id");
	return -1;
}

void writeConf(const char *friendly_name, const char* xml_feed_url) {
	FILE *conf_file;


	char *path = getConfPath();
	conf_file = fopen(path,"a");
	free(path);

	fprintf(conf_file, "%s,%s\n", friendly_name, xml_feed_url);
}

int removeChannel(char *channel_id) {

	/* Get the number of line we want to remove beforehand - it uses fopen() */
	int line = getLineToRemove(channel_id);
	if(line == -1) {
		fprintf(stderr, "ERROR removing channel - getLineToRemove() failed");
    return 1;
	}


  /* Open the file in read-write mode */
	char *path = getConfPath();

  int fd = open(path, O_RDWR);
	free(path);
  if (fd < 0) {
    fprintf(stderr, "ERROR removing channel - open() failed");
    return 1;
  }

  /* stat() the file to find the size */
  struct stat stat_buf;
  if (fstat(fd, &stat_buf) < 0) {
    fprintf(stderr, "ERROR removing channel - fstat() failed");
    return 1;
  }

  /* Map the file into the current process's address space -- only works on
     regular files */
  void *map = mmap(NULL, stat_buf.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  if (map == MAP_FAILED) {
    fprintf(stderr, "ERROR removing channel - mmap() failed");
    return 1;
  }

  /* Find the nth line */
  char *line_n = map;
  int i;
  for (i = 1; i < line; ++i) {
    /* Search for the next '\n' character.  Assumes Linux newline encoding */
    line_n = memchr(line_n, '\n', stat_buf.st_size - (line_n - (char *)map));
    /* Point to the character one past the newline */
    ++line_n;
  }

  /* Find the (n + 1)th line */
  char *line_n1 = memchr(line_n, '\n', stat_buf.st_size - (line_n - (char *)map));
  if (line_n1) {
    /* We found the end of the line, so swallow the newline */
    ++line_n1;

    /* Erase the line by copying the memory at line_n1 to line_n */
    memmove(line_n, line_n1, stat_buf.st_size - (line_n1 - (char *)map));

    /* Unmap the file */
    if (munmap(map, stat_buf.st_size) < 0) {
      fprintf(stderr, "ERROR removing channel - munmap() failed");
   		return 1;
    }

    /* Shrink the file by the size of the nth line */
    if (ftruncate(fd, stat_buf.st_size - (line_n1 - line_n)) < 0) {
      fprintf(stderr, "ERROR removing channel - ftruncate() failed");
   		return 1;
    }
  } else {
    /* The nth line was the last line, so unmap the file */
    if (munmap(map, stat_buf.st_size) < 0) {
      fprintf(stderr, "ERROR removing channel - munmap() failed");
   		return 1;
    }

    /* Chop off the last line */
    if (ftruncate(fd, line_n - (char *)map) < 0) {
      fprintf(stderr, "ERROR removing channel - ftruncate() failed");
   		return 1;
    }
  }

  /* Close the file */
  close(fd);

  return 0;
}
