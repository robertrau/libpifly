/*
	Author: Robert F. Rau II
	Copyright (C) 2018 Robert F. Rau II
*/
#ifndef LIBPIFLY_COMM_SPI_SERIALPERIPHERALINTERFACES_H
#define LIBPIFLY_COMM_SPI_SERIALPERIPHERALINTERFACES_H

#include <array>
#include <exception>
#include <memory>
#include <string>
#include <sstream>
#include <vector>

#include <string.h>

#include "comm/commexception.h"
#include "comm/spi/spi.h"
#include "comm/spi/channel.h"

namespace PiFly
{
	namespace Comm
	{
		namespace SPI
		{
			class SerialPeripheralInterface;
			/**
			 * ChannelDeleter is a deleter class for std::unique_ptr
			 * so that the SerialPeripheralInterface can be told when
			 * a channel is no longer being used by anything, thus allowing
			 * something else to grab and use the channel.
			 */
			class ChannelDeleter
			{
				friend class SerialPeripheralInterface;
				ChannelDeleter() = delete;
				ChannelDeleter(SerialPeripheralInterface& spi);
				SerialPeripheralInterface& mSpi;
			public:
				void operator()(Channel* channel) const;
			};

			typedef std::unique_ptr<Channel, ChannelDeleter> ChannelPtr;

			class SerialPeripheralInterface
			{
			private:
				friend class ChannelDeleter;
				void freeAssociatedChannel(ChipSelect cs);

			public:

				SerialPeripheralInterface();
				~SerialPeripheralInterface();

				ChannelPtr getChannel(ChipSelect cs, Mode mode, BitOrder bitOrder, ClockDivider clockDivider, ChipSelectPolarity polarity);
			};
		}
	}
}

#endif // LIBPIFLY_COMM_SPI_SERIALPERIPHERALINTERFACES_H
