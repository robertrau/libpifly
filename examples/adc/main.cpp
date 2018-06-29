/*
	Author: Robert F. Rau II
	Copyright (C) 2017 Robert F. Rau II
*/
#include <atomic>
#include <iostream>

#include <signal.h>

#include "adc/adc.h"

//using PiFly::ADC::AnalogChannel;
using namespace PiFly;
using PiFly::ADC::AnalogDigitalConverter;
using PiFly::Comm::SPI::SerialPeripheralInterface;

std::atomic<bool> interrupted;
void term_handle(int sig) {
	printf("Signal received\n");
	interrupted.store(true);
}

int main(int argc, char** argv) {
	signal(SIGINT, &term_handle);

	SerialPeripheralInterface spi;

	AnalogDigitalConverter adc(spi.getChannel(
		Comm::SPI::ChipSelect_1,
		Comm::SPI::Mode_CPOL0_CPHA1,
		Comm::SPI::BitOrder_LsbFirst,
		Comm::SPI::ClockDivider_16,
		Comm::SPI::ChipSelectPolarity_ActiveLow
	), ADC::ChannelSelect_1 | ADC::ChannelSelect_4);

	std::cout << "Hey there ;)" << std::endl;
	return 0;
}

