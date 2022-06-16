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

#include <cstring>

#include "comm/commexception.h"
#include "comm/comm.h"
#include "comm/spi/spi.h"

namespace PiFly {
	namespace Comm {
		namespace SPI {

			using std::array;
			using std::vector;

			class SerialPeripheralInterface;

			class Channel {
				friend class SerialPeripheralInterface;
				Channel(ChipSelect cs, Mode mode, BitOrder bitOrder, ClockDivider clockDivider, ChipSelectPolarity polarity, SerialPeripheralInterface& spi);

				bool start();
				void end();

				ChipSelect mCs;
				Mode mMode;
				BitOrder mBitOrder;
				ClockDivider mClockDivider;
				ChipSelectPolarity mPolarity;
				SerialPeripheralInterface& mSpi;
			public:
				Channel(const Channel&) = delete;
				Channel(Channel&&) = delete;
				~Channel();

				/** A single SPI transaction with seperate read and write buffers.
				 *
				 */
				bool transfer(SerialBuffer& write, SerialBuffer& read);
				/** A single SPI transaction with a single buffer that is written from and read to.
				 *
				 */
				bool transfer(SerialBuffer& readWrite);

				template<size_t size>
				bool transfer(SerialArray<size>& write, SerialArray<size>& read) {
					if(!start()) {
						return false;
					}

					// Really wish this cast wasn't necessary :(
					// but because libbcm2835 uses char* for data
					// and not void* we have to do this cast :/
					// Linting disabled due to this
					auto txPtr = reinterpret_cast<char*>(write.data()); // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
					auto rxPtr = reinterpret_cast<char*>(read.data()); // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
					bcm2835_spi_transfernb(txPtr, rxPtr, size);

					end();

					return true;
				}

				template<size_t size>
				bool transfer(SerialArray<size>& readWrite) {
					if(!start()) {
						return false;
					}

					// Really wish this cast wasn't necessary :(
					// but because libbcm2835 uses char* for data
					// and not void* we have to do this cast :/
					auto bufPtr = reinterpret_cast<char*>(readWrite.data()); // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
					bcm2835_spi_transfern(bufPtr, size);

					end();

					return true;
				}

				Channel& operator=(const Channel&) = delete;
				Channel& operator=(Channel&&) = delete;
			};
		}
	}
}

#endif // LIBPIFLY_COMM_SPI_CHANNEL_H
