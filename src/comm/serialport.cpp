#include "comm/serialport.h"

#include <iostream>

#include <unistd.h>

namespace PiFly
{
	namespace Comm
	{
		SerialPort::SerialPort(string devPath)
		{
			//serialFd = open(devPath.c_str(), O_RDWR | O_NONBLOCK | O_NDELAY);
			serialFd = open(devPath.c_str(), O_RDWR);
			if(serialFd < 0)
			{
				throw CommFdException(errno);
			}

			memset(&serialTTY, 0, sizeof(termios));

			if(tcgetattr(serialFd, &serialTTY) != 0)
			{
				throw CommFdException(errno);
			}

			// Set baud rate to 115200
			cfsetispeed(&serialTTY, B115200);
			cfsetospeed(&serialTTY, B115200);

			// make a raw serial port
			cfmakeraw(&serialTTY);

			if(tcsetattr(serialFd, TCSANOW, &serialTTY) != 0)
			{
				throw CommFdException(errno);
			}
		}

		SerialPort::~SerialPort()
		{
			if(serialFd >= 0)
			{
				close(serialFd);
			}
		}

		void SerialPort::read(SerialBuffer& buffer)
		{
			int resp;
			do
			{
				resp = ::read(serialFd, static_cast<void*>(buffer.data()), static_cast<size_t>(buffer.size()));
				//std::cout << "resp = " << resp << std::endl;
			} while(((resp == EAGAIN) || (resp == EWOULDBLOCK)) || (resp < 0));
			std::cout << "got it, resp = " << static_cast<uint32_t>(buffer[0]) << std::endl;
			if(resp >= 0)
			{
				buffer.resize(resp);
			}
			else if(resp == 0)
			{
				std:: cout << "resp = 0";
			}
			else
			{
				throw CommFdException(resp);
			}
		}
	}
}