// Boost Python includes explode with -Weffc++ enabled, so we temporarily disable
#pragma GCC diagnostic ignored "-Weffc++"
#include <boost/python.hpp> // NOLINT
#include <boost/python/suite/indexing/vector_indexing_suite.hpp> // NOLINT
#pragma GCC diagnostic pop

#include "comm/serialport.h"
#include "comm/spi/channel.h"
#include "comm/spi/serialperipheralinterface.h"

#include "gps/igpsprotocol.h"
#include "gps/skytraq/skytraqbinaryprotocol.h"

#include "adc/adc.h"

using PiFly::Comm::SerialBuffer;

namespace bp = boost::python;

class SpiChannelWrapper {
	PiFly::Comm::SPI::ChannelPtr mChannel;
public:
	SpiChannelWrapper() = delete;
	SpiChannelWrapper(const SpiChannelWrapper& spiWrapper) = delete;
	SpiChannelWrapper(PiFly::Comm::SPI::ChannelPtr channel) :
		mChannel(std::move(channel))
	{

	}

	bool transfer(SerialBuffer write, SerialBuffer read) {
		if(mChannel) {
			return mChannel->transfer(write, read);
		}
		return false;
	}

	bool transfer(SerialBuffer readWrite) {
		if(mChannel) {
			return mChannel->transfer(readWrite);
		}
		return false;
	}

	PiFly::Comm::SPI::ChannelPtr getChannelPtrOwnership() {
		return std::move(mChannel);
	}
};

void exportSPI() {
	using PiFly::Comm::SerialBuffer;
	using namespace PiFly::Comm::SPI;

	// map the SPI namespace to a sub-module
    // make "from pifly.comm.spi import <whatever>" work
    bp::object spiModule(bp::handle<>(bp::borrowed(PyImport_AddModule("pifly.comm.spi"))));
    // make "from pifly.comm import spi" work
    bp::scope().attr("spi") = spiModule;
    // set the current scope to the new sub-module
    bp::scope spi_scope = spiModule;

	bool (SpiChannelWrapper::*transfer_write_read)(SerialBuffer, SerialBuffer) = &SpiChannelWrapper::transfer;
	bool (SpiChannelWrapper::*transfer_readWrite)(SerialBuffer) = &SpiChannelWrapper::transfer;

    bp::class_<SpiChannelWrapper, boost::noncopyable>("Channel", bp::no_init)
        .def("transfer", transfer_write_read)
		.def("transfer", transfer_readWrite)
    ;

	bp::class_<SerialPeripheralInterface, boost::noncopyable>("SerialPeripheralInterface")
        .def("getChannel",
			bp::make_function(
				[](SerialPeripheralInterface& self, ChipSelect cs, Mode mode, BitOrder bitOrder, ClockDivider clockDivider, ChipSelectPolarity polarity) {
					return new SpiChannelWrapper(std::move(self.getChannel(cs, mode, bitOrder, clockDivider, polarity)));
				},
				bp::return_value_policy<bp::manage_new_object>(),
				boost::mpl::vector<SpiChannelWrapper*, SerialPeripheralInterface&, ChipSelect, Mode, BitOrder, ClockDivider, ChipSelectPolarity>()
			)
		)
    ;

	bp::enum_<BitOrder>("BitOrder")
		.value("LsbFirst", BitOrder_LsbFirst)
		.value("MsbFirst", BitOrder_MsbFirst)
	;

	bp::enum_<Mode>("Mode")
		.value("CPOL0_CPHA0", Mode_CPOL0_CPHA0)
		.value("CPOL0_CPHA1", Mode_CPOL0_CPHA1)
		.value("CPOL1_CPHA0", Mode_CPOL1_CPHA0)
		.value("CPOL1_CPHA1", Mode_CPOL1_CPHA1)
	;

	bp::enum_<ChipSelect>("ChipSelect")
		.value("_0", ChipSelect_0)
		.value("_1", ChipSelect_1)
		.value("_2", ChipSelect_2)
		.value("None", ChipSelect_None)
	;

	bp::enum_<ClockDivider>("ClockDivider")
		.value("_65536", ClockDivider_65536)
		.value("_32768", ClockDivider_32768)
		.value("_16384", ClockDivider_16384)
		.value("_8192", ClockDivider_8192)
		.value("_4096", ClockDivider_4096)
		.value("_2048", ClockDivider_2048)
		.value("_1024", ClockDivider_1024)
		.value("_512", ClockDivider_512)
		.value("_256", ClockDivider_256)
		.value("_128", ClockDivider_128)
		.value("_64", ClockDivider_64)
		.value("_32", ClockDivider_32)
		.value("_16", ClockDivider_16)
		.value("_8", ClockDivider_8)
		.value("_4", ClockDivider_4)
		.value("_2", ClockDivider_2)
		.value("_1", ClockDivider_1)
	;

	bp::enum_<ChipSelectPolarity>("ChipSelectPolarity")
		.value("ActiveLow", ChipSelectPolarity_ActiveLow)
		.value("ActiveHigh", ChipSelectPolarity_ActiveHigh)
	;
}

