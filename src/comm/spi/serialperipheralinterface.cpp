/*
	Author: Robert F. Rau II
	Copyright (C) 2018 Robert F. Rau II
*/
#include <iostream>
#include <thread>         // std::this_thread::sleep_for
#include <chrono>         // std::chrono::seconds

#include <system/libbcm2835.h>

#include "comm/spi/serialperipheralinterface.h"

namespace PiFly
{
	namespace Comm
	{
		namespace SPI
		{
			using System::Libbcm2835;

			SerialPeripheralInterface::SerialPeripheralInterface()
			{
				// TODO: Check for successful init, maybe throw if failed
				Libbcm2835::Init();
			}

			SerialPeripheralInterface::~SerialPeripheralInterface()
			{
				Libbcm2835::Close();
			}

			ChannelPtr SerialPeripheralInterface::getChannel(ChipSelect cs, Mode mode, BitOrder bitOrder, ClockDivider clockDivider, ChipSelectPolarity polarity)
			{
				return ChannelPtr(new Channel(cs, mode, bitOrder, clockDivider, polarity), ChannelDeleter(*this));
			}

			void SerialPeripheralInterface::freeAssociatedChannel(ChipSelect cs)
			{

			}

			ChannelDeleter::ChannelDeleter(SerialPeripheralInterface& spi) :
				mSpi(spi)
			{}

			void ChannelDeleter::operator()(Channel* channel) const
			{
				if(channel != nullptr)
				{
					mSpi.freeAssociatedChannel(channel->mCs);
					delete channel;
				}
			}
		}
	}
}
