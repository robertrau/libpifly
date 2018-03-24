/*
	Author: Robert F. Rau II
	Copyright (C) 2017 Robert F. Rau II
*/
#include <atomic>
#include <iostream>

#include <signal.h>

#include "gps/gps.h"
#include "gps/gpsexception.h"
#include "gps/skytraqbinaryprotocol.h"
#include "comm/commexception.h"

using PiFly::GPS::GlobalPositioningSystem;
using PiFly::GPS::SkyTraqBinaryProtocol;
using PiFly::GPS::ResultVector;
using PiFly::GPS::GpsException;
using PiFly::Comm::SerialPort;
using PiFly::Comm::CommException;

std::atomic<bool> interrupted;
void term_handle(int sig)
{
	std::cout << "Signal received" << std::endl;
	interrupted.store(true);
}

int main(int argc, char** argv)
{
	signal(SIGINT, &term_handle);

	try
	{
		SerialPort serialPort("/dev/serial0", SerialPort::Baudrate_230400);
		SkyTraqBinaryProtocol protocol(serialPort);
		GlobalPositioningSystem gps(protocol);

		std::cout << "Starting gps" << std::endl;
		gps.start();

		const size_t sampleBuffSize = 512;
		ResultVector sampleBuff(sampleBuffSize);
		while(!interrupted.load())
		{
			size_t samples = gps.getLatestSamples(sampleBuff);

			for(uint32_t i = 0; i < samples; i++)
			{
				std::cout << "FixType: " << sampleBuff[i].fixType <<
							 " Satellites in view: " << static_cast<uint32_t>(sampleBuff[i].satellitesInView) <<
							 " mean sea level: " << sampleBuff[i].meanSeaLevel <<
							 " lat: " << sampleBuff[i].latitude <<
							 " long: " << sampleBuff[i].longitude << "\n";
			}
		}

		gps.stop();
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

