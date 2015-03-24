/*****************************************************************************/
/* Josh Lindoo                                                               */
/* Login ID: lind6441                                                        */
/* CS-202, Winter 2015                                                       */
/* Programming Assignment 6                                                  */
/* Tee - Redirectin stdin to stdout                                          */
/*****************************************************************************/

/* webserv.c - a minimal web server (version 0.2)
 *      usage: ws portnumber
 *   features: supports the GET command only
 *             runs in the current directory
 *             forks a new child to handle each request
 *             has MAJOR security holes, for demo purposes only
 *             has many other weaknesses, but is a good start
 *      build: cc webserv.c socklib.c -o webserv
 */
#include    <stdio.h>
#include    <sys/types.h>
#include    <sys/stat.h>
#include    <string.h>

#define DEFAULT_PORT 50000

void process_rq( char *rq, int fd );

void main(int ac, char *av[])
{
    int     sock, fd;
    FILE    *fpin;
    char    request[BUFSIZ];

    sock = make_server_socket( DEFAULT_PORT );
    
    if ( sock == -1 )
    {
        printf("Failed to create server socket.. exiting\n");
        exit(2); //socket allocation failed
    }
    else
    {
        printf("Listening on port: %d\n", DEFAULT_PORT);
    }

    /* main loop here */

    int fd_client_1;
    int fd_client_2;
    while(1){
        /* accept two connections */
        printf("Waiting for clients to connect..\n");
        fd_client_1 = accept( sock, NULL, NULL );
        printf("Accepted 1..\n");
        fd_client_2 = accept( sock, NULL, NULL );
        printf("Accepted 2..\n");

        if(fd_client_1 == -1 || fd_client_2 == -1)
        {
            //error
            printf("Error accepting connections..\n");
            return;
        }


        /* do what client asks */
        process_rq(request, fd);

        printf("Request processed..\n");

        fclose(fpin);
    }
}

/* ------------------------------------------------------ *
   process_rq( char *rq, int fd )
   do what the request asks for and write reply to fd 
   handles request in a new process
   rq is HTTP command:  GET /foo/bar.html HTTP/1.0
   ------------------------------------------------------ */

void process_rq( char *rq, int fd )
{
    /* create a new process and return if not the child */
    if ( fork() != 0 )
        return;

    printf("I'm a child!\n");
    // initGame();
}

/* ------------------------------------------------------ *
   the reply header thing: all functions need one
   if content_type is NULL then don't send content type
   ------------------------------------------------------ */

header( FILE *fp, char *content_type )
{
    fprintf(fp, "HTTP/1.0 200 OK\r\n");
    if ( content_type )
        fprintf(fp, "Content-type: %s\r\n", content_type );
}

/* ------------------------------------------------------ *
   simple functions first:
        cannot_do(fd)       unimplemented HTTP command
    and do_404(item,fd)     no such object
   ------------------------------------------------------ */

cannot_do(int fd)
{
    FILE    *fp = fdopen(fd,"w");

    fprintf(fp, "HTTP/1.0 501 Not Implemented\r\n");
    fprintf(fp, "Content-type: text/plain\r\n");
    fprintf(fp, "\r\n");

    fprintf(fp, "That command is not yet implemented\r\n");
    fclose(fp);
}

do_404(char *item, int fd)
{
    FILE    *fp = fdopen(fd,"w");

    fprintf(fp, "HTTP/1.0 404 Not Found\r\n");
    fprintf(fp, "Content-type: text/plain\r\n");
    fprintf(fp, "\r\n");

    fprintf(fp, "The item you requested: %s\r\nis not found\r\n", 
            item);
    fclose(fp);
}

/* ------------------------------------------------------ *
   the directory listing section
   isadir() uses stat, not_exist() uses stat
   do_ls runs ls. It should not
   ------------------------------------------------------ */

isadir(char *f)
{
    struct stat info;
    return ( stat(f, &info) != -1 && S_ISDIR(info.st_mode) );
}

not_exist(char *f)
{
    struct stat info;
    return( stat(f,&info) == -1 );
}

do_ls(char *dir, int fd)
{
    FILE    *fp ;

    fp = fdopen(fd,"w");
    header(fp, "text/plain");
    fprintf(fp,"\r\n");
    fflush(fp);

    dup2(fd,1);
    dup2(fd,2);
    close(fd);
    execlp("ls","ls","-l",dir,NULL);
    perror(dir);
    exit(1);
}

/* ------------------------------------------------------ *
   the cgi stuff.  function to check extension and
   one to run the program.
   ------------------------------------------------------ */

char * file_type(char *f)
/* returns 'extension' of file */
{
    char    *cp;
    if ( (cp = strrchr(f, '.' )) != NULL )
        return cp+1;
    return "";
}

ends_in_cgi(char *f)
{
    return ( strcmp( file_type(f), "cgi" ) == 0 );
}

do_exec( char *prog, int fd )
{
    FILE    *fp ;

    fp = fdopen(fd,"w");
    header(fp, NULL);
    fflush(fp);
    dup2(fd, 1);
    dup2(fd, 2);
    close(fd);
    execl(prog,prog,NULL);
    perror(prog);
}
/* ------------------------------------------------------ *
   do_cat(filename,fd)
   sends back contents after a header
   ------------------------------------------------------ */

do_cat(char *f, int fd)
{
    char    *extension = file_type(f);
    char    *content = "text/plain";
    FILE    *fpsock, *fpfile;
    int c;

    if ( strcmp(extension,"html") == 0 )
        content = "text/html";
    else if ( strcmp(extension, "gif") == 0 )
        content = "image/gif";
    else if ( strcmp(extension, "jpg") == 0 )
        content = "image/jpeg";
    else if ( strcmp(extension, "jpeg") == 0 )
        content = "image/jpeg";

    fpsock = fdopen(fd, "w");
    fpfile = fopen( f , "r");
    if ( fpsock != NULL && fpfile != NULL )
    {
        header( fpsock, content );
        fprintf(fpsock, "\r\n");
        while( (c = getc(fpfile) ) != EOF )
            putc(c, fpsock);
        fclose(fpfile);
        fclose(fpsock);
    }
    exit(0);
}