#include "gps/gps.h"

#include <iostream>
#include <thread>

namespace PiFly
{
	namespace GPS
	{
		using std::thread;
		using std::exception;

		GlobalPositioningSystem::GlobalPositioningSystem(SerialPort& serialPort) :
			mSerialPort(serialPort)
		{
		}

		GlobalPositioningSystem::~GlobalPositioningSystem()
		{

		}

		void GlobalPositioningSystem::start()
		{

		}

		ResultVector GlobalPositioningSystem::getLatestSamples()
		{
			ResultVector resVec;

			SerialPort::SerialBuffer buff(256);
			mSerialPort.read(buff);
			//buff.shrink_to_fit();
			std::cout << buff.data();
			std::cout.flush();
			
			return resVec;
		}
	}
}
