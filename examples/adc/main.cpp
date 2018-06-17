/*
	Author: Robert F. Rau II
	Copyright (C) 2017 Robert F. Rau II
*/
#include <atomic>
#include <iostream>

#include <signal.h>

#include "adc/adc.h"

//using PiFly::ADC::AnalogChannel;
using PiFly::ADC::AnalogDigitalConverter;
using PiFly::Comm::SPI::SerialPeripheralInterface;

std::atomic<bool> interrupted;
void term_handle(int sig)
{
	printf("Signal received\n");
	interrupted.store(true);
}

int main(int argc, char** argv)
{
	signal(SIGINT, &term_handle);

	SerialPeripheralInterface spi;
	auto channel = spi.getChannel(
		PiFly::Comm::SPI::ChipSelect_1,
		PiFly::Comm::SPI::Mode_CPOL0_CPHA1,
		PiFly::Comm::SPI::BitOrder_LsbFirst,
		PiFly::Comm::SPI::ClockDivider_16,
		PiFly::Comm::SPI::ChipSelectPolarity_ActiveLow
	);
	AnalogDigitalConverter adc1(std::move(channel));

	AnalogDigitalConverter adc2(spi.getChannel(
		PiFly::Comm::SPI::ChipSelect_1,
		PiFly::Comm::SPI::Mode_CPOL0_CPHA1,
		PiFly::Comm::SPI::BitOrder_LsbFirst,
		PiFly::Comm::SPI::ClockDivider_16,
		PiFly::Comm::SPI::ChipSelectPolarity_ActiveLow
	));

	std::cout << "Hey there ;)" << std::endl;
	return 0;
}

