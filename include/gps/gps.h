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
			GpsException()
			{

			}
		};

		struct GPSResult
		{

		};

		typedef vector<GPSResult> ResultVector;

		class GlobalPositioningSystem
		{
		public:
			GlobalPositioningSystem(SerialPort& serialPort);
			virtual ~GlobalPositioningSystem();

			void start();
			ResultVector getLatestSamples();

		private:

			/*enum
			{

			} */
			void sendCommand();


			SerialPort& mSerialPort;
		};
	}
}

#endif // LIBPIFLY_GPS_H