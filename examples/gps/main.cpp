#include <iostream>

#include "gps/gps.h"

using PiFly::GPS::GlobalPositioningSystem;
using PiFly::GPS::GpsException;

int main(int argc, char** argv)
{
	try
	{
		GlobalPositioningSystem gps("/dev/serial0");

		gps.start();

		auto sampleBuff = gps.getLatestSamples();
		sampleBuff = gps.getLatestSamples();
		sampleBuff = gps.getLatestSamples();
	}
	catch(GpsException& ex)
	{
		std::cout << ex.what() << std::endl;
	}

	std::cout << "Hey there ;)" << std::endl;

	return 0;
}

