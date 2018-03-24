/*
	Author: Robert F. Rau II
	Copyright (C) 2017 Robert F. Rau II
*/
#include "gps/gps.h"
#include "gps/gpsexception.h"

#include <iostream>

namespace PiFly
{
	namespace GPS
	{
		GlobalPositioningSystem::GlobalPositioningSystem(IGpsProtocol& protocol) :
			mProtocol(protocol)
		{
			mRunning.store(false);
			mResultBuffer.reserve(mResultBufferSize);
			mResultBuffer.resize(0);
		}

		GlobalPositioningSystem::~GlobalPositioningSystem()
		{
			stop();
		}

		void GlobalPositioningSystem::stop()
		{
			if(mRunning.load())
			{
				mRunning.store(false);
				mRunThread->join();
			}
		}
		
		void GlobalPositioningSystem::start()
		{
			mRunning.store(true);
			mRunThread.reset(new thread([this]() {
				while(mRunning.load())
				{
					while(!mProtocol.haveResult() && mRunning.load())
					{
						mProtocol.update();
					}

					GpsResult result = mProtocol.getResult();

					{
						std::unique_lock<mutex> lck(mResultMutex);
						mResultBuffer.push_back(result);
						mNotify.notify_one();
					}
				}
			}));
		}

		size_t GlobalPositioningSystem::getLatestSamples(ResultVector& resultVector)
		{
			std::unique_lock<mutex> lock(mResultMutex);
			mNotify.wait(lock);
			resultVector.resize(mResultBuffer.size());
			std::copy(mResultBuffer.begin(), mResultBuffer.end(), resultVector.begin());
			size_t bufSize = mResultBuffer.size();
			mResultBuffer.resize(0);
			return bufSize;
		}
	}
}
