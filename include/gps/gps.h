/*
	Author: Robert F. Rau II
	Copyright (C) 2017 Robert F. Rau II
*/
#ifndef LIBPIFLY_GPS_GPS_H
#define LIBPIFLY_GPS_GPS_H

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <thread>
#include <vector>

#include "gps/igpsprotocol.h"
#include "comm/serialport.h"

namespace PiFly
{
	namespace GPS
	{
		using std::atomic;
		using std::mutex;
		using std::thread;
		using std::unique_ptr;
		using std::vector;

		typedef vector<GpsResult> ResultVector;

		class GlobalPositioningSystem
		{
		public:
			GlobalPositioningSystem(IGpsProtocol& protocol);
			virtual ~GlobalPositioningSystem();

			void start();
			void stop();
			void getLatestSamples(ResultVector& resultVector);

		private:

			atomic<bool> mRunning;
			unique_ptr<thread> mRunThread;
			IGpsProtocol& mProtocol;
			mutex mResultMutex;
			std::condition_variable mNotify;
			const size_t mResultBufferSize = 512;
			ResultVector mResultBuffer;
			bool mResultReady;
		};
	}
}

#endif // LIBPIFLY_GPS_GPS_H
