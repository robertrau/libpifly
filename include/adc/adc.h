/*
	Author: Robert F. Rau II
	Copyright (C) 2017 Robert F. Rau II
*/
#ifndef LIBPIFLY_ADC_ADC_H
#define LIBPIFLY_ADC_ADC_H

#include <bitset>
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
			AnalogInput() = default;
			friend class AnalogDigitalConverter;
			uint16_t sample{0};
		};

		enum Channel {
			Channel_0 = 0x0001,
			Channel_1 = 0x0002,
			Channel_2 = 0x0004,
			Channel_3 = 0x0008,
			Channel_4 = 0x0010,
			Channel_5 = 0x0020,
			Channel_6 = 0x0040,
			Channel_7 = 0x0080,
			Channel_8 = 0x0100,
			Channel_9 = 0x0200,
			Channel_10 = 0x0400,
			Channel_11 = 0x0800,
			Channel_12 = 0x1000,
			Channel_13 = 0x2000,
			Channel_14 = 0x4000,
			Channel_15 = 0x8000,
		};

		class AnalogDigitalConverter {
		public:
			AnalogDigitalConverter(const AnalogDigitalConverter&) = delete;
			AnalogDigitalConverter& operator=(const AnalogDigitalConverter&) = delete;

			AnalogDigitalConverter(AnalogDigitalConverter&&) = default;
			AnalogDigitalConverter& operator=(AnalogDigitalConverter&&) = default;

			AnalogDigitalConverter(Comm::SPI::ChannelPtr&& channel, std::bitset<16> channels);

			~AnalogDigitalConverter() = default;

			void update();
			//void udpate(uint16_t channelMask);

			AnalogInput& getAnalogInput(uint32_t channel);
		private:

			void programAuto1Mode();
			void startAuto1Mode();

			static uint32_t const MaxChannels = 16;

			std::bitset<16> mChannels;
			Comm::SPI::ChannelPtr mSpiChannel;
			std::array<AnalogInput, MaxChannels> mInputs;
		};
	}
}

#endif // LIBPIFLY_ADC_ADC_H
