PROJECT = test
HEADERS = $(PROJECT).h
OBJ     = test.o test_dice.o


C99 = cc -std=c99 -lcunit
CFLAGS = -Wall -pedantic -Werror 

$(PROJECT) : $(OBJ)
	$(C99) $(CFLAGS) -o $(PROJECT) $(OBJ)

%.o : %.c $(HEADERS)
	$(C99) $(CFLAGS) -c $<

clean:
	rm -f $(PROJECT) $(OBJ)
