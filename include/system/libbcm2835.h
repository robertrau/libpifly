/*
	Author: Robert F. Rau II
	Copyright (C) 2018 Robert F. Rau II
*/
#ifndef LIBPIFLY_SYSTEM_BCM2835_H
#define LIBPIFLY_SYSTEM_BCM2835_H

#include <array>
#include <exception>

#include <bcm2835.h>

namespace PiFly
{
	namespace System
	{
		class Libbcm2835
		{
		public:
			static bool Init();
			static bool Close();

		private:
			static bool initialized;
		};
	}
}

#endif // LIBPIFLY_SYSTEM_BCM2835_H
