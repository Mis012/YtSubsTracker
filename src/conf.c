#include <stdlib.h>
#include <unistd.h>
#include <pwd.h>
#include <sys/types.h>
#include <string.h>

char * getConfPath(){
	struct passwd *pw = getpwuid(getuid());

	const char *relative_path = "/.config/yt_subs_tracker/channels.conf";
	const char *homedir = pw->pw_dir;
	char *path = malloc(strlen(homedir)+strlen(relative_path)+1);
	strcpy(path, homedir);
	strcat(path, relative_path);
	return path;
}
