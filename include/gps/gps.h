#ifndef LIBPIFLY_GPS_H
#define LIBPIFLY_GPS_H

#include <exception>
#include <string>
#include <sstream>
#include <vector>

#include <string.h>

#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <termios.h>
#include <unistd.h>

#include "comm/serialport.h"

namespace PiFly
{
	namespace GPS
	{
		using std::string;
		using std::vector;
		using std::exception;
		using std::stringstream;

		using PiFly::Comm::SerialPort;

		class GpsException : public exception
		{
		public:
			GpsException(string msg) :
				mMessage(msg)
			{
			}

			virtual const char* what() const noexcept
			{
				stringstream ss;
				ss << "GPS Error: ";
				ss << mMessage;
				return ss.str().c_str();
			}
		
		private:
			string mMessage;
		};

		class GpsNackException : public GpsException
		{
		public:
			GpsNackException(string msg) : GpsException(msg) {};
		};

		class GpsFormatException : public GpsException
		{
		public:
			GpsFormatException(string msg) : GpsException(msg) {};
		};

		struct GPSResult
		{

		};

		typedef vector<GPSResult> ResultVector;

		class GlobalPositioningSystem
		{
			typedef SerialPort::SerialBuffer SerialBuffer;
		public:
			GlobalPositioningSystem(SerialPort& serialPort);
			virtual ~GlobalPositioningSystem();

			void start();
			ResultVector getLatestSamples();

		private:

			const uint8_t MSG_START_1 = 0xA0;
			const uint8_t MSG_START_2 = 0xA1;
			const uint8_t MSG_END_1 = 0x0D;
			const uint8_t MSG_END_2 = 0x0A;
			typedef enum
			{
				Command_ConfigSerial = 0x5,
				Command_ConfigNMEA = 0x8,
				Command_ConfigNavInterval = 0x11,
				Command_ACK = 0x83,
				Command_NACK = 0x84
			} Command;

			static uint8_t computeChecksum(uint16_t payloadLength, SerialBuffer& buffer);

			void updateBaudrate(SerialPort::Baudrate baud);
			void sendCommand(const SerialBuffer& command);
			bool receiveAckNack();

			void autoNegotiateBaudrate();

			SerialPort& mSerialPort;
		};
	}
}

#endif // LIBPIFLY_GPS_H