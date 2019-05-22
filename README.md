# CITS3002 Project 2019
# Names:            Kieren Underwood,   Daniel Maitland
# Student Numbers:  21315543            21986102
# Compiles on Linux


# How to Run "game.c"

Run on Linux!

First begin the server on a terminal:
  - "make"
  - "./game 4444 [number of players]"

Then run python clients on seperate terminal an arbitrary number of times:
  - "python s1.py"
  or 
  - "python s2.py"
  or 
  - "python [your_client_name].py"


To do:
  
    Finish report    
  https://docs.google.com/document/d/1XM3c3JYcjLoogfDgSszmgg3aFBHCcN3LYtxZf6teVmU/edit

# Assumptions:
  
- A single-player game (ie. MAX_PLAYERS = 1) does check for cheating. 

- MAX_PLAYERS is set at the start-up of the game, and must be reached--exactly--if the game is to commence. The game does not allow a range of players, eg. 4-8 players.

- Each player must wait until the all other players have entered a guess (or the time runs out), and then receive a notification of FAIL or PASS, before entering another guess. If another    guess is entered before, it is ignored. 

- Every player receives the same centralised dice rolls. A player does not get to metaphorically “roll his own dice” and guess the result.

- A player only receives notification of its own game status. All information about how many players are left, how many lives they each have, or even what the dice rolls were, are hidden     from the the player. If two players win in a draw, they both receive a VICT notification, without being told another player received one. You play this game blind!

- An ill-formed client message--eg. “101,MOV,CON,7”--will result in the player losing a life. It is not counted as cheating.

- We define cheating as a client sending a message with another ID number--ie. Imitating another player. Eg. Player 101 sends: “100,MOV,EVEN”. This person is eliminated from the game. 

- A player who drops out cannot win a game. They are also not sent an ELIM message. 

      
      
