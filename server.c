/*****************************************************************************/
/* Josh Lindoo                                                               */
/* Login ID: lind6441                                                        */
/* CS-202, Winter 2015                                                       */
/* Programming Assignment 7                                                  */
/* Pig Server - server for playing the pig game!                             */
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
#include <signal.h>

#define HOSTLEN 256
#define BACKLOG 1
#define PORTNUM 50000

int serverEn = 1; //enable for server loop to allow for clean shutdowns

//function declarations
int make_server_socket_q(int , int );


/*****************************************************************************/
/* Function: make_server_socket                                              */
/* Purpose: call the func to create a socket                                 */
/* Parameters: int portnum                                                   */
/* Returns: int                                                              */
/*****************************************************************************/
int make_server_socket(int portnum) {
    return make_server_socket_q(portnum, BACKLOG);
}


/*****************************************************************************/
/* Function: make_server_socket_q                                            */
/* Purpose: make server socket with port and backlog                         */
/* Parameters: int portnum, int backlog                                      */
/* Returns: int                                                              */
/*****************************************************************************/
int make_server_socket_q(int portnum, int backlog) {
    struct sockaddr_in saddr; //socket address info
    int sock_id; //server socket

    //create server socket
    sock_id = socket(PF_INET, SOCK_STREAM, 0);

    if ( sock_id == -1 ) return -1; 

    bzero((void *)&saddr, sizeof(saddr));

    //socket configuration
    saddr.sin_addr.s_addr = INADDR_ANY;
    saddr.sin_port = htons(portnum);
    saddr.sin_family = AF_INET;

    if ( bind(sock_id, (struct sockaddr *)&saddr, sizeof(saddr)) != 0 ) return -1; 
    if ( listen(sock_id, backlog) != 0 ) return -1; 
    
    return sock_id;
}


/*****************************************************************************/
/* Function: handleUsernames                                                 */
/* Purpose: get usernames and send to opponents                              */
/* Parameters: int fd1, int fd2                                              */
/* Returns: void                                                             */
/*****************************************************************************/
void handleUsernames(int fd1, int fd2)
{
    char username1[256], username2[256], response[256]; // client usernames and response

    // get usernames
    bzero( &username1, 256 );
    recv(fd1, username1, 256, 0);
    printf("username1: %s", username1);

    bzero( &username2, 256 );
    recv(fd2, username2, 256, 0);
    printf("username2: %s\n", username2);
    
    printf("Sending msg -> %s\n", username1);
    write(fd2, username1, strlen(username1));
    printf("Sending msg -> %s\n", username2);
    write(fd1, username2, strlen(username2));

    // wait for client response

    printf("Waiting for responses..\n");

    bzero( &response, 256 );
    recv(fd1, response, 256, 0);
    printf("response: %s\n", response);

    bzero( &response, 256 );
    recv(fd2, response, 256, 0);
    printf("response: %s\n\n", response);
}


/*****************************************************************************/
/* Function: sendRoll                                                        */
/* Purpose: send the roll to the clients                                     */
/* Parameters: int fd, int roll                                              */
/* Returns: int                                                              */
/*****************************************************************************/
int sendRoll(int fd, int roll)
{
    char msg[256], response[256]; // msg to be sent and client response

    bzero( &msg, 256 );
    sprintf(msg, "Roll: %d", roll);

    printf("\n------ New Roll -----\n");
    printf("Sending msg -> %s\n", msg);

    write(fd, msg, strlen(msg));

    // recv returns 0 if disconnected, or -1 if error
    // either essentially means the player is gone in most cases
    bzero( &response, 256 );
    recv(fd, response, 256, 0);
    printf("response: %s\n\n", response);

    return 1;
}


/*****************************************************************************/
/* Function: requestMove                                                     */
/* Purpose: send message to client to get user move                          */
/* Parameters: int fd                                                        */
/* Returns: int                                                              */
/*****************************************************************************/
int requestMove(int fd)
{
    char msg[256], response[256]; // msg to be sent and client response
    
    bzero( &msg, 256 );
    sprintf(msg, "Move");
    printf("\nSending msg -> %s\n", msg);
    write(fd, msg, strlen(msg));
   
    // wait for client to respond with move
    bzero( &response, 256 );
    recv(fd, response, 256, 0);
    printf("response: %s\n", response);

    if(strncmp("1", response, 1) == 0)
    { //roll
        return 1;
    }
    else if(strncmp("2", response, 1) == 0)
    { //hold
        return 2;
    }
    else
    { //rubbish, but this shouldn't happen since this is checked client-side
        //printf("WARN: Invalid move sent: %s\n", response);
        return -1;
    }
}

