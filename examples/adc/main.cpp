/*
	Author: Robert F. Rau II
	Copyright (C) 2017 Robert F. Rau II
*/
#include <atomic>
#include <chrono>
#include <iostream>
#include <thread>

#include <signal.h>

#include "adc/adc.h"

using namespace PiFly;
using PiFly::ADC::AnalogDigitalConverter;
using PiFly::ADC::AnalogInput;
using PiFly::Comm::SPI::SerialPeripheralInterface;

std::atomic<bool> interrupted;
void term_handle(int sig) {
	std::cout << "Signal received\n";
	interrupted.store(true);
}

int main(int argc, char** argv) {
	signal(SIGINT, &term_handle);

	SerialPeripheralInterface spi;

	AnalogDigitalConverter adc(spi.getChannel(
		Comm::SPI::ChipSelect_0,
		Comm::SPI::Mode_CPOL0_CPHA0,
		Comm::SPI::BitOrder_LsbFirst,
		Comm::SPI::ClockDivider_32,
		Comm::SPI::ChipSelectPolarity_ActiveLow
	), ADC::Channel_0 | ADC::Channel_1);

	AnalogInput& analogInput0 = adc.getAnalogInput(0);
	AnalogInput& analogInput1 = adc.getAnalogInput(1);

	std::cout << "0\t1\n";
	while(!interrupted.load()) {
		adc.update();
		std::cout << "\t\t0x" << std::hex << analogInput0.value() << ", 0x" << analogInput1.value() << std::dec << "\n";
		std::this_thread::sleep_for(std::chrono::milliseconds(200));
	}

	std::cout << "Hey there ;)" << std::endl;
	return 0;
}

