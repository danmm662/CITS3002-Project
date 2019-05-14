//
//  headers.h
//  
//
//  Created by Kieren Underwood on 1/5/19.
//

#ifndef headers_h
#define headers_h


#endif /* headers_h */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h> 
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <errno.h>

//Global variables/data structures
#define BUFFER_SIZE 1024
//#define maxLives    5         If you #DEFINE this, then you are not able to change it.
#define playerCode  100
#define TIMEOUT     30
#define MAX_PLAYERS  2

extern int numLives, currPlayers;
extern int playerInfo[][2];
//extern int idCode[900]; //May not have to use this at all...
extern enum {INIT, EVEN, ODD, DOUB, CON, WEL, START, 
             PASS, FAIL, ELIM, VICT, REJ, CAN, ERR
}  flag;

extern struct messageProperties {
    int id; 
    int flag; 
    int conChoice; 
} messageProperties;

//Functions from parse_message.c
extern struct messageProperties parse_message(char *);

//Functions from play_game_round.c
extern int*                     roll_dice(void);
extern bool                     check_even(int *);
extern bool                     check_odd(int *);
extern bool                     check_doubles(int *);
extern bool                     check_contains(int *, int);
extern bool                     check(int *, int, int);
extern void                     clear_game_info(void);
extern void                     init_game_data(void);

//Functions from handleClient.c
extern void                     handleClient(int);

//Message functions (temporary)
extern void                     message_pass(int);
extern void                     message_fail(int);
extern void                     message_welcome(int);
extern void                     message_elim(int);

extern int                      generateNewId(void);