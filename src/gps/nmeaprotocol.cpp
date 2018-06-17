/*
	Author: Robert F. Rau II
	Copyright (C) 2017 Robert F. Rau II
*/
#include "gps/nmeaprotocol.h"

#include <iostream>

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
			SerialArray<64> buffer;
			auto bytesRead = mSerialPort.read<64>(buffer.begin());
			for(uint32_t i = 0; i < bytesRead; i++)
			{
				std::cout << static_cast<char>(buffer[i]);
			}
		}

		GpsResult NMEAProtocol::getResult()
		{
			return GpsResult();
		}
	}
}