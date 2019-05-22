// CITS3002 Project 2019
// Names:            Kieren Underwood,   Daniel Maitland
// Student Numbers:  21315543            21986102
// Compiles on Linux

#include "game.h"

/*
* We call this before the game begins. It allocated shared memory for both child and parent
* processes to access (read and write). It also goes through and initializes parts of the memory 
* that need initializing.
*/
void init_game_data(void) {
    
    currPlayers = mmap( NULL,                           //Setting the number of current players to shared memory    
                        sizeof(int),  
                        PROT_READ | PROT_WRITE,         //Read and write access to memory
                        MAP_SHARED | MAP_ANONYMOUS,     //Shared so all processes can access
                        -1,
                        0 );
    //We check each time with MAP_FAILED, whether the memory could be allocated
    if(currPlayers == MAP_FAILED) {
        fprintf(stderr, "Unable to map currPlayers to shared memory\n");
        exit(EXIT_FAILURE);
    }

    gameFinished = mmap( NULL,                  //Setting the array of player info to shared memory
                   sizeof(bool),        
                   PROT_READ | PROT_WRITE,      //Read and write access to memory
                   MAP_SHARED | MAP_ANONYMOUS,  //Shared so all processes can access
                   -1,
                   0 );

    if(gameFinished == MAP_FAILED) {
        fprintf(stderr, "Unable to map pArray to shared memory\n");
        exit(EXIT_FAILURE);
    }

    pArray = mmap( NULL,                                //Setting the array of player info to shared memory
                   MAX_PLAYERS * sizeof(playerInfo),    //Number of players * size of the struct
                   PROT_READ | PROT_WRITE,              //Read and write access to memory
                   MAP_SHARED | MAP_ANONYMOUS,          //Shared so all processes can access
                   -1,
                   0 );

    if(pArray == MAP_FAILED){
        fprintf(stderr, "Unable to map pArray to shared memory\n");
        exit(EXIT_FAILURE);
    }

    
    //Game has not started so currPlayers = 0
    *currPlayers = 0;
    //Game is not finished yet
    *gameFinished = false;

    //Go through and set the default values for all of our potential players
    for(int i = 0; i < MAX_PLAYERS; i++) {
        pArray[i].playerID = (i + 100);
        pArray[i].numLives = MAX_LIVES;
        pArray[i].taken = false;
        pArray[i].eliminated = -1;
        pArray[i].won_last_round = false;
    }
}

//Assigns the client_fd values and tells us that the current player is taken
void generateNewPlayer(int client_fd, int currentPlayers) {
    
    pArray[currentPlayers].client_fd = client_fd;
    pArray[currentPlayers].taken = true;
    
}

