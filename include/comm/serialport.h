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

			SerialPort(string devPath);
			virtual ~SerialPort();

			void read(SerialBuffer& buffer);
			void write(SerialBuffer& buffer);
			
		private:
			int serialFd;
			struct termios serialTTY;
		};
	}
}

#endif // LIBPIFLY_COMM_SERIALPORT_H