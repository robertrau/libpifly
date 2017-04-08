#ifndef LIBPIFLY_GPS_IGPSPROTOCOL_H
#define LIBPIFLY_GPS_IGPSPROTOCOL_H

namespace PiFly
{
	namespace GPS
	{
		struct GpsResult
		{

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
