"""
    FILE: identity.py
    AUTHOR: Alba Ramos Pedroviejo - alba.ramosp@estudiante.uam.es
    AUTHOR: Nicolas Serrano Salas - nicolas.serranos@estudiante.uam.es
    DESCRIPTION: implements the functionality associated with the identity 
                 of users of the SecureBox server. Supports the creation and deletion of users, the search
                 of users and the obtention of a user's public key.
"""

import json
import requests
from Cryptodome.PublicKey import RSA
from error import *

# SERVER_URL = 'https://tfg.eps.uam.es:8080/api/users'
SERVER_URL = 'https://vega.ii.uam.es:8080/api/users'
    

"""
    METHOD: getPublicKey(id, token)
    ARGS_IN: id - string with the ID of the user whom we want the public key
             token - token to call the API
    DESCRIPTION: given a user's ID, obtains the public key of the user. Typically called when we want
                 to obtain the public key of a user to send him a file.
    AGS_OUT: public key of the user, -1 if error
"""
def getPublicKey(id, token): 
    print("Recuperando clave publica de ID#"+str(id)+"...", end="", flush=True)

    # Build request
    args = {"userID": id}
    headers = {"Authorization": "Bearer "+token, "content-type": "application/json"}

    r = requests.post(SERVER_URL+"/getPublicKey", data=json.dumps(args), headers=headers)
     
    try:
        ret = json.loads(r.content.decode('utf-8'))
    except ValueError:
        print("Error desconocido")
        return -1
    
    if r.status_code != 200:
        print("ERROR. ", end="", flush=True)
        tratarError(ret)
        return -1
    else:
        key = ret['publicKey']
        if key is not None:
            print("OK")
            return key
        else:
            print("ERROR. Este usuario no esta registrado y no tiene clave publica asociada.")
            return -1


"""
    METHOD: register(name, email, token)
    ARGS_IN: name - full name of the user to be registered
             email - email of the user to be registered
             token - token to call the API
    DESCRIPTION: registers a user in the server, given its name and email. For each user, generates
                 a pair of public-private keys using RSA and associates the user to its public key 
                 in the server, so other users can get it. Stores the private key in a file only 
                 available to its owner.
                 Shows the ID -NIA- associated to the registered user.
    AGS_OUT: -1 if error, 1  if registration was successfully completed
"""
def register(name, email, token):
    print("Generando par de claves RSA de 2048 bits...", end="", flush=True)
    privateKey = RSA.generate(2048) # RSA asymmetric key of 2048 bits
    encryptedPrivateKey = privateKey.exportKey('PEM') # PEM format required for RSA
    
    # Stores user's private key in a file in his computer
    f = open('privateKey.bin', 'wb')
    f.write(encryptedPrivateKey)
    f.close()

    # Generates a public key from the private one using the same format
    publicKey = (privateKey.publickey()).exportKey('PEM')
    
    publicKey = publicKey.decode('utf-8')
    
    print("OK")
    print("Creando identidad...", end="", flush=True)

    # Build request
    args = {"nombre": name, "email": email, "publicKey": publicKey}
    headers = {"Authorization": "Bearer "+token, "content-type": "application/json"}
    
    r = requests.post(SERVER_URL+"/register", data=json.dumps(args), headers=headers)
    try:
        ret = json.loads(r.content.decode('utf-8'))
    except ValueError:
        print("Error desconocido")
        return -1
    
    if r.status_code != 200:
        print("ERROR. ", end="", flush=True)
        tratarError(ret)
        return -1
    else:
        # Shows the userID or NIA of the registered user
        print("OK. Creada correctamente identidad con ID#"+str(ret['userID']))
        return 1


"""
    METHOD: deleteUser(id, token)
    ARGS_IN: id - userID of the user to be deleted
             token - token to call the API
    DESCRIPTION: deletes the user with the given ID from the server.
    AGS_OUT: -1 if error, 1  if user was successfully deleted
"""
def deleteUser(id, token):
    print("Eliminando usuario...", end="", flush=True)

    # Build request
    args = {"userID": id}
    headers = {"Authorization": "Bearer "+token, "content-type": "application/json"}
    
    r = requests.post(SERVER_URL+"/delete", data=json.dumps(args), headers=headers)
    try:
        ret = json.loads(r.content.decode('utf-8'))
    except ValueError:
        print("Error desconocido")
        return -1
    
    if r.status_code != 200:
        print("ERROR. ", end="", flush=True)
        tratarError(ret)
        return -1
    else:
        print("OK. Eliminado usuario con ID "+ret['userID'])
        return 1


"""
    METHOD: search(searchField, token)
    ARGS_IN: searchField - string to search in the username or email of the users of the server
             token - token to call the API
    DESCRIPTION: searches for a registered user with the string contained in his name or email. For the 
                 users found, shows their name, email, ID and public key.
    AGS_OUT: -1 if error, 1  if search was successful.
"""
def search(searchField, token):
    print("Buscando usuario "+str(searchField)+" en el servidor...", end="", flush=True)

    # Build request
    args = {"data_search": searchField}
    headers = {"Authorization": "Bearer "+token, "content-type": "application/json"}

    r = requests.post(SERVER_URL+"/search", data=json.dumps(args), headers=headers)
    try:
        ret = json.loads(r.content.decode('utf-8'))
    except ValueError:
        print("Error desconocido")
        return -1

    if r.status_code != 200:
        print("ERROR. ", end="", flush=True)
        tratarError(ret)
        return -1
    else:
        print("OK")
        if len(ret) == 0:
            print("No se han encontrado usuarios")
        else:
            # Show found users, printing with str as some users do not have certain fields
            print("Usuarios encontrados:")
            c=1
            for i in ret:
                print("["+str(c)+"] "+str(i['nombre'])+", "
                                    +str(i['email'])+", ID: "
                                    +str(i['userID'])+" Key: "
                                    +str(i['publicKey']))
                c+=1
                print()
        return 1

