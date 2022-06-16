/*
	Author: Robert F. Rau II
	Copyright (C) 2018 Robert F. Rau II
*/
#ifndef LIBPIFLY_COMM_SPI_SPI_H
#define LIBPIFLY_COMM_SPI_SPI_H

#include <bcm2835.h>

namespace PiFly {
	namespace Comm {
		namespace SPI {

			enum BitOrder {
				BitOrder_LsbFirst = BCM2835_SPI_BIT_ORDER_LSBFIRST,
				BitOrder_MsbFirst = BCM2835_SPI_BIT_ORDER_MSBFIRST
			};

			enum Mode {
				Mode_CPOL0_CPHA0 = BCM2835_SPI_MODE0,
				Mode_CPOL0_CPHA1,
				Mode_CPOL1_CPHA0,
				Mode_CPOL1_CPHA1
			};

			enum ChipSelect {
				ChipSelect_0 = BCM2835_SPI_CS0,
				ChipSelect_1,
				ChipSelect_2,
				ChipSelect_None
			};

			enum ClockDivider {
				ClockDivider_65536 = BCM2835_SPI_CLOCK_DIVIDER_65536,
				ClockDivider_32768 = BCM2835_SPI_CLOCK_DIVIDER_32768,
				ClockDivider_16384 = BCM2835_SPI_CLOCK_DIVIDER_16384,
				ClockDivider_8192 = BCM2835_SPI_CLOCK_DIVIDER_8192,
				ClockDivider_4096 = BCM2835_SPI_CLOCK_DIVIDER_4096,
				ClockDivider_2048 = BCM2835_SPI_CLOCK_DIVIDER_2048,
				ClockDivider_1024 = BCM2835_SPI_CLOCK_DIVIDER_1024,
				ClockDivider_512 = BCM2835_SPI_CLOCK_DIVIDER_512,
				ClockDivider_256 = BCM2835_SPI_CLOCK_DIVIDER_256,
				ClockDivider_128 = BCM2835_SPI_CLOCK_DIVIDER_128,
				ClockDivider_64 = BCM2835_SPI_CLOCK_DIVIDER_64,
				ClockDivider_32 = BCM2835_SPI_CLOCK_DIVIDER_32,
				ClockDivider_16 = BCM2835_SPI_CLOCK_DIVIDER_16,
				ClockDivider_8 = BCM2835_SPI_CLOCK_DIVIDER_8,
				ClockDivider_4 = BCM2835_SPI_CLOCK_DIVIDER_4,
				ClockDivider_2 = BCM2835_SPI_CLOCK_DIVIDER_2,
				ClockDivider_1 = BCM2835_SPI_CLOCK_DIVIDER_1
			};

			enum ChipSelectPolarity {
				ChipSelectPolarity_ActiveLow,
				ChipSelectPolarity_ActiveHigh
			};
		}
	}
}

#endif // LIBPIFLY_COMM_SPI_SPI_H
