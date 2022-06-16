/*
	Author: Robert F. Rau II
	Copyright (C) 2018 Robert F. Rau II
*/
#ifndef LIBPIFLY_GPS_SKYTRAQVENUS_H
#define LIBPIFLY_GPS_SKYTRAQVENUS_H

#include <chrono>

#include "comm/serialport.h"
#include "gps/igpsprotocol.h"
#include "gps/gpsexception.h"

namespace PiFly {
	namespace GPS {
		namespace Skytraq {
			using Comm::SerialPort;
			using Comm::SerialArray;
			using Comm::SerialBuffer;
			
			class SkyTraqVenus {
			public:	
				SkyTraqVenus(SerialPort& serialPort, double timeout = 0.5);

				const static size_t overheadSize = 7;
				const static size_t cmdByteIdx = 4;
				const static uint8_t MSG_START_1 = 0xA0;
				const static uint8_t MSG_START_2 = 0xA1;
				const static uint8_t MSG_END_1 = 0x0D;
				const static uint8_t MSG_END_2 = 0x0A;
				enum Command {
					Command_ConfigSerial = 0x5,
					Command_ConfigNMEA = 0x8,
					Command_MessageType = 0x9,
					Command_ConfigPositionRate = 0xE,
					Command_ConfigNavInterval = 0x11,
					Command_ACK = 0x83,
					Command_NACK = 0x84,
					Command_NavDataMsg = 0xA8
				};

				static uint8_t computeChecksum(uint16_t payloadLength, SerialBuffer& commandBuffer);

				template<size_t size>
				static uint8_t computeChecksum(uint16_t payloadLength, SerialArray<size>& buffer) {
					uint8_t checksum = 0;

					if(buffer.size() < (payloadLength + overheadSize)) {
						throw GpsException("command buffer is too small");
					}

					for(uint32_t i = 4; i < (payloadLength + 4); i++) {
						checksum = checksum ^ buffer[i];
					}
					return checksum;
				}

				void updateBaudrate(const SerialPort::Baudrate baud);
				void setPositionUpdateRate(uint8_t positionRate);

				enum MessageType {
					MessageType_None,
					MessageType_NMEA,
					MessageType_Binary
				};
				void setMessageType(MessageType messageType);

				template<size_t size>
				void sendCommand(const SerialArray<size>& command) {
					const size_t payloadLength = size;
					const size_t messageSize = size + 7;
					SerialArray<messageSize> buffer{};
					buffer[0] = MSG_START_1;
					buffer[1] = MSG_START_2;
					buffer[2] = payloadLength >> 8;
					buffer[3] = payloadLength;
					std::copy(command.begin(), command.end(), buffer.begin()+4);
					buffer[size + 4] = computeChecksum<messageSize>(payloadLength, buffer);
					buffer[size + 5] = MSG_END_1;
					buffer[size + 6] = MSG_END_2;

					mSerialPort.write<messageSize>(buffer);
				}

				void autoNegotiateBaudrate();

			private:
				SerialPort& mSerialPort;
				std::chrono::duration<double> ackNackReceiveTimeout;

				bool receiveAckNack();
			};
		}
	}
}

#endif // LIBPIFLY_GPS_SKYTRAQVENUS_H