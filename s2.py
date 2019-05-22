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
        message = 'INIT'.encode()
        print ('sending "%s"' % message)
        #sleep(5)
        sock.sendall(message)
        
        while True: 
            data = sock.recv(4096)
            if(data != None):
                mess = data.decode()
                print("Received: %s" % mess)
                if "WELCOME" in mess:
                    id = int(mess[-3:])
                    print("Our id is %d" % id)
                    #sleep(30)
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
