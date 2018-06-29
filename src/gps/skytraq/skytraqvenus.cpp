/*
	Author: Robert F. Rau II
	Copyright (C) 2017 Robert F. Rau II
*/
#include <array>
#include <iostream>
#include <sstream>

#include "comm/commexception.h"
#include "gps/gpsexception.h"
#include "gps/skytraq/skytraqvenus.h"

namespace PiFly {
	namespace GPS {
		namespace Skytraq {
			using std::array;

			SkyTraqVenus::SkyTraqVenus(SerialPort& serialPort, double timeout) :
				mSerialPort(serialPort),
				ackNackReceiveTimeout(std::chrono::duration<double>(timeout))
			{
			}

			void SkyTraqVenus::setPositionUpdateRate(uint8_t positionRate) {
				const size_t payloadLength = 3;
				SerialArray<payloadLength> command;

				command[0] = Command_ConfigPositionRate;
				command[1] = positionRate;
				command[2] = 0; // update to SRAM

				sendCommand<payloadLength>(command);
				if(!receiveAckNack()) {
					throw GpsNackException("Received NACK on position rate command");
				}
			}

			void SkyTraqVenus::autoNegotiateBaudrate() {
				const size_t MAX_BAUDS = 7;
				SerialPort::Baudrate baudrates[MAX_BAUDS] = {
					SerialPort::Baudrate_4800,
					SerialPort::Baudrate_9600,
					SerialPort::Baudrate_19200,
					SerialPort::Baudrate_38400,
					SerialPort::Baudrate_57600,
					SerialPort::Baudrate_115200,
					SerialPort::Baudrate_230400
				};

				bool foundBaud = false;
				bool outOfBauds = false;
				size_t currentBaud = 0;

				std::cout << "Starting baudrate auto negotiation" << std::endl;
				while(!foundBaud && !outOfBauds) {
					try {
						std::cout << "Trying baudrate of " << SerialPort::baudrateString(baudrates[currentBaud]) << "\n";
						mSerialPort.setBaudrate(baudrates[currentBaud]);
						updateBaudrate(baudrates[currentBaud]);
						foundBaud = true;
					} catch(...) {
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

				if(outOfBauds) {
					throw GpsException("Unable to auto negotiate baudrate");
				}

				if(foundBaud) {
					std::cout << "Auto negotiated a baudrate of " << SerialPort::baudrateString(baudrates[currentBaud]) << std::endl;
				}
			}

			void SkyTraqVenus::updateBaudrate(const SerialPort::Baudrate baud) {
				const uint16_t payloadSize = 4;
				SerialArray<payloadSize> command;
				uint8_t gpsBaud;
				switch(baud) {
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

				try {
					if(!receiveAckNack()) {
						throw GpsNackException("Received NACK on Config Serial Port command");
					} else {
						mSerialPort.setBaudrate(baud);
						mSerialPort.flush();
					}
				} catch(...) {
					// try one more time to make sure the baudrate took. Sometimes the first receive fails even if it worked.
					mSerialPort.setBaudrate(baud);

					sendCommand<payloadSize>(command);
					if(!receiveAckNack()) {
						throw GpsNackException("Received NACK on Config Serial Port command");
					} else {
						mSerialPort.flush();
					}
				}
			}

			void SkyTraqVenus::setMessageType(MessageType messageType) {
				SerialArray<3> command;
				command[0] = Command_MessageType;
				command[1] = messageType;
				command[2] = 0; // update to SRAM

				sendCommand<3>(command);
				if(!receiveAckNack()) {
					throw GpsNackException("Received NACK on message type command");
				}
			}

			bool SkyTraqVenus::receiveAckNack() {
				const size_t bytesToRead = 9;
				SerialArray<bytesToRead> respBuffer;

				uint32_t attempts = 0;
				bool haveAckNackMessage = false;
				uint16_t payloadSize = 0;
				size_t bytesRead = 0;
				
				using namespace std::chrono;
				auto startTime = steady_clock::now();
				duration<double> elapsed;
				
				while(!haveAckNackMessage && (attempts < 5)) {
					uint8_t startMsgByte;
					do {
						bytesRead = mSerialPort.read<1>(&startMsgByte);
						if(duration_cast<duration<double>>(steady_clock::now() - startTime) > ackNackReceiveTimeout) {
							throw Comm::CommTimeoutException("Timeout receiving ack/nack from GPS");
						}
					} while(startMsgByte != MSG_START_1);
					respBuffer[0] = startMsgByte;
					
					bytesRead += mSerialPort.read<1>(&startMsgByte);
					if(startMsgByte != MSG_START_2) {
						continue;
					} else {
						std::cout << "Got frame start\n"; 
					}
					respBuffer[1] = startMsgByte;

					do {
						bytesRead += mSerialPort.read<bytesToRead>(respBuffer.begin() + bytesRead, cmdByteIdx - bytesRead);
					} while(bytesRead < cmdByteIdx);

					payloadSize = (respBuffer[2] << 8) | respBuffer[3];

					if((payloadSize + overheadSize) > bytesToRead) {
						std::cout << "incoming message to large. Got " << payloadSize + overheadSize << " expected " << bytesToRead << ". Emptying buffer and trying again." << std::endl;
						SerialBuffer buff(payloadSize + overheadSize);
						do {
							bytesRead += mSerialPort.read(buff.begin() + bytesRead, (payloadSize + overheadSize) - bytesRead);
						} while(bytesRead < (payloadSize + overheadSize));
						attempts++;
					} else {
						haveAckNackMessage = true;
					}

					if(duration_cast<duration<double>>(steady_clock::now() - startTime) > ackNackReceiveTimeout) {
						throw Comm::CommTimeoutException("Timeout receiving ack/nack from GPS");
					}
				}

				do {
					bytesRead += mSerialPort.read<bytesToRead>(respBuffer.begin() + bytesRead, (payloadSize + overheadSize) - bytesRead);
				} while(bytesRead < (payloadSize + overheadSize));

				if((*(respBuffer.begin() + bytesRead - 1) == MSG_END_2) && (*(respBuffer.begin() + bytesRead - 2) == MSG_END_1)) {
					uint8_t checksum = computeChecksum(payloadSize, respBuffer);
					if(checksum != respBuffer[6]) {
						throw GpsFormatException("Incorrect checksum on response message");
					}

					Command commandByte = static_cast<Command>(respBuffer[4]);
					if(Command_ACK == commandByte) {
						return true;
					} else if(Command_NACK == commandByte) {
						return false;
					} else {
						throw GpsFormatException("Unexpected response message");
					}
				} else {
					throw GpsFormatException("Message missing terminating bytes.");
				}

				return haveAckNackMessage;
			}

			uint8_t SkyTraqVenus::computeChecksum(uint16_t payloadLength, SerialBuffer& commandBuffer) {
				uint8_t checksum = 0;

				if((commandBuffer.size() - overheadSize) != payloadLength) {
					throw GpsException("commandBuffer is not the correct size");
				}

				for(uint32_t i = 4; i < (payloadLength + 4u); i++) {
					checksum = checksum ^ commandBuffer[i];
				}
				return checksum;
			}
		}
	}
}
