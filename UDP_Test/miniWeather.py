#!/usr/bin/python
# Multicast client
# Adapted from: http://chaos.weblogs.us/archives/164
# Compass from: https://wiki.python.org/moin/PyQt/Compass%20widget

import sys
from PyQt4.QtCore import *
from PyQt4.QtGui import *

import socket
import json

ANY = "0.0.0.0" 
MCAST_ADDR = "224.0.0.7"
MCAST_PORT = 55555

class CompassWidget(QWidget):

	angleChanged = pyqtSignal(float)
	
	def __init__(self, parent = None):
	
		QWidget.__init__(self, parent)
		
		self._angle = 0.0
		self._margins = 10
		self._pointText = {0: "N", 45: "NE", 90: "E", 135: "SE", 180: "S",
						   225: "SW", 270: "W", 315: "NW"}
	
	def paintEvent(self, event):
	
		painter = QPainter()
		painter.begin(self)
		painter.setRenderHint(QPainter.Antialiasing)
		
		painter.fillRect(event.rect(), self.palette().brush(QPalette.Window))
		self.drawMarkings(painter)
		self.drawNeedle(painter)
		
		painter.end()
	
	def drawMarkings(self, painter):
	
		painter.save()
		painter.translate(self.width()/2, self.height()/2)
		scale = min((self.width() - self._margins)/120.0,
					(self.height() - self._margins)/120.0)
		painter.scale(scale, scale)
		
		font = QFont(self.font())
		font.setPixelSize(10)
		metrics = QFontMetricsF(font)
		
		painter.setFont(font)
		painter.setPen(self.palette().color(QPalette.Shadow))
		
		i = 0
		while i < 360:
		
			if i % 45 == 0:
				painter.drawLine(0, -40, 0, -50)
				painter.drawText(-metrics.width(self._pointText[i])/2.0, -52,
								 self._pointText[i])
			else:
				painter.drawLine(0, -45, 0, -50)
			
			painter.rotate(15)
			i += 15
		
		painter.restore()
	
	def drawNeedle(self, painter):
	
		painter.save()
		painter.translate(self.width()/2, self.height()/2)
		painter.rotate(self._angle)
		scale = min((self.width() - self._margins)/120.0,
					(self.height() - self._margins)/120.0)
		painter.scale(scale, scale)
		
		painter.setPen(QPen(Qt.NoPen))
		painter.setBrush(self.palette().brush(QPalette.Shadow))
		
		painter.drawPolygon(
			QPolygon([QPoint(-5, 0), QPoint(0, -45), QPoint(5, 0),
					  QPoint(0, 5), QPoint(-5, 0)])
			)
		
		painter.setBrush(self.palette().brush(QPalette.Highlight))
		
		painter.drawPolygon(
			QPolygon([QPoint(-5, -25), QPoint(0, -45), QPoint(5, -25),
					  QPoint(0, -30), QPoint(-5, -25)])
			)
		
		painter.restore()
	
	def sizeHint(self):
	
		return QSize(150, 150)
	
	def angle(self):
		return self._angle
	
	@pyqtSlot(float)
	def setAngle(self, angle):
	
		if angle != self._angle:
			self._angle = angle
			self.angleChanged.emit(angle)
			self.update()
	
	angle = pyqtProperty(float, angle, setAngle)

# Create a UDP socket
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, socket.IPPROTO_UDP)

# Allow multiple sockets to use the same PORT number
sock.setsockopt(socket.SOL_SOCKET,socket.SO_REUSEADDR,1)

# Bind to the port that we know will receive multicast data
sock.bind((ANY,MCAST_PORT))

# Tell the kernel that we want to add ourselves to a multicast group
# The address for the multicast group is the third param
status = sock.setsockopt(socket.IPPROTO_IP, socket.IP_ADD_MEMBERSHIP, socket.inet_aton(MCAST_ADDR) + socket.inet_aton(ANY))


# setblocking(0) is equiv to settimeout(0.0) which means we poll the socket.
# But this will raise an error if recv() or send() can't immediately find or send data. 
sock.setblocking(0)

if __name__ == "__main__":

	app = QApplication(sys.argv)
	
	window = QWidget()
	compass = CompassWidget()
	layout = QVBoxLayout()
	layout.addWidget(compass, 1)
	speed = QLabel()
	layout.addWidget(speed)
	temp = QLabel()
	layout.addWidget(temp)

	window.setLayout(layout)
	
	window.show()
	compass.setAngle (1)
	app.processEvents ()
	app.processEvents ()
	app.processEvents ()
	app.processEvents ()

	while 1:
		try:
			app.processEvents()
			data, addr = sock.recvfrom(1024)
		except socket.error as e:
			pass
		else:
			pkt = json.loads (data)
			compass.setAngle (pkt["windDir"])
			deg = u"\u00b0"
			speed.setText (QString ("Wind: %1 kph (Max %2)").arg(pkt["windSpeed"]*3.6).arg(pkt["windGust"]*3.6))
			temp.setText (QString ("Temp: %1%2C   Rain: %3 mm").arg(pkt["outTemp"]*3.6).arg(deg).arg(pkt["rain"]))
			app.processEvents()


	sys.exit(app.exec_())