void exportSerial() {
	using PiFly::Comm::SerialPort;

	bp::class_<SerialBuffer>("SerialBuffer")
        .def(bp::vector_indexing_suite<SerialBuffer>())
	;

	size_t (SerialPort::*read)(SerialBuffer::iterator, size_t) = &SerialPort::read;
	void (SerialPort::*write)(const SerialBuffer&) = &SerialPort::write;
	bp::scope serialport_scope = bp::class_<SerialPort, boost::noncopyable>("SerialPort", bp::init<std::string, SerialPort::Baudrate, bool>())
		.def("read", read)
		.def("write", write)
		.def("flush", &SerialPort::flush)
	;

	bp::enum_<SerialPort::Baudrate>("Baudrate")
		.value("_0", SerialPort::Baudrate_0)
		.value("_50", SerialPort::Baudrate_50)
		.value("_75", SerialPort::Baudrate_75)
		.value("_110", SerialPort::Baudrate_110)
		.value("_134", SerialPort::Baudrate_134)
		.value("_150", SerialPort::Baudrate_150)
		.value("_200", SerialPort::Baudrate_200)
		.value("_300", SerialPort::Baudrate_300)
		.value("_600", SerialPort::Baudrate_600)
		.value("_1200", SerialPort::Baudrate_1200)
		.value("_1800", SerialPort::Baudrate_1800)
		.value("_2400", SerialPort::Baudrate_2400)
		.value("_4800", SerialPort::Baudrate_4800)
		.value("_9600", SerialPort::Baudrate_9600)
		.value("_19200", SerialPort::Baudrate_19200)
		.value("_38400", SerialPort::Baudrate_38400)
		.value("_57600", SerialPort::Baudrate_57600)
		.value("_115200", SerialPort::Baudrate_115200)
		.value("_230400", SerialPort::Baudrate_230400)
	;
}

void exportComm() {
	// map the Comm namespace to a sub-module
    // make "from pifly.comm import <whatever>" work
    bp::object commModule(bp::handle<>(bp::borrowed(PyImport_AddModule("pifly.comm"))));
    // make "from pifly import comm" work
    bp::scope().attr("comm") = commModule;
    // set the current scope to the new sub-module
    bp::scope comm_scope = commModule;

	exportSPI();
	exportSerial();
}

// Disable -Weffc++ because bp::wrapper has no virtual destructor and I can't do anything about that
#pragma GCC diagnostic ignored "-Weffc++"
class IGpsProtocolWrapper : public PiFly::GPS::IGpsProtocol, public bp::wrapper<PiFly::GPS::IGpsProtocol> {
	bool getResult(PiFly::GPS::GpsResult& result) override {
		return this->get_override("getResult")(result);
	}
};
#pragma GCC diagnostic pop

void exportSkyTraq() {
	using namespace PiFly::GPS;
	using PiFly::GPS::Skytraq::SkyTraqBinaryProtocol;
	using PiFly::GPS::Skytraq::SkyTraqVenus;

	// map the Comm namespace to a sub-module
    // make "from pifly.gps.skytraq import <whatever>" work
    bp::object skytraqModule(bp::handle<>(bp::borrowed(PyImport_AddModule("pifly.gps.skytraq"))));
    // make "from pifly.gps import skytraq" work
    bp::scope().attr("skytraq") = skytraqModule;
    // set the current scope to the new sub-module
    bp::scope skytraq_scope = skytraqModule;

	bp::class_<SkyTraqBinaryProtocol, boost::noncopyable>("SkyTraqBinaryProtocol", bp::init<PiFly::Comm::SerialPort&>())
		.def("getResult", &SkyTraqBinaryProtocol::getResult)
	;

	bp::scope skyTraqVenusScope = bp::class_<SkyTraqVenus, boost::noncopyable>("SkyTraqVenus", bp::init<PiFly::Comm::SerialPort&, double>())
		.def("updateBaudrate", &SkyTraqVenus::updateBaudrate)
		.def("setPositionUpdateRate", &SkyTraqVenus::setPositionUpdateRate)
		.def("setMessageType", &SkyTraqVenus::setMessageType)
		.def("autoNegotiateBaudrate", &SkyTraqVenus::autoNegotiateBaudrate)
	;

	bp::enum_<SkyTraqVenus::MessageType>("MessageType")
		.value("None", SkyTraqVenus::MessageType_None)
		.value("NMEA", SkyTraqVenus::MessageType_NMEA)
		.value("Binary", SkyTraqVenus::MessageType_Binary)
	;
}

