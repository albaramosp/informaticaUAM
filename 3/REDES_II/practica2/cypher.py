"""
    FILE: cypher.py
    AUTHOR: Alba Ramos Pedroviejo - alba.ramosp@estudiante.uam.es
    AUTHOR: Nicolas Serrano Salas - nicolas.serranos@estudiante.uam.es
    DESCRIPTION: implements the functionality associated with the
                 encryption and signature of files.
"""
from identity import *
from Cryptodome.Cipher import AES, PKCS1_OAEP
from Cryptodome.Signature import pkcs1_15
from Cryptodome.Random import get_random_bytes
from Cryptodome.Util.Padding import pad
import json
from base64 import b64encode

import requests
import os
from Cryptodome.PublicKey import RSA
from Cryptodome.Hash import SHA256

"""
    METHOD: sign(message)
    ARGS_IN: message - message to be digitally signed
    DESCRIPTION: given a message, generates the digital signature associated to the owner.
                 For this, it digests the file using SHA256 and encrypts it with the owner's private key.
                 The receiver should decrypt it using the sender's public key, and then verify that the
                 digest is the same as the one he does to the plain text file received with the signature.
    AGS_OUT: -1 if error, 1  if successfully signed
"""
def sign(message):
    privateKey = RSA.importKey(open("privateKey.bin", "rb").read()) # Use the private key of the user
    h = SHA256.new(message) # Hash the message using SHA256
    signature = pkcs1_15.new(privateKey).sign(h)
    
    with open("signature.bin", 'wb') as f:
        f.write(signature)
        f.close()

    print("Firmando fichero... OK")
    return 1


"""
    METHOD: encrypt(token, filename, data, id, signature)
    ARGS_IN: token - tooken to call the API to get the receiver's public key
             filename - name of the file to be encrypted
             data - message to be encrypted
             id - userID to obtain the public key of the receiver of the encrypted file
             signature - indicates whether a digital signature should be added to the message
    DESCRIPTION: encrypts a message using AES.
                 For the encryption, concats a Initialization Vector (public) with the digital
                 envelope consisting on the symmetric key encrypted with the receiver's public key (RSA).
                 Then, appends the signature if exists, and then the message, these encrypted with the
                 symmetric key. Adds padding to the message to make it multiple of 16.
    AGS_OUT: -1 if error, 1  if successfully encrypted
"""
def encrypt(token, filename, data, id, signature=None):
    # Symmetric key
    key = SHA256.new(get_random_bytes(32)).digest() # 32 bytes is used to generate a key of 256 bits

    iv=get_random_bytes(16) # initialization vector of 16 bytes, public
    
    # Encrypt using CBC, IV of 16 bytes and symmetric key of 256 bytes
    cipher = AES.new(key, AES.MODE_CBC, iv=iv)

    if signature is not None:
        signatureMessage = cipher.encrypt(pad(signature+data, 16)) # Adds padding to be multiple of 16
    else:
        signatureMessage = cipher.encrypt(pad(data, 16)) # Adds padding

    # Encrypt the symmetric key with the receiver's public key
    publicKey = getPublicKey(id, token)

    if publicKey != -1 and publicKey:
        publicKey = RSA.importKey(publicKey)
        cipher_rsa = PKCS1_OAEP.new(publicKey)
        encryptedKey = cipher_rsa.encrypt(key)

        # Store the result:
        # - public IV used
        # - digital envelope (simmetric key encrypted with receiver's public key)
        # - encrypted with the symmetric key:
        #       - digital signature (if present)
        #       - plain message
        with open(filename, 'wb') as f:
            f.write(iv+encryptedKey+signatureMessage)
            print("Cifrando fichero... OK")
            f.close()
            if signature is not None:
                os.remove("signature.bin")
            return 1
    else:
        return -1


"""
    METHOD: decrypt(message, userId, token)
    ARGS_IN: message - encrypted content
             userId - userID to obtain the public key of the sender of the encrypted file
             token - token to call the API to get the sender's public key
    DESCRIPTION: decrypts a message using the symmetric key encrypted with our public key. 
                 Verifies the digital signature using the sender's public key, obtaing
                 using his userID.
    AGS_OUT: None if error, plain message  if successfully decrypted and signature is valid
"""
def decrypt(message, userId, token):
    print("Descifrando fichero...", end="", flush=True)
    # IV is 16 bytes long and is at the beginning, not encrypted
    iv = message[:16]

    # Decrypt the digital envelope of 256 bytes using our private key
    privateKey = RSA.importKey(open("privateKey.bin", "rb").read())
    cipher_rsa = PKCS1_OAEP.new(privateKey)
    try:
        symmetricKey = cipher_rsa.decrypt(message[16:272])
    except ValueError:
        print("ERROR. Su clave privada no corresponde con la clave publica usada para cifrar el fichero.")
        print("¿Esta seguro de que este fichero iba dirigido a usted?")
        return None

    print("OK")
    # Decrypt the rest of the message using the symmetric key
    cipher = AES.new(symmetricKey, AES.MODE_CBC, iv)
    signatureMessage = cipher.decrypt(message[272:])

    signature = signatureMessage[:256] # Signature is 256 bytes
    plain = signatureMessage[256:] # Rest is the plain message
    plain = plain[:-plain[-1]]

    # Verify the digital signature using the sender's public key
    publicKey = getPublicKey(userId, token)

    if publicKey != -1:
        print("Verificando firma...", end="", flush=True)

        publicKey = RSA.importKey(publicKey)
        h = SHA256.new(plain)

        try:
            pkcs1_15.new(publicKey).verify(h, signature)
            print ("OK")
            return plain

        except (ValueError, TypeError):
            print ("ERROR: la firma no es valida.")
            return None
    else:
        return None


"""
    METHOD: encryptSign(filename, message, id, token)
    ARGS_IN: filename - name of file to be encrypted
             message - message to be encrypted
             id - userID to obtain the public key of the receiver of the encrypted file
             token - token to call the API
    DESCRIPTION: given a message, signs and encrypts it for the user with the given id to be able
                 to decrypt it.
    AGS_OUT: -1 if error, 1  if successfully signed and encrypted
"""
def encryptSign(filename, message, id, token):
    sign(message) # Generates digital signature in signature.bin
    f = open("signature.bin", 'rb')
    signature = f.read()
    f.close()

    return encrypt(token, filename, message, id, signature)
    
