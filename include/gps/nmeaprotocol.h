#ifndef LIBPIFLY_GPS_NMEAPROTOCOL_H
#define LIBPIFLY_GPS_NMEAPROTOCOL_H

#include "comm/serialport.h"
#include "gps/igpsprotocol.h"

namespace PiFly
{
	namespace GPS
	{
		using Comm::SerialPort;
		
		class NMEAProtocol : public IGpsProtocol
		{
		public:
			NMEAProtocol(SerialPort& serialPort);

			virtual const bool haveResult() const;
			virtual void update();
			virtual GpsResult getResult();

		private:
			SerialPort& mSerialPort;
		};
	}
}

#endif // LIBPIFLY_GPS_NMEAPROTOCOL_H
