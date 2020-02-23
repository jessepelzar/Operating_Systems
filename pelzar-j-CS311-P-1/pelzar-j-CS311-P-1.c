/* Jesse Pelzar P1*/
#include <stdio.h>
#include <stdlib.h>
#include "time_functions.h"
#include <string.h>
#ifndef _WIN32
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>
#endif

#define LINE_SIZE 1024

clock_t start, end, total;

void charCpy();
void lineCpy();
void filePath();

FILE *in_file;
FILE *out_file;


int main() {

        start_timing();
        printf("Select the mode\n Enter a 0 for character-by-chracter mode\n Enter a 1 for line-by-line mode\n");
        int mode;
        scanf("%d", &mode);

        if (mode == 0) charCpy();
        if (mode == 1) lineCpy();


        stop_timing();
        printf("this program took %f seconds of execution time\n", get_wall_clock_diff());
}


void charCpy() {
        char each_byte;

        // add paths
        filePath();
        start_nanotime();
        //start_timing();
        // scan through the entire document by character
        while ((fscanf(in_file, "%c", &each_byte)) != EOF) {
                fputc(each_byte, out_file);
        }
        stop_timing();

        fclose(in_file);
        fclose(out_file);

        //printf("cpu time is: %f\n", get_CPU_time_diff());
        printf("nano time is: %d\n", get_nanodiff());
        printf("Success char copy\n");
}

void lineCpy() {
        char each_line[81];

        // add paths
        filePath();
        start_nanotime();

        int index = 0;

        while(!(feof(in_file))) {
                char* line = (char*)malloc(sizeof(char)*LINE_SIZE+2);
                fscanf(in_file, " %81c[^\n]\n", line);
                fwrite(line, sizeof(char), sizeof(each_line), out_file);
                printf("%s\n", line);
                each_line[index] = line;
        }
        stop_timing();
        fclose(in_file);
        fclose(out_file);

        //printf("cpu time is: %f\n", get_CPU_time_diff());
        printf("nano time is: %d\n", get_nanodiff());
        printf("Success line copy\n");
}

// file path selection function
void filePath() {
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
        strcat(in_path,"/tmp/coursein/p1-in.txt");
        strcat(out_path,"/tmp/courseout/p1-out.txt");

  #endif
        in_file = fopen(in_path, "r");
        out_file = fopen(out_path, "w");

        if ((in_file == NULL) || (out_file == NULL)) {
                printf("Error...r/ The files were unable to be opened\n");
                exit(1);
        }
}
