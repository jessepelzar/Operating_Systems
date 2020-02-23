#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <math.h>
#include <string.h>
#include "turbine_defines.h"
#ifndef _WIN32
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>
#endif
#include "time_functions.h"

#define HAVE_STRUCT_TIMESPEC
#define LINE_SIZE 32

int M = ROWCOUNT; // rows
int N = COLCOUNT; // cols
double T = TFARM_CYCLE_TIME;

// first 2d array
float current_val[ROWCOUNT][COLCOUNT];
// second 2d array
float max_val[ROWCOUNT][COLCOUNT];
// thrird 2d array
typedef struct turbine {
        int n;
        int m;
} turbine;

pthread_t thread_id[ROWCOUNT][COLCOUNT];

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

int counter = 0;

float average[ROWCOUNT][COLCOUNT];
float temp[ROWCOUNT][COLCOUNT];

float * target_val;
//float target_val[43];
float current_target;

FILE * in_file;
FILE * out_file;

void *generator(void * input);
void output();
int average_func(int i, int j);
int adjust_func(int i, int j);
void filePath();

int main(int argc, char const *argv[]) {

        filePath();
        if (pthread_mutex_init(&mutex, NULL) != 0) printf("mutex failed\n");
        if (pthread_cond_init(&cond, NULL) != 0) printf("cond failed\n");

        int target_size = 1000; int i = 0;
        //char *arr[256];
        char *arr = (char *) malloc(sizeof(char) * 100);

        target_val = (float*) malloc(target_size*sizeof(float));

        int state; int rows_filled = 0; int cols_filled = 0;
        // ---- start of file read -----
        while((fscanf(in_file, "%s", arr)) != EOF) {

                ++target_size;
                //realloc(target_val, sizeof(float) * target_size + 1);

                if(strcmp("\"maxvals\"", arr) == 0)             { state = 0; rows_filled = 0; cols_filled = 0; continue; }
                if(strcmp("\"current_vals\"", arr) == 0)        { state = 1; rows_filled = 0; cols_filled = 0; continue; }
                if(strcmp("\"cycle-target-values\"", arr) == 0) { state = 2; i = 0; continue; }

                switch(state) {
                  case 0: max_val[rows_filled][cols_filled] = atof(arr);
                  case 1: current_val[rows_filled][cols_filled] = atof(arr);
                  case 2: target_val[i] = atof(arr);
                }

                cols_filled++;
                if (cols_filled == COLCOUNT) { cols_filled = 0; rows_filled++; }
                i++;
        }

        fclose(in_file);// ---- end of file read -----

        for (int row = 0; row < M; row++) {
                for (int col = 0; col < N; col++) {

                        printf(" %0.2f | ", current_val[row][col]);
                        fprintf(out_file, "%0.2f", current_val[row][col]);
                        fprintf(out_file, "%s", " ");
                        fflush(out_file);
                }
                printf("\n");
                fprintf(out_file, "%s", "\n");
                fflush(out_file);
        }
        fprintf(out_file, "%s", "**********\n");
        fflush(out_file);


        turbine *gen;
        for (int t = 0; t < i; t++) {
                printf("************\n");
                printf("target val: %f\n", target_val[t]);
                printf("adjusted matrix: \n" );
                //counter = 0;
                for (int row = 0; row < M; row++) {
                        for (int col = 0; col < N; col++) {

                                current_target = target_val[t];
                                gen = malloc(sizeof(struct turbine));
                                (*gen).m = row;
                                (*gen).n = col;

                                pthread_create(&thread_id[row][col], NULL, generator, (void *)gen);
                                pthread_cond_broadcast( &cond );

                        }

                }
                millisleep(10000 * T);
                pthread_mutex_lock( &mutex );

                while(counter < M*N) {
                        pthread_cond_wait( &cond, &mutex );
                }
                output();
                pthread_mutex_unlock( &mutex );


        }
        fclose(out_file);

        printf("Press Any Key to Continue\n");
        getchar();

        return 0;
}

void * generator(void * input) {
        turbine *gen2 = (turbine*)input;
        int row = gen2->m;
        int col = gen2->n;


        average_func(row, col);
        adjust_func(row, col);
        pthread_mutex_lock(&mutex);

        counter++;

        pthread_cond_signal( &cond );
        pthread_mutex_unlock( &mutex );

        return NULL;

}

void output() {

        for (int row = 0; row < M; row++) {
                for (int col = 0; col < N; col++) {
                        printf("%0.2f ", temp[row][col]);

                        fprintf(out_file, "%0.2f", temp[row][col]);
                        fprintf(out_file, "%s", " ");
                        fflush(out_file);
                        current_val[row][col] = temp[row][col];
                }
                fprintf(out_file, "%s", "\n");
                fflush(out_file);
                printf("\n");
        }
        fprintf(out_file, "%s", "**********\n");
        fflush(out_file);

}


int average_func(int current_row, int current_col) {

        int i = current_row;
        int j = current_col;

        float a = current_val[i][j];
        float b = current_val[i+1][j];
        float c = current_val[i-1][j];
        float d = current_val[i][j+1];
        float e = current_val[i][j-1];

        int divisor = 5;
        average[i][j] = (a + b + c + d + e);

        if (current_row == 0) {
          --divisor;
          average[i][j] = ((average[i][j]) - c);
        }
        if (current_row == M-1) {
          --divisor;
          average[i][j] = ((average[i][j]) - b);
        }
        if (current_col == 0) {
          --divisor;
          average[i][j] = ((average[i][j]) - e);
        }
        if (current_col == N-1) {
          --divisor;
          average[i][j] = ((average[i][j]) - d);
        }
        else {
          average[i][j] = average[i][j];
        }
        average[i][j] /= divisor;

        return 0;
}

int adjust_func(int i, int j) {
  if (average[i][j] < current_target) {
          temp[i][j] = (current_val[i][j] * 0.3) + average[i][j];
  }
  else if (average[i][j] >= current_target) {
          temp[i][j] = -(current_val[i][j] * 0.3) + average[i][j];
  }
  else {
          temp[i][j] = current_target;
  }
  if (temp[i][j] < 0) {
          temp[i][j] = 0;
  }
  if (temp[i][j] > max_val[i][j]) {
          temp[i][j] = max_val[i][j];
  }
  return 0;
}

void filePath()
{
  #ifdef _WIN32
        char in_path[]="C:\\temp\\coursein\\p3-in.txt";
        char out_path[]="C:\\temp\\courseout\\p3-out.txt";
  #else
        char in_path[200], out_path[200];
        const char *homedir;
        homedir = getenv("HOME");
        if (homedir!= NULL)
                homedir = getpwuid(getuid())->pw_dir;
        printf("my home directory is: %s\n", homedir);
        strcpy(in_path,homedir);
        strcpy(out_path,homedir);
        strcat(in_path,"/tmp/coursein/p3-in.txt");
        strcat(out_path,"/tmp/courseout/p3-out.txt");

  #endif
        in_file = fopen(in_path, "r");
        out_file = fopen(out_path, "w");

        if ((in_file == NULL) || (out_file == NULL)) {
                printf("Error...r/ The files were unable to be opened\n");
                exit(1);
        }
}
