/*
	Author: Robert F. Rau II
	Copyright (C) 2017 Robert F. Rau II
*/
#include <atomic>
#include <iostream>

#include <signal.h>

#include "gps/gpsexception.h"
#include "gps/nmeaprotocol.h"
#include "gps/skytraq/skytraqbinaryprotocol.h"
#include "gps/skytraq/skytraqvenus.h"
#include "comm/commexception.h"

using PiFly::GPS::NMEAProtocol;
using PiFly::GPS::Skytraq::SkyTraqBinaryProtocol;
using PiFly::GPS::Skytraq::SkyTraqVenus;
using PiFly::GPS::GpsResult;
using PiFly::GPS::GpsException;
using PiFly::Comm::SerialPort;
using PiFly::Comm::CommException;

std::atomic<bool> interrupted;
void term_handle(int sig)
{
	std::cout << "Signal received" << std::endl;
	interrupted.store(true);
}

template <class Gps>
void run_loop(Gps& gps)
{
	while(!interrupted.load())
	{
		GpsResult result{};
		if(gps.getResult(result))
		{
			std::cout << "FixType: " << result.fixType <<
							" Satellites in view: " << static_cast<uint32_t>(result.satellitesInView) <<
							" GNSS Week: " << result.GNSSWeek <<
							" TOW: " << static_cast<double>(result.tow)/100.0 << "s" <<
							" mean sea level: " << static_cast<double>(result.meanSeaLevel)/100.0 <<
							" lat: " << result.latitude*1e-7 << "deg" <<
							" long: " << result.longitude*1e-7 << "deg\n";
		}
	}
}

int main(int argc, char** argv)
{
	signal(SIGINT, &term_handle);

	try
	{
		SerialPort serialPort("/dev/serial0", SerialPort::Baudrate_9600, false);
		SkyTraqVenus skytraqVenus(serialPort, 2.0);

		skytraqVenus.updateBaudrate(SerialPort::Baudrate_230400);
		std::cout << "Setting binary message type" << std::endl;
		skytraqVenus.setMessageType(SkyTraqVenus::MessageType_Binary);
		std::cout << "Setting update rate to 50Hz" << std::endl;
		skytraqVenus.setPositionUpdateRate(50);

		SkyTraqBinaryProtocol protocol(serialPort);

		std::cout << "Starting gps" << std::endl;
		run_loop(protocol);

		std::cout << "Setting update rate to 1Hz" << std::endl;
		skytraqVenus.setPositionUpdateRate(1);
		std::cout << "Setting message type to NMEA" << std::endl;
		skytraqVenus.setMessageType(SkyTraqVenus::MessageType_NMEA);
		std::cout << "Setting baudrate to 9600" << std::endl;
		skytraqVenus.updateBaudrate(SerialPort::Baudrate_9600);
	}
	catch(GpsException& ex)
	{
		std::cout << ex.what() << std::endl;
	}
	catch(CommException& ex)
	{
		std::cout << ex.what() << std::endl;
	}

	std::cout << "Hey there ;)" << std::endl;

	return 0;
}

