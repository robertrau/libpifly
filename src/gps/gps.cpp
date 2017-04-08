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

					auto result = mProtocol.getResult();

					mResultMutex.lock();
					mResultBuffer.push_back(result);
					mResultMutex.unlock();
				}
			}));
		}

		void GlobalPositioningSystem::getLatestSamples(ResultVector& resultVector)
		{
			std::lock_guard<mutex> lock(mutex);
			resultVector.resize(mResultBuffer.size());
			std::copy(mResultBuffer.begin(), mResultBuffer.end(), resultVector.begin());
			mResultBuffer.resize(0);
		}
	}
}
