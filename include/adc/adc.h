#ifndef LIBPIFLY_ADC_H
#define LIBPIFLY_ADC_H

#include <string>

using std::string;

namespace PiFly
{
	namespace ADC
	{
		void Init(void);

		class AnalogDigitalConverter
		{
		public:
			AnalogDigitalConverter();
		};
	}
}

#endif // LIBPIFLY_ADC_H

