'''
    ethernet.py
    Implementación del nivel Ethernet y funciones auxiliares para el envío y recepción de tramas Ethernet
    Autor: Javier Ramos <javier.ramos@uam.es>
    2019 EPS-UAM
'''

from rc1_pcap import *
import logging
import socket
import struct
from binascii import hexlify
import threading

#Tamaño máximo de una trama Ethernet (para las prácticas)
ETH_FRAME_MAX = 1514
#Tamaño mínimo de una trama Ethernet
ETH_FRAME_MIN = 60
PROMISC = 1
NO_PROMISC = 0
TO_MS = 10
#Dirección de difusión (Broadcast)
broadcastAddr = bytes([0xFF]*6)
#Diccionario que alamacena para un Ethertype dado qué función de callback se debe ejecutar
upperProtos = {}
levelInitialized = False

def getHwAddr(interface):
    '''
        Nombre: getHwAddr
        Descripción: Esta función obtiene la dirección MAC asociada a una interfaz
        Argumentos:
            -interface: Cadena con el nombre de la interfaz
        Retorno:
            -Dirección MAC de la itnerfaz
    '''
    s = socket.socket(socket.AF_PACKET, socket.SOCK_RAW)
    s.bind((interface,0))
    mac =  (s.getsockname()[4])
    s.close()
    return mac

'''
    Nombre: process_Ethernet_frame
    Descripción: Esta función se ejecutará cada vez que llegue una trama Ethernet.
        Esta función debe realizar, al menos, las siguientes tareas:
            -Extraer los campos de dirección Ethernet destino, origen y ethertype
            -Comprobar si la dirección destino es la propia o la de broadcast. En caso de que la trama no vaya en difusión o no sea para nuestra interfaz la descartaremos (haciendo un return).
            -Comprobar si existe una función de callback de nivel superior asociada al Ethertype de la trama:
                -En caso de que exista, llamar a la función de nivel superior con los parámetros que corresponde:
                    -us (datos de usuario)
                    -header (cabecera pcap_pktheader)
                    -payload (datos de la trama excluyendo la cabecera Ethernet)
                    -dirección Ethernet origen
                -En caso de que no exista retornar
    Argumentos:
        -us: datos de usuarios pasados desde pcap_loop (en nuestro caso será None)
        -header: estructura pcap_pkthdr que contiene los campos len, caplen y ts.
        -data: bytearray con el contenido de la trama Ethernet
    Retorno:
        -Ninguno
'''
def process_Ethernet_frame(us,header,data):
    destMAc = data[:6]
    originMAc = data[6:12]
    ethType= data[12:14]

    if destMAc != macAddress and destMAc != broadcastAddr:
        return

    ethTypeUnpacked = struct.unpack('h',ethType)
    if ethTypeUnpacked in upperProtos:
        """REQUESTCode = bytes([0x00,0x01])
        if data[20:22] != REQUESTCode:"""
        upperProtos[ethTypeUnpacked](us, header, data[14:],originMAc)

    return

def process_frame(us,header,data):
    '''
        Nombre: process_frame
        Descripción: Esta función se pasa a pcap_loop y se ejecutará cada vez que llegue una trama. La función
        ejecutará la función process_Ethernet_frame en un hilo nuevo para evitar interbloqueos entre 2 recepciones
        consecutivas de tramas dependientes. Esta función NO debe modifciarse
        Argumentos:
            -us: datos de usuarios pasados desde pcap_loop (en nuestro caso será None)
            -header: estructura pcap_pkthdr que contiene los campos len, caplen y ts.
            -data: bytearray con el contenido de la trama Ethernet
        Retorno:
            -Ninguno
    '''
    threading.Thread(target=process_Ethernet_frame,args=(us,header,data)).start()


class rxThread(threading.Thread):
    ''' Clase que implementa un hilo de recepción. De esta manera al iniciar el nivel Ethernet
        podemos dejar un hilo con pcap_loop que reciba los paquetes sin bloquear el envío.
        En esta clase NO se debe modificar código
    '''
    def __init__(self):
        threading.Thread.__init__(self)

    def run(self):
        global handle
        #Ejecuta pcap_loop. OJO: handle debe estar inicializado con el resultado de pcap_open_live
        if handle is not None:
            pcap_loop(handle,-1,process_frame,None)
    def stop(self):
        global handle
        #Para la ejecución de pcap_loop
        if handle is not None:
            pcap_breakloop(handle)


