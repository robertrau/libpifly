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

namespace PiFly {
	namespace GPS {
		namespace Skytraq {
			using std::array;

			template<typename T, size_t size>
			T extract(SerialArray<size>& buffer, size_t& offset) {
				T result = 0;
				for(uint32_t i = 0; i < sizeof(T); i++) {
					result |= buffer[offset + i] << (sizeof(T) - i - 1)*8;
				}
				offset += sizeof(T);
				return result;
			}

			SkyTraqBinaryProtocol::SkyTraqBinaryProtocol(SerialPort& serialPort) :
				mSerialPort(serialPort)
			{
			}

			SkyTraqBinaryProtocol::~SkyTraqBinaryProtocol()
			{
			}

			bool SkyTraqBinaryProtocol::getResult(GpsResult& result) {
				SerialArray<updateBufferSize> buffer;
				size_t bytesRead = 0;
				uint8_t startByte;
				do {
					bytesRead = mSerialPort.read<1>(&startByte);
				} while((bytesRead != 0) && (startByte != SkyTraqVenus::MSG_START_1));

				if(bytesRead == 0) {
					return false;
				}

				buffer[0] = startByte;
				do {
					bytesRead = mSerialPort.read<1>(&startByte);
				} while((bytesRead == 0) && (startByte != SkyTraqVenus::MSG_START_2));

				if(startByte != SkyTraqVenus::MSG_START_2) {
					std::cout << "NOTICE: Second byte wrong\n";
					return false;
				}
				bytesRead = 2;
				buffer[1] = startByte;

				do {
					bytesRead += mSerialPort.read<updateBufferSize>(buffer.begin() + bytesRead, SkyTraqVenus::cmdByteIdx - bytesRead);
				} while(bytesRead < SkyTraqVenus::cmdByteIdx);

				uint16_t payloadSize = (buffer[2] << 8) | buffer[3];

				if((payloadSize + SkyTraqVenus::overheadSize) > updateBufferSize) {
					std::cout << "WARNING: incoming message tooooo large, skipping!\n";
					return false;
				}

				do {
					bytesRead += mSerialPort.read<updateBufferSize>(buffer.begin() + bytesRead, (payloadSize + SkyTraqVenus::overheadSize) - bytesRead);
				} while(bytesRead < (payloadSize + SkyTraqVenus::overheadSize));

				if((*(buffer.begin() + bytesRead - 1) == SkyTraqVenus::MSG_END_2) && (*(buffer.begin() + bytesRead - 2) == SkyTraqVenus::MSG_END_1)) {
					auto checksum = SkyTraqVenus::computeChecksum<updateBufferSize>(payloadSize, buffer);
					auto checksumIdx = payloadSize + SkyTraqVenus::cmdByteIdx;

					if(checksum != buffer[checksumIdx]) {
						std::cout << "WARNING: Bad message checksum\n";
						return false;
					}
				}

				SkyTraqVenus::Command cmd = static_cast<SkyTraqVenus::Command>(buffer[SkyTraqVenus::cmdByteIdx]);

				if(cmd == SkyTraqVenus::Command_NavDataMsg) {
					size_t offset = SkyTraqVenus::cmdByteIdx + 1;
					
					result.fixType = static_cast<FixType>(buffer[offset] + 1);
					offset++;
					result.satellitesInView = buffer[offset];
					offset++;
					result.GNSSWeek = extract<uint16_t, updateBufferSize>(buffer, offset);
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
				} else {
					std::cout << "WARNING: Not a navigation message\n";
					return false;
				}

				return true;
			}
		}
	}
}
