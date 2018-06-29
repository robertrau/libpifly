/*
	Author: Robert F. Rau II
	Copyright (C) 2017 Robert F. Rau II
*/
#include <iostream>
#include <thread>         // std::this_thread::sleep_for
#include <chrono>         // std::chrono::seconds
#include <cassert>

#include "adc/adc.h"


namespace PiFly {
	namespace ADC {

		AnalogDigitalConverter::AnalogDigitalConverter(Comm::SPI::ChannelPtr channel, uint16_t channels) :
			mSpiChannel(std::move(channel))
		{
			programAuto1Mode(channels);
			startAuto1Mode();
		}

		AnalogDigitalConverter::~AnalogDigitalConverter() {

		}

		AnalogInput& AnalogDigitalConverter::getAnalogInput(uint32_t channel) {
			assert(channel < MaxChannels);

			return mInputs[channel];
		}

		void AnalogDigitalConverter::startAuto1Mode() {
			// See page 33 of the TI ads7956 datasheet for a better
			// understanding of whats going on here.

			Comm::SerialArray<2> startAuto1Mode({
				// The upper nibble tells it to go into auto 1 mode
				// the top bit of the lower nibble says that the rest of the frame should be programmed
				// the second to top bit resets the channel counter, but I believe its a no-op here.
				0x2C,
				// Bits 7, 8, 9 don't matter
				// Bit 6 puts it in 5V mode
				// The rest of the bits don't matter here
				0x70
			});
			// The datasheet shows, these 3 frames going out to get into
			// auto 1 mode. I don't know if the redundancy is neccessary yet.
			mSpiChannel->transfer(startAuto1Mode);
			// All subsiquent frames need to have bit 10 zero'd
			startAuto1Mode[0] = 0x28;
			mSpiChannel->transfer(startAuto1Mode);
			mSpiChannel->transfer(startAuto1Mode);
		}

		void AnalogDigitalConverter::programAuto1Mode(uint16_t channels) {
			Comm::SerialArray<2> enterAuto1ProgrammingMode({0x80, 0x00});
			mSpiChannel->transfer(enterAuto1ProgrammingMode);
			Comm::SerialArray<2> selectedChannels({
				static_cast<uint8_t>(channels >> 8),
				static_cast<uint8_t>(channels & 0x00FF)
			});
			mSpiChannel->transfer(selectedChannels);
		}

		void AnalogDigitalConverter::update() {
			Comm::SerialArray<2> readWrite({0x0, 0x0});
			mSpiChannel->transfer(readWrite);
			// Top nibble of the first byte contains the
			// channel that the sample belongs too.
			uint8_t sampleChannel = readWrite[0] >> 4;
			// The samples are all left justified and to use standard types,
			// we left justify them into a unsigned 16b sample.
			uint16_t sample =
				(static_cast<uint16_t>(readWrite[0]) << 12) |
				(static_cast<uint16_t>(readWrite[1]) << 4);

			if(sampleChannel < MaxChannels) {
				mInputs[sampleChannel].sample = sample;
			}
		}

		AnalogInput::AnalogInput() :
			sample(0)
		{

		}

		uint16_t AnalogInput::value() {
			return sample;
		}
	}
}
