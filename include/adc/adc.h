/*
	Author: Robert F. Rau II
	Copyright (C) 2017 Robert F. Rau II
*/
#ifndef LIBPIFLY_ADC_ADC_H
#define LIBPIFLY_ADC_ADC_H

#include <string>

#include "comm/spi/serialperipheralinterface.h"

using std::string;

namespace PiFly {
	namespace ADC {

		class AnalogDigitalConverter;
		class AnalogInput {
		public:
			uint16_t value();
		private:
			AnalogInput();
			friend class AnalogDigitalConverter;
			uint16_t sample;
		};

		typedef enum {
			ChannelSelect_1 = 0x0001,
			ChannelSelect_2 = 0x0002,
			ChannelSelect_3 = 0x0004,
			ChannelSelect_4 = 0x0008,
			ChannelSelect_5 = 0x0010,
			ChannelSelect_6 = 0x0020,
			ChannelSelect_7 = 0x0040,
			ChannelSelect_8 = 0x0080,
			ChannelSelect_9 = 0x0100,
			ChannelSelect_10 = 0x0200,
			ChannelSelect_11 = 0x0400,
			ChannelSelect_12 = 0x0800,
			ChannelSelect_13 = 0x1000,
			ChannelSelect_14 = 0x2000,
			ChannelSelect_15 = 0x4000,
			ChannelSelect_16 = 0x8000,
		} ChannelSelect;

		class AnalogDigitalConverter {
		public:
			AnalogDigitalConverter(Comm::SPI::ChannelPtr channel, uint16_t channels);
			~AnalogDigitalConverter();

			void update();
			AnalogInput& getAnalogInput(uint32_t channel);

		private:

			void programAuto1Mode(uint16_t channels);
			void startAuto1Mode();

			static uint32_t const MaxChannels = 16;

			Comm::SPI::ChannelPtr mSpiChannel;
			AnalogInput mInputs[MaxChannels];
		};
	}
}

#endif // LIBPIFLY_ADC_ADC_H
