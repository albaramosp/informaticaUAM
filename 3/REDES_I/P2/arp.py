'''
    arp.py
    Implementación del protocolo ARP y funciones auxiliares que permiten realizar resoluciones de direcciones IP.
    Autor: Javier Ramos <javier.ramos@uam.es>
    2019 EPS-UAM
'''



from ethernet import *
import logging
import socket
import struct
import fcntl
import time
from threading import Lock
from expiringdict import ExpiringDict
from time import sleep

#Semáforo global
globalLock =Lock()
#Dirección de difusión (Broadcast)
broadcastAddr = bytes([0xFF]*6)
#Cabecera ARP común a peticiones y respuestas. Específica para la combinación Ethernet/IP
ARPHeader = bytes([0x00,0x01,0x08,0x00,0x06,0x04])
#Ethertype carrespondiente al protocolo ARP
ARPEThType = bytes([0x08,0x06])
#Hardware carrespondiente 
HARDWType = bytes([0x00,0x01])
#Protocolo carrespondiente  ethernet
PROTOCOLType = bytes([0x08,0x00])
#Hardware size
HARDWSize = bytes([0x06])
#Protocol size
PROTOCOLSize = bytes([0x04])
#longitud (en bytes) de la cabecera común ARP
ARP_HLEN = 6


#Opcode para arpRequest
REQUESTCode = bytes([0x00,0x01])
#Opcode para arpReply
REPLYCode = bytes([0x00,0x02])

#Variable que alamacenará que dirección IP se está intentando resolver
requestedIP = None
#Variable que alamacenará que dirección MAC resuelta o None si no se ha podido obtener
resolvedMAC = None
#Variable que alamacenará True mientras estemos esperando una respuesta ARP
awaitingResponse = False

#Variable para proteger la caché
cacheLock = Lock()
#Caché de ARP. Es un diccionario similar al estándar de Python solo que eliminará las entradas a los 10 segundos
cache = ExpiringDict(max_len=100, max_age_seconds=10)



def getIP(interface):
    '''
        Nombre: getIP
        Descripción: Esta función obtiene la dirección IP asociada a una interfaz. Esta funció NO debe ser modificada
        Argumentos:
            -interface: nombre de la interfaz
        Retorno: Entero de 32 bits con la dirección IP de la interfaz
    '''
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    ip = fcntl.ioctl(
        s.fileno(),
        0x8915,  # SIOCGIFADDR
        struct.pack('256s', (interface[:15].encode('utf-8')))
    )[20:24]
    s.close()
    return struct.unpack('!I',ip)[0]

def printCache():
    '''
        Nombre: printCache
        Descripción: Esta función imprime la caché ARP
        Argumentos: Ninguno
        Retorno: Ninguno
    '''
    print('{:>12}\t\t{:>12}'.format('IP','MAC'))
    with cacheLock:
        for k in cache:
            if k in cache:
                print ('{:>12}\t\t{:>12}'.format(socket.inet_ntoa(struct.pack('!I',k)),':'.join(['{:02X}'.format(b) for b in cache[k]])))


'''
    Nombre: processARPRequest
    Decripción: Esta función procesa una petición ARP. Esta función debe realizar, al menos, las siguientes tareas:
        -Extraer la MAC origen contenida en la petición ARP
        -Si la MAC origen de la trama ARP no es la misma que la recibida del nivel Ethernet retornar
        -Extraer la IP origen contenida en la petición ARP
        -Extraer la IP destino contenida en la petición ARP
        -Comprobar si la IP destino de la petición ARP es la propia IP:
            -Si no es la propia IP retornar
            -Si es la propia IP:
                -Construir una respuesta ARP llamando a createARPReply (descripción más adelante)
                -Enviar la respuesta ARP usando el nivel Ethernet (sendEthernetFrame)
    Argumentos:
        -data: bytearray con el contenido de la trama ARP (después de la cabecera común)
        -MAC: dirección MAC origen extraída por el nivel Ethernet
    Retorno: Ninguno
'''
def processARPRequest(data,MAC):
    global myIP
    senderMac = data[8:14]
    
    if senderMac == MAC:
        ipOrigen = data[14:18]
        targetIP = data[24:28]
        myIPUnpacked = struct.pack('!I', myIP)
       
        if targetIP == myIPUnpacked:
            arpFrame = createARPReply(ipOrigen, senderMac)
            sendEthernetFrame(arpFrame,len(arpFrame),ARPEThType,senderMac)


