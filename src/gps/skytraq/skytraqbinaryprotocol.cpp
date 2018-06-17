/*
	Author: Robert F. Rau II
	Copyright (C) 2017 Robert F. Rau II
*/
#include <array>
#include <chrono>
#include <iostream>

#include "comm/commexception.h"
#include "gps/gpsexception.h"
#include "gps/skytraq/skytraqbinaryprotocol.h"

namespace PiFly
{
	namespace GPS
	{
		namespace Skytraq
		{
			using std::array;

			template<typename T, size_t size>
			T extract(SerialArray<size>& buffer, size_t& offset)
			{
				T result = 0;
				for(uint32_t i = 0; i < sizeof(T); i++)
				{
					result |= buffer[offset + i] << (sizeof(T) - i - 1)*sizeof(uint8_t);
				}
				offset += sizeof(T);
				return result;
			}

			SkyTraqBinaryProtocol::SkyTraqBinaryProtocol(SerialPort& serialPort, SkyTraqVenus& skytraqVenus) :
				mSerialPort(serialPort),
				mSkyTraqVenus(skytraqVenus),
				mHaveResult(false)
			{
				auto currentBaudrate = mSerialPort.getBaudrate();
				mSkyTraqVenus.autoNegotiateBaudrate();
				mSerialPort.flush();
				mSkyTraqVenus.updateBaudrate(currentBaudrate);
				mSerialPort.flush();
				std::cout << "Setting binary message type" << std::endl;
				mSkyTraqVenus.setMessageType(SkyTraqVenus::MessageType_Binary);
				mSerialPort.flush();
				std::cout << "Setting update rate to 50Hz" << std::endl;
				mSkyTraqVenus.setPositionUpdateRate(50);
				mSerialPort.flush();
			}

			const bool SkyTraqBinaryProtocol::haveResult() const
			{
				return mHaveResult;
			}

			bool SkyTraqBinaryProtocol::getResult(GpsResult& result)
			{
				SerialArray<updateBufferSize> buffer;
				size_t bytesRead = 0;
				uint8_t startByte;
				do
				{
					bytesRead = mSerialPort.read<1>(&startByte);
				} while((bytesRead != 0) && (startByte != SkyTraqVenus::MSG_START_1));

				if(bytesRead == 0)
				{
					return false;
				}

				buffer[0] = startByte;
				do
				{
					bytesRead = mSerialPort.read<1>(&startByte);
				} while((bytesRead == 0) && (startByte != SkyTraqVenus::MSG_START_2));

				if(startByte != SkyTraqVenus::MSG_START_2)
				{
					std::cout << "NOTICE: Second byte wrong\n";
					return false;
				}
				bytesRead = 2;
				buffer[1] = startByte;

				do
				{
					bytesRead += mSerialPort.read<updateBufferSize>(buffer.begin() + bytesRead, SkyTraqVenus::cmdByteIdx - bytesRead);
				} while(bytesRead < SkyTraqVenus::cmdByteIdx);

				uint16_t payloadSize = (buffer[2] << 8) | buffer[3];

				if((payloadSize + SkyTraqVenus::overheadSize) > updateBufferSize)
				{
					std::cout << "WARNING: incoming message tooooo large, skipping!\n";
					return false;
				}

				do
				{
					bytesRead += mSerialPort.read<updateBufferSize>(buffer.begin() + bytesRead, (payloadSize + SkyTraqVenus::overheadSize) - bytesRead);
				} while(bytesRead < (payloadSize + SkyTraqVenus::overheadSize));

				if((*(buffer.begin() + bytesRead - 1) == SkyTraqVenus::MSG_END_2) && (*(buffer.begin() + bytesRead - 2) == SkyTraqVenus::MSG_END_1))
				{
					auto checksum = SkyTraqVenus::computeChecksum<updateBufferSize>(payloadSize, buffer);
					auto checksumIdx = payloadSize + SkyTraqVenus::cmdByteIdx;

					if(checksum != buffer[checksumIdx])
					{
						std::cout << "WARNING: Bad message checksum\n";
						return false;
					}
				}

				SkyTraqVenus::Command cmd = static_cast<SkyTraqVenus::Command>(buffer[SkyTraqVenus::cmdByteIdx]);

				if(cmd == SkyTraqVenus::Command_NavDataMsg)
				{
					size_t offset = SkyTraqVenus::cmdByteIdx + 1;
					
					result.fixType = static_cast<FixType>(buffer[offset] + 1);
					offset++;

					// skip num sv
					result.satellitesInView = buffer[offset];
					offset++;

					// skip GNSS week
					result.GNSSWeek = extract<uint16_t, updateBufferSize>(buffer, offset);

					// skip TOW
					result.tow = extract<uint32_t, updateBufferSize>(buffer, offset);

					result.latitude = extract<int32_t, updateBufferSize>(buffer, offset);
					result.longitude = extract<int32_t, updateBufferSize>(buffer, offset);

					// skip ellipsiod altitude
					(void)extract<uint32_t, updateBufferSize>(buffer, offset);

					result.meanSeaLevel = extract<uint32_t, updateBufferSize>(buffer, offset);

					result.gdop = extract<uint16_t, updateBufferSize>(buffer, offset);
					result.pdop = extract<uint16_t, updateBufferSize>(buffer, offset);
					result.hdop = extract<uint16_t, updateBufferSize>(buffer, offset);
					result.vdop = extract<uint16_t, updateBufferSize>(buffer, offset);
					result.tdop = extract<uint16_t, updateBufferSize>(buffer, offset);
				}
				else
				{
					std::cout << "WARNING: Not a navigation message\n";
					return false;
				}

				return true;
			}

