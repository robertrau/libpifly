/*
	Author: Robert F. Rau II
	Copyright (C) 2017 Robert F. Rau II
*/
#include <iostream>
#include <thread>         // std::this_thread::sleep_for
#include <chrono>         // std::chrono::seconds

#include <bcm2835.h>

#include "adc/adc.h"

namespace PiFly
{
	namespace ADC
	{
		AnalogDigitalConverter::AnalogDigitalConverter(Comm::SPI::ChannelPtr channel) :
			mChannel(std::move(channel))
		{

		}

		AnalogDigitalConverter::~AnalogDigitalConverter()
		{

		}

		void AnalogDigitalConverter::update()
		{
			Comm::SerialArray<2> readWrite({0x0, 0x0});
			mChannel->transfer(readWrite);
		}
	}
}
