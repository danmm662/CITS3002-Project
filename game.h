// CITS3002 Project 2019
// Names:            Kieren Underwood,   Daniel Maitland
// Student Numbers:  21315543            21986102
// Compiles on Linux

#ifndef headers_h
#define headers_h


#endif /* headers_h */

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h> 
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h> 
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <ctype.h>

//Global variables/data structures

#define BUFFER_SIZE  1024
#define MAX_LIVES    3        
#define TIMEOUT      30


extern int *currPlayers;
extern bool *gameFinished;
extern int MAX_PLAYERS;

extern enum {INIT, EVEN, ODD, DOUB, CON, WELCOME, START, 
             PASS, FAIL, ELIM, VICT, REJECT, CANCEL, ERR, CHEAT 
}  flag;

extern struct messageProperties {
    int id;                     //Corresponds to the playerID
    int flag;                   //Tells us which dice choice the player has sent
    int conChoice;              //The int connected to the CON choice
} messageProperties;

extern struct playerInfo {
    int playerID;               //A 3-digit int which the player must use for sending messages
    int client_fd;              //The int used to send messages through the socket--must match the playerID
    int numLives;               //Set initially as #define MAX_LIVES
    bool taken;                 //Tells us whether this player already exists or not
    bool won_last_round;        //Whether the player won the last round
    int eliminated;             //Cases for eliminated: -3 for dropped out, -2 for cheating
} playerInfo;                   // -1 still alive, >-1 corresponds to round # the player was eliminated

//All of our players are stored in this array
extern struct playerInfo *pArray;

//Functions from parse_message
extern struct messageProperties parse_message(char *, int);

//Functions from dice_and_check.c
extern int*                     roll_dice(void);
extern bool                     check_even(int *);
extern bool                     check_odd(int *);
extern bool                     check_doubles(int[]);
extern bool                     check_contains(int *, int);
extern bool                     check(int *, int, int);

//Functions from play_game_round.c
extern void                     init_game_data(void);
extern void                     generateNewPlayer(int, int);
extern void                     playGame(void);
extern void                     playRound(int, int, int *);

//Functions from messages.c
extern void                     send_message(int, int);
extern void                     send_reject(int);
extern void                     handleInit(int);

//Functions from playSpGame.c
extern void                     playSpGame(void);