/*
	Author: Robert F. Rau II
	Copyright (C) 2017 Robert F. Rau II
*/
#ifndef LIBPIFLY_COMM_COMMEXCEPTION_H
#define LIBPIFLY_COMM_COMMEXCEPTION_H

#include <exception>
#include <string>
#include <sstream>

#include <cstring>

namespace PiFly {
	namespace Comm {
		using std::string;
		using std::exception;
		using std::stringstream;

		class CommException : public exception {
		public:
			CommException() = default;
			CommException(string msg) :
				mMessage(std::move(msg))
			{

			}

			virtual const char* what() const noexcept override {
				return mMessage.c_str();
			}

		protected:
			string mMessage {""};
		};

		class CommFdException : public CommException {
		public:
			CommFdException(int _err, string file=__FILE__, int line=__LINE__) :
				err(_err)
			{

			}

			CommFdException(string message, int _err, string file=__FILE__, int line=__LINE__) :
				err(_err)
			{
				mMessage = message;
			}

			const char* what() const noexcept override {
				stringstream ss;
				ss << "Communication file descriptor error: " << mMessage << ":  errno = ";
				ss << err;
				ss << ": ";
				ss << strerror(err);
				return ss.str().c_str();
			}

			const string message() {
				stringstream ss;
				ss << "Comm fd error: " << mMessage << ": errno = ";
				ss << err;
				ss << ": ";
				ss << strerror(err);
				return ss.str();
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
