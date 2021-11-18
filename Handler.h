//
// Created by Yuriy Mikhaildi on 10/20/21.
//

// MIME Types
//.html text/html
//.txt  text/plain
//.png  image/png
//.gif  image/gif
//.jpg  image/jpg
//.css  text/css
//.js   application/javascript


#ifndef HANDLER_H
#define HANDLER_H

#include <stddef.h>
#include <stdio.h>
#include <strings.h>

#define MAXLINE  8192  /* m ax text line length */
#define MAXBUF   8192  /* max I/O buffer size */
#define LISTENQ  1024  /* second argument to listen() */
#define REQLINE  1024  /* req param string size */

/* request handle */
typedef struct {
    char r_method[REQLINE];
    char r_uri[REQLINE];
    char r_version[REQLINE];
} Request;

/* handle the request,
 * takes parsed input, handles write file to buf */
void handle_request(Request *, int);

/* handle the response,
 * takes file buffer, handles response packet w/HTTP header */
void handle_response(long, int, Request *, FILE*);

/*error wrapper */
void handle_error(int);


/*helper to find the fyle type*/
int compare_file_type(char*, char*);


#endif //HANDLER_H
