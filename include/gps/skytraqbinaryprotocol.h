/*
	Author: Robert F. Rau II
	Copyright (C) 2017 Robert F. Rau II
*/
#ifndef LIBPIFLY_GPS_SKYTRAQBINARYPROTOCOL_H
#define LIBPIFLY_GPS_SKYTRAQBINARYPROTOCOL_H

#include "comm/serialport.h"
#include "gps/gps.h"
#include "gps/igpsprotocol.h"

namespace PiFly
{
	namespace GPS
	{
		using Comm::SerialPort;

		class SkyTraqBinaryProtocol : public IGpsProtocol
		{
			typedef SerialPort::SerialBuffer SerialBuffer;
			template<size_t size>
			using SerialArray = SerialPort::SerialArray<size>;

		public:

			SkyTraqBinaryProtocol(SerialPort& serialPort);

			virtual const bool haveResult() const;
			virtual void update();
			virtual GpsResult getResult();

		private:

			constexpr static double ackNackReceiveTimeout = 0.5;
			const static size_t overheadSize = 7;
			const static size_t updateBufferSize = 128;
			const static size_t cmdByteIdx = 4;
			const uint8_t MSG_START_1 = 0xA0;
			const uint8_t MSG_START_2 = 0xA1;
			const uint8_t MSG_END_1 = 0x0D;
			const uint8_t MSG_END_2 = 0x0A;
			typedef enum
			{
				Command_ConfigSerial = 0x5,
				Command_ConfigNMEA = 0x8,
				Command_MessageType = 0x9,
				Command_ConfigPositionRate = 0xE,
				Command_ConfigNavInterval = 0x11,
				Command_ACK = 0x83,
				Command_NACK = 0x84,
				Command_NavDataMsg = 0xA8
			} Command;

			uint8_t computeChecksum(uint16_t payloadLength, SerialBuffer& commandBuffer);

			template<size_t size>
			static uint8_t computeChecksum(uint16_t payloadLength, SerialArray<size>& buffer)
			{
				uint8_t checksum = 0;

				if(buffer.size() < (payloadLength + overheadSize))
				{
					throw GpsException("command buffer is too small");
				}

				for(uint32_t i = 4; i < (payloadLength + 4); i++)
				{
					checksum = checksum ^ buffer[i];
				}
				return checksum;
			}

			void updateBaudrate(const SerialPort::Baudrate baud);
			void setPositionUpdateRate(uint8_t positionRate);

			typedef enum
			{
				MessageType_None,
				MessageType_NMEA,
				MessageType_Binary
			} MessageType;
			void setMessageType(MessageType messageType);

			template<size_t size>
			void sendCommand(const SerialArray<size>& command)
			{
				const size_t payloadLength = size;
				const size_t messageSize = size + 7;
				SerialArray<messageSize> buffer;
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

			bool receiveAckNack();

			void autoNegotiateBaudrate();

			SerialPort& mSerialPort;
			bool mHaveResult;
			GpsResult mGpsResult;
		};
	}
}

#endif // LIBPIFLY_GPS_SKYTRAQBINARYPROTOCOL_H
