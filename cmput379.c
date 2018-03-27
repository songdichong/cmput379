#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h> 
#include <signal.h>
#include "cmput379.h"
//This create a new empty list
struct linkedList *createList(){
	struct linkedList *newList= (struct linkedList*) malloc(sizeof(struct linkedList));
	newList->size = 0;
	newList->head = NULL;
	return newList;
}

//Given a newItem, do something to the list(modifiy,insert or do nothing)
void searchList(struct dirent *newItem,struct linkedList *curList,char *path){
	//newItem = item to be inserted
	//curList = currentList
	//if the list is empty(file)
	if(curList->size ==0 ){
		struct node* newNode = (struct node*) malloc(sizeof(struct node));
		curList->size = (curList->size)+1;
		curList->head = newNode;
		newNode->d_type=newItem->d_type;
		strncpy(newNode->d_name,newItem->d_name,sizeof(newNode->d_name));
		newNode->state = 1;
		newNode->next = NULL;
		if(newNode->d_type == DT_DIR){
			newNode->subList = createList();
			char buf[256];
			snprintf(buf,sizeof buf,"%s/%s",path,newNode->d_name);
			checkSubdir(buf,newNode->subList);
		}
	}
	else{
		//find the position of the newitem (if there are any)
		int position = 0;
		struct node *current = curList->head;
		struct node *prev = NULL;
		while (current!=NULL && strcmp(current->d_name,newItem->d_name)<0  && position<(curList->size)){
			prev = current;
			current = current->next;
			position += 1;
		}
		
		//if it is not here
		if(current==NULL || strcmp(current->d_name,newItem->d_name)!=0){
			//creat  new Node 
			struct node *newNode = (struct node*)malloc(sizeof(struct node));
			newNode->d_type=newItem->d_type;
			strncpy(newNode->d_name,newItem->d_name,sizeof(newNode->d_name));
			newNode->state = 1;
			//inser it to the list (pre condition: there is a node before it)
			if (prev == NULL) {
				curList->head = newNode;
			}
			else {
				prev->next = newNode;
			}
			if(newNode->d_type == DT_DIR){
				newNode->subList = createList();
				char buf[256];
				snprintf(buf,sizeof buf,"%s/%s",path,newNode->d_name);
				checkSubdir(buf,newNode->subList);
			}
			newNode->next = current;
			curList->size = (curList->size)+1;
			//printf("%s added at position %d\n", newNode->file->d_name,position);
		}else if(current->d_type ==DT_DIR && current->state!= 3){  //if this is a directory that is not removed 
			char buf[256];
			snprintf(buf,sizeof buf,"%s/%s",path,current->d_name);
			int modified = checkSubdir(buf,current->subList);
			if (modified == 1){
				current->state = 2;
			}
		}
	}
}
//this will remove all the "non-existed" file from the list
//, then it will set all file to be non-existed and old
void updateList(struct linkedList *curList) {
	struct node *current = curList->head;
	struct node *prev = NULL;
	while (current != NULL) {
		if (current->state == 3) {  //if files is not here , remove it
			if (prev != NULL) { 
				prev->next = current->next; 
			}else{
				curList->head =	current->next;
			}
			curList->size -=1;
		}
		else{	//if file is here, update state to OLD
			current->state = 0;
			prev = current;
		}
		current = current->next;
	}
}
//this will print out the list
void printList(struct linkedList *curList){
	//printf("Size of ll : %d \n",curList->size);
	if(curList->size >0){
		node *current = curList->head;
		int i =0;
		while (i < (curList->size)) {
			if (current->state == 4) {
				printf("- %s \n",current->d_name);
			}
			char s='0';	//'0' indicate this file does not have to be printed
			if (current->state == 3) {
				s = '-';
			}
			else if (current->state != 0) { //
				//if it is a new file
				if (current->state == 1 ||current->state ==4) {
					//8
					if (current->d_type == DT_REG) {
						s = '+';
					}
					//4
					else if (current->d_type == DT_DIR) {
						s = 'd';
					}
					else {
						s = 'o';
					}
				}
				else if (current->state == 2) {	//modified file
					s = '*';
				}
			}
			if(s!='0'){
				printf("%c %s \n", s, current->d_name);
			}
			i += 1;
			current = current->next;
		}
	}
}

