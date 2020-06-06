"""
    FILE: client.py
    AUTHOR: Alba Ramos Pedroviejo - alba.ramosp@estudiante.uam.es
    AUTHOR: Nicolas Serrano Salas - nicolas.serranos@estudiante.uam.es
    DESCRIPTION: implements a client to communicate with the SecureBox server via the command line
"""

import sys
from identity import *
from cypher import *
from file import *
import argparse
from argparse import RawTextHelpFormatter, SUPPRESS
import os

# Token is read from file token.txt
token = None

# Identity is created when the user is registered
identity = None


"""
    METHOD: loadToken()
    ARGS_IN: -
    DESCRIPTION: reads the user's token from file 'token.txt' and stores it in a global variable.
                 If the file is not found or it's empty, None value is stored.
    AGS_OUT: -
"""
def loadToken():
    global token

    try:
        f = open('token.txt', 'r')
        token = f.read()
        f.close()

        if len(token) == 0:
            token = None
        
    except (FileNotFoundError):
        print("ERROR: necesita tener un token de autenticacion.\n"+
              "Puede obtenerlo desde la pagina de SecureBox.")
        value = str(input("Si ya tiene un token, puede introducirlo a continuación o pulsar ENTER para salir: "))

        if (len(value) > 0):
            token = value
            f = open('token.txt', 'w')
            f.write(value)
            f.close()
        else:
            sys.exit()


"""
    METHOD: loadIdentity()
    ARGS_IN: -
    DESCRIPTION: reads the user's identity from privateKey.bin. This file is created when the user is 
                 registered. If the user has no identity, it has no public key, so the registration
                 must be the first action. Other functions will check that the identity has been 
                 created before allowing execution.
    AGS_OUT: -
"""
def loadIdentity():
    global identity

    try:
        f = open('privateKey.bin', 'rb')
        identity = f.read()
        f.close()
        
    except (FileNotFoundError):
        identity = None


"""
    METHOD: downloadsDirectoryCheck()
    ARGS_IN: -
    DESCRIPTION: creates a directory to store the files downloaded from the server if it does not already exist.
    AGS_OUT: -
"""
def downloadsDirectoryCheck():
    if not os.path.exists("./downloads"):
        os.makedirs("./downloads")


"""
    METHOD: createArgParser()
    ARGS_IN: -
    DESCRIPTION: creates the argument parser for the client program. Adds a description of the program,
                 requirements and supported arguments with an explanation of their functionality.
    AGS_OUT: parser created
"""
def createArgParser():
    # Format the messages of the argument parser
    parser = argparse.ArgumentParser(description=
            '+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n\n'+
            '                       SECUREBOX CLIENT                    \n\n\n'+
            '           Alba Ramos Pedroviejo & Nicolas Serrano Salas\n'+
            '                            REDES II 2020 \n\n'+
            '                   Todos los derechos reservados\n\n'+
            '+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n\n\n'+
            'Funcionalidad para comunicarse con SecureBox.\n'+
            'Permite gestionar usuarios e identidades, '+
            'subir y bajar ficheros en el servidor y cifrar y firmar ficheros en local.\n\n\n', 
            formatter_class=RawTextHelpFormatter, usage=SUPPRESS)

    parser._optionals.title = "Posibles argumentos"

    # Add the supported arguments
    parser.add_argument('--create_id', dest='create', nargs='+', default=False, help='Registra un usuario. Indicar nombre, apellido/s y email.')
    parser.add_argument('--search_id',dest='search', nargs='+',default = False, help='Busca un usuario. Indicar texto de busqueda.')
    parser.add_argument('--delete_id', dest='deleteUser', default=False, help='Borra un usuario. Indicar su ID.')

    parser.add_argument('--upload', dest='upload', default=False, help='Sube un fichero. Indicar su nombre. Usar junto a --dest_id')
    parser.add_argument('--download', dest='download', default=False, help='Descarga un fichero. Indicar ID del fichero a bajar. Usar junto a --source_id.')
    parser.add_argument('--delete_file', dest='deleteFile', default=False, help='Borra un fichero. Indicar el ID del mismo.')

    parser.add_argument('--source_id', dest='idSender', default=False, help='Indicar el ID del usuario que sube un fichero.')
    parser.add_argument('--dest_id', dest='idReceiver', default=False, help='Indicar el ID del usuario al que se destina un fichero.')
    parser.add_argument('--list_files', dest='listFiles', action='store_true', default=False, help='Listar ficheros que el usuario tiene en SecureBox. No necesita indicar nada.')

    parser.add_argument('--encrypt', dest='encrypt', default=False, help='Encriptar fichero. Indicar nombre del mismo. Usar junto a --dest_id')
    parser.add_argument('--sign', dest='sign', default=False, help='Firma digitalmente un fichero. Indicar nombre del mismo.')
    parser.add_argument('--enc_sign', dest='encryptSign', default=False, help='Firma y encripta un fichero. Indicar nombre del mismo. Usar junto a --dest_id')

    args = parser.parse_args()

    return args


