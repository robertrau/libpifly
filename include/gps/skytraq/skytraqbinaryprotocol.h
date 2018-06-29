/*
	Author: Robert F. Rau II
	Copyright (C) 2017 Robert F. Rau II
*/
#ifndef LIBPIFLY_GPS_SKYTRAQBINARYPROTOCOL_H
#define LIBPIFLY_GPS_SKYTRAQBINARYPROTOCOL_H

#include "comm/serialport.h"
#include "gps/igpsprotocol.h"
#include "gps/skytraq/skytraqvenus.h"

namespace PiFly {
	namespace GPS {
		namespace Skytraq {
			using Comm::SerialPort;
			using Comm::SerialArray;
			using Comm::SerialBuffer;

			class SkyTraqBinaryProtocol : public IGpsProtocol {
			public:

				SkyTraqBinaryProtocol(SerialPort& serialPort);
				~SkyTraqBinaryProtocol();

				bool getResult(GpsResult& result);
			private:
				const static size_t updateBufferSize = 128;

				SerialPort& mSerialPort;
			};
		}
	}
}

#endif // LIBPIFLY_GPS_SKYTRAQBINARYPROTOCOL_H
