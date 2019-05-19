# CITS3002-Project
Socket Programming 

To do:
  
    Deal with timeouts for players making a move, make sure they lose a life
  
    Tier 3:
    
      Players leaving mid game
    
      Players trying to join mid game
  
    Tier 4:
    
      Handle incorrect moves without crashing server
    
      Kick cheating players
  
    Write report
    
  https://docs.google.com/document/d/1XM3c3JYcjLoogfDgSszmgg3aFBHCcN3LYtxZf6teVmU/edit
  
  
  Assumptions:
  
  
      Games will not start until exactly MAX_PLAYERS is reached, a lobby with less than MAX_PLAYERS will not continue until either more players join or it times out
      
      The server will shutdown after a game is completed, no new game will be started
      
      