/*****************************************************************************/
/* Function: sendMoveToOpponent                                              */
/* Purpose: send move to opponent                                            */
/* Parameters: int fd, int move                                              */
/* Returns: int                                                              */
/*****************************************************************************/
int sendMoveToOpponent(int fd, int move)
{
    char msg[256], response[256]; // msg to be sent and client response

    bzero( &msg, 256 );
    if(move == 1)
    {
        sprintf(msg, "Your opponent decided to roll.");
    }
    else
    {
        sprintf(msg, "Your opponent decided to hold.");
    }

    printf("Sending msg -> %s\n", msg);
    write(fd, msg, strlen(msg));

    // recv returns 0 if disconnected, or -1 if error
    // either essentially means the player is gone in most cases
    bzero( &response, 256 );
    recv(fd, response, 256, 0);
    printf("response: %s\n", response);

    return 1;
}


/*****************************************************************************/
/* Function: sendLose                                                        */
/* Purpose: send lose message to loser                                       */
/* Parameters: int fd, int winnerScore, int loserScore                       */
/* Returns: void                                                             */
/*****************************************************************************/
void sendWin(int fd, int winnerScore, int loserScore)
{
    char msg[256]; // msg to be sent
    
    bzero( &msg, 256 );
    sprintf(msg, "You are the winner with a score of %d! Your opponent only had %d points.. nice work!"
               , winnerScore, loserScore);
    printf("Sending msg -> %s\n", msg);
    write(fd, msg, strlen(msg));
}


/*****************************************************************************/
/* Function: sendLose                                                        */
/* Purpose: send lose message to loser                                       */
/* Parameters: int fd, int winnerScore, int loserScore                       */
/* Returns: void                                                             */
/*****************************************************************************/
void sendLose(int fd, int winnerScore, int loserScore)
{
    char msg[256]; // msg to be sent
    
    bzero( &msg, 256 );
    sprintf(msg, "Sorry.. you lost. Try to get more points next time ;)\nYour score: %d, Your opponent's score: %d", loserScore, winnerScore);
    printf("Sending msg -> %s\n", msg);
    write(fd, msg, strlen(msg));
}


/*****************************************************************************/
/* Function: sendTie                                                         */
/* Purpose: send tie message to players                                      */
/* Parameters: int fd1, int fd2, int score                                   */
/* Returns: void                                                             */
/*****************************************************************************/
void sendTie(int fd1, int fd2, int score)
{
    char msg[256]; // msg to be sent
    
    bzero( &msg, 256 );
    sprintf(msg, "Wow! You both tied with a score of %d!! Nice.", score);
    printf("Sending msg -> %s\n", msg);
    write(fd1, msg, strlen(msg));
    write(fd2, msg, strlen(msg));
}


/*****************************************************************************/
/* Function: sendScore                                                       */
/* Purpose: send the score to one of the players                             */
/* Parameters: int fd, int score, int opponentScore                          */
/* Returns: int                                                              */
/*****************************************************************************/
int sendScore(int fd, int score, int opponentScore)
{
    char msg[256]; // msg to be sent
    
    bzero( &msg, 256 );
    sprintf(msg, "Your score: %d, Opponent's score: %d\n\n", score, opponentScore);
    printf("Sending msg -> %s\n", msg);
    write(fd, msg, strlen(msg));
}


