#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include "headers.h"

#define MAX_BUFFER_SIZE 4096

int iman(char* man_page)
{

    
    const char *host = "man.he.net";
    char path[1024];
    snprintf(path, sizeof(path),"?topic=%s&section=all",man_page);
    //  DNS resolution for man.he.net
    struct addrinfo hints, *res;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET; // IPv4
    hints.ai_socktype = SOCK_STREAM;

    if (getaddrinfo(host, "http", &hints, &res) != 0)
    {
        perror("getaddrinfo");
        return 1;
    }

    //  open a TCP socket to the IP address
    int sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (sockfd == -1)
    {
        perror("socket");
        return 1;
    }

    //  connect to the website's server
    if (connect(sockfd, res->ai_addr, res->ai_addrlen) == -1)
    {
        perror("connect");
        return 1;
    }

    // send a GET request to the website's server
    char request[4096];

    snprintf(request, sizeof(request), "GET %s HTTP/1.1\r\nHost: %s\r\n\r\n", path, host);
    if (send(sockfd, request, strlen(request), 0) == -1)
    {
        perror("send");
        return 1;
    }

    // read the body of the website and print it to stdout
    char buffer[MAX_BUFFER_SIZE];
    int bytes_received;
    int headers_end = 0; // Flag to indicate the end of headers

    // while ((bytes_received = recv(sockfd, buffer, sizeof(buffer) - 1, 0)) > 0)
    // {
    //     buffer[bytes_received] = '\0';
        
    // } //this gets last 4096 bytes but we need the first 4096 bytes
        // Buffer to hold received data (including space for null-termination)
    size_t total_bytes_received = 0; // Total bytes received

    // Assume you have initialized the sockfd and connected it to a remote server

    while (total_bytes_received < 4096) {
        // Calculate how many bytes to read in this iteration
        size_t bytes_to_receive = sizeof(buffer) - 1 - total_bytes_received;
        
        ssize_t bytes_received = recv(sockfd, buffer + total_bytes_received, bytes_to_receive, 0);

        if (bytes_received < 0) {
            perror("recv");
            break; // Error occurred, break out of the loop
        } else if (bytes_received == 0) {
            // The remote end closed the connection
            break;
        } else {
            // Increment the total bytes received
            total_bytes_received += bytes_received;
            buffer[total_bytes_received] = '\0'; // Null-terminate the received data
        }
    }
    // printf("%s",buffer);
    
     int in_section = 0; // Flag to indicate whether we are inside a relevant section
    int in_pre = 0;     // Flag to indicate whether we are inside a <PRE> block

    char *line = strtok((char *)buffer, "\n");

    while (line != NULL) {
        // Check if we are inside a <PRE> block
        if (strstr(line, "<PRE>") != NULL) {
            in_pre = 1;
        } else if (strstr(line, "</PRE>") != NULL) {
            in_pre = 0;
        }

        // Check if we are inside a relevant section
        if (in_pre) {
            if (strstr(line, "NAME") != NULL ||
                strstr(line, "SYNOPSIS") != NULL ||
                strstr(line, "DESCRIPTION") != NULL) {
                in_section = 1;
                printf("%s\n", line);
            } else if (in_section) {
                // Print lines within the relevant section
                printf("%s\n", line);
            }
        }

        // Check if we are at the end of a relevant section
        // if (in_section && strlen(line) == 0) {
        //     in_section = 0; // Reset the flag when we encounter an empty line
        // }

        line = strtok(NULL, "\n");
    }

    if (bytes_received == -1)
    {
        perror("recv");
        return 1;
    }

    // close the socket
    close(sockfd);

    // cleanup
    freeaddrinfo(res);

    return 0;
}
