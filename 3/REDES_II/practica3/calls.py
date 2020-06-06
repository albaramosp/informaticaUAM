"""
    FILE: calls.py
    AUTHOR: Alba Ramos Pedroviejo - alba.ramosp@estudiante.uam.es
    AUTHOR: Nicolas Serrano Salas - nicolas.serranos@estudiante.uam.es
    DESCRIPTION: implements the methods related with the communication between users.
"""

from appJar import gui
import numpy as np
from server import *
import socket
import select
import threading
import time
import bisect

class Calls():
    def __init__(self, nick, client):
        self.server = Server()

        # Receive control messages
        self.socketListen = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        try:
            self.socketListen.bind((self.server.getIp(), TCPsrc))
            self.socketListen.listen(1)

        except socket.error as msg:
            self.socketListen.close()

        self.client = client
        self.nick = nick

        # Send control messages
        self.peerTCP = None

        # Handle sending and reception of UDP data
        self.socketUDPListen = None # receive
        self.socketUDPpeer = None # send

        # Buffering and control flow
        self.videoBuffer = []
        self.maxBufferLenght = 40
        self.startReproducing = False
        self.lastReproducedNumber = -1
        self.lastReproducedFrame = None

        # Threads:
        # Listens to calls
        self.thread = threading.Thread(target = self.receiveCall)
        self.thread.start()

        # Listen to commands during the call
        self.processCallCommandsThread = None

    """
    METHOD: closeCall()
    DESCRIPTION: shuts down TCP listening socket
    """
    def closeCall(self):
        self.socketListen.shutdown(socket.SHUT_RDWR)
    
    """
    METHOD: listenVideo()
    DESCRIPTION: opens the UDP listening port
    """
    def listenVideo(self):
        # Opens connection to listen to incoming video
        self.socketUDPListen = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        try:
            self.socketUDPListen.bind(('', UDPsrc))
            self.socketUDPListen.settimeout(0.001)

        except socket.error as msg:
            self.socketUDPListen.close()
            
    """
    METHOD: receivePeerVideo()
    DESCRIPTION: reads, decode and insert into the buffer the read message from the UDP listening port
    ARGS_OUT: -1
    """
    def receivePeerVideo(self):
        # Receive video packages
        try:
            response = self.socketUDPListen.recv(MAX_DATAGRAM) # video package
            split = response.split(b'#', 4)
            tupla = (int(split[0].decode("utf-8")), split[-1])
            self.insertInBuffer(tupla) # The package is stored compressed
        except Exception as msg:
            return -1

    """
    METHOD: connectVideo(ipDst, UDPport)
    ARGS_IN: ipDst - IP address of peer
			 UDPport - port of peer	
    DESCRIPTION: connects to the UDP listening port of the peer
    """
    def connectVideo(self, ipDst, UDPport):
        self.socketUDPpeer = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        
        try:
            self.socketUDPpeer.connect_ex((ipDst, int(UDPport)))
        except socket.error as msg:
            print("Socker error: " + str(msg))

    """
    METHOD: sendVideo(content)
    ARGS_IN: content - message to be sent
    DESCRIPTION: sends a message through the UDP port
    """
    def sendVideo(self, content):
        try:
            self.socketUDPpeer.send(content)
        except Exception:
            return -1

    """
    METHOD: receiveCall()
    DESCRIPTION: Listens from the TCP listening port and process the connection requests
    """
    def receiveCall(self):
        while True:
            try:
                auxPeerTCP, addr = self.socketListen.accept()
            except socket.error:
                break
            
            
            response = auxPeerTCP.recv(MAX_BUFF) # CALLING nick UDPsrc
            data = response.decode().split(" ")
            
            # If your are in call, BUSY
            if self.peerTCP is not None:
                auxPeerTCP.send(("CALL_BUSY").encode())
                auxPeerTCP.close()

            
            # If you are not in call and receive CALLING command
            elif data[0] == 'CALLING':
                self.peerTCP = auxPeerTCP
                ret = self.client.getApp().yesNoBox("Llamada entrante", "Llamada entrante de " 
                                                + data[1] + ". ¿Aceptar?", parent=None)
                if ret: 
                    self.acceptCall()
                else:
                    self.denyCall()
                    continue

                # Call accepted
                self.client.handleCallProcess(self.peerTCP.getpeername()[0], data[2])
                self.processCallCommandsThread = threading.Thread(target = self.processCallCommands)
                self.processCallCommandsThread.start()


            # If you receive another command while openning a connection, ERROR
            else:
                self.peerTCP.close()
                self.peerTCP = None

    """
    METHOD: processCallCommands()
    DESCRIPTION: Listens from the TCP listening port and process the call command requests
    """
    def processCallCommands(self):
        while True:
            try:
                command = self.peerTCP.recv(MAX_BUFF).decode().split(" ")
            except Exception:
                break
            
            if command[0] == "CALL_HOLD":
                self.client.app.infoBox("Llamada en pausa", str(command[1])+" ha pausado la llamada")
                self.client.peerPaused = True
            elif command[0] == "CALL_RESUME":
                self.client.peerPaused = False
            elif command[0] == "CALL_END":
                self.processCallEnd(notify=False)
                self.client.showMainPage(reset=True)
                break


    """
    METHOD: sendCall(portTCP, ipDst)
             portTCP - TCP port of the peer to call
             ipDst - ip of the peer to call
    DESCRIPTION: starts a call comunication with and user
    ARGS_OUT: -3 timeout error, -2 none defined error, -1 denied call user is busy
              0 denied call, UDP port of the peer
    """
    def sendCall(self, portTCP, ipDst):
        # Creates the TCP socket and connects to the peer
        self.peerTCP = socket.socket(socket.AF_INET, socket.SOCK_STREAM) # TCP
        self.peerTCP.settimeout(10)
        
        # If you can't connect to the user in 10 seconds raise and exception
        try:
            self.peerTCP.connect((ipDst, portTCP))
        except:
            self.peerTCP.close()
            self.peerTCP = None
            return -3

        self.peerTCP.settimeout(None)
        
        # Sends the calling petition to the peer
        # message = "CALLING " + nick + " " + str(UDPsrc)
        message = "CALLING " + self.nick + " " + str(UDPsrc)
        self.peerTCP.send(message.encode())

        # Waits to receive the response from the peer
        response = self.peerTCP.recv(MAX_BUFF)
        data = response.decode().split(" ")

        # Process response
        if data[0] == 'CALL_ACCEPTED':
            self.processCallCommandsThread = threading.Thread(target = self.processCallCommands)
            return data[2] # UDP port in which the destination peer listens
        elif data[0] == 'CALL_DENIED':
            self.peerTCP.close()
            self.peerTCP = None
            return 0
        elif data[0] == 'CALL_BUSY':
            self.peerTCP.close()
            self.peerTCP = None
            return -1
        else:
            self.peerTCP.close()
            self.peerTCP = None
            return -2
    """
    METHOD: denyCall()
    DESCRIPTION: Denies the call
    """
    def denyCall(self):
        self.peerTCP.send(("CALL_DENIED " + self.nick).encode())
        self.peerTCP.close()
        self.peerTCP = None
    
    """
    METHOD: acceptCall()
    DESCRIPTION: Accepts the call
    """
    def acceptCall(self):
        self.peerTCP.send(("CALL_ACCEPTED " + self.nick + " " + str(UDPsrc)).encode())

    """
    METHOD: notifyCallHold()
    DESCRIPTION: Notifies the user pause
    """
    def notifyCallHold(self):
        message = "CALL_HOLD " + self.nick + " "
        self.peerTCP.send(message.encode())

    """
    METHOD: notifyCallResume()
    DESCRIPTION: Notifies the user resume
    """
    def notifyCallResume(self):
        message = "CALL_RESUME " + self.nick + " "
        self.peerTCP.send(message.encode())

    """
    METHOD: processCallEnd(notify)
    ARGS_IN: notify - boolean that tells if it has to notify the peer the finishing
    DESCRIPTION: Processes the finish of a call 
    """
    def processCallEnd(self, notify=True):
        if notify is True:
            message = "CALL_END " + self.nick + " "
            self.peerTCP.send(message.encode())
        try:
            self.peerTCP.shutdown(socket.SHUT_RDWR)
        except:
            self.peerTCP.close()
        self.peerTCP = None
        
        try:
            self.socketUDPListen.shutdown(socket.SHUT_RDWR)
        except:
            self.socketUDPListen.close()

        self.socketUDPListen = None
        self.socketUDPpeer.close()
        self.socketUDPpeer = None

        self.videoBuffer = []


    """
    METHOD: insertInBuffer(data)
    ARGS_IN: data - data to be inserted
    DESCRIPTION: Try to insert the data in the buffer in an ordered way
    """
    def insertInBuffer(self, data):
        if len(self.videoBuffer) >= self.maxBufferLenght:
            return False
        else:
            bisect.insort(self.videoBuffer, data)
            return True

    """
    METHOD: extractFromBuffer()
    DESCRIPTION: Extracts the corresponding frame to be reproduced from the buffer
    ARGS_OUT: frame to be reproduced
    """
    def extractFromBuffer(self):
        # If buffer's lenght exceeds 3/4 of maximun lenght, start reproducing
        if len(self.videoBuffer) >= (3/4*self.maxBufferLenght):
            self.startReproducing = True
        # If buffer's length is 0, buffer is empty, stop reproducing
        elif len(self.videoBuffer) == 0:
            self.startReproducing = False

        if self.startReproducing is True:
            out = self.videoBuffer.pop(0)
            # Delay
            if out[0] <= self.lastReproducedNumber:
                return self.lastReproducedFrame
            # Ok
            elif out[0] == self.lastReproducedNumber+1:
                self.lastReproducedNumber += 1
                self.lastReproducedFrame = out[1]
                return self.lastReproducedFrame
            # Loss
            else:
                self.lastReproducedNumber += 1
                self.insertInBuffer(out)
                return self.lastReproducedFrame
            
        else: 
            return None