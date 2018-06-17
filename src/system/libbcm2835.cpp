/*
	Author: Robert F. Rau II
	Copyright (C) 2018 Robert F. Rau II
*/
#include <iostream>
#include <thread>         // std::this_thread::sleep_for
#include <chrono>         // std::chrono::seconds

#include <bcm2835.h>

#include "system/libbcm2835.h"

namespace PiFly
{
	namespace System
	{
		bool Libbcm2835::initialized = false;

		bool Libbcm2835::Init()
		{
			if(!initialized)
			{
				auto ret = bcm2835_init();
				if(ret == 1)
				{
					initialized = true;
				}
			}
			return initialized;
		}

		bool Libbcm2835::Close()
		{
			if(initialized)
			{
				auto ret = bcm2835_close();
				if(ret == 1)
				{
					initialized = false;
				}
			}

			return !initialized;
		}
	}
}
