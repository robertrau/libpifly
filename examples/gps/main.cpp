#include <atomic>
#include <iostream>

#include <signal.h>

#include "gps/gps.h"

using PiFly::GPS::GlobalPositioningSystem;
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
		GlobalPositioningSystem gps(serialPort);

		std::cout << "Starting gps" << std::endl;
		gps.start();

		while(!interrupted.load())
		{
			auto sampleBuff = gps.getLatestSamples();
		}

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

