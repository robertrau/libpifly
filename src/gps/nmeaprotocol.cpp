/*
	Author: Robert F. Rau II
	Copyright (C) 2017 Robert F. Rau II
*/
#include "gps/nmeaprotocol.h"

namespace PiFly
{
	namespace GPS
	{
		NMEAProtocol::NMEAProtocol(SerialPort& serialPort) :
			mSerialPort(serialPort)
		{

		}
		
		const bool NMEAProtocol::haveResult() const
		{
			return false;
		}
		
		void NMEAProtocol::update()
		{

		}

		GpsResult NMEAProtocol::getResult()
		{
			return GpsResult();
		}
	}
}