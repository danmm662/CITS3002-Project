#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#define BUFFER_SIZE 1024

enum flag{INIT, EVEN, ODD, DOUB, CON, ERR};

struct messageProperties {
    int id, flag, conChoice;
};

extern struct messageProperties parse_message(char *);
extern int*                     roll_dice(void);
extern bool                     check_even(int *);
extern bool                     check_odd(int *);
extern bool                     check_doubles(int *);
extern bool                     check_contains(int *, int);
extern bool                     check(int *, int, int);