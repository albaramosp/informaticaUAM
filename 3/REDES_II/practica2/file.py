"""
    FILE: file.py
    AUTHOR: Alba Ramos Pedroviejo - alba.ramosp@estudiante.uam.es
    AUTHOR: Nicolas Serrano Salas - nicolas.serranos@estudiante.uam.es
    DESCRIPTION: implements the functionality associated with the files 
                 that users can upload and download from the server. Supports file upload and download,
                 file deletion and file listing.
"""

from Cryptodome.Cipher import AES
from Cryptodome.Random import get_random_bytes
from Cryptodome.Util.Padding import pad
import json
from base64 import b64encode

import requests
from Cryptodome.PublicKey import RSA
from cypher import *
from error import *

# SERVER_URL = 'https://tfg.eps.uam.es:8080/api/files'
SERVER_URL = 'https://vega.ii.uam.es:8080/api/files'


"""
    METHOD: upload(data, nia, filename, token)
    ARGS_IN: data - content of the file to be uploaded
             nia - id of the sender, to sign the file that is uploaded.
             filename - name of the file that is uploaded, so that the encrypted file is the
             same as the existing one and not a new one.
             token - token to call the API
    DESCRIPTION: uploads a file to SecureBox. The file is digitally signed by the sender and
                 encrypted USING AES.
"""
def upload(data, nia, filename, token):
    if encryptSign(filename, data, nia, token) != -1:
        print("Subiendo fichero al servidor...", end="", flush=True)

        # Build request
        headers = {"Authorization": "Bearer "+token}
        r = requests.post(SERVER_URL+"/upload", files={'ufile': open(filename, 'rb')}, headers=headers)

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
            print("Subida realizada correctamente, ID del fichero: "+str(ret['file_id']))
            return 1
    else:
        return -1

"""
    METHOD: download(idFile, udUser, token)
    ARGS_IN: idFile - id of the file to be downloaded
             idUser - id of the user from whom we want to verify the digital signature.
             token - token to call the API
    DESCRIPTION: downloads from SecureBox a file given its fileID. The file is encrypted, so it must 
                 be decrypted. It is also digitally signed by the sender, so we should validate
                 the signature using his public key.
"""
def download(idFile, idUser, token):
    # Build request
    headers = {"Authorization": "Bearer "+token, "content-type": "application/json"}
    
    args = {'file_id':idFile}
    r = requests.post(SERVER_URL+"/download", data=json.dumps(args), headers=headers)
    
    if r.status_code != 200:
        try:
            ret = json.loads(r.content.decode('utf-8'))
        except ValueError:
            print("Error desconocido")
            return -1
        
        print("ERROR. ", end="", flush=True)
        tratarError(ret)   
        return -1
    else:
        print("Descargando fichero de SecureBox... OK")
        message = r.content
        print(str(len(message))+" bytes descargados correctamente")

        secret = decrypt(message, idUser, token)
        if secret is not None:
            # Obtain the name associated to the downloaded file, it's in the response headers
            name = str((r.headers['Content-Disposition'].split("filename=\"", 1)[1])[:-1])

            with open("./downloads/"+name, "wb") as f:
                f.write(secret)
                print("Fichero '"+name+"' descargado y verificado correctamente")
                f.close()       
        

"""
    METHOD: list(token)
    ARGS_IN: token - token to call the API
    DESCRIPTION: lists the name and ID of the files that a user has uploaded to SecureBox
    AGS_OUT: -1 if error, 1  if listing was successful
"""
def list(token):
    print("Listando ficheros del usuario...", end="", flush=True)

    # Build request
    headers = {"Authorization": "Bearer "+token, "content-type": "application/json"}
    
    r = requests.get(SERVER_URL+"/list", headers=headers) 
    
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
        # Show user's files with their name and ID
        if ret['num_files'] == 0:
            print("El usuario no tiene ficheros subidos a SecureBox")
        else:
            print(str(ret['num_files'])+" ficheros encontrados:")
            c=1
            for i in ret['files_list']:
                print("["+str(c)+"] "+str(i['fileName'])+", ID#"
                                    +str(i['fileID']))
                c+=1
                print()
        return 1


"""
    METHOD: deleteFile(id, token)
    ARGS_IN: id - ID of the file to be deleted 
             token - token to call the API
    DESCRIPTION: deletes a file with a given id from the SecureBox server
    AGS_OUT: -1 if error, 1  if deletion was successful
"""
def deleteFile(id, token):
    print("Eliminando fichero del servidor...", end="", flush=True)

    # Build request
    args = {'file_id':id}
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
        ret = json.loads(r.content.decode('utf-8'))
        print("OK")
        print("Fichero con ID#"+str(ret['file_id'])+" eliminado correctamente")
        
        return 1