"""
    METHOD: switchArgs(args)
    ARGS_IN: args - argument parser
    DESCRIPTION: handles the argument usage in the client program using a given argument parser
    AGS_OUT: -
"""
def switchArgs(args):
    global token
    global identity

    # Registers a user in the system. This must be the first action.
    # Creates the identity. On other functions, check that the identity is created before execution.
    if args.create:
        print()
        print("Ha seleccionado registrar un usuario")
        print()
        loadToken()

        if token is not None:
            # Full name includes one or two surnames
            if len(args.create)==3:
                name = args.create[0]+' '+args.create[1]
                register(name, args.create[-1], token)
            elif len(args.create)==4:
                name = args.create[0]+' '+args.create[1]+' '+args.create[2]
                register(name, args.create[-1], token)
            else:
                print("ERROR: se deben indicar nombre, apellido/s y email") 
        else:
            print("ERROR: es necesario tener un token para ejecutar esta accion")

    # Searchs for a user. Identity and token must be created
    elif args.search:
        print()
        print("Ha seleccionado buscar un usuario")
        print()
        loadToken()
        loadIdentity()

        if token is not None and identity is not None:
            string=args.search[0]
            i=1
            while i < len(args.search):
                string=string+" "+args.search[i]
                i+=1
            search(string, token)
        else:
            print("ERROR: es necesario tener un token y estar registrado para ejecutar esta accion")

    # Deletes a user. Identity and token must be created
    elif args.deleteUser:
        print()
        print("Ha seleccionado borrar un usuario")
        print()
        loadToken()
        loadIdentity()

        if token is not None and identity is not None:
            deleteUser(args.deleteUser, token)
        else:
            print("ERROR: es necesario tener un token y estar registrado para ejecutar esta accion")


    # Signs a file locally, so no token is needed
    elif args.sign:
        print()
        print("Ha seleccionado firmar un fichero")
        print()
        loadIdentity()

        if identity is not None:
            try:
                f = open(args.sign, 'rb')
                message = f.read()
                sign(message)
                f.close()
            except (FileNotFoundError, FileExistsError):
                print("ERROR: no se pudo abrir el fichero. Compruebe que existe y tiene permisos sobre el")
        else:
            print("ERROR: es necesario estar registrado para ejecutar esta accion")

    # Encrypts a file locally, so not identity is needed 
    elif args.encrypt:
        print()
        print("Ha seleccionado encriptar")
        print()
        loadToken()

        if token is not None:
            if not args.idReceiver:
                print("ERROR: indicar ID del usuario destinatario con --dest_id")
            else:
                try:
                    f = open(args.encrypt, 'rb')
                    message = f.read()
                    encrypt(token, args.encrypt, message, args.idReceiver)
                    f.close()
                except (FileNotFoundError, FileExistsError):
                    print("ERROR: no se pudo abrir el fichero. Compruebe que existe y tiene permisos sobre el")
        else:
            print("ERROR: es necesario tener un token para ejecutar esta accion")

    # Encrypts and signs a file locally
    elif args.encryptSign:
        print()
        print("Ha seleccionado firmar y encriptar")
        print()
        loadToken()
        loadIdentity()

        if token is not None and identity is not None:
            if not args.idReceiver:
                print("ERROR: indicar ID del usuario destinatario con --dest_id")
            else:
                try:
                    f = open(args.encryptSign, 'rb')
                    message = f.read()
                    encryptSign(args.encryptSign, message, args.idReceiver, token)
                    f.close()
                except (FileNotFoundError, FileExistsError):
                    print("ERROR: no se pudo abrir el fichero. Compruebe que existe y tiene permisos sobre el")
        else:
            print("ERROR: es necesario tener un token y estar registrado para ejecutar esta accion")
    
    # Uploads a file, so identity and token are needed
    elif args.upload:
        print()
        print("Solicitado envio de fichero a SecureBox ")
        print()
        loadToken()
        loadIdentity()

        if token is not None and identity is not None:
            if not args.idReceiver:
                print("ERROR: indicar ID del usuario destinatario con --dest_id")
            else:
                try:
                    f = open(args.upload, 'rb')
                    message = f.read()
                    upload(message, args.idReceiver, args.upload, token)
                    f.close()
                except (FileNotFoundError, FileExistsError):
                    print("ERROR: no se pudo abrir el fichero. Compruebe que existe y tiene permisos sobre el")
        else:
            print("ERROR: es necesario tener un token y estar registrado para ejecutar esta accion")

    # Lists a user's files, so identity and token are needed
    elif args.listFiles:
        print()
        print("Ha seleccionado listar los ficheros del usuario") 
        print()
        loadToken()
        loadIdentity()

        if token is not None and identity is not None:
            list(token)
        else:
            print("ERROR: es necesario tener un token y estar registrado para ejecutar esta accion")


    # Deletes a file, so identity and token are needed
    elif args.deleteFile:
        print()
        print("Ha seleccionado borrar un fichero del servidor") 
        print()
        loadToken()
        loadIdentity()

        if token is not None and identity is not None:
            deleteFile(args.deleteFile, token)
        else:
            print("ERROR: es necesario tener un token y estar registrado para ejecutar esta accion")

    # Downloads a file, so identity and token are needed
    elif args.download:
        print()
        print("Ha seleccionado descargar un fichero del servidor") 
        print()
        loadToken()
        loadIdentity()

        if token is not None and identity is not None:
            if not args.idSender:
                print("ERROR: indicar ID del usuario emisor con --source_id")
            else:
                downloadsDirectoryCheck() # create directory to store downloads, if it does not already exist
                download(args.download, args.idSender, token)
        else:
            print("ERROR: es necesario tener un token y estar registrado para ejecutar esta accion")

    # Unrecognized argument
    else:
        print()
        print("Introduzca la opcion -h para ver las distintas funcionalidades disponibles")
        print()


def main():
    args = createArgParser()

    switchArgs(args)


if __name__ == "__main__":
    main()
