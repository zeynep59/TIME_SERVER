// Necessary libraries
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <stdbool.h>

// the port number on which your server will listen for incoming connections
#define PORT_NUMBER 60000
// maximum number of pending connections that our server can handle
#define MAX_PENDING 5

int main(int argc, char *argv[])
{
    // the sockfd (short for socket descriptor) is a integer value that is used to identify a specific socket.
    int sockfd;
    // to store the address of a socket and  to bind the server socket to a specific port and address, we can use the bind() function and pass
    //  the sockfd, server_addr, and sizeof(server_addr) as arguments.
    struct sockaddr_in server_addr, client_addr;
    // to store the size of the sockaddr_in structure.
    socklen_t sin_size;

    // create a socket             (type of socket)
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    // AF_INET (short for Address Family INET) is a constant value that is used to specify the address family for an IPv4 socket
    { // send error if socket can not be created
        perror("Error creating socket");
        // and exit with error code 1
        exit(1);
    }

    // set the socket options
    int opt = 1;
    // setsockopt() function is used to set the options for a socket. It allows you to customize the behavior of the socket
    // by setting various options that control how the socket behaves
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int)) == -1)
    { // SOL_SOCKET for socket-level options
        // The SO_REUSEADDR option allows the socket to bind to a port that is already in use by another socket
        // send error if socket options can not be set
        perror("Error setting socket options");
        //  exit with error code 1
        exit(1);
    }

    // bind the socket to a specific port

    // to store the address of a socket and to bind the server socket to a specific port and address, we can use the bind() function and pass
    // the sockfd, server_addr, and sizeof(server_addr) as arguments.
    server_addr.sin_family = AF_INET; // port family
    // The htons() function helps you do this by converting a 16-bit integer value from the host byte order to the network byte order.
    server_addr.sin_port = htons(PORT_NUMBER); // declare port number
    // INADDR_ANY is a constant value that is used to specify the wildcard IP address. In socket programming, the wildcard address is a special address that is used to bind a socket to all available interfaces
    server_addr.sin_addr.s_addr = INADDR_ANY;
    // The memset() function sets the first count bytes of dest to the value c. The value of c is converted to an unsigned character.
    // The memset() function returns a pointer to dest.
    memset(server_addr.sin_zero, '\0', sizeof(server_addr.sin_zero));
    // bind function comes from arpa/inet.h library.
    // it is used to specify the address and port number that a socket will use to receive incoming connections or to send outgoing data.
    if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) == -1)
    {
        // throw error if socket could not be binded.
        perror("Error binding socket");
        // and exit with error code 1
        exit(1);
    }

    // listen for incoming connections
    if (listen(sockfd, MAX_PENDING) == -1)
    {       
        //if there is an error while listening the incoming connections 
        perror("Error listening for incoming connections");
        // exit with error code 1
        exit(1);
    }
    //resizing so the code can use this variable to accept connection from client
    sin_size = sizeof(struct sockaddr_in);
    // accept a connection from a client
    int clientfd = accept(sockfd, (struct sockaddr *)&client_addr, &sin_size);
    // if CLOSE_SERVER is written on telnet terminal this flag will be turn true
    bool close_flag = false;

    while (1)
    {
        // accept a connection from a client

        if (clientfd == -1)
        {
            //connection is faulty
            perror("Error accepting connection\n\n");
            //retry using continue
            continue;
        }

        // receive data from the client
        char request[1024];
        // When a connection is accepted, we can use the recv() function to receive data from the client
        int bytes_received = recv(clientfd, request, 1024, 0);
        // null-terminate the request string
        request[bytes_received] = '\0'; 

        if (bytes_received == -1)
        {
            //if the recieved data's size is less than 0
            perror("Error receiving data from client\n\n");
            //close the program
            close(clientfd);
            //break the loop
            continue;
        }

        // process the request and generate a response
        char response[1024];
        // according the telnet command the response will be send on telnet terminal
        if (strncmp(request, "GET_TIME_ZONE", 13) == 0)
        {
            // get the current time zone
            time_t current_time = time(NULL);
            //creating  a struct for the current time
            //and pulling the current time zone
            struct tm *local_time = localtime(&current_time);
            // get time zone from the struct and send it to response
            strftime(response, 1024, "%z\n", local_time);
        }
        else if (strncmp(request, "GET_DATE", 8) == 0)
        {
            // get the current date
            time_t current_time = time(NULL);
            //creating  a struct for the current time
            //and pulling date of the current day, month and year
            struct tm *local_time = localtime(&current_time);
            // formatting time and assign to response
            strftime(response, 1024, "%d.%m.%Y\n", local_time);
        }
        else if (strncmp(request, "GET_TIME_DATE", 13) == 0)
        {
            // get the current time and date
            time_t current_time = time(NULL);
            //creating  a struct for the current time
            //and pulling the current date and time
            struct tm *local_time = localtime(&current_time);
            // formatting time and assign to response
            strftime(response, 1024, "%H:%M:%S, %d.%m.%Y\n", local_time);
        }

        else if (strncmp(request, "GET_DAY_OF_WEEK", 15) == 0)
        {
            // get the current day of the week
            time_t current_time = time(NULL);
            //creating  a struct for the current time
            //and pulling the week information
            struct tm *local_time = localtime(&current_time);
            strftime(response, 1024, "%A\n", local_time);
        }
        else if (strncmp(request, "GET_TIME", 8) == 0)
        {
            // get the current time
            time_t current_time = time(NULL);
            //creating  a struct for the current time
            //while pulling the current hour,minute and second.
            struct tm *local_time = localtime(&current_time);
            // formatting time and assign to response
            strftime(response, 1024, "%H:%M:%S\n", local_time);
        }
        else if (strncmp(request, "CLOSE_SERVER", 12) == 0)
        {
            //starting to close the system
            strcpy(response, "GOOD BYE\n");
            //confirming the closing boolean
            close_flag = true;
        }

        else
        {
            //if the entered request isn't applicable to the projects format
            //inform the user
            strcpy(response, "INCORRECT REQUEST\n");
        }

        // send the response back to the client
        // When a connection is accepted, you can use the send() function to send data back to the client:
        int bytes_sent = send(clientfd, response, strlen(response), 0);
        // if CLOSE_SERVER request is taken from telnet then the connection will be closed.
        if (close_flag)
        {
            //if the user enter the closing request
            //close the project
            close(clientfd);
            //exit the loop
            break;
        }

        if (bytes_sent == -1)
        {
            //if the program isn't able to fabricate responce
            //send an error
            perror("Error sending data to client");
        }

        // close the connection with the client
    }
}
