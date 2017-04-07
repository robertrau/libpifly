#include "gps/gps.h"

#include <iostream>
#include <sstream>
#include <thread>

namespace PiFly
{
	namespace GPS
	{
		using std::thread;
		using std::exception;
		using std::stringstream;

		GlobalPositioningSystem::GlobalPositioningSystem(SerialPort& serialPort) :
			mSerialPort(serialPort)
		{
			auto currentBaudrate = mSerialPort.getBaudrate();
			autoNegotiateBaudrate();
			updateBaudrate(currentBaudrate);
			//updateBaudrate(SerialPort::Baudrate_115200);
		}

		GlobalPositioningSystem::~GlobalPositioningSystem()
		{

		}

		void GlobalPositioningSystem::autoNegotiateBaudrate()
		{
			const size_t MAX_BAUDS = 6;
			SerialPort::Baudrate baudrates[MAX_BAUDS] = {
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
				}

				if(currentBaud < MAX_BAUDS)
				{
					currentBaud++;
				}
				else if(currentBaud == MAX_BAUDS)
				{
					outOfBauds = true;
				}
			}

			if(outOfBauds)
			{
				throw GpsException("Unable to auto negotiate baudrate");
			}

			if(foundBaud)
			{
				std::cout << "Auto negotiated a baudrate of " << SerialPort::baudrateString(baudrates[currentBaud-1]) << std::endl;
			}
		}

		void GlobalPositioningSystem::start()
		{

		}

		ResultVector GlobalPositioningSystem::getLatestSamples()
		{
			ResultVector resVec;

			SerialPort::SerialBuffer buff(512);
			size_t bytesRead = mSerialPort.read(buff, buff.size());
			buff.resize(bytesRead);
			std::cout << buff.data();
			
			return resVec;
		}

		void GlobalPositioningSystem::updateBaudrate(SerialPort::Baudrate baud)
		{
			SerialBuffer command(11);
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
				throw GpsException("Unsupported baudrate");
			}

			uint16_t payloadLength = 4;
			command[0] = MSG_START_1;
			command[1] = MSG_START_2;
			command[2] = payloadLength >> 8;
			command[3] = payloadLength;
			command[4] = Command_ConfigSerial;
			command[5] = 0; // COM 1
			command[6] = gpsBaud;
			command[7] = 0; // Update to SRAM
			command[8] = computeChecksum(payloadLength, command);
			command[9] = MSG_END_1;
			command[10] = MSG_END_2;

			sendCommand(command);

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
				mSerialPort.setBaudrate(baud);
				mSerialPort.flush();
				sendCommand(command);
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

		void GlobalPositioningSystem::sendCommand(const SerialBuffer& command)
		{
			mSerialPort.write(command);
		}

		bool GlobalPositioningSystem::receiveAckNack()
		{
			SerialBuffer respBuffer(9);
			size_t bytesToRead = 9;
			size_t bytesRead = 0;
			do
			{
				bytesRead += mSerialPort.read(respBuffer, bytesToRead - bytesRead);
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

		uint8_t GlobalPositioningSystem::computeChecksum(uint16_t payloadLength, SerialBuffer& commandBuffer)
		{
			uint8_t checksum = 0;

			if((commandBuffer.size() - 7) != payloadLength)
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
