/*
	Author: Robert F. Rau II
	Copyright (C) 2017 Robert F. Rau II
*/
#include <array>
#include <iostream>

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
					extract<uint32_t, updateBufferSize>(buffer, offset);

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
					mSerialPort.setBaudrate(baudrates[currentBaud]);
					std::this_thread::sleep_for(std::chrono::milliseconds(30));
					updateBaudrate(baudrates[currentBaud]);
					foundBaud = true;
				}
				catch(GpsFormatException& ex)
				{
					if(currentBaud < MAX_BAUDS)
					{
						currentBaud++;
					}
					else if(currentBaud == MAX_BAUDS)
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

		void SkyTraqBinaryProtocol::updateBaudrate(SerialPort::Baudrate baud)
		{
			SerialArray<4> command;
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
				stringstream ss;
				ss << "Unsupported baudrate ";
				ss << SerialPort::baudrateString(baud);
				throw GpsException(ss.str());
			}

			command[0] = Command_ConfigSerial;
			command[1] = 0; // COM 1
			command[2] = gpsBaud;
			command[3] = 0; // Update to SRAM

			sendCommand<4>(command);

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
			catch(GpsFormatException& ex)
			{
				// try one more time to make sure the baudrate took. Sometimes the first receive fails even if it worked.
				mSerialPort.setBaudrate(baud);
				mSerialPort.flush();

				sendCommand<4>(command);
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
			SerialBuffer respBuffer(9);
			size_t bytesToRead = 9;
			size_t bytesRead = 0;
			do
			{
				bytesRead += mSerialPort.read(respBuffer.begin() + bytesRead, bytesToRead - bytesRead);
			} while(bytesRead < bytesToRead);

			if(respBuffer.size() != 9)
			{
				stringstream ss;
				ss << "Incorrect response length from GPS. Got ";
				ss << respBuffer.size();
				ss << " expected 9. Response = {";
				for(uint32_t i = 0; i < respBuffer.size(); i++)
				{
					ss << std::hex << static_cast<uint32_t>(respBuffer[i]) << ", ";
				}
				ss << "}";
				throw GpsFormatException(ss.str());
			}

			if((respBuffer[0] != MSG_START_1) && (respBuffer[1] != MSG_START_2))
			{
				stringstream ss;
				ss << "Incorrectly formated response from GPS. Response = {" << std::hex << static_cast<uint32_t>(respBuffer[0]);
				for(uint32_t i = 1; i < respBuffer.size(); i++)
				{
					ss << ", " << std::hex << static_cast<uint32_t>(respBuffer[i]);
				}
				ss << "}";
				throw GpsFormatException(ss.str());
			}
			
			uint16_t payloadLength = (respBuffer[2] << 8) & 0xFF00;
			payloadLength |= (respBuffer[3] & 0x00FF);

			if(payloadLength != 2)
			{
				throw GpsFormatException("Incorrect payload length for ACK or NACK message");
			}

			uint8_t checksum = computeChecksum(payloadLength, respBuffer);
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
