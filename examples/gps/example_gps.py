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

	result = GpsResult()
	while running:
		if protocol.getResult(result):
			stringList = []
			stringList.append("FixType: " + str(result.fixType))
			stringList.append(" Satellites in view: " + str(result.satellitesInView))
			stringList.append(" GNSS Week: " + str(result.GNSSWeek))
			stringList.append(" TOW: " + str(result.tow/100.0) + "s")
			stringList.append(" mean sea level: " + str(result.meanSeaLevel/100.0))
			stringList.append(" lat: " + str(result.latitude*1e-7) + "deg")
			stringList.append(" long: " + str(result.longitude*1e-7) + "deg")
			print(''.join(stringList))

	# Set the GPS back to defaults
	serialPort.flush()
	skytraqVenus.setPositionUpdateRate(1)
	skytraqVenus.setMessageType(SkyTraqVenus.MessageType.NMEA)
	skytraqVenus.updateBaudrate(SerialPort.Baudrate._9600)
