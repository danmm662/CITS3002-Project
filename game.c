#include "game.h"

int numLives = 3;

/**
* Based on code found at https://github.com/mafintosh/echo-servers.c (Copyright (c) 2014 Mathias Buus)
* Copyright 2019 Nicholas Pritchard, Ryan Bunney
**/

/**
 * @brief A simple example of a network server written in C implementing a basic echo
 * 
 * This is a good starting point for observing C-based network code but is by no means complete.
 * We encourage you to use this as a starting point for your project if you're not sure where to start.
 * Food for thought:
 *   - Can we wrap the action of sending ALL of out data and receiving ALL of the data?
 */

void message_pass(int client) {
    //Set up buffer to send string
    char * messbuf;
    messbuf = calloc(BUFFER_SIZE, sizeof(char));
    sprintf(messbuf, "%d,PASS\n", playerCode);
    int err = send(client, messbuf, strlen(messbuf), 0);
    //Check to see if message was sent
    if(err < 0) {
        fprintf(stderr, "PASS message failed to send");
        exit(EXIT_FAILURE);
    }
    free(messbuf);
}    

void message_fail(int client) {
    //Set up buffer to send string
    char * messbuf;
    messbuf = calloc(BUFFER_SIZE, sizeof(char));
    sprintf(messbuf, "%d,FAIL\n", playerCode);
    int err = send(client, messbuf, strlen(messbuf), 0);
    //Check to see if message was sent
    if(err < 0) {
        fprintf(stderr, "FAIL message failed to send");
        exit(EXIT_FAILURE);
    }
    free(messbuf);
}

void message_welcome(int client) {
    //Set up buffer to send string
    char * messbuf;
    messbuf = calloc(BUFFER_SIZE, sizeof(char));
    sprintf(messbuf, "%d,WELCOME\n", playerCode);
    int err = send(client, messbuf, strlen(messbuf), 0);
    //Check to see if message was sent
    if(err < 0) {
        fprintf(stderr, "WELCOME message failed to send");
        exit(EXIT_FAILURE);
    }
    free(messbuf);
}

void message_elim(int client) {
    //Set up buffer to send string
    char * messbuf;
    messbuf = calloc(BUFFER_SIZE, sizeof(char));
    sprintf(messbuf, "%d,ELIM\n", playerCode);
    int err = send(client, messbuf, strlen(messbuf), 0);
    //Check to see if message was sent
    if(err < 0) {
        fprintf(stderr, "ELIM message failed to send");
        exit(EXIT_FAILURE);
    }
    free(messbuf);
}