/*****************************************************************************/
/* Function: runGame                                                         */
/* Purpose: handle game logic                                                */
/* Parameters: int fd1, int fd2                                              */
/* Returns: void                                                             */
/*****************************************************************************/
void runGame(int fd1, int fd2)
{
    int winning = 100;

    int scoreP1 = 0;
    int scoreP2 = 0;
    int scoreRoundP1 = 0;
    int scoreRoundP2 = 0;
    int isStandingP1 = 1;
    int isStandingP2 = 1;

    // get client usernames and send back to opponent
    handleUsernames(fd1,fd2);

    while(1)
    {
        // send roll to each user
        srand((unsigned) time(NULL));
        int currentRoll = rand() % 6 + 1; // roll for this round


        if( scoreP1 >= winning && scoreP2 >= winning )
        { //tie?

            if(scoreP1 > scoreP2)
            { //P1 wins
                sendWin(fd1, scoreP1, scoreP2);
                sendLose(fd2, scoreP1, scoreP2);
            }
            else if(scoreP2 > scoreP1)
            { //P2 wins
                sendWin(fd2, scoreP2, scoreP1);
                sendLose(fd1, scoreP2, scoreP1);
            }
            else
            { //tie
                sendTie(fd1, fd2, scoreP1);
            }
            exit(EXIT_SUCCESS);
        }
        else if( scoreP1 >= winning)
        { //P1 wins
            sendWin(fd1, scoreP1, scoreP2);
            sendLose(fd2, scoreP1, scoreP2);

            exit(EXIT_SUCCESS);
        }
        else if( scoreP2 >= winning)
        { //P2 wins
            sendWin(fd2, scoreP2, scoreP1);
            sendLose(fd1, scoreP2, scoreP1);

            exit(EXIT_SUCCESS);
        }
        else
        { //continue with round

            // receive user moves
            int moveP1 = 0;
            int moveP2 = 0;

            printf("Waiting for moves..\n");
            if(isStandingP1) moveP1 = requestMove(fd1);
            printf("moveP1 = %d\n", moveP1);

            if(isStandingP2) moveP2 = requestMove(fd2);
            printf("moveP2 = %d\n", moveP2);

            //if player disconnected, give win to the other one
            if(moveP1 == -1) 
            {
                printf("P1 failed to make move, P2 wins\n");
                sendWin(fd2, scoreP2, scoreP1);
                exit(EXIT_SUCCESS);
            }
            if(moveP2 == -1) 
            {
                printf("P2 failed to make move, P1 wins\n");
                sendWin(fd1, scoreP1, scoreP2);
                exit(EXIT_SUCCESS);
            }

            //if player sat.. they aren't standing anymore
            if(moveP1 == 2) isStandingP1 = 0;
            if(moveP2 == 2) isStandingP2 = 0;

            sendMoveToOpponent(fd1, moveP2);
            sendMoveToOpponent(fd2, moveP1);

            printf("\nMoves -> user1: %d, user2: %d\n", moveP1, moveP2);

 
            if( currentRoll == 1 || (!isStandingP1 && !isStandingP2 || 
                scoreP1+scoreRoundP1 > winning || scoreP2+scoreRoundP2 > winning) )
            { //send end of round stuff

                //add score if players already sat down
                if(!isStandingP1) scoreP1 += scoreRoundP1;
                if(!isStandingP2) scoreP2 += scoreRoundP2;

                if(isStandingP1 || isStandingP2)
                { //don't show roll if round ended from two people sitting
                    //send roll
                    if(sendRoll(fd1, currentRoll) == -1)
                    { // error/disconnect
                        sendWin(fd2, scoreP2, scoreP1);
                    }

                    if(sendRoll(fd2, currentRoll) == -1)
                    { // error/disconnect
                        sendWin(fd1, scoreP1, scoreP2);
                    }
                }

                //send scores
                sendScore(fd1, scoreP1, scoreP2);
                sendScore(fd2, scoreP2, scoreP1);

                //reset round scores
                scoreRoundP1 = 0;
                scoreRoundP2 = 0;

                //reset players to standing
                isStandingP1 = 1;
                isStandingP2 = 1;

            }
            else
            {
                //add roll to round scores
                if(isStandingP1) scoreRoundP1 += currentRoll;
                if(isStandingP2) scoreRoundP2 += currentRoll;

                //send roll
                if(sendRoll(fd1, currentRoll) == -1)
                { // error/disconnect
                    sendWin(fd2, scoreP2, scoreP1);
                }

                if(sendRoll(fd2, currentRoll) == -1)
                { // error/disconnect
                    sendWin(fd1, scoreP1, scoreP2);
                }
            }
            
        }
    }

}
/*****************************************************************************/
/* Function: forkGame                                                        */
/* Purpose: fork the process so multiple games can happen                    */
/* Parameters:                                                               */
/* Returns: void                                                             */
/*****************************************************************************/
void forkGame(int fd1, int fd2)
{
    if( fork() == 0) runGame(fd1,fd2);
    //otherwise go back to receiving connections
}


/*****************************************************************************/
/* Function: main                                                            */
/* Purpose: accept client connections and start games                        */
/* Parameters:                                                               */
/* Returns: int                                                              */
/*****************************************************************************/
int main() 
{
    int sock_id; // server socket id
    if ((sock_id = make_server_socket(PORTNUM)) == -1) 
    {
        perror("Error opening socket");
        exit(1);
    }

    while(1) 
    {
        int fd1, fd2; // client sockets

        // accept connections
        fd1 = accept(sock_id, NULL, NULL);
        if ( fd1 == -1) 
        {
            exit(EXIT_FAILURE);
        }
        printf("One client has connected.. waiting for another.\n");

        fd2 = accept(sock_id, NULL, NULL);
        if (fd2 == -1) 
        {
            exit(EXIT_FAILURE);
        }

        // let the games begin!
        printf("Starting new game!\n\n");
        forkGame(fd1, fd2);
    }

    return 0;
} 
