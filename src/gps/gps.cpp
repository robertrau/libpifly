#include "gps/gps.h"

#include <iostream>
#include <thread>

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <termios.h>
#include <unistd.h>

namespace PiFly
{
	namespace GPS
	{
		using std::thread;
		using std::exception;

		GlobalPositioningSystem::GlobalPositioningSystem(string ttydev)
		{
			gpsTTY = open(ttydev.c_str(), O_RDWR | O_NONBLOCK | O_NDELAY);
			if(gpsTTY < 0)
			{
				throw GpsFdException(errno);
			}

			struct termios tty;
			memset(&tty, 0, sizeof(termios));

			if(tcgetattr(gpsTTY, &tty) != 0)
			{
				throw GpsFdException(errno);
			}

			// Set baud rate to 115200
			cfsetispeed(&tty, B115200);
			cfsetospeed(&tty, B115200);

			// make a raw serial port
			cfmakeraw(&tty);

			if(tcsetattr(gpsTTY, TCSANOW, &tty) != 0)
			{
				throw GpsFdException(errno);
			}
		}

		GlobalPositioningSystem::~GlobalPositioningSystem()
		{
			if(gpsTTY >= 0)
			{
				close(gpsTTY);
			}
		}

		void GlobalPositioningSystem::start()
		{

		}

		ResultVector GlobalPositioningSystem::getLatestSamples()
		{
			ResultVector resVec;

			uint8_t buffer[256];
			read(gpsTTY, buffer, 256);
			std::cout << buffer << std::endl;
			
			return resVec;
		}
	}
}
