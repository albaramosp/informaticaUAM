"""
    FILE: practica3_client.py
    AUTHOR: Alba Ramos Pedroviejo - alba.ramosp@estudiante.uam.es
    AUTHOR: Nicolas Serrano Salas - nicolas.serranos@estudiante.uam.es
	AUTHOR: profesores de la asignatura REDES2
    DESCRIPTION: implements the main view and main controllers of the application.
"""

# import the library

from appJar import gui
from PIL import Image, ImageTk
import numpy as np
import cv2
import tkinter as tk

from user import *
from calls import *

class VideoClient(object):
	def __init__(self, window_size, nick):
		self.socketUDPListen = None

		self.nick = nick

		self.processCallCommandsThread = None

		self.cap = None
		self.useVideo = False
		self.videoFile = None

		self.isPaused = False
		self.peerPaused = False
		
		# Srote the main GUI
		self.app = gui("TGV - The Good Videocall", window_size)
		self.app.setGuiPadding(10,10)

		# Graphic design
		self.app.setBg(MainBgColor, override=True) # background color for window and widgets
		self.app.setFont(size=14, underline=False, slant="roman")

		# Prepare user interface
		self.app.addLabel("title", "TGV - The Good Videocall")
		self.app.addLabel("description", "Realiza videollamadas gratuitas de forma sencilla. ")
		self.app.addLabel("slogan", "De estudiantes, para estudiantes.")

		self.app.getLabelWidget("title").config(font="Times 20 bold")
		self.app.getLabelWidget("slogan").config(font="Times 14 italic")

		# Register the method to capture video
		self.cap = cv2.VideoCapture(0)

		# Shows Configure buttons on main page
		self.app.addImage("video", "imgs/webcam.gif")
		self.showMainPage()

		# Congestion control info
		self.sendFPS = 50
		self.sendResolution = "HIGH"

		# Control exit
		self.app.setStopFunction(self.stopApp)

		# Create subwindow to show the list of users when requested
		self.app.startSubWindow("Lista de Usuarios")
		self.app.addLabel("users", "Usuarios disponibles")
		self.app.addListBox("list", [])
		self.app.setBg("#64b0bc")
		self.app.setPadding([2, 2])
		self.app.stopSubWindow()

		self.calls = Calls(self.nick, self)

	"""
		METHOD: getApp()
		DESCRIPTION: returns the app object
	"""
	def getApp(self):
		return self.app

	"""
		METHOD: start()
		DESCRIPTION: launch app
	"""
	def start(self):
		self.app.infoBox("Bienvenido a TGV", "Le recordamos que por defecto se usará su cámara como grabadora.\nSi desea cambiarlo pulse\nUsar video", parent=None)
		self.app.go()

	"""
		METHOD: stopApp()
		DESCRIPTION: stablishes the operations to be done before exiting.
		ARGS_OUT: True
	"""
	def stopApp(self):
		try:
			self.calls.processCallEnd()
		except:
			pass

		self.calls.closeCall()
		return True

		
	"""
		METHOD: capturaVideo()
		DESCRIPTION: captures the frame to be shown in each moment. This frame will come from the webcam or 
					 from an external video file. It also shows incoming frames from the peer.
					 Attaches a header to manage losses and congestion control when sending a frame.
	"""
	def capturaVideo(self):
		# If call is paused, does nothing
		if self.isPaused is False and self.peerPaused is False:
			# Capture a frame from camera or video
			try:
				ret, frame = self.cap.read()
				if ret is True:
					frame = cv2.resize(frame, (640,480))
					self.frame_peque = cv2.resize(frame, (160, 120))
				
					# Compress to JPG format with 50% of resolution
					encode_param = [cv2.IMWRITE_JPEG_QUALITY,50]
					result, encimg = cv2.imencode('.jpg', frame, encode_param)
					
					if result == False: 
						print('Error al codificar imagen')

					encimg = encimg.tobytes()

					# Attach header
					enviar = [bytes(str(self.orderCount)+"#", 'utf-8'),
								bytes(str(time.time())+"#", 'utf-8'),
								bytes(self.sendResolution+"#", 'utf-8'),
								bytes(str(self.sendFPS)+"#", 'utf-8'),
								encimg]
					# Update number of send packages
					self.orderCount += 1

					# Data now is ready and in correct format to be send
					if self.calls.sendVideo(b"".join(enviar)) == -1:
						pass
				
			except Exception as msg:
				print(msg)
				pass
		
			# Try to receive frame from peer, if any
			if self.calls.receivePeerVideo() == -1:
				pass

			# Extract data from buffer
			data = self.calls.extractFromBuffer()
			if data is not None:
				# Decompress received data
				decimg = cv2.imdecode(np.frombuffer(data, np.uint8), 1)
				frame_compuesto = decimg

				# Show both images, send and received, on the screen
				frame_compuesto[0:self.frame_peque.shape[0], 0:self.frame_peque.shape[1]] = self.frame_peque
				
				# Format conversion to be used in the GUI
				cv2_im = cv2.cvtColor(frame_compuesto,cv2.COLOR_BGR2RGB)
				img_tk = ImageTk.PhotoImage(Image.fromarray(cv2_im))
				self.app.setImageData("video", img_tk, fmt = 'PhotoImage')

		# To be called again in fps ms
		fps = 1000//self.sendFPS
		self.app.after(fps, self.capturaVideo)
		

	"""
		METHOD: showMainPage(reset)
		ARGS_IN: reset - indicates whether the call has ended or not, to show a set of buttons or not
		DESCRIPTION: manages the images and buttons shown in the GUI when a call starts and ends.
	"""
	def showMainPage(self, reset=False):
		if reset is True:
			self.app.removeButton("Usar video")
			self.app.removeButton("Colgar")
			self.app.removeButton("Pausar")
			self.app.removeButton("Salir")	

			#Show main img
			self.app.setImage("video", "imgs/webcam.gif")	

		# Adds buttons
		self.app.addButtons(["Usar video", "Listar", "Llamar", "Salir"], self.mainButtonsCallback)

		self.app.setButtonBg("Usar video", BtnBgColor)
		self.app.setButtonFg("Usar video", FgColor)
		# Change text if using a video file instead of webcam
		if self.useVideo is True:
			self.app.setButton("Usar video", "Usar webcam")
		self.app.setButtonBg("Listar", BtnBgColor)
		self.app.setButtonFg("Listar", FgColor)
		self.app.setButtonBg("Llamar", BtnBgColor)
		self.app.setButtonFg("Llamar", FgColor)
		self.app.setButtonBg("Salir", BtnBgColor)
		self.app.setButtonFg("Salir", FgColor)


	"""
		METHOD: handleCallProcess(dstIP, dstUDPPort)
		ARGS_IN: dstIP - IP address of peer
				 dstUDPPort - port of peer
		DESCRIPTION: manages the connection with the peer to send and receive video frames, as well as GUI variation.
	"""
	def handleCallProcess(self, dstIP, dstUDPPort):
		self.calls.listenVideo()

		# Stablish connection with the other peer
		self.calls.connectVideo(dstIP, dstUDPPort)
		
		# Restar the counter of sended packages
		self.orderCount = 0

		# Send and receive frames every 20 ms
		self.app.after(0, self.capturaVideo)

		self.app.removeButton("Usar video")
		self.app.removeButton("Listar")
		self.app.removeButton("Llamar")
		self.app.removeButton("Salir")	
		self.app.addButtons(["Usar video", "Colgar", "Pausar", "Salir"], self.callButtonsCallback)

		self.app.setButtonBg("Usar video", BtnBgColor)
		self.app.setButtonFg("Usar video", FgColor)
		if self.useVideo is True:
			self.app.setButton("Usar video", "Usar webcam")	
		self.app.setButtonBg("Colgar", BtnBgColor)
		self.app.setButtonFg("Colgar", FgColor)
		self.app.setButtonBg("Pausar", BtnBgColor)
		self.app.setButtonFg("Pausar", FgColor)
		self.app.setButtonBg("Salir", BtnBgColor)
		self.app.setButtonFg("Salir", FgColor)


	"""
		METHOD: setImageResolution(resolution)
		ARGS_IN: resolution - size of the image
		DESCRIPTION: sets the resolution of the captured image
	"""
	def setImageResolution(self, resolution):		
		# sets the resolution of the captured image from webcam
		# You may add a higher value if your cam supports it, but do not modify these ones
		if resolution == "LOW":
			self.cap.set(cv2.CAP_PROP_FRAME_WIDTH, 160) 
			self.cap.set(cv2.CAP_PROP_FRAME_HEIGHT, 120) 
		elif resolution == "MEDIUM":
			self.cap.set(cv2.CAP_PROP_FRAME_WIDTH, 320) 
			self.cap.set(cv2.CAP_PROP_FRAME_HEIGHT, 240) 
		elif resolution == "HIGH":
			self.cap.set(cv2.CAP_PROP_FRAME_WIDTH, 640) 
			self.cap.set(cv2.CAP_PROP_FRAME_HEIGHT, 480) 
				
	"""
		METHOD: mainButtonsCallback(button)
		ARGS_IN: button - button clicked
		DESCRIPTION: executes the functionality associated to the clicked button 
					 from the main window (exit, list users or connect to user)
	"""
	def mainButtonsCallback(self, button):
		# Exit app
		if button == "Salir":
			self.app.stop()
		elif button == "Usar video":
			if self.useVideo is False:
				self.videoFile = self.app.openBox(title="Seleccionar video", fileTypes=[('video', '*.mp4'), ('video', '*.avi'), ('video', '*.mpg'), ('video', '*.mpeg'), ('video', '*.mkv')], asFile=False)
				if self.videoFile:
					self.useVideo = True
					self.cap.release()
					self.cap = cv2.VideoCapture(self.videoFile)
					self.app.setButton("Usar video", "Usar webcam")
			else:
				self.useVideo = False
				self.cap.release()
				self.cap = cv2.VideoCapture(0)
				self.app.setButton("Usar video", "Usar video")
		# Connect and call a user
		elif button == "Llamar": 
			nick = self.app.textBox("Llamar", "Introduce el nick del usuario al que llamar") 
			
			try: 
				# Try to get the user's IP and port
				user = User()
				ret = user.getUserData(nick)
				if ret is None:
					self.app.warningBox("Error", "Usuario no encontrado")
					return
				
				ipDst = ret[3]
				TCPdst = int(ret[4])

				# Choose the highest common protocol between us and the user
				protocol = user.getMaxCommonProtocol(ret[5])

				if protocol is None:
					self.app.warningBox("Error", "No existen protocolos comunes con este usuario.")
					return

				# Start the call
				# Returns error code or peer UDP port if successful
				callCtrl = self.calls.sendCall(TCPdst, ipDst)

				# Timeout
				if callCtrl == -3:
					self.app.warningBox("Error 404", "Imposible contactar con el usuario.")
				# User denied the call
				elif callCtrl == 0:
					self.app.warningBox("Llamada rechazada", "El usuario ha rechazado la llamada.")
				# User is in another call
				elif callCtrl == -1:
					self.app.warningBox("Ocupado", "El usuario se encuentra en una llamada.")
				# Unrecognized error
				elif callCtrl == -2:
					self.app.warningBox("Error 500", "Internal server error.")
				# Call accepted
				else:
					self.handleCallProcess(ipDst, callCtrl)
					self.processCallCommandsThread = threading.Thread(target = self.calls.processCallCommands)
					self.processCallCommandsThread.start()
					
			except TypeError:
				pass    
		# List the nicks of the users in the system
		elif button == "Listar":
			user = User()
			users = user.listUsers()

			self.app.clearListBox("list", callFunction=False)
			self.app.addListItems("list", users)
			self.app.showSubWindow("Lista de Usuarios")


	"""
		METHOD: callButtonsCallback(button)
		ARGS_IN: button - button clicked
		DESCRIPTION: executes the functionality associated to the clicked button during a call
	"""
	def callButtonsCallback(self, button):
		if button == "Usar video":
			if self.useVideo is False:
				self.videoFile = self.app.openBox(title="Seleccionar video", fileTypes=[('video', '*.mp4'), ('video', '*.avi'), ('video', '*.mpg'), ('video', '*.mpeg')], asFile=False)
				if self.videoFile:
					self.useVideo = True
					self.cap.release()
					self.cap = cv2.VideoCapture(self.videoFile)
					self.app.setButton("Usar video", "Usar webcam")
			else:
				self.useVideo = False
				self.cap.release()
				self.cap = cv2.VideoCapture(0)
				self.app.setButton("Usar video", "Usar video")
		
		elif button == "Colgar":
			self.calls.processCallEnd()
			self.showMainPage(reset=True)
			pass
		
		elif button == "Pausar":
			if self.peerPaused is False:
				# Continue call
				if self.isPaused is True:
					self.calls.notifyCallResume()
					self.isPaused = False
					self.app.setButton("Pausar", "Pausar")
				# Pause call
				else:
					self.calls.notifyCallHold()
					self.isPaused = True
					self.app.setButton("Pausar", "Reanudar")
		elif button == "Salir":
			self.app.stop()


if __name__ == '__main__':
	# First action is to log in the user, you must be logged in to use the app
	user = User()
	user.loginWindow("300x200")
	user.start()

	if user.getStatus() == 0:
		# If login is successful, enter main window
		vc = VideoClient("640x520", user.getNick())

		# Crear aquí los threads de lectura, de recepción y,
		# en general, todo el código de inicialización que sea necesario
		# ...


		# Lanza el bucle principal del GUI
		# El control ya NO vuelve de esta función, por lo que todas las
		# acciones deberán ser gestionadas desde callbacks y threads
		vc.start()