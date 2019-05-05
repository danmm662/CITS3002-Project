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
#include <netinet/in.h>
#include <unistd.h>

//Global variables/data structures
#define BUFFER_SIZE 1024
#define numLives    5
#define playerCode  100

extern int **playerInfo;
//extern int idCode[900] idCode; //May not have to use this at all...
extern enum {INIT, EVEN, ODD, DOUB, CON, ERR}  flag;

extern struct messageProperties {
    int id; 
    int flag; 
    int conChoice; 
} messageProperties;

//Functions from parse_message
extern struct messageProperties parse_message(char *);

//Functions from play_game_round
extern int*                     roll_dice(void);
extern bool                     check_even(int *);
extern bool                     check_odd(int *);
extern bool                     check_doubles(int *);
extern bool                     check_contains(int *, int);
extern bool                     check(int *, int, int);
extern void                     clear_game_info(void);
extern void                     init_game_data(void);