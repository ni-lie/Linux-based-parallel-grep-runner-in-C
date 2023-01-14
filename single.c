#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>

#define BUFFER 1000

// function prototype
void enqueue(char *);
char *dequeue();
void print();


// linked list implementation: https://prepinsta.com/c-program/implementation-of-queues-using-linked-list/
struct taskNode{
    char *dir;
    struct taskNode *next;
}; 

struct taskNode *front = NULL;
struct taskNode *rear = NULL;

void enqueue(char *directory){
    struct taskNode *newTask;
    newTask = (struct taskNode*)malloc(sizeof(struct taskNode));
	newTask->dir = (char *)malloc(BUFFER);

    strcpy(newTask->dir, directory);
    newTask->next = NULL;

    if((front == NULL) && (rear == NULL))
        front = rear = newTask;
    
    else{
        rear->next = newTask;
        rear = newTask;
    }
}

char *dequeue()
{
	struct taskNode *temp;
	temp = front;
	if((front == NULL)&&(rear == NULL)){
		printf("\nQueue is Empty\n");
		return NULL;
	}
	else{
		char *deqDir = (char*)malloc(BUFFER);
		strcpy(deqDir, front->dir);
		
		front = front->next;
		free(temp);
		
		if(front == NULL)
			rear = NULL;
		// if(rear != NULL)
		// 	printf("rear = %s\n", rear->dir);
		// else if(rear!=NULL)
		// 	printf("rear = %s\n", rear->dir);

		return deqDir;
	}
	
}

void print()	//function to display the queue
{
	struct taskNode* temp;
	if((front == NULL)&&(rear == NULL)){
		printf("\nQueue is Empty\n");
	}
	else{
		temp = front;
		while(temp){
			printf("%s",temp->dir);
			temp = temp->next;
		}
		printf("\n");
	}
}

int singleGrep(char *path, char *stringToSearch){
	//int id = *arg;
	// 1. If the task queue is empty: standby
	// printf("Start!\n");
	// 2. Otherwise
	// 2.1 Dequeue the next task from the task queue
	char *dequeuedTask = (char*)malloc(BUFFER);
	// printf("malloc is success\n");
	dequeuedTask = dequeue();
	//strcpy(dequeuedTask, dequeue());
	// printf("dequeuedTask = %s\n", dequeuedTask);
	// print();

	// 2.2 Output [n] DIR path where n is the relevant worker ID and path is the
	// absolute path of the said directory
	char *absPath = (char*)malloc(BUFFER);															// source: https://stackoverflow.com/questions/229012/getting-absolute-path-of-a-file
	realpath(dequeuedTask, absPath);																// get absolute path of dequeuedTask or directory

	printf("[%d] DIR %s\n", 0, absPath);

	// 2.3 Invoke opendir on the directory associated with the dequeued task
	struct dirent *dp;
	DIR *dir;																						// source: https://pubs.opengroup.org/onlinepubs/009604599/functions/opendir.html
	//FILE *openFile;
	dir = opendir(absPath);
	

	char *command = (char*)malloc(BUFFER);															// for system(command)

	// if(!dir){
	// 	perror(absPath);
	// 	printf("Error! No Directory\n");
	// 	return -1;
	// }
	// 2.4. For each child object referred to by successive readdir invocations:
	while((dp = readdir(dir)) != NULL){
		char *abs_path = (char*)malloc(BUFFER);
		realpath(absPath, abs_path);
		// printf("dp = %s\n", dp->d_name);
		// printf("absolute path: %s\n", abs_path);
		if (strcmp(dp->d_name, ".") == 0 || strcmp(dp->d_name, "..") == 0)								// ignore current and parent directories
			continue;
		

		// 3.2. If the child object is a file
		else if(dp->d_type == DT_REG){																	// if dp == regular file
			char *filePath = (char *)malloc(BUFFER);
			strcpy(filePath, abs_path);
			strcat(filePath, "/");
			strcat(filePath, dp->d_name);

			char *fileAbsPath = (char *)malloc(BUFFER);													// get absolute path of file
			realpath(filePath, fileAbsPath);
			// printf("filename = %s\n", fileAbsPath);

			// printf("filename = %s\n", dp->d_name);
			// 3.2.1. Invoke grep with arguments search string and path via the system 
			// function where search string is the search string supplied as a program argument 
			// and path is the path (relative or absolute) of the said file

			strcpy(command, "grep ");																	// grep stringToSearch file.txt >/dev/null
			strcat(command, stringToSearch);															// grep command: https://swcarpentry.github.io/shell-novice/07-find/index.html#:~:text=The%20grep%20command%20searches%20through,contain%20the%20letters%20'not'.
			strcat(command, " ");
			strcat(command, fileAbsPath);
			strcat(command, " >/dev/null");
			
			
			// 3.2.2. If a match was found by grep, print [n] PRESENT path where n is
			// the relevant worker ID and path is the absolute path of the said file
			if(!system(command))																		// system(command) returns 0 if success
				printf("[0] PRESENT %s\n", fileAbsPath);			
			// 	3.2.3. Otherwise (i.e., no match is found), print [n] ABSENT path where n
			// is the relevant worker ID and path is the absolute path of the said file
			else
				printf("[0] ABSENT %s\n", fileAbsPath);		
			free(filePath);
			free(fileAbsPath);

		}

		// 3.1. If the child object is a directory:
		else{																							// if dp == directory or folder
			char *next = (char *)malloc(BUFFER);
			// printf("%s\n", dp->d_name);
			strcpy(next, abs_path);
			strcat(next, "/");
			strcat(next, dp->d_name);

			// printf("next = %s\n", next);
			// 3.1.1 Enqueue a new task onto the task queue containing the path to the
			// said directory
			enqueue(next);
			// 3.1.2. Output [n] ENQUEUE path where n is the relevant worker ID and
			// path is the absolute path of the said directory
			printf("[0] ENQUEUE %s\n", next);

			free(next);
			singleGrep(next, stringToSearch);
			
		}

	}
		closedir(dir);
	free(dequeuedTask);
	free(absPath);
		return 0;
}

	
	

int main(int argc, char *argv[]){
	enqueue(argv[2]);										// enqueue roothpath
	singleGrep(argv[2], argv[3]);  							// argv[2] = rootpath, argv[3] = search string
    return 0;
}

// int main()//main function to use all our declared function
// {
// 	int opt,n,i;
//     char data[BUFFER];
// 	printf("Enter Your Choice:-");
// 	while(opt!=0){
// 		printf("\n\n1 for Insert the Data in Queue\n2 for show the Data in Queue \n3 for Delete the data from the Queue\n0 for Exit");
// 		scanf("%d",&opt);
// 		switch(opt){
// 			case 1:
// 				printf("\nEnter the size: ");
// 				scanf("%d",&n);
// 				printf("\nEnter your data\n");
// 				i=0;
// 				while(i<n){
// 					scanf("%s",&data);
// 					enqueue(data);
// 					i++;
// 				}
// 				break;
// 			case 2:
// 				print();
// 				break;
// 			case 3:
// 				dequeue();
// 				break;
// 			case 0:
// 				break;
// 			default:
// 				printf("\nIncorrect Choice");
// 			
// 		}
// 	}
// }