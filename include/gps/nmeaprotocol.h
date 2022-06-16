/*
	Author: Robert F. Rau II
	Copyright (C) 2017 Robert F. Rau II
*/
#ifndef LIBPIFLY_GPS_NMEAPROTOCOL_H
#define LIBPIFLY_GPS_NMEAPROTOCOL_H

#include "comm/serialport.h"
#include "gps/igpsprotocol.h"

namespace PiFly {
	namespace GPS {
		using Comm::SerialPort;
		using Comm::SerialArray;
		using Comm::SerialBuffer;

		class NMEAProtocol : public IGpsProtocol {
		public:
			NMEAProtocol(SerialPort& serialPort);

			bool getResult(GpsResult& gpsResult) override;

		private:
			SerialPort& mSerialPort;
		};
	}
}

#endif // LIBPIFLY_GPS_NMEAPROTOCOL_H
