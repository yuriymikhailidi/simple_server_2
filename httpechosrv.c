/* 
 * tcpechosrv.c - A concurrent TCP echo server using threads
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>      /* for fgets */
#include <strings.h>     /* for bzero, bcopy */
#include <unistd.h>      /* for read, write */
#include <sys/socket.h>  /* for socket use */
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <dirent.h>
#include <fcntl.h>
#include "Handler.h"

#define MAXLINE  8192  /* max text line length */
#define MAXBUF   8192  /* max I/O buffer size */
#define LISTENQ  1024  /* second argument to listen() */

int open_listenfd(int port);
void echo(int connfd);
void *thread(void *vargp);

int main(int argc, char **argv) 
{
    int listenfd, *connfdp, port, clientlen=sizeof(struct sockaddr_in);
    struct sockaddr_in clientaddr;
    pthread_t tid; 

    if (argc != 2) {
	fprintf(stderr, "usage: %s <port>\n", argv[0]);
	exit(0);
    }
    port = atoi(argv[1]);

    listenfd = open_listenfd(port);
    while (1) {
	connfdp = malloc(sizeof(int));
	*connfdp = accept(listenfd, (struct sockaddr*)&clientaddr, &clientlen);
	pthread_create(&tid, NULL, thread, connfdp);
    }
}

/* thread routine */
void * thread(void * vargp) 
{  
    int connfd = *((int *)vargp);
    pthread_detach(pthread_self()); 
    free(vargp);
    echo(connfd);
    close(connfd);
    return NULL;
}

/*
 * echo - read and echo text lines until client closes connection
 */
/*
 * parser function, based of https://codereview.stackexchange.com/questions/245454/parse-string-into-a-struct,
 *      parser function breaks the buffer to array of strings, the strings are put in the struct, the request info
 *      command, url (root file path), http version.
 */
int check_path(char* file_path){
    printf("server checking path: %s\n", file_path);
    /* ../www/ */
    char* www = "/www/";
    char* ret = strstr(file_path, www);
    if(ret){
        return 1;
    } else {
        return -1;
    }
}
void echo(int connfd)
{
    size_t n;
    char buffer[MAXLINE];
    n = read(connfd, buffer, MAXLINE);

    printf("server received the following request:\n%s\n",buffer);

    /*parse the request */
    char* request_type = strtok(buffer," ");
    char* file_name = strtok(NULL, " ");
    char* http_version = strtok(NULL, "\r");

    /* basic vars */
    Request *req = malloc(sizeof (Request));
    char* req_GET = "GET";
    // Parse GET command
    // Validate key length
    if (strlen(file_name) == 0) {
        handle_error( connfd);
        return;
    }
    if (!strcmp(request_type, req_GET)) {
//        /* create file path, allocate the buf */
        char full_path[MAXLINE];
        char file_root_path[MAXLINE];
        char* root_one = "./www";
        char* root_two = ".";

        /*set up default path */
        if(!strcmp(file_name, "/"))
            strcat(file_name, "index.html");

        /* check if we have a valid file path */
        if(check_path(file_name) < 0){
            /* needs mod */;
            strcpy(file_root_path, root_one);
        } else {
        /* no mod add root */
            strcpy(file_root_path, root_two);
        }

        /* sets up full the root path */
        strcpy((char *) full_path, file_root_path);
        strcat((char *) full_path, file_name);
        printf("server found root. %s\n", full_path);

        /*construct the request struct */
        strcpy(req->r_method, request_type);
        strcpy(req->r_uri, full_path);
        strcpy(req->r_version, http_version);

        handle_request(req, connfd);

    } else {
        handle_error( connfd);
        return;
    }
    free(req);
}

/* 
 * open_listenfd - open and return a listening socket on port
 * Returns -1 in case of failure 
 */
int open_listenfd(int port) 
{
    int listenfd, optval=1;
    struct sockaddr_in serveraddr;
  
    /* Create a socket descriptor */
    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        return -1;

    /* Eliminates "Address already in use" error from bind. */
    if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, 
                   (const void *)&optval , sizeof(int)) < 0)
        return -1;

    /* listenfd will be an endpoint for all requests to port
       on any IP address for this host */
    bzero((char *) &serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET; 
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY); 
    serveraddr.sin_port = htons((unsigned short)port); 
    if (bind(listenfd, (struct sockaddr*)&serveraddr, sizeof(serveraddr)) < 0)
        return -1;

    /* Make it a listening socket ready to accept connection requests */
    if (listen(listenfd, LISTENQ) < 0)
        return -1;
    return listenfd;
} /* end open_listenfd */
