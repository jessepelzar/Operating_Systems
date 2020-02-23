/* Jesse Pelzar P2*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#ifndef _WIN32
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>
#endif
#include "time_functions.h"

#define BUFFER_SIZE 10
#define LINE_SIZE 1024


char *buffer[BUFFER_SIZE];

sem_t fillCount;
sem_t emptyCount;
sem_t manager;

void *producer(void* inputFile);
void *consumer(void* outputFile);
void filePath();

FILE * in_file;
FILE * out_file;

char *GET();
char PUT(char* val);

int fill = 0;
int use = 0;
int count = 0;

int main()
{
        filePath();

        sem_init(&fillCount,0,0);
        sem_init(&emptyCount,0,BUFFER_SIZE);
        sem_init(&manager,0,1);

        pthread_t id_1;
        pthread_t id_2;

        pthread_attr_t attr1;
        pthread_attr_t attr2;
        pthread_attr_init(&attr1);
        pthread_attr_init(&attr2);


// start timing
        start_nanotime();
        pthread_create(&id_1, &attr1, producer, (void*)in_file);
        pthread_create(&id_2, &attr2, consumer, (void*)out_file);
// stop stop_timing
        stop_timing();
        pthread_join(id_1, NULL);
        pthread_join(id_2, NULL);

        fclose(in_file);
        fclose(out_file);

        sem_destroy(&fillCount);
        sem_destroy(&emptyCount);
        sem_destroy(&manager);
        printf("\nWall time: %f seconds \nnano time is: %d seconds\n", get_wall_clock_diff(), get_nanodiff());

        pthread_exit(NULL);
        return 0;
}

char PUT(char* val) {
        buffer[fill] = val;
        printf("buffer in: %d\n", fill);
        fill = (fill + 1) % BUFFER_SIZE;
        count++;
}

char *GET() {
        char *temp = buffer[use];
        printf("buffer out: %d\n", use);
        use = (use + 1) % BUFFER_SIZE;;

        return temp;
}

void* producer(void *inputFile)
{
        FILE *f_in;
        f_in = (FILE*)inputFile;

        while(!(feof(f_in))) {
                char* line = (char*)malloc(sizeof(char)*LINE_SIZE+2);
                fgets(line, LINE_SIZE+2, f_in);

                sem_wait(&emptyCount); // decrements count when allowed
                sem_wait(&manager);
                PUT(line);
                sem_post(&manager);
                sem_post(&fillCount); // increments count when allowed
        }
        // insert special code
        sem_wait(&emptyCount); // decrements count when allowed
        sem_wait(&manager);
        PUT("~~\n");
        sem_post(&manager);
        sem_post(&fillCount); // increments count when allowed

        fclose(f_in);
        pthread_exit(NULL);
}

void* consumer(void *outputFile)
{
        FILE *f_out;
        f_out = (FILE*)outputFile;

        while(1) {
                sem_wait(&fillCount);
                sem_wait(&manager);
                char* temp = (char*)malloc(sizeof(char)*LINE_SIZE+2);
                temp = GET();
                printf("string compare: %d\n", strcmp(temp,"~~\n"));
                if (strcmp(temp,"~~\n") != 0) {
                        fprintf(f_out, "%s", temp);
                        fflush(f_out);
                }
                else {
                        break;
                }
                sem_post(&manager);
                sem_post(&emptyCount);

        }
        fclose(f_out);
        pthread_exit(NULL);
}




void filePath()
{
  #ifdef _WIN32
        char in_path[]="C:\\temp\\coursein\\p1-in.txt";
        char out_path[]="C:\\temp\\courseout\\p1-out.txt";
  #else
        char in_path[200], out_path[200];
        const char *homedir;
        homedir = getenv("HOME");
        if (homedir!= NULL)
                homedir = getpwuid(getuid())->pw_dir;
        printf("my home directory is: %s\n", homedir);
        strcpy(in_path,homedir);
        strcpy(out_path,homedir);
        strcat(in_path,"/tmp/coursein/p2-in.txt");
        strcat(out_path,"/tmp/courseout/p2-out.txt");

  #endif
        in_file = fopen(in_path, "r");
        out_file = fopen(out_path, "w");

        if ((in_file == NULL) || (out_file == NULL)) {
                printf("Error...r/ The files were unable to be opened\n");
                exit(1);
        }
}
