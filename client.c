/*****************************************************************************/
/* Josh Lindoo                                                               */
/* Login ID: lind6441                                                        */
/* CS-202, Winter 2015                                                       */
/* Programming Assignment 7                                                  */
/* Pig Client - client for playing the pig game!                             */
/*****************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <time.h>
#include <strings.h>

#define HOSTLEN 256
#define BACKLOG 1
#define PORTNUM 50000

/*****************************************************************************/
/* Function: connect_to_server                                               */
/* Purpose: setup socket to connect to server and connect                    */
/* Parameters: char *host, int portnum                                       */
/* Returns: int                                                              */
/*****************************************************************************/
int connect_to_server(char *host, int portnum) {
    int sock_id;
    struct sockaddr_in servadd;
    struct hostent *hp;

    sock_id = socket( AF_INET, SOCK_STREAM, 0 );
    if ( sock_id == -1 ) return -1; 

    bzero( &servadd, sizeof(servadd) );
    hp = gethostbyname( host );
    if (hp == NULL) return -1; 

    bcopy(hp->h_addr, (struct sockaddr *)&servadd.sin_addr, hp->h_length);
    servadd.sin_port = htons(portnum);
    servadd.sin_family = AF_INET;
    if ( connect(sock_id,(struct sockaddr *)&servadd, sizeof(servadd)) !=0) return -1; 
    
    return sock_id;
}


/*****************************************************************************/
/* Function: main                                                            */
/* Purpose: connect to server and handle server commands                     */
/* Parameters:                                                               */
/* Returns: int                                                              */
/*****************************************************************************/
int main() 
{
    int fd; //descriptor of server connection

    fd = connect_to_server("127.0.0.1", PORTNUM);
    if(fd == -1) 
    {
        perror("connect_to_server");
        printf("Make sure server is running..\n");
        exit(EXIT_FAILURE);
    }

    char buffer[256]; //buffer for msg transmission
    bzero( &buffer, 256 );

    // send username
    char username[265]; //username buffer
    bzero( &username, 256 );
    printf("Please enter a username to begin:\n");
    fgets(username, 256, stdin);
    write(fd, username, strlen(username));

    //receive opponent name
    recv(fd, buffer, sizeof(buffer), 0);
    write(fd, "ok", 2);
    printf("\nOpponent's Name: %s\nLet's get started!\n", buffer);

    //main game loop
    int gameOn = 1; //game is going
    while(gameOn)
    {
        //receive msg
        bzero( &buffer, 256);
        recv(fd, buffer, sizeof(buffer), 0);

        if(strncmp("You are the winner", buffer, strlen("You are the winner")) == 0)
        {
            printf("%s\n", buffer);
            gameOn = 0;
        }
        else if(strncmp("Sorry.. you lost.", buffer, strlen("Sorry.. you lost.")) == 0)
        {
            printf("%s\n", buffer);
            gameOn = 0;
        }
        else if(strncmp("Wow! You both tied.", buffer, strlen("Wow! You both tied")) == 0)
        {
            printf("%s\n", buffer);
            gameOn = 0;
        }
        else if(strncmp("Roll", buffer, 4) == 0)
        {
            //display roll
            printf("%s\n\n", buffer);

            //respond after processing
            write(fd, "ok", 2);
        }
        else if(strncmp("Your opponent", buffer, strlen("Your opponent")) == 0)
        {
            printf("%s\n", buffer);
            write(fd, "ok", 2);
        }
        else if(strncmp("Move", buffer, 4) == 0)
        {
            char move[256]; //move buffer
            int noMove = 1; //flag for rejecting input until valid input is given

            while(noMove)
            {
                printf("Enter your move [Roll (1), Hold (2)]:\n"); 
                bzero( &move, 256 );
                fgets(move, 256, stdin);
                
                if(strncmp("1", move, 1) == 0)
                {
                    write(fd, "1", 1);
                    printf("\nNice choice! Your move has been sent. Waiting on other player..\n\n");
                    noMove = 0;
                }
                else if(strncmp("2", move, 1) == 0)
                {
                    write(fd, "2", 1);
                    printf("\nNice choice! Your move has been sent. Waiting on other player..\n\n");
                    noMove = 0;
                }
                else
                {
                    printf("\nC'mon.. you didn't think I'd take THAT for an answer did you?\n\n");
                }
            }
        }
        else
        { //catch-all for messages that don't require client action other than printing
            printf("%s\n", buffer);
        }


    }

    close(fd);

    return 0;
} 
