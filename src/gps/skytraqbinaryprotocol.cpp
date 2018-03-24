/*
	Author: Robert F. Rau II
	Copyright (C) 2017 Robert F. Rau II
*/
#include <array>
#include <chrono>
#include <iostream>

#include "comm/commexception.h"
#include "gps/gpsexception.h"
#include "gps/skytraqbinaryprotocol.h"

namespace PiFly
{
	namespace GPS
	{
		using std::array;

		template<typename T, size_t size>
		T extract(SerialPort::SerialArray<size>& buffer, size_t& offset)
		{
			T result = 0;
			for(uint32_t i = 0; i < sizeof(T); i++)
			{
				result |= buffer[offset + i] << (sizeof(T) - i - 1)*sizeof(uint8_t);
			}
			offset += sizeof(T);
			return result;
		}

		SkyTraqBinaryProtocol::SkyTraqBinaryProtocol(SerialPort& serialPort) :
			mSerialPort(serialPort),
			mHaveResult(false)
		{
			auto currentBaudrate = mSerialPort.getBaudrate();
			autoNegotiateBaudrate();
			updateBaudrate(currentBaudrate);
			setMessageType(MessageType_Binary);
			setPositionUpdateRate(50);
		}

		const bool SkyTraqBinaryProtocol::haveResult() const
		{
			return mHaveResult;
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
				if(startMsgByte != MSG_START_1)
				{
					continue;
				}

				buffer[0] = startMsgByte;
				bytesRead += mSerialPort.read<1>(&startMsgByte);
				if(startMsgByte != MSG_START_2)
				{
					continue;
				}

				haveMessage = true;

				buffer[1] = startMsgByte;
				do
				{
					bytesRead += mSerialPort.read<updateBufferSize>(buffer.begin() + bytesRead, cmdByteIdx - bytesRead);
				} while(bytesRead < cmdByteIdx);

				uint16_t payloadSize = (buffer[2] << 8) | buffer[3];

				if((payloadSize + overheadSize) > updateBufferSize)
				{
					std::cout << "WARNING: incoming message tooooo large, skipping!\n";
					continue;
				}

				do
				{
					bytesRead += mSerialPort.read<updateBufferSize>(buffer.begin() + bytesRead, (payloadSize + overheadSize) - bytesRead);
				} while(bytesRead < (payloadSize + overheadSize));

				if((*(buffer.begin() + bytesRead - 1) == MSG_END_2) && (*(buffer.begin() + bytesRead - 2) == MSG_END_1))
				{
					auto checksum = computeChecksum<updateBufferSize>(payloadSize, buffer);
					auto checksumIdx = payloadSize + cmdByteIdx;

					if(checksum == buffer[checksumIdx])
					{
						goodChecksum = true;
					}
				}
			} while(!haveMessage);

