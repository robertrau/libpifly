#! /usr/bin/python3

import signal
import sys

import libpifly_python
from pifly.comm import SerialPort
from pifly.gps import GpsResult
from pifly.gps.skytraq import *

running = True
def sigintHandler(signal, frame):
	print("interrupt")
	global running
	running = False

if __name__ == "__main__":
	signal.signal(signal.SIGINT, sigintHandler)

	serialPort = SerialPort("/dev/serial0", SerialPort.Baudrate._9600, False)
	skytraqVenus = SkyTraqVenus(serialPort, 2.0)

	serialPort.flush()
	skytraqVenus.updateBaudrate(SerialPort.Baudrate._230400)
	skytraqVenus.setMessageType(SkyTraqVenus.MessageType.Binary)
	skytraqVenus.setPositionUpdateRate(50)
	protocol = SkyTraqBinaryProtocol(serialPort)

    TimeNow = datetime.now()
    stringList = []
    stringList.append('<?xml version="1.0" encoding="UTF-8"?>\n')
    stringList.append('<kml xmlns="http://www.opengis.net/kml/2.2">\n')
    stringList.append('  <Document>\n')
    stringList.append('    <name>PiFly ' + TimeNow.strftime("%m/%d/%y  %H:%M:%S") + '</name>\n')
    stringList.append('    <open>1</open>')     # Specifies whether a Document or Folder appears closed or open when first loaded into the Places panel. 0=collapsed (the default), 1=expanded.
    stringList.append('    <description>RocketFlight</description>')
    print(''.join(stringList))

	result = GpsResult()
	while running:
		if protocol.getResult(result):
			stringList = []
			stringList.append('<?xml version="1.0" encoding="UTF-8"?>\n')
			stringList.append('<kml xmlns="http://www.opengis.net/kml/2.2">\n')
			stringList.append('  <Document>\n')
			stringList.append('    <name>')
			stringList.append(" mean sea level: " + str(result.meanSeaLevel/100.0))
			stringList.append(" lat: " + str(result.latitude*1e-7) + "deg")
			stringList.append(" long: " + str(result.longitude*1e-7) + "deg")
			print(''.join(stringList))

	# Set the GPS back to defaults
	serialPort.flush()
	skytraqVenus.setPositionUpdateRate(1)
	skytraqVenus.setMessageType(SkyTraqVenus.MessageType.NMEA)
	skytraqVenus.updateBaudrate(SerialPort.Baudrate._9600)
