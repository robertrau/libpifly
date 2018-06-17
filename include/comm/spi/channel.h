/*
	Author: Robert F. Rau II
	Copyright (C) 2018 Robert F. Rau II
*/
#ifndef LIBPIFLY_COMM_SPI_CHANNEL_H
#define LIBPIFLY_COMM_SPI_CHANNEL_H

#include <array>
#include <exception>
#include <memory>
#include <string>
#include <sstream>
#include <vector>

#include <string.h>

#include "comm/commexception.h"
#include "comm/comm.h"
#include "comm/spi/spi.h"

namespace PiFly
{
	namespace Comm
	{
		namespace SPI
		{
			using std::array;
			using std::vector;

			class Channel
			{
				friend class SerialPeripheralInterface;
				friend class ChannelDeleter;
				Channel(ChipSelect cs, Mode mode, BitOrder bitOrder, ClockDivider clockDivider, ChipSelectPolarity polarity);

				bool start();
				void end();

				ChipSelect mCs;
				Mode mMode;
				BitOrder mBitOrder;
				ClockDivider mClockDivider;
				ChipSelectPolarity mPolarity;

			public:

				bool transfer(SerialBuffer write, SerialBuffer read);
				bool transfer(SerialBuffer readWrite);

				template<size_t size>
				bool transfer(SerialArray<size> write, SerialArray<size> read)
				{
					if(!start())
					{
						return false;
					}

					// Really wish this cast wasn't necessary :(
					char* txPtr = reinterpret_cast<char*>(write.data());
					char* rxPtr = reinterpret_cast<char*>(read.data());
					bcm2835_spi_transfernb(txPtr, rxPtr, size);

					end();

					return true;
				}
				
				template<size_t size>
				bool transfer(SerialArray<size> readWrite)
				{
					if(!start())
					{
						return false;
					}

					char* bufPtr = reinterpret_cast<char*>(readWrite.data());
					bcm2835_spi_transfern(bufPtr, size);

					end();

					return true;
				}
			};
		}
	}
}

#endif // LIBPIFLY_COMM_SPI_CHANNEL_H
