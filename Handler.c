//
// Created by Yuriy Mikhaildi on 10/20/21.
//
#include "Handler.h"
#include <string.h>
#include <stddef.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/file.h>
#include <sys/stat.h>


/* handle the error */
void handle_error(int connfd){
    char error_buf[MAXLINE];
    printf("server returning a http message with the following content."
           "\n\"HTTP/1.1 500 Internal Server Error\"\n");
    strcpy(error_buf, "HTTP/1.1 500 Internal Server Error");
    write(connfd, error_buf, strlen(error_buf));
}
int compare_file_type(char* name, char* type){
    int file_name_len, type_str_len;
    file_name_len = strlen(name);
    type_str_len = strlen(type);

    if(type_str_len > file_name_len)
        return 0;
    /* loop from end to check the file type against name */
    for(int i = file_name_len - 1, j = type_str_len - 1; j >= 0; --i, --j)
        if(name[i] != type[j])
            return 0;

    return 1;
}
/* file size method is base of https://stackoverflow.com/questions/6537436/how-do-you-get-file-size-by-fd */
void handle_request(Request* req, int connfd){
    /*local*/
    long file_size;
    struct stat file_stats;

    /* file path */
    const char* file_path = req->r_uri;

    /*get file stats*/
    if(stat(file_path, &file_stats) < 0)
        handle_error( connfd);

    /*record file size */
    file_size = file_stats.st_size;


    FILE* file = fopen(file_path, "rb");

    if(file == NULL) {
        handle_error(connfd);
        return;
    }

    handle_response(file_size, connfd, req, file);
}

void handle_response(long file_size, int connfd, Request* request, FILE* file){
        /* allocate packet memory */
        char header[MAXLINE];
        char payload[file_size];
        char header_str[MAXLINE];

        /*get file payload */
        long read_bytes = fread(payload, 1, file_size, file);

        printf("server is handeling response\n");


        /* check the type and create header*/
        if(compare_file_type(request->r_uri, "html"))
            sprintf(header,"%s 200 Document Follows\r\nContent-Type:text/html\r\nContent-Length: %ld\r\n\r\n",
                    request->r_version, file_size);
        else if(compare_file_type(request->r_uri, "txt"))
            sprintf(header, "%s 200 Document Follows\r\nContent-Type:text/plain\r\nContent-Length: %ld\r\n\r\n",
                    request->r_version, file_size);
        else if(compare_file_type(request->r_uri, "png"))
            sprintf(header, "%s 200 Document Follows\r\nContent-Type:image/png\r\nContent-Length: %ld\r\n\r\n",
                    request->r_version, file_size);
        else if(compare_file_type(request->r_uri, "gif"))
            sprintf(header, "%s 200 Document Follows\r\nContent-Type:image/gif\r\nContent-Length: %ld\r\n\r\n",
                    request->r_version, file_size);
        else if(compare_file_type(request->r_uri, "jpg"))
            sprintf(header, "%s 200 Document Follows\r\nContent-Type:image/jpg\r\nContent-Length: %ld \r\n\r\n",
                    request->r_version, file_size);
        else if(compare_file_type(request->r_uri, "ico"))
            sprintf(header, "%s 200 Document Follows\r\nContent-Type:image/x-icon\r\nContent-Length: %ld \r\n\r\n",
                    request->r_version, file_size);
        else if(compare_file_type(request->r_uri, "css"))
            sprintf(header, "%s 200 Document Follows\r\nContent-Type:text/css\r\nContent-Length: %ld\r\n\r\n",
                    request->r_version, file_size);
        else if(compare_file_type(request->r_uri, "js"))
            sprintf(header, "%s 200 Document Follows\r\nContent-Type:application/javascript\r\nContent-Length: %ld\r\n\r\n",
                    request->r_version, file_size);
        else
            handle_error(connfd);
        
        /* copy header in */
        long max_packet = strlen(header) + file_size;
        char packet [max_packet];
        strcpy(packet, header);

        /*copy payload */
        memcpy(packet + strlen(header), payload, file_size);

        /*print ACK */
        printf("server returning a http message with the following content.\n%s\n", packet);

        /* send response */
        long write_bytes = write(connfd, packet, max_packet );

        bzero(payload, file_size);
        bzero(header_str, MAXLINE);
        fclose(file);
}