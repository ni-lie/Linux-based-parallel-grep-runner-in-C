#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <pthread.h>

#define BUFFER 1000

int count = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;


// argv[2] = path, argv[3] = stringToSearch (GLOBAL VARIABLES)
char *path;
char *stringToSearch;

// linked list implementation using queues: https://prepinsta.com/c-program/implementation-of-queues-using-linked-list/
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
    if ((front == NULL)) {
        return NULL;
    } else {
        char *deqDir = (char*)malloc(BUFFER);
        strcpy(deqDir, front->dir); 

        struct taskNode *temp;
        temp = front;
        front = front->next;

        if(front == NULL)
            rear = NULL;
        free(temp->dir);
        free(temp);

        return deqDir;
    }

}

void print()	
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

void *multiGrep(int *id){
    while(1){
        if(count == 0)
            break;
        // lock then dequeue then get absolute path
        pthread_mutex_lock(&mutex);
        char *dequeuedTask;
        dequeuedTask = dequeue();
        pthread_mutex_unlock(&mutex);

        // if dequeued task is not null
        if(dequeuedTask!=NULL){
            char path1[BUFFER];
            char * const absPath = realpath(dequeuedTask, path1);                                  // absolute path: https://stackoverflow.com/questions/229012/getting-absolute-path-of-a-file
            printf("[%d] DIR %s\n", *id, path1);
            count--;
            

            struct dirent *dp;                                                                     // Traversing folders in C: https://iq.opengenus.org/traversing-folders-in-c/
            DIR *dir;	
            dir = opendir(absPath);
        
            
            while((dp = readdir(dir)) != NULL){
                if (strcmp(dp->d_name, ".") == 0 || strcmp(dp->d_name, "..") == 0)                  // if current or parent directory, ignore
                    continue;
                else if(dp->d_type == DT_REG){                                                      // if dp == file; source: https://stackoverflow.com/questions/23958040/checking-if-a-dir-entry-returned-by-readdir-is-a-directory-link-or-file-dent
                    char filePath[BUFFER];
                    strcpy(filePath, path1);
                    strcat(filePath, "/");
                    strcat(filePath, dp->d_name);

                    char fileAbsPath[BUFFER];
                    char * const file1 = realpath(filePath, fileAbsPath);

                    char command[BUFFER];
                    strcpy(command, "grep ");                                   // grep cs140 <abspath> >/dev/null
                    strcat(command, stringToSearch);
                    strcat(command, " ");
                    strcat(command, fileAbsPath);
                    strcat(command, " >/dev/null");                            // redirect its standard output stream to /dev/null: https://unix.stackexchange.com/questions/119648/redirecting-to-dev-null
                
                    if(!system(command)){                                      // System function in C: https://www.tutorialspoint.com/c_standard_library/c_function_system.htm
                        printf("[%d] PRESENT %s\n", *id, fileAbsPath);
                        count--;
                    }
                        
                    else{
                        printf("[%d] ABSENT %s\n", *id, fileAbsPath);
                        count--;
                    }
                        
                }
                
                // if dp is directory, enqueue it 
                else{
                    char next[BUFFER];
                    strcpy(next, path1);
                    strcat(next, "/");
                    strcat(next, dp->d_name);

                    pthread_mutex_lock(&mutex);
                    enqueue(next);
                    pthread_mutex_unlock(&mutex);
                
                    printf("[%d] ENQUEUE %s\n", *id, next);
                }
            }
            closedir(dir);
            free(dequeuedTask);
        }
    }
    pthread_exit(NULL);                                                     // terminate thread: https://pubs.opengroup.org/onlinepubs/7908799/xsh/pthread_exit.html#:~:text=The%20pthread_exit()%20function%20terminates,were%20pushed%20and%20then%20executed.
}

// source: https://iq.opengenus.org/traversing-folders-in-c/
void myfilerecursive(char *basePath)
{
    char path[BUFFER];
    struct dirent *dp;
    DIR *dir = opendir(basePath);

    if (!dir)
        return;

    while ((dp = readdir(dir)) != NULL)
    {
        if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0)
        {
            // printf("%s\n", dp->d_name);
            strcpy(path, basePath);
            strcat(path, "/");
            strcat(path, dp->d_name);
            count++;
            myfilerecursive(path);
        }
    }

    closedir(dir);
}	
	

int main(int argc, char *argv[]){
    int totalWorkers = atoi(argv[1]);				            // convert string to int					
    pthread_t thread[totalWorkers];
    int id[totalWorkers];

    path = argv[2];
    stringToSearch = argv[3];

    myfilerecursive(path);                                      // count the number of files and subdirectories; count is used to
    count++;                                                    // increment count by 1 in order to account for the root directory

    for(int i=0; i<totalWorkers; i++){
        id[i] = i;
    }

	enqueue(argv[2]);										    // enqueue roothpath

    for(int i=0; i<totalWorkers; i++){
        pthread_create(&thread[i], NULL, (void *) multiGrep, &id[i]);
    }

    for(int j=0; j<totalWorkers; j++){
        pthread_join(thread[j], NULL);
    }

    return 0;
}

