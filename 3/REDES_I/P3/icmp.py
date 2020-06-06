from ip import *
from threading import Lock
import struct

ICMP_PROTO = 1


ICMP_ECHO_REQUEST_TYPE = 8
ICMP_ECHO_REPLY_TYPE = 0

timeLock = Lock()
icmp_send_times = {}

'''
    Nombre: process_ICMP_message
    Descripción: Esta función procesa un mensaje ICMP. Esta función se ejecutará por cada datagrama IP que contenga
    un 1 en el campo protocolo de IP
    Esta función debe realizar, al menos, las siguientes tareas:
        -Calcular el checksum de ICMP:
            -Si es distinto de 0 el checksum es incorrecto y se deja de procesar el mensaje
        -Extraer campos tipo y código de la cabecera ICMP
        -Loggear (con logging.debug) el valor de tipo y código
        -Si el tipo es ICMP_ECHO_REQUEST_TYPE:
            -Generar un mensaje de tipo ICMP_ECHO_REPLY como respuesta. Este mensaje debe contener
            los datos recibidos en el ECHO_REQUEST. Es decir, "rebotamos" los datos que nos llegan.
            -Enviar el mensaje usando la función sendICMPMessage
        -Si el tipo es ICMP_ECHO_REPLY_TYPE:
            -Extraer del diccionario icmp_send_times el valor de tiempo de envío usando como clave los campos srcIP e icmp_id e icmp_seqnum
            contenidos en el mensaje ICMP. Restar el tiempo de envio extraído con el tiempo de recepción (contenido en la estructura pcap_pkthdr)
            -Se debe proteger el acceso al diccionario de tiempos usando la variable timeLock
            -Mostrar por pantalla la resta. Este valor será una estimación del RTT
        -Si es otro tipo:
            -No hacer nada

    Argumentos:
        -us: son los datos de usuarios pasados por pcap_loop (en nuestro caso este valor será siempre None)
        -header: estructura pcap_pkthdr que contiene los campos len, caplen y ts.
        -data: array de bytes con el conenido del mensaje ICMP
        -srcIP: dirección IP que ha enviado el datagrama actual.
    Retorno: Ninguno
        
'''
def process_ICMP_message(us,header,data,srcIp):
    if chksum(data[0:]) == 0:
        if chksum(data) != 0:
            return
        tipo = data[0]
        code = data[1]
        checksum = data[2:4]
        identifier = struct.unpack('!H', data[4:6])[0]
        sequenceNum = struct.unpack('!H', data[6:8])[0]

        logging.debug("Valor de tipo: %d"%tipo)
        logging.debug("Valor de codigo: %d"%code)

        if tipo == ICMP_ECHO_REQUEST_TYPE:
            sendICMPMessage(data[8:], ICMP_ECHO_REPLY_TYPE, code, identifier, sequenceNum, struct.unpack('!I',srcIp)[0])

        elif tipo == ICMP_ECHO_REPLY_TYPE:
            icmpsend = struct.unpack('!I', srcIp)[0]+identifier+sequenceNum
            timeLock.acquire()
            sendTime = icmp_send_times[icmpsend]
            timeLock.release()

            receiveTime = header.ts.tv_sec
            rtt = float(receiveTime) - sendTime
            logging.debug("RTT aproximado: %f"%rtt)

    
'''
    Nombre: sendICMPMessage
    Descripción: Esta función construye un mensaje ICMP y lo envía.
    Esta función debe realizar, al menos, las siguientes tareas:
        -Si el campo type es ICMP_ECHO_REQUEST_TYPE o ICMP_ECHO_REPLY_TYPE:
            -Construir la cabecera ICMP
            -Añadir los datos al mensaje ICMP
            -Calcular el checksum y añadirlo al mensaje donde corresponda
            -Si type es ICMP_ECHO_REQUEST_TYPE
                -Guardar el tiempo de envío (llamando a time.time()) en el diccionario icmp_send_times
                usando como clave el valor de dstIp+icmp_id+icmp_seqnum
                -Se debe proteger al acceso al diccionario usando la variable timeLock

            -Llamar a sendIPDatagram para enviar el mensaje ICMP
            
        -Si no:
            -Tipo no soportado. Se devuelve False

    Argumentos:
        -data: array de bytes con los datos a incluir como payload en el mensaje ICMP
        -type: valor del campo tipo de ICMP
        -code: valor del campo code de ICMP 
        -icmp_id: entero que contiene el valor del campo ID de ICMP a enviar
        -icmp_seqnum: entero que contiene el valor del campo Seqnum de ICMP a enviar
        -dstIP: entero de 32 bits con la IP destino del mensaje ICMP
    Retorno: True o False en función de si se ha enviado el mensaje correctamente o no
        
'''
def sendICMPMessage(data,type,code,icmp_id,icmp_seqnum,dstIP):
    message = bytearray()

    if type == ICMP_ECHO_REPLY_TYPE or type == ICMP_ECHO_REQUEST_TYPE:
        header = bytearray()
        header.extend(bytes([type]))
        header.extend(bytes([code]))
        header.extend(bytes([0x0]*2))
        header.extend(struct.pack('!H', icmp_id))
        header.extend(struct.pack('!H', icmp_seqnum))
        dataLength = len(data)

        if (len(header)+dataLength)%2 != 0:
            header.extend(0)
        
        header.extend(data)
        header[2:4] = struct.pack('<H', chksum(header))

        if type == ICMP_ECHO_REQUEST_TYPE:
            icmpsend = dstIP+icmp_id+icmp_seqnum
            timeLock.acquire()
            icmp_send_times[icmpsend] = time.time()
            timeLock.release()
        
        return sendIPDatagram(dstIP, header, ICMP_PROTO)

    return False


'''
    Nombre: initICMP
    Descripción: Esta función inicializa el nivel ICMP
    Esta función debe realizar, al menos, las siguientes tareas:
        -Registrar (llamando a registerIPProtocol) la función process_ICMP_message con el valor de protocolo 1

    Argumentos:
        -Ninguno
    Retorno: Ninguno
        
'''
def initICMP():
    registerIPProtocol(process_ICMP_message, ICMP_PROTO)
    