"""
    FILE: user.py
    AUTHOR: Alba Ramos Pedroviejo - alba.ramosp@estudiante.uam.es
    AUTHOR: Nicolas Serrano Salas - nicolas.serranos@estudiante.uam.es
    DESCRIPTION: implements the user functions related with the discovery server
"""

from appJar import gui
import numpy as np
from server import *
from tkinter import messagebox

MainBgColor = "#94c9d1"
BtnBgColor = "#35747e"
FgColor = "white"

class User():
    def __init__(self):
        self.server = Server() # To obtain our IP address
        self.status = -1 # For error handling during login
        self.protocols = ["V0"] # Protocols supported
        self.nick = None

    """
    METHOD: getNick()
    DESCRIPTION: returns de nick of the registered user
    ARGS_OUT: username or None
    """
    def getNick(self):
        return self.nick

    """
    METHOD: loginWindow(windowSize)
    ARGS_IN: windowSize - dimensions of the window to be displayed
    DESCRIPTION: configures the login window
    """
    def loginWindow(self, windowSize):
        self.app = gui("TGV - The Good Videocall", windowSize)
        self.app.setGuiPadding(10,10)

        self.app.setBg(MainBgColor)
        self.app.setFont(size=14, underline=False, slant="roman")
        self.app.setPadding([4, 4])

        self.app.addLabel("title", "Iniciar sesión")
        self.app.getLabelWidget("title").config(font="Times 20 bold")
        
        self.app.addLabelEntry("Usuario")
        self.app.addLabelSecretEntry("Contraseña")

        self.app.addButtons(["Aceptar", "Salir"], self.buttonsLoginCallback)

        self.app.setButtonBg("Aceptar", BtnBgColor)
        self.app.setButtonFg("Aceptar", FgColor)
        self.app.setButtonBg("Salir", BtnBgColor)
        self.app.setButtonFg("Salir", FgColor)
    
    """
    METHOD: buttonsLoginCallback(button)
    ARGS_IN: button - selected button
    DESCRIPTION: controller of the login window
    """
    def buttonsLoginCallback(self, button):
        if button == "Salir":
            self.app.stop()
        elif button == "Aceptar":
            nick = self.app.getEntry("Usuario")
            password = self.app.getEntry("Contraseña")

            if self.server.connect(timeout=5) == -1:
                self.app.warningBox("Error 408", "Request Timeout")
                return

            # Our IP is stored when instancing the server object
            ip = self.server.getIp()

            # Sends request to log in user along with IP+port
            logRequest = ('REGISTER ' + nick + ' ' + ip + ' ' + str(TCPsrc) + ' ' + password + ' ' + str(self.protocols[0]))

            if len(self.protocols) > 1:
                prot = self.protocols[1:]
                for i in prot:
                    logRequest += ("#"+str(i)) # Appends supported protocols to message, separated by '#'
            
            self.server.send(logRequest.encode()) 

            # Gets server answer and closes connection
            response = self.server.receive()
            self.server.send(('QUIT').encode())
            self.server.disconnect()
            
            if response.decode() == 'NOK WRONG_PASS':
                self.app.warningBox("Error", "Contraseña incorrecta")
            else:
                self.status = 0
                self.nick = nick
                self.app.stop()  

    """
    METHOD: getUserData(nick)
    ARGS_IN: nick - nickname of the user
    DESCRIPTION: asks to the discovery server for the ip and port of the corresponding user
    ARGS_OUT: decoded response from the discovery server
    """
    def getUserData(self, nick):
        self.server.connect()
        self.server.send(('QUERY ' + nick + '').encode())
        data = self.server.receive()
        ret = data.decode().split(" ")

        self.server.send(('QUIT').encode())
        self.server.disconnect()
        
        if ret[0] == 'NOK':
            self.status = -1
        else: 
            return ret
    
    """
    METHOD: listUsers()
    DESCRIPTION: asks to the discovery server for the list of nicknames of all users
    ARGS_OUT: list of nicknames of all users
    """
    def listUsers(self):
        self.server.connect()
        self.server.send(('LIST_USERS').encode())
        data = self.server.receive(size=MAX_DATAGRAM)
        ret = data.decode().split(" ")
        
        self.server.send(('QUIT').encode())
        self.server.disconnect()

        if ret[0] == 'NOK':
            self.app.warningBox("Error", "Imposible listar usuarios", parent=None)
            return
        
        users = []
        # data[16:] Because we take out 'OK USER_LIST totalUsers'
        # nicks are separated using '#'
        for i in data[16:].decode().split("#"):
            users.append(i.split(" ")[0])
        
        return users

    """
    METHOD: getStatus()
    DESCRIPTION: returns the logged status of the user
    ARGS_OUT: 0 if the user correctly logged in, -1 if the user has not be logged
    """
    def getStatus(self):
        return self.status           

    """
    METHOD: start()
    DESCRIPTION: starts the login window
    """
    def start(self):
        self.app.go()


    """
    METHOD: getMaxCommonProtocol(destProtocols)
    ARGS_IN: destProtocols - list of protocols
    DESCRIPTION: returns the highest common protocol between the user's protocols and the passed ones
    ARGS_OUT: highest common protocol or None
    """
    def getMaxCommonProtocol(self, destProtocols):
        protocols = []

        # Store each destination user's protocols in a list
        for i in destProtocols.split("#"):
            protocols.append(i.split(" ")[0])
        
        # If the other peer has just one protocol, the above split won't work
        if len(protocols) == 0:
            protocols.append(destProtocols[0])

        # Check the highest common protocol
        bothProtocols=[]
        for i in range(0, len(protocols)):
            for j in range(0, len(self.protocols)):
                if(protocols[i] == self.protocols[j]):
                    bothProtocols.append(protocols[i])
        
        if len(bothProtocols) == 0:
            return None
        else :
            return bothProtocols[-1]