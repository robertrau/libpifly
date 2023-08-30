#! /usr/bin/python3
##
# @brief
# GPS python script for libpifly on PiFly board with SkyTraq Venus838 GPS receiver
#
# @author Rob Rau
# 
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
			stringList.append(f" TOW: {result.tow/100.0:.2f} s")
			stringList.append(f" mean sea level: {result.meanSeaLevel/100.0:.1f}")
			stringList.append(f" lat: {result.latitude*1e-7:.7f} deg")
			stringList.append(f" long: {result.longitude*1e-7:.7f} deg")
			print(''.join(stringList))

	# Set the GPS back to defaults
	serialPort.flush()
	skytraqVenus.setPositionUpdateRate(1)
	skytraqVenus.setMessageType(SkyTraqVenus.MessageType.NMEA)
	skytraqVenus.updateBaudrate(SerialPort.Baudrate._9600)
