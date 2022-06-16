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

#include <cstring>

#include "comm/commexception.h"
#include "comm/spi/spi.h"
#include "comm/spi/channel.h"

namespace PiFly {
	namespace Comm {
		namespace SPI {

			using ChannelPtr = std::unique_ptr<Channel>;

			class SerialPeripheralInterface {
			private:
				friend class Channel;
				void freeAssociatedChannel(ChipSelect cs);

				bool mUsedChipSelects[ChipSelect_None];
			public:
				SerialPeripheralInterface(const SerialPeripheralInterface&) = delete;
				SerialPeripheralInterface& operator=(const SerialPeripheralInterface&) = delete;

				SerialPeripheralInterface(SerialPeripheralInterface&&) = default;
				SerialPeripheralInterface& operator=(SerialPeripheralInterface&&) = default;

				SerialPeripheralInterface();
				~SerialPeripheralInterface();

				ChannelPtr getChannel(ChipSelect cs, Mode mode, BitOrder bitOrder, ClockDivider clockDivider, ChipSelectPolarity polarity);
			};
		}
	}
}

#endif // LIBPIFLY_COMM_SPI_SERIALPERIPHERALINTERFACES_H
