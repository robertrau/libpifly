/*
	Author: Robert F. Rau II
	Copyright (C) 2017 Robert F. Rau II
*/
#ifndef LIBPIFLY_GPS_SKYTRAQBINARYPROTOCOL_H
#define LIBPIFLY_GPS_SKYTRAQBINARYPROTOCOL_H

#include "comm/serialport.h"
#include "gps/gps.h"
#include "gps/igpsprotocol.h"
#include "gps/skytraq/skytraqvenus.h"

namespace PiFly
{
	namespace GPS
	{
		namespace Skytraq
		{
			using Comm::SerialPort;
			using Comm::SerialArray;
			using Comm::SerialBuffer;

			class SkyTraqBinaryProtocol : public IGpsProtocol
			{
			public:

				SkyTraqBinaryProtocol(SerialPort& serialPort, SkyTraqVenus& skytraqVenus);

				virtual const bool haveResult() const;
				virtual void update();
				virtual GpsResult getResult();

				bool getResult(GpsResult& result);
			private:
				const static size_t updateBufferSize = 128;

				SkyTraqVenus& mSkyTraqVenus;
				SerialPort& mSerialPort;
				bool mHaveResult;
				GpsResult mGpsResult;
			};
		}
	}
}

#endif // LIBPIFLY_GPS_SKYTRAQBINARYPROTOCOL_H
