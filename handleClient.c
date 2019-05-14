#include "game.h"

//Does all the client handling, atm only allows multiple people to play a single player game on same server
//Would probably be good to 
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

    printf("New client's message: %s\n", buf);

    struct messageProperties p;
    p = parse_message(buf);

    if(p.flag == INIT) {
        //int id = generateNewId();   //If client sends INIT, give them an id and send it to them
        message_welcome(client_fd);   //Need to pass id to the message() function, ie message(int flag, int id)
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

    while(true)
    {   
        //sleep(1);
        if (numLives < 1)
        {
            printf("Client lost all lives, eliminated\n");
            message_elim(client_fd);
            close(client_fd); 
            currPlayers--;       
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