void handleClient(int client_fd){
    char *buf;
    int err;
    buf = calloc(BUFFER_SIZE, sizeof(char)); // Clear our buffer so we don't accidentally send/print garbage
    int read = recv(client_fd, buf, BUFFER_SIZE, 0); // Try to read from the incoming client, expecting INIT

    if (read <= 0 && errno == EAGAIN) {         //Timeouts for INIT message, not sure if necessary
        fprintf(stderr, "Client timed out\n");
        exit(EXIT_FAILURE);
    }
    else if (read <= 0){
        fprintf(stderr, "Client read failed\n");
        message_elim(client_fd);
        exit(EXIT_FAILURE);
    }

    printf("Client's message: %s\n", buf);

    struct messageProperties p;
    p = parse_message(buf);

    if(p.flag == INIT) {
        message_welcome(client_fd);
    } else {
        fprintf(stderr, "INIT was not the first message received\n");
        exit(EXIT_FAILURE);
    }

    memset(buf, 0, BUFFER_SIZE);
    sprintf(buf, "Let the games begin!\n");
    err = send(client_fd, buf, strlen(buf), 0);
    if(err < 0) {
        fprintf(stderr, "Failed to send beginning message");
        exit(EXIT_FAILURE);
    }

    srand(time(NULL));

    while(true)
    {   
        //sleep(1);
        if (numLives < 1)
        {
            printf("Client lost all lives, eliminated\n");
            message_elim(client_fd);
            close(client_fd);        
            exit(EXIT_SUCCESS);
        }

        //sleep(1); //Wait 3 seconds
        memset(buf, 0, BUFFER_SIZE);
        read = recv(client_fd, buf, BUFFER_SIZE, 0); // See if we have a response
        
        if (read <= 0 && errno == EAGAIN) {                  //Timeout for client messages, if read fails then client
            printf("Client took too long, lost a life\n");  //client loses a life and starts loop again
            numLives--;
            continue;
        }
        else if (read <= 0) {
            fprintf(stderr, "Client read failed\n");
            exit(EXIT_FAILURE);
        }

        printf("Client's Message: %s\n", buf);

        p = parse_message(buf); //parse the new message and return it to p
        int *diceRoll = roll_dice();

        /*
            * At the moment, there is a lot of duplicated code in this switch statement... 
            * This is probably not a good sign, 
            * but I was told to keep all the send() and read() statements in here
            * ... rather than sending them out to another function. 
            * We could probably create a function to send the PASS/FAIL messages
            */
        switch (p.flag)
        {
        case INIT:
            exit(EXIT_FAILURE); //Here, the client sends another INIT message--not expected so exit()
        case EVEN:
            if(check(diceRoll, p.flag, 0)) {
                message_pass(client_fd);
            } else {
                message_fail(client_fd);
                printf("Lost one life\n");
                numLives--;
            }
            break;
        case ODD:
            if(check(diceRoll, p.flag, 0)) {
                message_pass(client_fd);
            } else {
                message_fail(client_fd);
                printf("Lost one life\n");
                numLives--;
            }
            break;
        case DOUB:
            if(check(diceRoll, p.flag, 0)) {
                message_pass(client_fd);
            } else {
                message_fail(client_fd);
                printf("Lost one life\n");
                numLives--;
            }
            break;
        case CON:
            if (check(diceRoll, p.flag, p.conChoice)) {
                message_pass(client_fd);
            } else {
                message_fail(client_fd);
                printf("Lost one life\n");
                numLives--;
            }
            break;
        case ERR:
            printf("Invalid move, client loses a life\n");
            numLives--;
            break;
        }
    }
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        fprintf(stderr, "Usage: %s [port]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int port = atoi(argv[1]);

    int server_fd, client_fd, opt_val, err, pid;
    struct sockaddr_in server, client;
    //char *buf;

    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (server_fd < 0)
    {
        fprintf(stderr, "Could not create socket\n");
        exit(EXIT_FAILURE);
    }

    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = htonl(INADDR_ANY);

    opt_val = 1;

    struct timeval tv;
    //Set the timeout value to 30 seconds
    tv.tv_sec = timeout;
    tv.tv_usec = 0;
    setsockopt(server_fd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);
    
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt_val, sizeof opt_val);


    err = bind(server_fd, (struct sockaddr *)&server, sizeof(server));
    if (err < 0)
    {
        fprintf(stderr, "Could not bind socket\n");
        exit(EXIT_FAILURE);
    }

    err = listen(server_fd, 128);
    if (err < 0)
    {
        fprintf(stderr, "Could not listen on socket\n");
        exit(EXIT_FAILURE);
    }

    printf("Server is listening on %d\n", port);

    socklen_t client_len = sizeof(client);

    while (true) {
        client_fd = accept(server_fd, (struct sockaddr *)&client, &client_len);

        if (client_fd < 0)
        {
            fprintf(stderr, "Could not establish new connection\n");
            exit(EXIT_FAILURE);
        }

        pid = fork();

        switch(pid) {

            case -1 :                   //Failure to fork
                perror("Fork error");
                exit(EXIT_FAILURE);
                break;            
            case 0:                     //Client process
                close(server_fd);
                handleClient(client_fd);
                exit(EXIT_SUCCESS);            
            default:                    //Parent process
                close(client_fd);
        }
    }

   
}
