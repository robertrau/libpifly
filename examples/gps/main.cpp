/*
	Author: Robert F. Rau II
	Copyright (C) 2017 Robert F. Rau II
*/
#include <atomic>
#include <iostream>

#include <signal.h>

#include "gps/gps.h"
#include "gps/gpsexception.h"
#include "gps/nmeaprotocol.h"
#include "gps/skytraq/skytraqbinaryprotocol.h"
#include "gps/skytraq/skytraqvenus.h"
#include "comm/commexception.h"

using PiFly::GPS::GlobalPositioningSystem;
using PiFly::GPS::NMEAProtocol;
using PiFly::GPS::Skytraq::SkyTraqBinaryProtocol;
using PiFly::GPS::Skytraq::SkyTraqVenus;
using PiFly::GPS::ResultVector;
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
		GpsResult result;

		if(gps.getResult(result))
		{
			std::cout << "FixType: " << result.fixType <<
							" Satellites in view: " << static_cast<uint32_t>(result.satellitesInView) <<
							" GNSS Week: " << result.GNSSWeek <<
							" TOW: " << result.tow <<
							" mean sea level: " << result.meanSeaLevel <<
							" lat: " << result.latitude <<
							" long: " << result.longitude << "\n";
		}
	}
}

int main(int argc, char** argv)
{
	signal(SIGINT, &term_handle);

	try
	{		
		SerialPort serialPort("/dev/serial0", SerialPort::Baudrate_230400, false);
		SkyTraqVenus skytraqVenus(serialPort, 2.0);
		SkyTraqBinaryProtocol protocol(serialPort, skytraqVenus);

		std::cout << "Starting gps" << std::endl;
		run_loop(protocol);
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

