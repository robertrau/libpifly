/*
	Author: Robert F. Rau II
	Copyright (C) 2017 Robert F. Rau II
*/
#ifndef LIBPIFLY_GPS_IGPSPROTOCOL_H
#define LIBPIFLY_GPS_IGPSPROTOCOL_H

namespace PiFly
{
	namespace GPS
	{
		typedef enum
		{
			FixType_NoFix = 1,
			FixType_2D = 2,
			FixType_3D = 3,
			FixType_3DDGNSS = 4,
		} FixType;
		struct GpsResult
		{
			FixType fixType;
			uint8_t satellitesInView;
			uint32_t meanSeaLevel;
			int32_t latitude;
			int32_t longitude;
			uint16_t gdop;
			uint16_t pdop;
			uint16_t hdop;
			uint16_t vdop;
			uint16_t tdop;
		};

		class IGpsProtocol
		{
		public:
			virtual const bool haveResult() const = 0;
			virtual void update() = 0;
			virtual GpsResult getResult() = 0;
		};
	}
}

#endif // LIBPIFLY_GPS_IGPSPROTOCOL_H