			void SkyTraqBinaryProtocol::update()
			{
				SerialArray<updateBufferSize> buffer;
				bool goodChecksum = false;
				bool haveMessage = false;
				do
				{
					uint8_t startMsgByte;
					auto bytesRead = mSerialPort.read<1>(&startMsgByte);
					if(startMsgByte != SkyTraqVenus::MSG_START_1)
					{
						continue;
					}

					buffer[0] = startMsgByte;
					bytesRead += mSerialPort.read<1>(&startMsgByte);
					if(startMsgByte != SkyTraqVenus::MSG_START_2)
					{
						continue;
					}

					haveMessage = true;

					buffer[1] = startMsgByte;
					do
					{
						bytesRead += mSerialPort.read<updateBufferSize>(buffer.begin() + bytesRead, SkyTraqVenus::cmdByteIdx - bytesRead);
					} while(bytesRead < SkyTraqVenus::cmdByteIdx);

					uint16_t payloadSize = (buffer[2] << 8) | buffer[3];

					if((payloadSize + SkyTraqVenus::overheadSize) > updateBufferSize)
					{
						std::cout << "WARNING: incoming message tooooo large, skipping!\n";
						continue;
					}

					do
					{
						bytesRead += mSerialPort.read<updateBufferSize>(buffer.begin() + bytesRead, (payloadSize + SkyTraqVenus::overheadSize) - bytesRead);
					} while(bytesRead < (payloadSize + SkyTraqVenus::overheadSize));

					if((*(buffer.begin() + bytesRead - 1) == SkyTraqVenus::MSG_END_2) && (*(buffer.begin() + bytesRead - 2) == SkyTraqVenus::MSG_END_1))
					{
						auto checksum = SkyTraqVenus::computeChecksum<updateBufferSize>(payloadSize, buffer);
						auto checksumIdx = payloadSize + SkyTraqVenus::cmdByteIdx;

						if(checksum == buffer[checksumIdx])
						{
							goodChecksum = true;
						}
					}
				} while(!haveMessage);

				if(goodChecksum)
				{
					SkyTraqVenus::Command cmd = static_cast<SkyTraqVenus::Command>(buffer[SkyTraqVenus::cmdByteIdx]);

					if(cmd == SkyTraqVenus::Command_NavDataMsg)
					{
						size_t offset = SkyTraqVenus::cmdByteIdx + 1;
						
						mGpsResult.fixType = static_cast<FixType>(buffer[offset] + 1);
						offset++;

						// skip num sv
						mGpsResult.satellitesInView = buffer[offset];
						offset++;

						// skip GNSS week
						mGpsResult.GNSSWeek = extract<uint16_t, updateBufferSize>(buffer, offset);

						// skip TOW
						mGpsResult.tow = extract<uint32_t, updateBufferSize>(buffer, offset);

						mGpsResult.latitude = extract<int32_t, updateBufferSize>(buffer, offset);
						mGpsResult.longitude = extract<int32_t, updateBufferSize>(buffer, offset);

						// skip ellipsiod altitude
						(void)extract<uint32_t, updateBufferSize>(buffer, offset);

						mGpsResult.meanSeaLevel = extract<uint32_t, updateBufferSize>(buffer, offset);

						mGpsResult.gdop = extract<uint16_t, updateBufferSize>(buffer, offset);
						mGpsResult.pdop = extract<uint16_t, updateBufferSize>(buffer, offset);
						mGpsResult.hdop = extract<uint16_t, updateBufferSize>(buffer, offset);
						mGpsResult.vdop = extract<uint16_t, updateBufferSize>(buffer, offset);
						mGpsResult.tdop = extract<uint16_t, updateBufferSize>(buffer, offset);

						mHaveResult = true;
					}
				}
			}

			GpsResult SkyTraqBinaryProtocol::getResult()
			{
				mHaveResult = false;
				return mGpsResult;
			}
		}
	}
}
