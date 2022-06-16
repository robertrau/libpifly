/*
	Author: Robert F. Rau II
	Copyright (C) 2017 Robert F. Rau II
*/
#include "comm/serialport.h"
#include "comm/commexception.h"

#include <iostream>

#include <unistd.h>

namespace PiFly
{
	namespace Comm
	{
		SerialPort::SerialPort(string devPath, Baudrate baud, bool blocking) :
			serialFd(-1),
			serialTTY(),
			mBlocking(blocking)
		{
			// Disable var-arg linting for these open calls, we have no other interface to use.
			if(mBlocking) {
				serialFd = ::open(devPath.c_str(), O_RDWR); //NOLINT(cppcoreguidelines-pro-type-vararg)
			} else {
				serialFd = ::open(devPath.c_str(), O_RDWR | O_NONBLOCK | O_NDELAY); //NOLINT(cppcoreguidelines-pro-type-vararg)
			}

			if(serialFd < 0) {
				throw CommFdException(errno);
			}

			memset(&serialTTY, 0, sizeof(termios));

			if(tcgetattr(serialFd, &serialTTY) != 0) {
				throw CommFdException(errno);
			}

			if(cfsetispeed(&serialTTY, linuxBaudrateMap(baud)) != 0) {
				throw CommFdException(errno);
			}
			
			if(cfsetospeed(&serialTTY, linuxBaudrateMap(baud)) != 0) {
				throw CommFdException(errno);
			}
			
			// make a raw serial port
			cfmakeraw(&serialTTY);

			if(tcsetattr(serialFd, TCSANOW, &serialTTY) != 0) {
				throw CommFdException(errno);
			}
		}

		SerialPort::~SerialPort() {
			if(serialFd >= 0) {
				close(serialFd);
			}
		}

		size_t SerialPort::read(SerialBuffer::iterator first, size_t readBytes) {
			if(!mBlocking) {
				int resp = ::read(serialFd, static_cast<void*>(&(*first)), readBytes);
				
				if(resp > 0) {
					return resp;
				} else if((resp < 0) && (errno != EAGAIN)) {
					throw CommFdException(errno);
				} else {
					return 0;
				}
			} else {
				int resp = ::read(serialFd, static_cast<void*>(&(*first)), readBytes);
				
				if(resp > 0) {
					return resp;
				} else if(resp == 0) {
					throw CommFdException(errno);
				} else {
					throw CommFdException(resp);
				}
			}
		}

		void SerialPort::write(const SerialBuffer& buffer) {
			size_t bytesWritten = 0;
			ssize_t resp;
			do {
				resp = ::write(serialFd, static_cast<const void*>(buffer.data()), buffer.size());
				if(resp > 0) {
					bytesWritten += resp;
				} else if(resp == 0) {
					throw CommFdException(errno);
				} else {
					throw CommFdException(resp);
				}
			} while(bytesWritten < buffer.size());
		}

		void SerialPort::setBaudrate(Baudrate baud) {
			if(cfsetispeed(&serialTTY, linuxBaudrateMap(baud)) != 0) {
				throw CommFdException(errno);
			}

			if(cfsetospeed(&serialTTY, linuxBaudrateMap(baud)) != 0) {
				throw CommFdException(errno);
			}

			if(tcsetattr(serialFd, TCSANOW, &serialTTY) != 0) {
				throw CommFdException(errno);
			}
		}

		SerialPort::Baudrate SerialPort::getBaudrate() {
			speed_t currentBaud = cfgetispeed(&serialTTY);
			return linuxBaudrateMap(currentBaud);
		}

		void SerialPort::flush() {
			tcflush(serialFd, TCIOFLUSH);
		}

		SerialPort::Baudrate SerialPort::linuxBaudrateMap(speed_t baud) {
			switch(baud) {
				case B0:
					return Baudrate_0;
				case B50:
					return Baudrate_50;
				case B75:
					return Baudrate_75;
				case B110:
					return Baudrate_110;
				case B134:
					return Baudrate_134;
				case B150:
					return Baudrate_150;
				case B200:
					return Baudrate_200;
				case B300:
					return Baudrate_300;
				case B600:
					return Baudrate_600;
				case B1200:
					return Baudrate_1200;
				case B1800:
					return Baudrate_1800;
				case B2400:
					return Baudrate_2400;
				case B4800:
					return Baudrate_4800;
				case B9600:
					return Baudrate_9600;
				case B19200:
					return Baudrate_19200;
				case B38400:
					return Baudrate_38400;
				case B57600:
					return Baudrate_57600;
				case B115200:
					return Baudrate_115200;
				case B230400:
					return Baudrate_230400;
				default:
					std::cout << "linuxBaudrateMap speed_t Input baud: " << baud << "\n";
					throw CommException("Unsupported baudrate");
			}
		}

		speed_t SerialPort::linuxBaudrateMap(Baudrate baud) {
			switch(baud) {
				case Baudrate_0:
					return B0;
				case Baudrate_50:
					return B50;
				case Baudrate_75:
					return B75;
				case Baudrate_110:
					return B110;
				case Baudrate_134:
					return B134;
				case Baudrate_150:
					return B150;
				case Baudrate_200:
					return B200;
				case Baudrate_300:
					return B300;
				case Baudrate_600:
					return B600;
				case Baudrate_1200:
					return B1200;
				case Baudrate_1800:
					return B1800;
				case Baudrate_2400:
					return B2400;
				case Baudrate_4800:
					return B4800;
				case Baudrate_9600:
					return B9600;
				case Baudrate_19200:
					return B19200;
				case Baudrate_38400:
					return B38400;
				case Baudrate_57600:
					return B57600;
				case Baudrate_115200:
					return B115200;
				case Baudrate_230400:
					return B230400;
				default:
					std::cout << "linuxBaudrateMap Baudrate Input baud: " << baud << "\n";
					throw CommException("Unsupported baudrate");
			}
		}

		string SerialPort::baudrateString(Baudrate baud) {
			switch(baud) {
				case Baudrate_0:
					return "0";
				case Baudrate_50:
					return "50";
				case Baudrate_75:
					return "75";
				case Baudrate_110:
					return "110";
				case Baudrate_134:
					return "134";
				case Baudrate_150:
					return "150";
				case Baudrate_200:
					return "200";
				case Baudrate_300:
					return "300";
				case Baudrate_600:
					return "600";
				case Baudrate_1200:
					return "1200";
				case Baudrate_1800:
					return "1800";
				case Baudrate_2400:
					return "2400";
				case Baudrate_4800:
					return "4800";
				case Baudrate_9600:
					return "9600";
				case Baudrate_19200:
					return "19200";
				case Baudrate_38400:
					return "38400";
				case Baudrate_57600:
					return "57600";
				case Baudrate_115200:
					return "115200";
				case Baudrate_230400:
					return "230400";
				default:
					return "unsupported";
			}
		}
	}
}