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

			const static size_t updateBufferSize = 512;
			const uint8_t MSG_START_1 = 0xA0;
			const uint8_t MSG_START_2 = 0xA1;
			const uint8_t MSG_END_1 = 0x0D;
			const uint8_t MSG_END_2 = 0x0A;
			typedef enum
			{
				Command_ConfigSerial = 0x5,
				Command_ConfigNMEA = 0x8,
				Command_ConfigNavInterval = 0x11,
				Command_ACK = 0x83,
				Command_NACK = 0x84
			} Command;

			static uint8_t computeChecksum(uint16_t payloadLength, SerialBuffer& buffer);

			void updateBaudrate(SerialPort::Baudrate baud);
			void sendCommand(const SerialBuffer& command);
			bool receiveAckNack();

			void autoNegotiateBaudrate();

			SerialPort& mSerialPort;
		};
	}
}

#endif // LIBPIFLY_GPS_SKYTRAQBINARYPROTOCOL_H
