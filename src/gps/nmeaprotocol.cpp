/*
	Author: Robert F. Rau II
	Copyright (C) 2017 Robert F. Rau II
*/
#include "gps/nmeaprotocol.h"

#include <iostream>

namespace PiFly {
	namespace GPS {
		NMEAProtocol::NMEAProtocol(SerialPort& serialPort) :
			mSerialPort(serialPort)
		{

		}

		bool NMEAProtocol::getResult(GpsResult& gpsResult) {
			return false;
		}
	}
}