'''
    Nombre: registerCallback
    Descripción: Esta función recibirá el nombre de una función y su valor de ethertype asociado y añadirá en la tabla
        (diccionario) de protocolos de nivel superior el dicha asociación.
        Este mecanismo nos permite saber a qué función de nivel superior debemos llamar al recibir una trama de determinado tipo.
        Por ejemplo, podemos registrar una función llamada process_IP_datagram asociada al Ethertype 0x0800 y otra llamada process_arp_packet
        asocaida al Ethertype 0x0806.
    Argumentos:
        -callback_fun: función de callback a ejecutar cuando se reciba el Ethertype especificado.
            La función que se pase como argumento debe tener el siguiente prototipo: funcion(us,header,data,srcMac)
            Dónde:
                -us: son los datos de usuarios pasados por pcap_loop (en nuestro caso este valor será siempre None)
                -header: estructura pcap_pkthdr que contiene los campos len, caplen y ts.
                -data: payload de la trama Ethernet. Es decir, la cabecera Ethernet NUNCA se pasa hacia arriba.
                -srcMac: dirección MAC que ha enviado la trama actual.
            La función no retornará nada. Si una trama se quiere descartar basta con hacer un return sin valor y dejará de procesarse.
        -ethertype: valor de Ethernetype para el cuál se quiere registrar una función de callback.
    Retorno: Ninguno
'''
def registerCallback(callback_func, ethertype):
    global upperProtos
    #upperProtos es el diccionario que relaciona función de callback y ethertype
    upperProtos[struct.unpack('h',ethertype)] = callback_func


'''
    Nombre: startEthernetLevel
    Descripción: Esta función recibe el nombre de una interfaz de red e inicializa el nivel Ethernet.
        Esta función debe realizar , al menos, las siguientes tareas:
            -Comprobar si el nivel Ethernet ya estaba inicializado (mediante una variable global). Si ya estaba inicializado devolver -1.
            -Obtener y almacenar en una variable global la dirección MAC asociada a la interfaz que se especifica
            -Abrir la interfaz especificada en modo promiscuo usando la librería rc1-pcap
            -Arrancar un hilo de recepción (rxThread) que llame a la función pcap_loop.
            -Si todo es correcto marcar la variable global de nivel incializado a True
    Argumentos:
        -Interface: nombre de la interfaz sobre la que inicializar el nivel Ethernet
    Retorno: 0 si todo es correcto, -1 en otro caso
'''
def startEthernetLevel(interface):
    global macAddress,handle,levelInitialized,recvThread

    handle = None
    errbuf = bytearray()

    if levelInitialized == True:
        return -1

    macAddress = getHwAddr(interface)

    handle = pcap_open_live(interface, ETH_FRAME_MAX, PROMISC, TO_MS, errbuf)

    recvThread = rxThread() # funcion que abre el loop
    recvThread.daemon = True
    recvThread.start()

    levelInitialized = True
    return 0


'''
    Nombre: stopEthernetLevel
    Descripción_ Esta función parará y liberará todos los recursos necesarios asociados al nivel Ethernet.
        Esta función debe realizar, al menos, las siguientes tareas:
            -Parar el hilo de recepción de paquetes
            -Cerrar la interfaz (handle de pcap)
            -Marcar la variable global de nivel incializado a False
    Argumentos: Ninguno
    Retorno: 0 si todo es correcto y -1 en otro caso
'''

def stopEthernetLevel():
    global macAddress,handle,levelInitialized,recvThread

    #probar que la not funciona
    if levelInitialized == False or not handle:
        return -1

    else:
        recvThread.stop()
        pcap_close(handle)
        levelInitialized = False

    return 0


'''
    Nombre: sendEthernetFrame
    Descripción: Esta función construirá una trama Ethernet con lo datos recibidos y la enviará por la interfaz de red.
        Esta función debe realizar, al menos, las siguientes tareas:
            -Construir la trama Ethernet a enviar (incluyendo cabecera + payload). Los campos propios (por ejemplo la dirección Ethernet origen)
                deben obtenerse de las variables que han sido inicializadas en startEthernetLevel
            -Comprobar los límites de Ethernet. Si la trama es muy pequeña se debe rellenar con 0s mientras que
                si es muy grande se debe devolver error.
            -Llamar a pcap_inject para enviar la trama y comprobar el retorno de dicha llamada. En caso de que haya error notificarlo
    Argumentos:
        -data: datos útiles o payload a encapsular dentro de la trama Ethernet
        -len: longitud de los datos útiles expresada en bytes
        -etherType: valor de tipo Ethernet a incluir en la trama
        -dstMac: Dirección MAC destino a incluir en la trama que se enviará
    Retorno: 0 si todo es correcto, -1 en otro caso
'''
def sendEthernetFrame(data,len,etherType,dstMac):
    global macAddress,handle
    header = bytearray()

    header.extend(dstMac)
    header.extend(macAddress)
    header.extend(etherType)
    dataToSend = bytearray(header)
    dataToSend.extend(data)
    dataToSendLen = len+14
    if dataToSendLen > ETH_FRAME_MAX:
        return -1

    if dataToSendLen < ETH_FRAME_MIN:
        rest = ETH_FRAME_MIN -dataToSendLen
        dataToSend.extend(bytes([0]*rest))
        dataToSendLen = ETH_FRAME_MIN

    if pcap_inject(handle,bytes(dataToSend),dataToSendLen) == -1:
        logging.info('Error alenviar la traza')
        return -1
    
    return 0
