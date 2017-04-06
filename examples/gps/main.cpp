#include <iostream>

#include "gps/gps.h"

using PiFly::GPS::GlobalPositioningSystem;
using PiFly::GPS::GpsException;
using PiFly::Comm::SerialPort;

int main(int argc, char** argv)
{
	try
	{
		SerialPort serialPort("/dev/serial0");
		GlobalPositioningSystem gps(serialPort);

		std::cout << "Starting gps" << std::endl;
		gps.start();

		while(true)
		{
			auto sampleBuff = gps.getLatestSamples();
		}

	}
	catch(GpsException& ex)
	{
		std::cout << ex.what() << std::endl;
	}

	std::cout << "Hey there ;)" << std::endl;

	return 0;
}

