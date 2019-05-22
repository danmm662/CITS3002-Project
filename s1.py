# CITS3002 Project 2019
# Names:            Kieren Underwood,   Daniel Maitland
# Student Numbers:  21315543            21986102
# Compiles on Linux

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
        #sleep(10)
        message = 'INIT'.encode()
        print ('sending "%s"' % message)
        sock.sendall(message)

        # Look for the response
        #amount_received = 0
        #amount_expected = len(message)
        
        while True: #amount_received < amount_expected:
            data = sock.recv(4096)
            # #amount_received += len(data)
            if(data != None):
                mess = data.decode()
                print("Received: %s" % mess)
                if "WELCOME" in mess:
                    id = int(mess[-3:])
                    print("Our id is %d" % id)
                    message = ('%d,MOV,CON,1' % id).encode()
                    print("Received welcome message")
                    sock.sendall(message) # Client has ID 100
                    #sock.sendall(message)
                    print('sending,"%s"' % message)
                elif "ELIM" in mess:
                    print("We lost, closing connection")
                    break
                elif "PASS" in mess:
                    print("Your choice was correct")
                    message = ('%d,MOV,EVEN' % id).encode()
                    sock.sendall(message) # Client sends EVEN choice
                    print('sending,"%s"' % message)
                elif "FAIL" in mess:
                    print("Your choice was wrong")
                    #message = ('101,MOV,ODD').encode()
                    message = ('%d,MOV,ODD' % id).encode()
                    sock.sendall(message)
                    print('sending,"%s"' % message)
                elif "REJECT" in mess:
                    print("Server is full, try again later")
                    break
                elif "CANCEL" in mess:
                    print("Took too long to fill up server")
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
