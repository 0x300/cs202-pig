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
    int fd; //fd of socket

    fd = connect_to_server("127.0.0.1", 50000);

    if(fd == -1)
    {
        perror("Failed to connect to server");
        exit(EXIT_FAILURE);
    }

    char* msg = "test";
    printf("%s\n", msg);
    write(fd, msg, strlen(msg));

    close(fd);

    return 0;
}