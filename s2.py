"""
This is a simple example of a client program written in Python.
Again, this is a very basic example to complement the 'basic_server.c' example.


When testing, start by initiating a connection with the server by sending the "init" message outlined in 
the specification document. Then, wait for the server to send you a message saying the game has begun. 

Once this message has been read, plan out a couple of turns on paper and hard-code these messages to
and from the server (i.e. play a few rounds of the 'dice game' where you know what the right and wrong 
dice rolls are). You will be able to edit this trivially later on; it is often easier to debug the code
if you know exactly what your expected values are. 

From this, you should be able to bootstrap message-parsing to and from the server whilst making it easy to debug.
Then, start to add functions in the server code that actually 'run' the game in the background. 
"""

import socket 
from time import sleep
# Create a TCP/IP socket
sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

# Connect the socket to the port where the server is listening
server_address = ('localhost', 4444)
print ('connecting to %s port %s' % server_address)
sock.connect(server_address)

count=0
message = 'This is the message.  It will be repeated.'.encode()
try:
   
        # Send data
        message = 'INIT'.encode()
        print ('sending "%s"' % message)
        #sleep(5)
        sock.sendall(message)

        # Look for the response
        #amount_received = 0
        #amount_expected = len(message)
        
        while True: #amount_received < amount_expected:
            sleep(2)
            data = sock.recv(4096)
            # #amount_received += len(data)
            if(data != None):
                mess = data.decode()
                print("Received: %s" % mess)
                if "WELCOME" in mess:
                    id = int(mess[-3:])
                    print("Our id is %d" % id)
                    message = ('%d,MOV,DOUB' % id).encode()
                    print("Received welcome message")
                    sock.sendall(message) # Client has ID 100
                    print('sending,"%s"' % message)
                elif "ELIM" in mess:
                    print("We lost, closing connection")
                    break
                elif "PASS" in mess:
                    print("Your choice was correct")
                    message = ('%d,MOV,DOUB' % id).encode()
                    sock.sendall(message) # Client sends EVEN choice
                    print('sending,"%s"' % message)
                elif "FAIL" in mess:
                    print("Your choice was wrong")
                    message = ('%d,MOV,ODD' % id).encode()
                    sock.sendall(message)
                    print('sending,"%s"' % message)
                elif "REJECT" in mess:
                    print("Server is full, try again later")
                    break
                elif "VICT" in mess:
				    print("We won")
				    break
                else:
                    print ( 'received "%s"' % mess)
           
            data = None

finally:    
    print ('Closing socket')
    sock.close()
