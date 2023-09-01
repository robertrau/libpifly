#! /usr/bin/python3

import signal
import sys
from datetime import datetime


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

    # Open Results output file
    TimeNow = datetime.now()
    ResultFileTimeStamp = TimeNow.strftime("%Y%m%d-%H%M")
    PathFile_fd = open("PiFlyResults" + ResultFileTimeStamp + ".kml", 'w')

	serialPort = SerialPort("/dev/serial0", SerialPort.Baudrate._9600, False)
	skytraqVenus = SkyTraqVenus(serialPort, 2.0)

	serialPort.flush()
	skytraqVenus.updateBaudrate(SerialPort.Baudrate._230400)
	skytraqVenus.setMessageType(SkyTraqVenus.MessageType.Binary)
	skytraqVenus.setPositionUpdateRate(50)
	protocol = SkyTraqBinaryProtocol(serialPort)

    stringList = []
    stringList.append('<?xml version="1.0" encoding="UTF-8"?>\n')
    stringList.append('<kml xmlns="http://www.opengis.net/kml/2.2">\n')
    stringList.append('  <Document>\n')
    stringList.append('    <name>PiFly ' + TimeNow.strftime("%m/%d/%y  %H:%M:%S") + '</name>\n')
    stringList.append('    <open>1</open>')     # Specifies whether a Document or Folder appears closed or open when first loaded into the Places panel. 0=collapsed (the default), 1=expanded.
    stringList.append('    <description>RocketFlight</description>')
    stringList.append('  <Folder>\n')
    stringList.append('    <name>Barometric Altitude</name>\n')
    stringList.append('    <Style id="PiFly GPS Data">\n')
    stringList.append('      <LineStyle><color>FF0000FF</color><width>8</width></LineStyle>\n')
    stringList.append('      <BalloonStyle>\n')
    stringList.append('        <text>\n')
    stringList.append('GPS Data\n')
    stringList.append('        </text>\n')
    stringList.append('      </BalloonStyle>\n')
    stringList.append('    </Style>\n')
    stringList.append('    <Placemark>\n')
    stringList.append('      <name>Barometric Altitude</name>\n')
    stringList.append('      <styleUrl>#PiFly GPS Data</styleUrl>\n')
    stringList.append('      <LineString>\n')
    stringList.append('        <tessellate>1</tessellate>\n')
    stringList.append('        <altitudeMode>absolute</altitudeMode>\n')    # See: https://developers.google.com/kml/documentation/altitudemode
    stringList.append('        <coordinates>\n')
    
    kmlHeader = ''.join(stringList)

ResultFile_fd.write(kmlHeader)
    
	result = GpsResult()
	while running:
		if protocol.getResult(result):
			stringList = []

			stringList.append("          " + str(result.latitude*1e-7) + "," + str(result.longitude*1e-7) + "," + str(result.meanSeaLevel/100.0))



    stringList = []
    stringList.append('        </coordinates>\n')
    stringList.append('      </LineString>\n')
    stringList.append('    </Placemark>\n')
    stringList.append('  </Folder>\n')
    stringList.append('</Document>\n')
    stringList.append('</kml>\n')

    ResultFile_fd.write(''.join(stringList))
    

    
	# Set the GPS back to defaults
	serialPort.flush()
	skytraqVenus.setPositionUpdateRate(1)
	skytraqVenus.setMessageType(SkyTraqVenus.MessageType.NMEA)
	skytraqVenus.updateBaudrate(SerialPort.Baudrate._9600)
