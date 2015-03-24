/*****************************************************************************/
/* Josh Lindoo                                                               */
/* Login ID: lind6441                                                        */
/* CS-202, Winter 2015                                                       */
/* Programming Assignment 6                                                  */
/* Tee - Redirectin stdin to stdout                                          */
/*****************************************************************************/

//includes
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <getopt.h>
#include <string.h>

//global options
int enAppend = 0;

/*****************************************************************************/
/* Function: main                                                            */
/* Purpose: handle args, open files, and echo to files/stdout                */
/* Parameters: int argc, char* argv[]                                        */
/* Returns: int                                                              */
/*****************************************************************************/
int main( int argc, char* argv[] )
{
    //handle options/args
    int opt; //option to handle
    while ((opt = getopt(argc, argv, "ai")) != -1) {
        switch (opt) {
            case 'a':
                //append to file
                enAppend = 1;
                break;
            case 'i':
                //ignore SIGINT
                signal(SIGINT, SIG_IGN);
                break;
            default:
                printf("usage: tee [-ai] [file ...]\n");
                exit(EXIT_FAILURE);
        }
    }

    //open files appropriately
    int fileNameIndex = optind; //make copy since incrementing optind has side effects
    FILE** fileArray = (FILE**)malloc((argc - optind) * sizeof(FILE*)); //array to store FILE *'s for output files
    int fileIndex = 0; //index to store fd's
    while(fileNameIndex < argc)
    {
        if(enAppend)
        {
            //open for w/r, creates file if dne, and appends
            fileArray[fileIndex] = fopen(argv[fileNameIndex], "a+");
        }
        else
        {

            fileArray[fileIndex] = fopen(argv[fileNameIndex], "w+");
        }
        
        fileIndex++;
        fileNameIndex++;
    }

    //print to stdout and all files
    char *line = NULL; //line string
    size_t linecap = 0; //used to tell getline to use dynamic size
    ssize_t linelen; //length of line read
    while ((linelen = getline(&line, &linecap, stdin)) > 0)
    {
        printf("%s", line);

        fileNameIndex = optind;
        fileIndex = 0;
        while(fileNameIndex < argc)
        {
            fprintf(fileArray[fileIndex], "%s", line);
            fflush(fileArray[fileIndex]);
            
            fileIndex++;
            fileNameIndex++;
        }
    }

    //close all files and free dynamic memory after EOF
    fileNameIndex = optind;
    fileIndex = 0;
    while(fileNameIndex < argc)
    {
        fclose(fileArray[fileIndex]);
        fileIndex++;
        fileNameIndex++;
    }

    free(fileArray);

    return 0;
}