'''
    Nombre: processARPReply
    Decripción: Esta función procesa una respuesta ARP. Esta función debe realizar, al menos, las siguientes tareas:
        -Extraer la MAC origen contenida en la petición ARP
        -Si la MAC origen de la trama ARP no es la misma que la recibida del nivel Ethernet retornar
        -Extraer la IP origen contenida en la petición ARP
        -Extraer la MAC destino contenida en la petición ARP
        -Extraer la IP destino contenida en la petición ARP
        -Comprobar si la IP destino de la petición ARP es la propia IP:
            -Si no es la propia IP retornar
            -Si es la propia IP:
                -Comprobar si la IP origen se corresponde con la solicitada (requestedIP). Si no se corresponde retornar
                -Copiar la MAC origen a la variable global resolvedMAC
                -Añadir a la caché ARP la asociación MAC/IP.
                -Cambiar el valor de la variable awaitingResponse a False
                -Cambiar el valor de la variable requestedIP a None
    Las variables globales (requestedIP, awaitingResponse y resolvedMAC) son accedidas concurrentemente por la función ARPResolution y deben ser protegidas mediante un Lock.
    Argumentos:
        -data: bytearray con el contenido de la trama ARP (después de la cabecera común)
        -MAC: dirección MAC origen extraída por el nivel Ethernet
    Retorno: Ninguno
'''
def processARPReply(data,MAC):
    global requestedIP,resolvedMAC,awaitingResponse,cache, myIP
    senderMac = data[8:14]
    if senderMac == MAC:
        ipOrigen = data[14:18]
        targetIP = data[24:28]
        targetMAc = data[18:24]
        myIPUnpacked = struct.pack('!I', myIP)
        
        if targetIP == myIPUnpacked:
            if requestedIP and ipOrigen == struct.pack('!I', requestedIP):
                globalLock.acquire()
                resolvedMAC = senderMac
                globalLock.release()
                cacheLock.acquire()
                cache[requestedIP] = senderMac
                cacheLock.release()
                globalLock.acquire()
                awaitingResponse = False
                requestedIP = None
                globalLock.release()


'''
    Nombre: createARPRequest
    Descripción: Esta función construye una petición ARP y devuelve la trama con el contenido.
    Argumentos:
        -ip: dirección a resolver
    Retorno: Bytes con el contenido de la trama de petición ARP
'''
def createARPRequest(ip):
    global myMAC,myIP
    frame = bytearray()

    frame.extend(ARPHeader)
    frame.extend(REQUESTCode)
    frame.extend(myMAC)
    frame.extend(struct.pack('!I', myIP))
    frame.extend(bytes([0]*6))    
    frame.extend(struct.pack('!I', ip))
    return frame


'''
    Nombre: createARPReply
    Descripción: Esta función construye una respuesta ARP y devuelve la trama con el contenido.
    Argumentos:
        -IP: dirección IP a la que contestar
        -MAC: dirección MAC a la que contestar
    Retorno: Bytes con el contenido de la trama de petición ARP
'''
def createARPReply(IP,MAC):
    global myMAC,myIP
    frame = bytearray()

    frame.extend(ARPHeader)
    frame.extend(REPLYCode)
    frame.extend(myMAC)
    frame.extend(struct.pack('!I', myIP))
    frame.extend(MAC)
    frame.extend(IP)  
    return frame

