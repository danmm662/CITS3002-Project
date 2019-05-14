//
//  messages.c
//  
//
//  Created by Kieren Underwood on 1/5/19.
//

#include "game.h"

void send_message(int client_fd, int flag) {
    //Set up buffer to send string
    char * messbuf;
    messbuf = calloc(BUFFER_SIZE, sizeof(char));

    //Search through the clients to find the matching playerCode
    //Is this the best way to do it?
    int playerID;
    for(int i = 0; i < MAX_PLAYERS; i++) {
        if(playerInfo[i][1] == client_fd && playerInfo[i][4] == false) {
            playerID = playerInfo[i][0];
            break;
        }
    }
    
    switch(flag) {
        case PASS :
            sprintf(messbuf, "%d,PASS", playerID);
        case FAIL :
            sprintf(messbuf, "%d,FAIL", playerID);
        case ELIM :
            sprintf(messbuf, "%d,ELIM", playerID);
        case WEL :
            sprintf(messbuf, "%d,WELCOME", playerID);
        case VICT :
            sprintf(messbuf, "%d,VICT", playerID);
        case CAN :
            sprintf(messbuf, "%d,CAN", playerID);
        case REJ :
            sprintf(messbuf, "%d,REJ", playerID);
    }  

    int err = send(client_fd, messbuf, strlen(messbuf), 0);
    if(err < 0) {
        fprintf(stderr, "Message failed to send");
        exit(EXIT_FAILURE);
    }
    free(messbuf);
}

/*
* If successful this will @return a struct containing the guess of the player
* Right now, it DOES NOT handle the case where it sends a message which was corrupt
*/
struct messageProperties handleGuess(void) {
    char *buf;
    buf = calloc(BUFFER_SIZE, sizeof(char)); 

    if (read <= 0 && errno == EAGAIN) {         //Timeouts for INIT message, not sure if necessary
        fprintf(stderr, "Client timed out\n");
        //exit(EXIT_FAILURE);
    }
    else if (read <= 0){
        fprintf(stderr, "Client read failed\n");
        //message_elim(client_fd);
        //send_message(client_fd, ELIM);
        //PROBABLY SHOULD MAKE A WAY FOR THIS TO RETURN A STATE WHERE THEY LOSE A LIFE IF 
        //THE MESSAGE IS NOT READABLE
        //exit(EXIT_FAILURE);
    }

    printf("New client's message: %s\n", buf);

    struct messageProperties p;
    p = parse_message(buf);
    return p;
}
/*
* @returns a bool which is true if we received an INIT, false if we do not
* The whole function will only be called when we have forked() another process,
* so this is why we are calling exit(EXIT_SUCCESS) or exit(EXIT_FAILURE) upon completion
*/
void handleInit(int client_fd) {

    char *buf;
    buf = calloc(BUFFER_SIZE, sizeof(char)); 

    if (read <= 0 && errno == EAGAIN) {         //Timeouts for INIT message, not sure if necessary
        fprintf(stderr, "Client timed out\n");
        exit(EXIT_FAILURE);
    }
    else if (read <= 0){
        fprintf(stderr, "Client read failed\n");
        //message_elim(client_fd);
        send_message(client_fd, ELIM);
        exit(EXIT_FAILURE);
    }

    printf("New client's message: %s\n", buf);

    struct messageProperties p;
    p = parse_message(buf);

    if(p.flag == INIT) {
        generateNewPlayer(client_fd, numLives);     //If client sends INIT, give them an id and send it to them
        send_message(client_fd, WEL);               //Need to pass id to the message() function, ie message(int flag, int id)
        exit(EXIT_SUCCESS);
    } else {
        fprintf(stderr, "INIT was not the first message received\n");
        exit(EXIT_FAILURE);
    }

}