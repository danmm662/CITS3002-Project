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

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        fprintf(stderr, "Usage: %s [port]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int port = atoi(argv[1]);

    int server_fd, client_fd, err, opt_val;
    struct sockaddr_in server, client;
    char *buf;

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
    // Will block until a connection is made
    client_fd = accept(server_fd, (struct sockaddr *)&client, &client_len);

    if (client_fd < 0)
    {
        fprintf(stderr, "Could not establish new connection\n");
        exit(EXIT_FAILURE);
    }

    /*       
    setup_game/teardown_game() {} : 
    * this will set up the initial state of the game (number of rounds, players
        etc.)/ print out final game results and cancel socket connections. 
            
    Accepting multiple connections (we recommend not starting this until after implementing some
    of the basic message parsing/game playing): 
    * Whilst in a while loop
        - Accept a new connection 
        - Create a child process 
        - In the child process (which is associated with client), perform game_playing functionality
        (or read the messages) 
    **/

    buf = calloc(BUFFER_SIZE, sizeof(char)); // Clear our buffer so we don't accidentally send/print garbage
    sleep(2);
    int read = recv(client_fd, buf, BUFFER_SIZE, 0); // Try to read from the incoming client

    if (read < 0)
    {
        fprintf(stderr, "Client read failed\n");
        exit(EXIT_FAILURE);
    }
    printf("Client's message: %s\n", buf);

    struct messageProperties p;
    p = parse_message(buf);

    switch (p.flag)
    {
    case INIT:
        memset(buf,0,BUFFER_SIZE);
        sprintf(buf, "WELCOME,%d\n", playerCode);
        err = send(client_fd, buf, strlen(buf), 0);
        if (err < 0)
        {
            fprintf(stderr, "Failed to send WELCOME message\n");
            exit(EXIT_FAILURE);
        }
        break;
    default:
        exit(EXIT_FAILURE);
    }

    memset(buf,0,BUFFER_SIZE);
    sprintf(buf, "Let the games begin!\n");
    err = send(client_fd, buf, strlen(buf), 0);

    while (true)
    {
        if (numLives < 1)
        {
            memset(buf,0,BUFFER_SIZE);
            sprintf(buf, "%d,ELIM\n", playerCode);
            err = send(client_fd, buf, strlen(buf), 0);
            if (err < 0)
            {
                exit(EXIT_FAILURE);
            }
            exit(EXIT_SUCCESS);
        }

        //sleep(1); //Wait 3 seconds
        memset(buf,0,BUFFER_SIZE);
        read = recv(client_fd, buf, BUFFER_SIZE, 0); // See if we have a response
        printf("Client's Message: %s\n", buf);

        if (read < 0)
        {
            fprintf(stderr, "Client read failed\n");
            exit(EXIT_FAILURE);
        }

        p = parse_message(buf); //parse the new message and return it to p
        //buf = calloc(BUFFER_SIZE, sizeof(char));          //reset the buffer to send new messages
        int *diceRoll = roll_dice();

        /*
            * At the moment, there is a lot of duplicated code in this switch statement... 
            * This is probably not a good sign, but I was told to keep all the send() and read() statements in here
            * ... rather than sending them out to another function. We could probably create a function to send the PASS/FAIL messages
            */
        switch (p.flag)
        {
        case INIT:
            exit(EXIT_FAILURE); //Here, the client sends another INIT message--not expected so exit()
        case EVEN:
            if (check(diceRoll, p.flag, 0))
            {
                sprintf(buf, "%d,PASS\n", playerCode);
                err = send(client_fd, buf, strlen(buf), 0);
                if (err < 0)
                {
                    fprintf(stderr, "PASS messaged failed to send\n");
                    exit(EXIT_FAILURE);
                }
            }
            else
            {
                numLives--;
                sprintf(buf, "%d,FAIL\n", playerCode);
                err = send(client_fd, buf, strlen(buf), 0);
                if (err < 0)
                {
                    fprintf(stderr, "FAIL messaged failed to send\n");
                    exit(EXIT_FAILURE);
                }
            }
        case ODD:
            if (check(diceRoll, p.flag, 0))
            {
                sprintf(buf, "%d,PASS\n", playerCode);
                err = send(client_fd, buf, strlen(buf), 0);
                if (err < 0)
                {
                    fprintf(stderr, "PASS messaged failed to send\n");
                    exit(EXIT_FAILURE);
                }
            }
            else
            {
                numLives--;
                sprintf(buf, "%d,FAIL\n", playerCode);
                err = send(client_fd, buf, strlen(buf), 0);
                if (err < 0)
                {
                    fprintf(stderr, "FAIL messaged failed to send\n");
                    exit(EXIT_FAILURE);
                }
            }

        case DOUB:
            if (check(diceRoll, p.flag, 0))
            {
                sprintf(buf, "%d,PASS\n", playerCode);
                err = send(client_fd, buf, strlen(buf), 0);
                if (err < 0)
                {
                    fprintf(stderr, "PASS messaged failed to send\n");
                    exit(EXIT_FAILURE);
                }
            }
            else
            {
                numLives--;
                sprintf(buf, "%d,FAIL\n", playerCode);
                err = send(client_fd, buf, strlen(buf), 0);
                if (err < 0)
                {
                    fprintf(stderr, "FAIL messaged failed to send\n");
                    exit(EXIT_FAILURE);
                }
            }
        case CON:
            if (check(diceRoll, p.flag, p.conChoice)) {
                sprintf(buf, "%d,PASS\n", playerCode);
                err = send(client_fd, buf, strlen(buf), 0);
                if (err < 0) {
                    fprintf(stderr, "PASS messaged failed to send\n");
                    exit(EXIT_FAILURE);
                }
            }
            else {
                numLives--;
                sprintf(buf, "%d,FAIL\n", playerCode);
                err = send(client_fd, buf, strlen(buf), 0);
                if (err < 0){
                    fprintf(stderr, "FAIL messaged failed to send\n");
                    exit(EXIT_FAILURE);
                }
            }
        default:
            break; //Exit if our parse message hasn't turned up the right flag.
        }
        //free(buf);
    }
}
