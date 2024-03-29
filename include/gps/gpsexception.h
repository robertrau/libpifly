/*
	Author: Robert F. Rau II
	Copyright (C) 2017 Robert F. Rau II
*/
#ifndef LIBPIFLY_GPS_GPSEXCEPTION_H
#define LIBPIFLY_GPS_GPSEXCEPTION_H

#include <exception>
#include <string>
#include <sstream>

namespace PiFly {
	namespace GPS {
		using std::string;
		using std::exception;
		using std::stringstream;

		class GpsException : public exception {
		public:
			GpsException() = default;
			GpsException(string msg) :
				mMessage(std::move(msg))
			{
			}

			const char* what() const noexcept override {
				stringstream ss;
				ss << "GPS Error: ";
				ss << mMessage;
				return ss.str().c_str();
			}
		
		private:
			string mMessage{""};
		};

		class GpsNackException : public GpsException {
		public:
			GpsNackException(string msg) : GpsException(msg) {};
		};

		class GpsFormatException : public GpsException {
		public:
			GpsFormatException(string msg) : GpsException(msg) {};
		};
	}
}

#endif // LIBPIFLY_GPS_GPSEXCEPTION_H
