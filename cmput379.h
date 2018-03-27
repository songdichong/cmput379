#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h> 
#include <signal.h>
typedef struct node{
	unsigned char d_type;
	char d_name[256];
	int state; //indicate old,new,modified,removed,(remove and created) (0,1,2,3,4)
	struct node * next;	
	struct linkedList *subList; // only avaliable for subdirectories
}node;

typedef struct linkedList{
	node *head;
	int size;
}linkedList;

struct linkedList *createList();
void searchList(struct dirent *newItem,struct linkedList *curList,char* path);
void updateList(struct linkedList *curList);
void printList(struct linkedList *curList);
int findFilePos(char filename[125], struct linkedList *curList);
void setFileState(int filePos, struct linkedList *curList, int new_state);
int checkSubdir(char* path, struct linkedList *subList);
int updateSubList(struct linkedList *curList);
int searchSubList(struct dirent *newItem,struct linkedList *curList);
