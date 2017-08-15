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
		void Init(void)
		{
			auto ret = bcm2835_init();
			if(ret != 1)
			{
				std::cout << "Failed to initialize bcm2835 library" << std::endl;
				return;
			}
			std::cout << "Initialized bcm2835 library" << std::endl;


			ret = bcm2835_spi_begin();
			if(ret != 1)
			{
				std::cout << "Failed to begin spi operation" << std::endl;
				return;
			}

			std::cout << "Setting spi clock to about 15Mhz" << std::endl;
			bcm2835_spi_setClockDivider(BCM2835_SPI_CLOCK_DIVIDER_16);

			bcm2835_spi_setDataMode(BCM2835_SPI_MODE1);
			
			bcm2835_spi_chipSelect(BCM2835_SPI_CS0);

			bcm2835_spi_setChipSelectPolarity(BCM2835_SPI_CS1, 0);

			uint8_t txBuff[2] = {0x0, 0x0};
			uint8_t rxBuff[2];

			for(;;)
			{
				bcm2835_spi_transfernb((char*)txBuff, (char*)rxBuff, 2);
				uint16_t sample = rxBuff[0] << 8 | rxBuff[1];
				std::cout << "byte[0] = " << static_cast<uint16_t>(rxBuff[0]) << "\t byte[1] = " << static_cast<uint16_t>(rxBuff[1]) << std::endl;
				std::this_thread::sleep_for (std::chrono::milliseconds(100));
			}

			bcm2835_spi_end();

			ret = bcm2835_close();
			if(ret != 1)
			{
				std::cout << "Failed to close bcm2835 library" << std::endl;
				return;
			}
			std::cout << "Closed bcm2835 library" << std::endl;
		}
	}
}