void exportGPS() {
	using PiFly::GPS::FixType;
	using PiFly::GPS::GpsResult;
	using PiFly::GPS::IGpsProtocol;

	// map the Comm namespace to a sub-module
    // make "from pifly.gps import <whatever>" work
    bp::object gpsModule(bp::handle<>(bp::borrowed(PyImport_AddModule("pifly.gps"))));
    // make "from pifly import gps" work
    bp::scope().attr("gps") = gpsModule;
    // set the current scope to the new sub-module
    bp::scope gps_scope = gpsModule;

	bp::enum_<FixType>("FixType")
		.value("NoFix", PiFly::GPS::FixType_NoFix)
		.value("TwoD", PiFly::GPS::FixType_2D)
		.value("ThreeD", PiFly::GPS::FixType_3D)
		.value("ThreeDDGNSS", PiFly::GPS::FixType_3DDGNSS)
	;

	bp::class_<GpsResult>("GpsResult")
		.def_readonly("fixType", &GpsResult::fixType)
		.def_readonly("satellitesInView", &GpsResult::satellitesInView)
		.def_readonly("GNSSWeek", &GpsResult::GNSSWeek)
		.def_readonly("tow", &GpsResult::tow)
		.def_readonly("meanSeaLevel", &GpsResult::meanSeaLevel)
		.def_readonly("latitude", &GpsResult::latitude)
		.def_readonly("longitude", &GpsResult::longitude)
		.def_readonly("gdop", &GpsResult::gdop)
		.def_readonly("pdop", &GpsResult::pdop)
		.def_readonly("hdop", &GpsResult::hdop)
		.def_readonly("vdop", &GpsResult::vdop)
		.def_readonly("tdop", &GpsResult::tdop)
	;

	bp::class_<IGpsProtocolWrapper, boost::noncopyable>("IGpsProtocol")
		.def("getResult", bp::pure_virtual(&IGpsProtocol::getResult))
	;

	exportSkyTraq();
}

class AdcWrapper {
	PiFly::ADC::AnalogDigitalConverter* mAdc;
public:
	AdcWrapper(const AdcWrapper&) = delete;
	void operator=(const AdcWrapper&) = delete;

	AdcWrapper(SpiChannelWrapper& wrapper, uint16_t channels) :
		mAdc(new PiFly::ADC::AnalogDigitalConverter(wrapper.getChannelPtrOwnership(), channels))
	{
	}

	~AdcWrapper() {
		if(mAdc != nullptr) {
			delete mAdc;
		}
	}

	void update() {
		mAdc->update();
	}

	PiFly::ADC::AnalogInput& getAnalogInput(uint32_t channel) {
		return mAdc->getAnalogInput(channel);
	}
};

void exportADC() {
	using namespace PiFly::ADC;

	// map the Comm namespace to a sub-module
    // make "from pifly.adc import <whatever>" work
    bp::object adcModule(bp::handle<>(bp::borrowed(PyImport_AddModule("pifly.adc"))));
    // make "from pifly import adc" work
    bp::scope().attr("adc") = adcModule;
    // set the current scope to the new sub-module
    bp::scope adc_scope = adcModule;

	bp::class_<AnalogInput, boost::noncopyable>("AnalogInput", bp::no_init)
		.add_property("value", &AnalogInput::value)
	;

	bp::enum_<Channel>("Channel")
		.value("_0", Channel_0)
		.value("_1", Channel_1)
		.value("_2", Channel_2)
		.value("_3", Channel_3)
		.value("_4", Channel_4)
		.value("_5", Channel_5)
		.value("_6", Channel_6)
		.value("_7", Channel_7)
		.value("_8", Channel_8)
		.value("_9", Channel_9)
		.value("_10", Channel_10)
		.value("_11", Channel_11)
		.value("_12", Channel_12)
		.value("_13", Channel_13)
		.value("_14", Channel_14)
		.value("_15", Channel_15)
	;

	bp::class_<AdcWrapper, boost::noncopyable>("AnalogDigitalConverter", bp::init<SpiChannelWrapper&, uint16_t>())
		.def("update", &AdcWrapper::update)
		.def("getAnalogInput", &AdcWrapper::getAnalogInput, bp::return_internal_reference<>())
	;
}

BOOST_PYTHON_MODULE(libpifly_python) { // NOLINT(cppcoreguidelines-pro-bounds-array-to-pointer-decay, cppcoreguidelines-pro-bounds-constant-array-index)
	bp::object package = bp::scope();
    package.attr("__path__") = "pifly";

	exportComm();
	exportGPS();
	exportADC();
}
