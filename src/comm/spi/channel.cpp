/*
	Author: Robert F. Rau II
	Copyright (C) 2018 Robert F. Rau II
*/
#include <iostream>
#include <thread>         // std::this_thread::sleep_for
#include <chrono>         // std::chrono::seconds

#include <cassert>

#include "comm/spi/channel.h"
#include "comm/spi/serialperipheralinterface.h"

namespace PiFly {
	namespace Comm {
		namespace SPI {

			Channel::Channel(ChipSelect cs, Mode mode, BitOrder bitOrder, ClockDivider clockDivider, ChipSelectPolarity polarity, SerialPeripheralInterface& spi) :
				mCs(cs),
				mMode(mode),
				mBitOrder(bitOrder),
				mClockDivider(clockDivider),
				mPolarity(polarity),
				mSpi(spi)
			{

			}

			Channel::~Channel() {
				std::cout << "Destroying channel" << std::endl;
				mSpi.freeAssociatedChannel(mCs);
			}

			bool Channel::start() {
				/*auto ret = bcm2835_spi_begin();
				if(ret != 1) {
					std::cout << "Failed to begin spi operation" << std::endl;
					return false;
				}*/

				//std::cout << "Setting spi clock to about 15Mhz" << std::endl;
				bcm2835_spi_setClockDivider(mClockDivider);

				bcm2835_spi_setDataMode(mMode);

				bcm2835_spi_chipSelect(mCs);

				bcm2835_spi_setChipSelectPolarity(mCs, mPolarity);

				return true;
			}

			void Channel::end() {
				//bcm2835_spi_end();
			}

			bool Channel::transfer(SerialBuffer& write, SerialBuffer& read) {
				if(!start()) {
					return false;
				}

				uint32_t transferLen = write.size() >= read.size() ? read.size() : write.size();
				// Really wish this cast wasn't necessary :(
				// but because libbcm2835 uses char* for data
				// and not void* we have to do this cast :/
				// Linting disabled due to this
				auto txPtr = reinterpret_cast<char*>(write.data()); // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
				auto rxPtr = reinterpret_cast<char*>(read.data()); // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
				bcm2835_spi_transfernb(txPtr, rxPtr, transferLen);

				end();

				return true;
			}

			bool Channel::transfer(SerialBuffer& readWrite) {
				if(!start()) {
					return false;
				}

				// Really wish this cast wasn't necessary :(
				// but because libbcm2835 uses char* for data
				// and not void* we have to do this cast :/
				// Linting disabled due to this
				auto bufPtr = reinterpret_cast<char*>(readWrite.data()); // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
				bcm2835_spi_transfern(bufPtr, readWrite.size());

				end();

				return true;
			}
		}
	}
}
