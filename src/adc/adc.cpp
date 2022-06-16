/*
	Author: Robert F. Rau II
	Copyright (C) 2017 Robert F. Rau II
*/
#include <iostream> // NOLINT
#include <thread> // NOLINT         // std::this_thread::sleep_for
#include <chrono> // NOLINT         // std::chrono::seconds
#include <cassert> // NOLINT

#include "adc/adc.h"


namespace PiFly {
	namespace ADC {

		AnalogDigitalConverter::AnalogDigitalConverter(Comm::SPI::ChannelPtr&& channel, std::bitset<16> channels) :
			mChannels(channels),
			mSpiChannel(std::move(channel)),
			mInputs{}
		{
			programAuto1Mode();
			startAuto1Mode();
		}

		AnalogInput& AnalogDigitalConverter::getAnalogInput(uint32_t channel) {
			// Silence clang-tidy warning about code we have no control over
			assert(channel < MaxChannels); // NOLINT(cppcoreguidelines-pro-bounds-array-to-pointer-decay, cppcoreguidelines-pro-bounds-constant-array-index)

			// Linting disabled because of linkage problems with .at()
			return mInputs[channel]; // NOLINT(cppcoreguidelines-pro-bounds-constant-array-index)
		}

		void AnalogDigitalConverter::startAuto1Mode() {
			// Silence clang-tidy warning about code we have no control over
			assert(mSpiChannel); // NOLINT(cppcoreguidelines-pro-bounds-array-to-pointer-decay, cppcoreguidelines-pro-bounds-constant-array-index)
			// See page 33 of the TI ads7956 datasheet for a better
			// understanding of whats going on here.

			Comm::SerialArray<2> startAuto1Mode({
				// The upper nibble tells it to go into auto 1 mode
				// the top bit of the lower nibble says that the rest of the frame should be programmed
				// the second to top bit resets the channel counter, but I believe its a no-op here.
				0x2C,
				// Bits 7, 8, 9 don't matter
				// Bit 6 puts it in 5V mode, bit 5 will power 
				// down device, 4 puts GPIO data in responses
				// The rest of the bits don't matter here
				0x40
			});
			Comm::SerialArray<2> readData{};
			// The datasheet shows, these 3 frames going out to get into
			// auto 1 mode. I don't know if the redundancy is neccessary yet.
			mSpiChannel->transfer(startAuto1Mode, readData);
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
			// All subsiquent frames need to have bit 10 zero'd
			startAuto1Mode[0] = 0x28;
			mSpiChannel->transfer(startAuto1Mode, readData);
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
			mSpiChannel->transfer(startAuto1Mode, readData);
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}

		void AnalogDigitalConverter::programAuto1Mode() {
			// Silence clang-tidy warning about code we have no control over
			assert(mSpiChannel); // NOLINT(cppcoreguidelines-pro-bounds-array-to-pointer-decay, cppcoreguidelines-pro-bounds-constant-array-index)

			Comm::SerialArray<2> readData{};
			Comm::SerialArray<2> enterAuto1ProgrammingMode({0x00, 0x00});
			mSpiChannel->transfer(enterAuto1ProgrammingMode, readData);
			std::this_thread::sleep_for(std::chrono::milliseconds(1));

			enterAuto1ProgrammingMode[0] = 0x80;
			mSpiChannel->transfer(enterAuto1ProgrammingMode, readData);
			std::this_thread::sleep_for(std::chrono::milliseconds(1));

			Comm::SerialArray<2> selectedChannels({
				static_cast<uint8_t>(mChannels.to_ulong() >> 8),
				static_cast<uint8_t>(mChannels.to_ulong() & 0x00FF)
			});
			mSpiChannel->transfer(selectedChannels, readData);
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}

		void AnalogDigitalConverter::update() {
			Comm::SerialArray<2> write({0x0, 0x0});
			Comm::SerialArray<2> read({0x0, 0x0});
			mSpiChannel->transfer(write, read);

			for(uint32_t i = 0; i < mChannels.count(); i++) {
				//Comm::SerialArray<2> write({0x0, 0x0});
				//Comm::SerialArray<2> read({0x0, 0x0});
				mSpiChannel->transfer(write, read);
				// Top nibble of the first byte contains the
				// channel that the sample belongs too.
				uint8_t sampleChannel = read[0] >> 4;
				// The samples are all left justified and to use standard types,
				// we left justify them into a unsigned 16b sample.
				uint16_t sample =
					(static_cast<uint16_t>(read[0]) << 12) |
					(static_cast<uint16_t>(read[1]) << 4) |
					0x1F; // Pad the bottom 6bits so we are arithmetically correct.

				if(sampleChannel < MaxChannels) {
					// Linting disabled because of linkage problems with .at()
					mInputs[sampleChannel].sample = sample; // NOLINT(cppcoreguidelines-pro-bounds-constant-array-index)
				}
			}
		}

		uint16_t AnalogInput::value() {
			return sample;
		}
	}
}
