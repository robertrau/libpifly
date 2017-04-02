#ifndef LIBPIFLY_GPS_H
#define LIBPIFLY_GPS_H

#include <exception>
#include <string>
#include <sstream>
#include <vector>

#include <errno.h>
#include <string.h>

namespace PiFly
{
	namespace GPS
	{
		using std::string;
		using std::vector;
		using std::exception;
		using std::stringstream;

		class GpsException : public exception
		{
		public:
			GpsException()
			{

			}
		};

		class GpsFdException : public GpsException
		{
		public:
			GpsFdException(int _err, string file=__FILE__, int line=__LINE__) : GpsException(),
				err(_err)
			{

			}

			virtual const char* what() const noexcept
			{
				stringstream ss;
				ss << "GPS file descriptor error. errno = ";
				ss << err;
				ss << ": ";
				ss << strerror(err);
				return ss.str().c_str();
			}

		private:
			int err;
		};

		struct GPSResult
		{

		};

		typedef vector<GPSResult> ResultVector;

		class GlobalPositioningSystem
		{
		public:
			GlobalPositioningSystem(string tty);
			virtual ~GlobalPositioningSystem();

			void start();
			ResultVector getLatestSamples();

		private:

			/*enum
			{

			} */
			void sendCommand();

			int gpsTTY;
		};
	}
}

#endif // LIBPIFLY_GPS_H