/*
* We only enter this function once
* It sends a START message to all clients
* It then enters a loop where it will a) play a round, b) check and update numLives and eliminated status of all players,
* and, c) check if a winner has been found. 
*/
void playGame(void) {

    //Set the default values before our game rounds have begun
    bool victorFound = false;
    int round = 0;
    int playersLeft = MAX_PLAYERS;
    int *rolled_dice; 

    //Send START message to all players
    for(int i = 0; i < MAX_PLAYERS; i++) {
        send_message(pArray[i].client_fd, START);
    }

    while(!victorFound) {
    
        pid_t child_pid, wpid;
        int status = 0;

        //Roll a dice for all players to access
        rolled_dice = roll_dice();

        printf("\nRound %d:\n", round + 1);
        printf("Dice roll is %d,%d\n", rolled_dice[0], rolled_dice[1]);

        for(int i = 0; i < MAX_PLAYERS; i++) {
            
            //Only play the round with players not eliminated
            if (pArray[i].eliminated == -1) {
                //Fork() and then play a round in the child processes
                //Wait() for the all the child process to be completed
                if((child_pid = fork()) == 0) {
                    int client_no = pArray[i].client_fd;
                    //If this sleep is removed, the game plays out instantaneously and is hard to follow
                    sleep(1);
                    playRound(i, client_no, rolled_dice);
                    exit(EXIT_SUCCESS);
                }
            }
        }

        while((wpid = wait(&status)) > 0 );

        round++;        

        //Check to see whether or not the players won the last round
        for(int i = 0; i < MAX_PLAYERS; i++) {
            if (pArray[i].eliminated == -2) {               //Kicking cheating player
                send_message(pArray[i].client_fd, ELIM);
                pArray[i].eliminated = 0;
                playersLeft--;
                continue;
            }
            else if (pArray[i].eliminated == -3) {          //Decrementing players left for when player drops out
                pArray[i].eliminated = 0;
                playersLeft--;
                continue;
            }
            else if (pArray[i].eliminated >= 0) {           //Skip over players already eliminated
                continue;
            }
            else if (!pArray[i].won_last_round) {           //Send FAIL to players who guessed wrong
                pArray[i].numLives--;
                send_message(pArray[i].client_fd, FAIL);
            }
            else {
                send_message(pArray[i].client_fd, PASS);    //Send PASS to players who guessed correctly
            }

            //Then check if numLives = 0 and then eliminate if neccessary
            if(pArray[i].numLives < 1) {                    
                pArray[i].eliminated = round;
                playersLeft--;
            }
        }
        

        //Check if there is a victor and send victory message if there is one
        for (int i = 0; i < MAX_PLAYERS; i++) {
            if (playersLeft == 1){                              //Normal case, if there is one player left in game
                if (pArray[i].eliminated < 0) {                 //Find player not elimmed, send VICT
                    printf("Player %d won!\n", pArray[i].playerID);
                    send_message(pArray[i].client_fd, VICT);
                    close(pArray[i].client_fd);
                }
                else if (pArray[i].eliminated == round) {       //Only need to send ELIM to players eliminated this round
                    printf("Player %d eliminated\n", i + 100);
                    send_message(pArray[i].client_fd, ELIM);
                    close(pArray[i].client_fd);
                }
                victorFound = true;
            }
            else if (playersLeft == 0) {                        //If there is a draw between 2 or more players
                if (pArray[i].eliminated == round) {            //Find players who were eliminated in the last round--they all WON at the same time
                    printf("Player %d won!\n", pArray[i].playerID);
                    send_message(pArray[i].client_fd, VICT);
                    close(pArray[i].client_fd);
                }
                victorFound = true;
            }
            else {                                              //When game hasn't finished yet
                if (pArray[i].eliminated == round) {            //Extra checks for those people who should be sent an eliminated message
                    printf("Player %d eliminated\n", i + 100);
                    send_message(pArray[i].client_fd, ELIM);
                    close(pArray[i].client_fd);
                }
            }
        }
    }
    return;
}

/*
* We enter a round and process the guess of each player
* The message is either flagged as ERR or CHEAT or sent through to be checked
* Once checked, we return true of false to indicated a correct or incorrect guess
* Shared memory of won_last_round is then changed to indicate the updated guess
*/
void playRound(int player, int client_fd, int *diceRoll) {
    
    //Get the guess from a client
    int playerID = player + 100;

    char *buf;
    buf = calloc(BUFFER_SIZE, sizeof(char)); 

    struct messageProperties p;

    int read = recv(client_fd, buf, BUFFER_SIZE, 0);

    if (read == 0) {
        printf("Player %d dropped out\n", playerID);
        pArray[player].eliminated = -3;
        return;
    }
    else if (read < 0 && errno == EAGAIN) {         //Timeout for client's move
        printf("Player %d timed out\n", playerID);
        pArray[player].won_last_round = false;       //Set to false so they lose a life
        return;                                      //Return to play_game()
    }
    else if (read < 0){                            //Don't know what to do for this one
        fprintf(stderr, "Client read failed\n");
        pArray[player].won_last_round = false;
        return;
    }    

    p = parse_message(buf, client_fd);

    if(p.flag == CHEAT) {                       //Kicked for cheating
        printf("Player %d found cheating, kicked from game\n", playerID);
        pArray[player].eliminated = -2;
        return;
    }
    else if(p.flag == ERR) {                    //Lose a life for an invalid method
        printf("Player %d made an invalid move\n", playerID);
        pArray[player].won_last_round = false;
        return;
    }

    //Check whether this guess is correct
    bool checkedGuess = check(diceRoll, p.flag, p.conChoice);
    
    //If correct, then make players bool-->won_last_round = true
    //If false, make players bool-->won_last_round = false
    if(checkedGuess) {
        printf("Player %d made a correct guess\n", playerID);
        pArray[player].won_last_round = true;
    } 
    else {
        printf("Player %d made an incorrect guess\n", playerID);
        pArray[player].won_last_round = false;
    }
    //Return to the playGame function
}


