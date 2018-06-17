/*
	Author: Robert F. Rau II
	Copyright (C) 2017 Robert F. Rau II
*/
#ifndef LIBPIFLY_ADC_ADC_H
#define LIBPIFLY_ADC_ADC_H

#include <string>

#include "comm/spi/serialperipheralinterface.h"

using std::string;

namespace PiFly
{
	namespace ADC
	{
		class Channel
		{
			uint32_t getLatestSample();
		};

		class AnalogDigitalConverter
		{
		public:
			AnalogDigitalConverter(Comm::SPI::ChannelPtr channel);
			~AnalogDigitalConverter();
			void update();

			Channel& getChannel(uint32_t channel);
		private:
			Comm::SPI::ChannelPtr mChannel;
		};
	}
}

#endif // LIBPIFLY_ADC_ADC_H
