#include <stdio.h>
#include <stdlib.h>
#include <sys/inotify.h>
#include <unistd.h>
#include <string.h>
#include <ftw.h>
#include "logger.h"

#define EVENT_SIZE (sizeof(struct inotify_event))	//	SIZE OF ONE EVENT
#define BUFF_LEN (2048 * EVENT_SIZE + 32)	// 	BUFFER LENGHT
#define DICTIONARY_SIZE 69697

struct entry
{
	struct entry *next;
	long key;
	char* path;
};

static struct entry *dictionary[DICTIONARY_SIZE];
int handleEvent(struct inotify_event *event);
int addFile(const char* fpath, const struct stat *sb, int flag);
char* getValue(int key);
void addElement(int key, char *path);

unsigned hash(long key){
	return (key + 101) % DICTIONARY_SIZE;
}

int inot;
char* lastMoved[2];

int main(int argc, char const *argv[])
{

	if(argc != 2){
		warnf("WARNING: Missing arguments\n");
		return 0;
	}
	
	inot = inotify_init();

	if(inot < 0){
		errorf("ERROR: Notify not initialized\n");
		return 0;
	}
	
	if(ftw(argv[1], addFile, 20) == -1) {
		errorf("ERROR: Could not add watch to %s\n", argv[1]);
		return 0;
	} else{
		infof("Starting File/Directory Monitor on %s\n -----------------\n", argv[1]);
	}


	//	MONITOR
	char buff[BUFF_LEN];

	while(1){
		int i = 0;
		int readed = read(inot, buff, BUFF_LEN);

		if(readed < 0){
			errorf("ERROR: Error while reading\n");
			return 0;
		}

		while(i < readed){
			struct inotify_event *event = (struct inotify_event *) &buff[i];
			if(event->len){
				handleEvent(event);
				i += EVENT_SIZE + event->len;
			}
		}

	}

	return 0;
}

int addFile(const char* fpath, const struct stat *sb, int flag){
	if(flag == FTW_D){
		int watch_desc = inotify_add_watch(inot, fpath, IN_CREATE | IN_MODIFY | IN_DELETE | IN_MOVE);
		addElement(watch_desc, fpath);
		return 0;
	}
}

int handleEvent(struct inotify_event *event){
	char *path = getValue(event->wd);

	if(event->mask & IN_CREATE){
		//	CREATE DIRECTORY
		if(event->mask & IN_ISDIR){
			char directory[strlen(path) + strlen(event->name) + 1];
			infof("- [DIRECTORY CREATE] - %s\n", event->name);
			strcpy(directory, path);
			strcat(directory, "/");
			strcat(directory, event->name);
			addFile(directory, NULL, FTW_D);

		} else{
			//	CREATE FILE
			infof("- [FILE CREATE] - %s\n", event->name);
		}
	}

	else if(event->mask & IN_MODIFY){
		//	MODIFY FILE
		infof("- [FILE MODIFY] - %s\n", event->name);
	}

	else if(event->mask & IN_DELETE){
		if(event->mask & IN_ISDIR){
			//	REMOVAL DIRECTORY
			infof("- [DIRECTORY REMOVAL] - %s\n", event->name);	
			inotify_rm_watch(inot, event->wd);	
		} else{
			//	REMOVAL FILE
			infof("- [FILE REMOVAL] - %s\n", event->name);
		}
	}

	else if(event->mask & IN_MOVED_FROM){
		lastMoved[0] = event->name;
		lastMoved[1] = path;
	}

	else if(event->mask & IN_MOVED_TO){
		if(strcmp(lastMoved[1], path) == 0){
			if(event->mask & IN_ISDIR){
				//	RENAME DIRECTORY
				infof("- [DIRECTORY RENAME] - %s -> %s\n", lastMoved[0], event->name);
			}else{
				//	RENAME FILE
				infof("- [FILE RENAME] - %s -> %s\n", lastMoved[0], event->name);
			}
		}
		lastMoved[0] = NULL;
		lastMoved[1] = NULL;
	}


	return 0;
}

char* getValue(int key){
	struct entry *element;
	for(element = dictionary[hash(key)]; element != NULL; element = element->next){
		if(key == element->key){
			return element->path;
		}
	}

	return NULL;
}

struct entry* getElement(int key){
	struct entry *element;
	for(element = dictionary[hash(key)]; element != NULL; element = element->next){
		if(key == element->key){
			return element;
		}
	}

	return NULL;
}

void addElement(int key, char *path){
	struct entry *element;
	unsigned hashval;
	if((element = getElement(key)) == NULL){
		element = malloc(sizeof(*element));
		hashval = hash(key);
		element->next = dictionary[hashval];
		element->key = key;
		element->path = malloc(strlen(path)+1);
		strcpy(element->path, path);
		dictionary[hashval] = element;
	}
	else {
		free(element->path);
		element->path = malloc(strlen(path)+1);
		strcpy(element->path, path);
	}
}