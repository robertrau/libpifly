/*
	Author: Robert F. Rau II
	Copyright (C) 2017 Robert F. Rau II
*/
#ifndef LIBPIFLY_COMM_H
#define LIBPIFLY_COMM_H

#include <array>
#include <vector>

namespace PiFly {
	namespace Comm {
		typedef std::vector<uint8_t> SerialBuffer;
		template<size_t size>
		using SerialArray = std::array<uint8_t, size>;
	}
}

#endif // LIBPIFLY_COMM_H