			if(goodChecksum)
			{
				Command cmd = static_cast<Command>(buffer[cmdByteIdx]);

				if(cmd == Command_NavDataMsg)
				{
					size_t offset = cmdByteIdx + 1;
					
					mGpsResult.fixType = static_cast<FixType>(buffer[offset] + 1);
					offset++;

					// skip num sv
					mGpsResult.satellitesInView = static_cast<FixType>(buffer[offset]);
					offset++;

					// skip GNSS week
					(void)extract<uint16_t, updateBufferSize>(buffer, offset);

					// skip TOW
					(void)extract<uint32_t, updateBufferSize>(buffer, offset);

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

		void SkyTraqBinaryProtocol::setPositionUpdateRate(uint8_t positionRate)
		{
			const size_t payloadLength = 3;
			SerialArray<payloadLength> command;

			command[0] = Command_ConfigPositionRate;
			command[1] = positionRate;
			command[2] = 0; // update to SRAM

			sendCommand<payloadLength>(command);
			if(!receiveAckNack())
			{
				throw GpsNackException("Received NACK on position rate command");
			}
		}

		GpsResult SkyTraqBinaryProtocol::getResult()
		{
			mHaveResult = false;
			return mGpsResult;
		}

		void SkyTraqBinaryProtocol::autoNegotiateBaudrate()
		{
			const size_t MAX_BAUDS = 7;
			SerialPort::Baudrate baudrates[MAX_BAUDS] = {
				SerialPort::Baudrate_4800,
				SerialPort::Baudrate_9600,
				SerialPort::Baudrate_19200,
				SerialPort::Baudrate_38400,
				SerialPort::Baudrate_57600,
				SerialPort::Baudrate_115200,
				SerialPort::Baudrate_230400};
			bool foundBaud = false;
			bool outOfBauds = false;
			size_t currentBaud = 0;

			std::cout << "Starting baudrate auto negotiation" << std::endl;
			while(!foundBaud && !outOfBauds)
			{
				try
				{
					std::cout << "Trying baudrate of " << SerialPort::baudrateString(baudrates[currentBaud]) << "\n";
					mSerialPort.setBaudrate(baudrates[currentBaud]);
					updateBaudrate(baudrates[currentBaud]);
					foundBaud = true;
				}
				catch(...)
				{
					if(currentBaud < MAX_BAUDS-1)
					{
						currentBaud++;
					}
					else if(currentBaud == MAX_BAUDS-1)
					{
						outOfBauds = true;
					}
				}
			}

			if(outOfBauds)
			{
				throw GpsException("Unable to auto negotiate baudrate");
			}

			if(foundBaud)
			{
				std::cout << "Auto negotiated a baudrate of " << SerialPort::baudrateString(baudrates[currentBaud]) << std::endl;
			}
		}

		void SkyTraqBinaryProtocol::updateBaudrate(const SerialPort::Baudrate baud)
		{
			const uint16_t payloadSize = 4;
			SerialArray<payloadSize> command;
			uint8_t gpsBaud;
			switch(baud)
			{
			case SerialPort::Baudrate_4800:
				gpsBaud = 0;
				break;
			case SerialPort::Baudrate_9600:
				gpsBaud = 1;
				break;
			case SerialPort::Baudrate_19200:
				gpsBaud = 2;
				break;
			case SerialPort::Baudrate_38400:
				gpsBaud = 3;
				break;
			case SerialPort::Baudrate_57600:
				gpsBaud = 4;
				break;
			case SerialPort::Baudrate_115200:
				gpsBaud = 5;
				break;
			case SerialPort::Baudrate_230400:
				gpsBaud = 6;
				break;
			default:
				std::cout << "updateBaudrate Input baud: " << baud << "\n";
				stringstream ss;
				ss << "Unsupported baudrate ";
				ss << SerialPort::baudrateString(baud);
				throw GpsException(ss.str());
			}

			command[0] = Command_ConfigSerial;
			command[1] = 0; // COM 1
			command[2] = gpsBaud;
			command[3] = 0; // Update to SRAM

			sendCommand<payloadSize>(command);

			try
			{
				if(!receiveAckNack())
				{
					throw GpsNackException("Received NACK on Config Serial Port command");
				}
				else
				{
					mSerialPort.setBaudrate(baud);
					mSerialPort.flush();
				}
			}
			catch(...)
			{
				// try one more time to make sure the baudrate took. Sometimes the first receive fails even if it worked.
				mSerialPort.setBaudrate(baud);

				sendCommand<payloadSize>(command);
				if(!receiveAckNack())
				{
					throw GpsNackException("Received NACK on Config Serial Port command");
				}
				else
				{
					mSerialPort.flush();
				}
			}
		}

		void SkyTraqBinaryProtocol::setMessageType(MessageType messageType)
		{
			SerialArray<3> command;
			command[0] = Command_MessageType;
			command[1] = messageType;
			command[2] = 0; // update to SRAM

			sendCommand<3>(command);
			if(!receiveAckNack())
			{
				throw GpsNackException("Received NACK on message type command");
			}
		}

		bool SkyTraqBinaryProtocol::receiveAckNack()
		{
			const size_t bytesToRead = 9;
			SerialArray<bytesToRead> respBuffer;
			size_t bytesRead = 0;
			bool haveMessageStart = false;

			using namespace std::chrono;
			auto startTime = steady_clock::now();
			duration<double> elapsed;
			do
			{
				elapsed = duration_cast<duration<double>>(steady_clock::now() - startTime);
				uint8_t startMsgByte;
				bytesRead = mSerialPort.read<1>(&startMsgByte);
				if(startMsgByte != MSG_START_1)
				{
					continue;
				}

				respBuffer[0] = startMsgByte;
				bytesRead += mSerialPort.read<1>(&startMsgByte);
				if(startMsgByte != MSG_START_2)
				{
					continue;
				}
				respBuffer[1] = startMsgByte;
				haveMessageStart = true;
			} while(!haveMessageStart && (elapsed.count() < ackNackReceiveTimeout));

			if(elapsed.count() >= ackNackReceiveTimeout)
			{
				throw Comm::CommTimeoutException("Timeout receiving ack/nack from GPS");
			}

			do
			{
				bytesRead += mSerialPort.read<bytesToRead>(respBuffer.begin() + bytesRead, cmdByteIdx - bytesRead);
			} while(bytesRead < cmdByteIdx);

			uint16_t payloadSize = (respBuffer[2] << 8) | respBuffer[3];

			if((payloadSize + overheadSize) > bytesToRead)
			{
				throw GpsFormatException("incoming message to large.");
			}

			do
			{
				bytesRead += mSerialPort.read<bytesToRead>(respBuffer.begin() + bytesRead, (payloadSize + overheadSize) - bytesRead);
			} while(bytesRead < (payloadSize + overheadSize));

			if((*(respBuffer.begin() + bytesRead - 1) == MSG_END_2) && (*(respBuffer.begin() + bytesRead - 2) == MSG_END_1))
			{
				uint8_t checksum = computeChecksum(payloadSize, respBuffer);
				if(checksum != respBuffer[6])
				{
					throw GpsFormatException("Incorrect checksum on response message");
				}

				Command commandByte = static_cast<Command>(respBuffer[4]);
				if(Command_ACK == commandByte)
				{
					return true;
				}
				else if(Command_NACK == commandByte)
				{
					return false;
				}
				else
				{
					throw GpsFormatException("Unexpected response message");
				}
			}
			else
			{
				throw GpsFormatException("Message missing terminating bytes.");
			}
		}

		uint8_t SkyTraqBinaryProtocol::computeChecksum(uint16_t payloadLength, SerialBuffer& commandBuffer)
		{
			uint8_t checksum = 0;

			if((commandBuffer.size() - overheadSize) != payloadLength)
			{
				throw GpsException("commandBuffer is not the correct size");
			}

			for(uint32_t i = 4; i < (payloadLength + 4); i++)
			{
				checksum = checksum ^ commandBuffer[i];
			}
			return checksum;
		}
	}
}
