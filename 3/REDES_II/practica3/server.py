"""
    FILE: server.py
    AUTHOR: Alba Ramos Pedroviejo - alba.ramosp@estudiante.uam.es
    AUTHOR: Nicolas Serrano Salas - nicolas.serranos@estudiante.uam.es
    DESCRIPTION: implements the methods used in the communication with the discovery server.
"""


import socket

SERVER_URL_TFG = "tfg.eps.uam.es"
SERVER_URL_VEGA = "vega.ii.uam.es"
MAX_BUFF = 1024
MAX_DATAGRAM = 65536

TCPsrc = 18082 # TCP port where user listens
UDPsrc = 18083 # UDP port where user listens

class Server():
    """
    From:
    https://stackoverflow.com/questions/166506/finding-local-ip-addresses-using-pythons-stdlib
    """
    def __init__(self):
        s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM) # UDP socket
        s.connect(("8.8.8.8", 80)) # To obtain our external IP
        self.ipUser = s.getsockname()[0]
        s.close()

    """
    METHOD: connect(ip, port, timeout)
    ARGS_IN: ip - ip address of the server to be connected to
             port - port of the server to be connected to
             timeout - time (in seconds) of timeout if connection is not possible
    DESCRIPTION: opens a TCP socket and connects to the server indicated by the given ip and port.
                 If no ip address and port are given, connects to the discovery server.
    ARGS_OUT: -1 if timeout happens
    """
    def connect(self, ip=SERVER_URL_VEGA, port=8000, timeout=None):
        self.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM) # TCP socket
        self.socket.settimeout(timeout)

        try:
            self.socket.connect((ip, port))
        except: 
            return -1
        
        self.socket.settimeout(None)

    """
    METHOD: disconnect()
    DESCRIPTION: closes the TCP socket
    """
    def disconnect(self):
        self.socket.close()

    """
    METHOD: send(message)
    ARGS_IN: message - encoded message to be sent through the socket
    DESCRIPTION: sends the encoded message received by argument through the socket 
    """
    def send(self, message):
        self.socket.send(message)

    """
    METHOD: receive(size)
    ARGS_IN: size - length of the buffer to be read from the socket
    DESCRIPTION: reads and returns a message received from the socket of the size received by argument
    ARGS_OUT: message from the socket
    """
    def receive(self, size=MAX_BUFF):
        data = self.socket.recv(size)
        return data

    """
    METHOD: getIp()
    DESCRIPTION: returns your external ip
    ARGS_OUT: user external ip
    """
    def getIp(self):
        return self.ipUser