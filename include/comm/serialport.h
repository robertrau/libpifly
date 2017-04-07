#ifndef LIBPIFLY_COMM_SERIALPORT_H
#define LIBPIFLY_COMM_SERIALPORT_H

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

namespace PiFly
{
	namespace Comm
	{
		using std::string;
		using std::vector;
		using std::exception;
		using std::stringstream;

		class CommException : public exception
		{
		public:
			CommException()
			{

			}
		};

		class CommFdException : public CommException
		{
		public:
			CommFdException(int _err, string file=__FILE__, int line=__LINE__) : CommException(),
				err(_err)
			{

			}

			virtual const char* what() const noexcept
			{
				stringstream ss;
				ss << "Communication file descriptor error. errno = ";
				ss << err;
				ss << ": ";
				ss << strerror(err);
				return ss.str().c_str();
			}

		private:
			int err;
		};

		class SerialPort
		{
		public:
			typedef vector<uint8_t> SerialBuffer;
			typedef enum
			{
				Baudrate_0,
				Baudrate_50,
				Baudrate_75,
				Baudrate_110,
				Baudrate_134,
				Baudrate_150,
				Baudrate_200,
				Baudrate_300,
				Baudrate_600,
				Baudrate_1200,
				Baudrate_1800,
				Baudrate_2400,
				Baudrate_4800,
				Baudrate_9600,
				Baudrate_19200,
				Baudrate_38400,
				Baudrate_57600,
				Baudrate_115200,
				Baudrate_230400
			} Baudrate;

			SerialPort(string devPath, Baudrate baud);
			virtual ~SerialPort();

			size_t read(SerialBuffer& buffer, size_t readBytes);
			void write(const SerialBuffer& buffer);

			void setBaudrate(Baudrate baud);
			Baudrate getBaudrate();
			void flush();

			static string baudrateString(Baudrate baud);

		private:
			int serialFd;
			struct termios serialTTY;

			static speed_t linuxBaudrateMap(Baudrate baud);
			static Baudrate linuxBaudrateMap(speed_t baud);
		};
	}
}

#endif // LIBPIFLY_COMM_SERIALPORT_H