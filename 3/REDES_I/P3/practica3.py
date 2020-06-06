'''
    practica3.py
    Envía datagramas UDP o ICMP sobre protocolo IP. 

    Autor: Javier Ramos <javier.ramos@uam.es>
    2019 EPS-UAM
'''


from udp import *
from icmp import *
import sys
import binascii
import signal
import argparse
import struct
from argparse import RawTextHelpFormatter
import time
import logging
import socket

DST_PORT = 80
ICMP_ECHO_REQUEST_TYPE = 8
ICMP_ECHO_REQUEST_CODE = 0

ipTstampOption =	bytes([68,12,13,0x01,10,0,0,3])

if __name__ == "__main__":
	ICMP_ID = 0
	ICMP_SEQNUM = 0
	parser = argparse.ArgumentParser(description='Envía datagramas UDP o mensajes ICMP con diferentes opciones',
	formatter_class=RawTextHelpFormatter)
	parser.add_argument('--itf', dest='interface', default=False,help='Interfaz a abrir')
	parser.add_argument('--dstIP',dest='dstIP',default = False,help='Dirección IP destino')
	parser.add_argument('--debug', dest='debug', default=False, action='store_true',help='Activar Debug messages')
	parser.add_argument('--addOptions', dest='addOptions', default=False, action='store_true',help='Añadir opciones a los datagranas IP')
	parser.add_argument('--dataFile',dest='dataFile',default = False,help='Fichero con datos a enviar')
	args = parser.parse_args()

	if args.debug:
		logging.basicConfig(level = logging.DEBUG, format = '[%(asctime)s %(levelname)s]\t%(message)s')
	else:
		logging.basicConfig(level = logging.INFO, format = '[%(asctime)s %(levelname)s]\t%(message)s')

	if args.interface is False:
		logging.error('No se ha especificado interfaz')
		parser.print_help()
		sys.exit(-1)

	if args.dstIP is False:
		logging.error('No se ha especificado dirección IP')
		parser.print_help()
		sys.exit(-1)

	ipOpts = None
	if args.addOptions:
		ipOpts = ipTstampOption +struct.pack('!I',int(time.time()))

	data = b'default test'
	if args.dataFile:
		with open(args.dataFile,'r') as f:
			#Leemos el contenido del fichero
			data=f.read()
			#Pasamos los datos de cadena a bytes
			data = data.encode()
	
	startEthernetLevel(args.interface)
	initARP(args.interface)
	initICMP()
	initUDP()
	if initIP(args.interface,ipOpts) == False:
		logging.error('Inicializando nivel IP')
		sys.exit(-1)

	
	
	
	while True:
		try:
			msg = input('Introduzca opcion:\n\t1.Enviar ping\n\t2.Enviar datagrama UDP:')
			if msg == 'q':
				break
			elif msg == '1':
				sendICMPMessage(data,ICMP_ECHO_REQUEST_TYPE,ICMP_ECHO_REQUEST_CODE,ICMP_ID,ICMP_SEQNUM,struct.unpack('!I',socket.inet_aton(args.dstIP))[0])
				ICMP_SEQNUM += 1
			elif msg == '2':
				sendUDPDatagram(data,DST_PORT,struct.unpack('!I',socket.inet_aton(args.dstIP))[0])
		except KeyboardInterrupt:
			print('\n')
			break

	logging.info('Cerrando ....')
	stopEthernetLevel()
