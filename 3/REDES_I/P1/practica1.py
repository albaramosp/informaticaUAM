'''
    practica1.py
    Muestra el tiempo de llegada de los primeros 50 paquetes a la interfaz especificada
    como argumento y los vuelca a traza nueva con tiempo actual

    Autor: Javier Ramos <javier.ramos@uam.es>
    2019 EPS-UAM
'''

from rc1_pcap import *
import sys
import binascii
import signal
import argparse
from argparse import RawTextHelpFormatter
import time
import logging

ETH_FRAME_MAX = 1514
PROMISC = 1
NO_PROMISC = 0
TO_MS = 10
num_paquete = 0
TIME_OFFSET = 30*60
flag = 0

def signal_handler(nsignal,frame):
	logging.info('Control C pulsado')
	if handle:
		pcap_breakloop(handle)
		

def procesa_paquete(us,header,data):
	global num_paquete, pdumper, flag
	header.ts.tv_sec = header.ts.tv_sec + 1800
	logging.info('Nuevo paquete de {} bytes capturado a las {}.{}'.format(header.len,header.ts.tv_sec,header.ts.tv_usec))
	num_paquete += 1
	
	n = args.nbytes
	if header.len < n:
		n = header.len
	logging.info("Primeros  bytes del paquete: " + " ".join('{:02x}'.format(c) for c in data[:n]))
	

	if flag ==1:
		pcap_dump(pdumper, header, data)
	
if __name__ == "__main__":
	global pdumper,args,handle
	file_descripter = 0

	signal.signal(signal.SIGINT, signal_handler)
	
	parser = argparse.ArgumentParser(description='Captura trafico de una interfaz ( o lee de fichero) y muestra la longitud y timestamp de los 50 primeros paquetes',
	formatter_class=RawTextHelpFormatter)
	parser.add_argument('--file', dest='tracefile', default=False,help='Fichero pcap a abrir')
	parser.add_argument('--itf', dest='interface', default=False,help='Interfaz a abrir')
	parser.add_argument('--nbytes', dest='nbytes', type=int, default=14,help='Numero de bytes a mostrar por paquete')
	parser.add_argument('--debug', dest='debug', default=False, action='store_true',help='Activar Debug messages')
	args = parser.parse_args()


	if len(sys.argv) <= 1:
		parser.print_help()
		sys.exit(-1)

	if args.debug:
		logging.basicConfig(level = logging.DEBUG, format = '[%(asctime)s %(levelname)s]\t%(message)s')
	else:
		logging.basicConfig(level = logging.INFO, format = '[%(asctime)s %(levelname)s]\t%(message)s')

	
	

	if args.tracefile is False and args.interface is False:

		logging.error('No se ha especificado interfaz ni fichero')
		parser.print_help()
		sys.exit(-1)


	errbuf = bytearray()
	handle = None
	pdumper = None
	capture_name = "captura."
	

	if args.interface:
		handle = pcap_open_live(args.interface, ETH_FRAME_MAX,1,100,errbuf)
		capture_name = capture_name + args.interface + "." + str(int(time.time())) + ".pcap"
		flag = 1
		file_descripter=pcap_open_dead(DLT_EN10MB, ETH_FRAME_MAX) 	
		pdumper = pcap_dump_open(file_descripter, capture_name)


	if args.tracefile:
		capture_name = capture_name+args.tracefile
		handle = pcap_open_offline(args.tracefile, errbuf)


	ret = pcap_loop(handle, -1, procesa_paquete, None)
	if ret == -1:
		logging.error('Error al capturar un paquete')
	elif ret == -2:
		logging.debug('pcap_breakloop() llamado')
	elif ret == 0:
		logging.debug('No mas paquetes o limite superado')

	logging.info('{} paquetes procesados'.format(num_paquete))
	pcap_close(handle)

	if flag ==1 :
		pcap_close(file_descripter)
		pcap_dump_close(pdumper)

	