int findFilePos(char filename[125], struct linkedList *curList){
	struct node *current = curList->head;
	struct node *prev;
	int position = 0;
	while (current!=NULL && strcmp(current->d_name,filename)<0){
			prev = current;
			current = current->next;
			position += 1;
	}
	//if the file is not here or this is the tail, then return -1
	if (current == NULL || strcmp(current->d_name,filename)>0){
		position = -1;
	}
	return position;
}

void setFileState(int filePos,struct linkedList *curList,int new_state){
	struct node *current = curList->head;
	int position =0;
	while (position<filePos){
		current = current->next;
		position+=1;
	}
	
	current->state = new_state;
	return;
}
////////////////////////////
//FOR SUBDIRECTORIES ONLY
int searchSubList(struct dirent *newItem,struct linkedList *curList){ //return 1 if a file is added
	//newItem = item to be inserted
	//curList = currentList
	//if the list is empty(file)
	int output =1;
	if(curList->size ==0 ){
		struct node* newNode = (struct node*) malloc(sizeof(struct node));
		curList->size = (curList->size)+1;
		curList->head = newNode;
		newNode->d_type=newItem->d_type;
		strncpy(newNode->d_name,newItem->d_name,sizeof(newNode->d_name));
		newNode->state = 1;
		newNode->next = NULL;
	}
	else{
		//find the position of the newitem (if there are any)
		int position = 0;
		struct node *current = curList->head;
		struct node *prev = NULL;
		while (current!=NULL && strcmp(current->d_name,newItem->d_name)<0  && position<(curList->size)){
			prev = current;
			current = current->next;
			position += 1;
		}
		
		//if it is not here
		if(current==NULL || strcmp(current->d_name,newItem->d_name)!=0){
			//creat  new Node 
			struct node *newNode = (struct node*)malloc(sizeof(struct node));
			newNode->d_type=newItem->d_type;
			strncpy(newNode->d_name,newItem->d_name,sizeof(newNode->d_name));
			newNode->state = 1;
			//inser it to the list (pre condition: there is a node before it)
			if (prev == NULL) {
				curList->head = newNode;
			}
			else {
				prev->next = newNode;
			}
			newNode->next = current;
			curList->size = (curList->size)+1;
		}else if(strcmp(current->d_name,newItem->d_name)==0){ 	//if file is here
			current->state = 0;
			output =0;
		}
	}
	return output;
}
int updateSubList(struct linkedList *curList) {			//return 1 if a file is removed
	int output =0;
	struct node *current = curList->head;
	struct node *prev = NULL;
	while (current != NULL) {
		if (current->state == 3) {  //if files is not here , remove it
			if (prev != NULL) { 
				prev->next = current->next; 
			}else{
				curList->head =	current->next;
			}
			curList->size -=1;
			output =1;
		}else if (current->state ==0){	// if file is old, set it to remove in next iteration
			current->state = 3;
			prev = current;
		}
		else{	//if file is here, update state to OLD
			current->state = 0;
			prev = current;
		}
		current = current->next;
	}
	return output;
}

int checkSubdir(char* path,struct linkedList *subList){		
	//Assume Subdir is a valid directory
	//return 1 if there is a changes in the directory
	int modified = 0;
    DIR *dir;
    char * file_name;
    int fileno, y;
    dir = opendir(path);
    struct dirent *dp = (struct dirent*) malloc (sizeof(struct dirent));
    while ((dp=readdir(dir)) != NULL) {
		if ( (strcmp(dp->d_name, ".")!=0) && (strcmp(dp->d_name, "..")!=0) ){
			y = searchSubList(dp, subList);
			if(y ==1){
				modified = 1;	
			}
		}
    }
  	y= updateSubList(subList);
	if(y ==1){
		modified = 1;	
	}
  	closedir(dir);
	return modified;
}


