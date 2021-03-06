/*
	Author: Robert F. Rau II
	Copyright (C) 2017 Robert F. Rau II
*/
#ifndef LIBPIFLY_COMM_COMMEXCEPTION_H
#define LIBPIFLY_COMM_COMMEXCEPTION_H

#include <exception>
#include <string>
#include <sstream>

#include <string.h>

namespace PiFly {
	namespace Comm {
		using std::string;
		using std::exception;
		using std::stringstream;

		class CommException : public exception {
		public:
			CommException() {}
			CommException(string msg) :
				mMessage(msg)
			{

			}

			virtual const char* what() const noexcept {
				return mMessage.c_str();
			}

		private:
			string mMessage;
		};

		class CommFdException : public CommException {
		public:
			CommFdException(int _err, string file=__FILE__, int line=__LINE__) : 
				err(_err)
			{

			}

			virtual const char* what() const noexcept {
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

		class CommTimeoutException : public CommException {
		public:
			CommTimeoutException(string msg) : CommException(msg) {}
		};
	}
}

#endif // LIBPIFLY_COMM_COMMEXCEPTION_H
