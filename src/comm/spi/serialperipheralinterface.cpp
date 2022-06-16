/*
	Author: Robert F. Rau II
	Copyright (C) 2018 Robert F. Rau II
*/
#include <iostream>
#include <thread>         // std::this_thread::sleep_for
#include <chrono>         // std::chrono::seconds

#include <cassert>

#include <system/libbcm2835.h>

#include "comm/spi/serialperipheralinterface.h"

namespace PiFly {
	namespace Comm {
		namespace SPI {
			using System::Libbcm2835;

			SerialPeripheralInterface::SerialPeripheralInterface() :
				mUsedChipSelects{false}
			{
				// TODO: Check for successful init, maybe throw if failed

				Libbcm2835::Init();

				auto ret = bcm2835_spi_begin();
				if(ret != 1) {
					std::cout << "Failed to begin spi operation" << std::endl;
				}
			}

			SerialPeripheralInterface::~SerialPeripheralInterface() {
				bcm2835_spi_end();
				Libbcm2835::Close();
			}

			ChannelPtr SerialPeripheralInterface::getChannel(ChipSelect cs, Mode mode, BitOrder bitOrder, ClockDivider clockDivider, ChipSelectPolarity polarity) {
				if(cs < ChipSelect_None) {
					if(mUsedChipSelects[cs]) {
						return ChannelPtr(nullptr);
					}
					mUsedChipSelects[cs] = true;
				} else if(cs > ChipSelect_None) {
					return ChannelPtr(nullptr);
				}

				return ChannelPtr(
					new Channel(cs, mode, bitOrder, clockDivider, polarity, *this)
				);
			}

			void SerialPeripheralInterface::freeAssociatedChannel(ChipSelect cs) {
				// Silence clang-tidy warning about code we have no control over
				assert(cs < ChipSelect_None); // NOLINT(cppcoreguidelines-pro-bounds-array-to-pointer-decay, cppcoreguidelines-pro-bounds-constant-array-index)
				mUsedChipSelects[cs] = false;
			}
		}
	}
}
