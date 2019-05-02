# CITS3002 Project 2019
# Names:            Kieren Underwood,   Daniel Maitland
# Student Numbers:  21315543            21986102
#

PROJECT = game
HEADERS = $(PROJECT).h
OBJ     = game.o parse_message.o play_game_round.o 


C99 = cc -std=c99
CFLAGS = -Wall -pedantic -Werror

$(PROJECT) : $(OBJ)
	$(C99) $(CFLAGS) -o $(PROJECT) $(OBJ)

%.o : %.c $(HEADERS)
	$(C99) $(CFLAGS) -c $<

clean:
	rm -f $(PROJECT) $(OBJ)


