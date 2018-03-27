#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <time.h>
#include <unistd.h> 
#include <signal.h>
#include "cmput379.h"
#include <sys/inotify.h>
#include <errno.h>
#include <poll.h>
#define DEBUG 1
int pendingtime;
struct linkedList *dirList;
char* path;
int fd, poll_num;//make these variables gloabl so sigint can 
int *wd;

void checkdir(char* path){
    DIR *dir;
    char * file_name;
    int fileno;
    time_t time1;
    struct tm *timeinfo;
    struct dirent *dp;
    char buf[80];
    time ( &time1 );
    timeinfo = localtime ( &time1 );
    strftime(buf, sizeof(buf), "%a %b %d %H:%M:%S %Z %Y\n", timeinfo);
    printf("%s",buf);

    
    dir = opendir(path);
    if(dir == NULL){
	perror ("Cannot open this path");
        exit (EXIT_FAILURE);
    }
    dp = (struct dirent*) malloc (sizeof(struct dirent));
    while ((dp=readdir(dir)) != NULL) {
		if ( (strcmp(dp->d_name, ".")!=0) && (strcmp(dp->d_name, "..")!=0) ){
			searchList(dp, dirList,path);
		}
    }
	printList(dirList);
  	updateList(dirList);
  	closedir(dir);
}

void sigHandle(int signum)
{
    //if receive sigusr1, checkdir immediately
    if (signum == SIGUSR1)
    {
        checkdir(path);
    }
    else if (signum == SIGALRM)
    {
        checkdir(path); 
        alarm(pendingtime);
    }
    else if (signum== SIGINT)
    {
	/* Close inotify file descriptor */
	inotify_rm_watch( fd, wd );
	close(fd);
	free(wd);
	exit(EXIT_SUCCESS);
     }
}

/* Note: the example of inotify and its use are obtained from 
 * http://man7.org/linux/man-pages/man7/inotify.7.html */
void checkFileEvent(int fd, int *wd, int argc, char* argv[]){
	char buf[4096]
		 __attribute__ ((aligned(__alignof__(struct inotify_event))));
	const struct inotify_event *event;
	int i;
	ssize_t len;
	char *ptr;

	/* Loop while events can be read from inotify file descriptor. */

	for (;;) {

		/* Read some events. */

		len = read(fd, buf, sizeof buf);
		if (len == -1 && errno != EAGAIN) {
			perror("read");
			exit(EXIT_FAILURE);
		}

		/* If the nonblocking read() found no events to read, then
			 it returns -1 with errno set to EAGAIN. In that case,
			 we exit the loop. */

		if (len <= 0)
			break;

		/* Loop over all events in the buffer */

		for (ptr = buf; ptr < buf + len;
			ptr += sizeof(struct inotify_event) + event->len) {
			int filePos;
			event = (const struct inotify_event *) ptr;
			/* Print event type */
			if (event->mask & (IN_CREATE|IN_MOVED_TO)){
				if ((filePos=findFilePos(event->name,dirList))>=0){
					setFileState(filePos, dirList, 4);
				}
			}
			
			if (event->mask & (IN_DELETE|IN_MOVED_FROM)){
				if ((filePos=findFilePos(event->name,dirList))>=0){
					setFileState(filePos, dirList, 3);
				}
			}

			if (event->mask & IN_MODIFY){
				if ((filePos=findFilePos(event->name,dirList))>=0){
					setFileState(filePos, dirList, 2);
				}
			}
			if(event->mask & (IN_DELETE_SELF|IN_MOVE_SELF)){
				inotify_rm_watch( fd, wd );
				close(fd);
				free(wd);
				exit(EXIT_SUCCESS);
			}
			/*missing: director monitor. If there is a child directory use readdir to it and setFileState here if it is changed*/
		 }
	}
}
	


int main(int argc, char *argv[]){
	char buf;
	nfds_t nfds;
	struct pollfd fds[2];
	
	//create new list
	if (argc<3){
		printf("not enough arguments\n");
		exit(EXIT_FAILURE);
	}
	pendingtime = atoi(argv[1]);
	path = argv[2];
	
	/* Create the file descriptor for accessing the inotify API */

	fd = inotify_init1(IN_NONBLOCK);
	if (fd == -1) {
		 perror("inotify_init1");
		 exit(EXIT_FAILURE);
	}
	
	
	dirList = createList(dirList);
	#ifdef DEBUG
	#endif
        //first checkdir    
        checkdir(path);
	
	//create signal handler using sigaction
	struct sigaction act;
	memset(&act,"\0",sizeof(act));
	act.sa_handler = &sigHandle;
	act.sa_flags = SA_NODEFER;

	sigaction(SIGUSR1,&act,NULL);
	sigaction(SIGINT,&act,NULL);
	sigaction(SIGALRM,&act,NULL);
        alarm(pendingtime);
    
	/* Inotify part */
	/* Allocate memory for watch descriptors */

	wd = calloc(argc, sizeof(int));
	if (wd == NULL) {
		perror("calloc");
		exit(EXIT_FAILURE);
	}

	/* Mark directories for events */

	 wd[0] = inotify_add_watch(fd, path,IN_CREATE| IN_DELETE | IN_MODIFY | IN_MOVED_FROM|IN_MOVED_TO |IN_DELETE_SELF|IN_MOVE_SELF);
	 if (wd[0] == -1) {
		fprintf(stderr, "Cannot watch '%s'\n", path);
		perror("inotify_add_watch");
		exit(EXIT_FAILURE);
	 }

	/* Prepare for polling */

	nfds = 2;

	/* Console input */

	fds[0].fd = STDIN_FILENO;
	fds[0].events = POLLIN;

	/* Inotify input */

	fds[1].fd = fd;
	fds[1].events = POLLIN;

	while (1) {
		poll_num = poll(fds, nfds, -1);
		if (poll_num == -1) {
			if (errno == EINTR)
				continue;
			perror("poll");
			exit(EXIT_FAILURE);
		}

		if (poll_num > 0) {
			 if (fds[1].revents & POLLIN) {

				/* Inotify events are available */

				checkFileEvent(fd, wd, argc, argv);
			 }
		}
	}

    return 0;
}