'''
    Nombre: process_arp_frame
    Descripción: Esta función procesa las tramas ARP.
        Se ejecutará por cada trama Ethenet que se reciba con Ethertype 0x0806 (si ha sido registrada en initARP).
        Esta función debe realizar, al menos, las siguientes tareas:
            -Extraer la cabecera común de ARP (6 primeros bytes) y comprobar que es correcta
            -Extraer el campo opcode
            -Si opcode es 0x0001 (Request) llamar a processARPRequest (ver descripción más adelante)
            -Si opcode es 0x0002 (Reply) llamar a processARPReply (ver descripción más adelante)
            -Si es otro opcode retornar de la función
            -En caso de que no exista retornar
    Argumentos:
        -us: Datos de usuario pasados desde la llamada de pcap_loop. En nuestro caso será None
        -header: cabecera pcap_pktheader
        -data: array de bytes con el contenido de la trama ARP
        -srcMac: MAC origen de la trama Ethernet que se ha recibido
    Retorno: Ninguno
'''
def process_arp_frame(us,header,data,srcMac):
    hardwareType = data[:2]
    protocolType = data[2:4]
    hardwareSize = data[4:5]
    protocolSize = data[5:6]
    if hardwareType == HARDWType and protocolType == PROTOCOLType and hardwareSize == HARDWSize and protocolSize == PROTOCOLSize:
        opCode = data[6:8]
        if opCode == REPLYCode:
            processARPReply(data, srcMac)
        elif opCode == REQUESTCode:
            processARPRequest(data, srcMac)
        




'''
    Nombre: initARP
    Descripción: Esta función construirá inicializará el nivel ARP. Esta función debe realizar, al menos, las siguientes tareas:
        -Registrar la función del callback process_arp_frame con el Ethertype 0x0806
        -Obtener y almacenar la dirección MAC e IP asociadas a la interfaz especificada
        -Realizar una petición ARP gratuita y comprobar si la IP propia ya está asignada. En caso positivo se debe devolver error.
        -Marcar la variable de nivel ARP inicializado a True
'''
def initARP(interface):
    global myIP, myMAC, arpInitialized, levelInitialized
    arpInitialized = False
    registerCallback(process_arp_frame, ARPEThType)
    myIP = getIP(interface)
    myMAC = getHwAddr(interface)
    if ARPResolution(myIP):
        logging.info('Esta ip ya esta en uso')
        return False
    arpInitialized = True
    return True


'''
    Nombre: ARPResolution
    Descripción: Esta función intenta realizar una resolución ARP para una IP dada y devuelve la dirección MAC asociada a dicha IP
        o None en caso de que no haya recibido respuesta. Esta función debe realizar, al menos, las siguientes tareas:
            -Comprobar si la IP solicitada existe en la caché:
            -Si está en caché devolver la información de la caché
            -Si no está en la caché:
                -Construir una petición ARP llamando a la función createARPRequest (descripción más adelante)
                -Enviar dicha petición
                -Comprobar si se ha recibido respuesta o no:
                    -Si no se ha recibido respuesta reenviar la petición hasta un máximo de 3 veces. Si no se recibe respuesta devolver None
                    -Si se ha recibido respuesta devolver la dirección MAC
        Esta función necesitará comunicarse con el la función de recepción (para comprobar si hay respuesta y la respuesta en sí) mediante 3 variables globales:
            -awaitingResponse: indica si está True que se espera respuesta. Si está a False quiere decir que se ha recibido respuesta
            -requestedIP: contiene la IP por la que se está preguntando
            -resolvedMAC: contiene la dirección MAC resuelta (en caso de que awaitingResponse) sea False.
        Como estas variables globales se leen y escriben concurrentemente deben ser protegidas con un Lock
'''
def ARPResolution(ip):
    global requestedIP,awaitingResponse,resolvedMAC
    
    if ip in cache:
        cacheLock.acquire()
        cacheConsult = cache[ip]
        cacheLock.release()
        return cacheConsult

    globalLock.acquire()
    requestedIP = ip
    awaitingResponse = True
    globalLock.release()
    arpFrame = createARPRequest(requestedIP)
    for n in range(0,3):
        sendEthernetFrame(arpFrame,len(arpFrame),ARPEThType,broadcastAddr)
        sleep(1)
        if awaitingResponse == False:
            return resolvedMAC

    return None
