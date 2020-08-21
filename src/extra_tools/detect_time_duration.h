#pragma once

#include <chrono>

namespace lipaboy_lib::extra {

	using std::chrono::steady_clock;

	decltype(auto) getCurrentTime() { return steady_clock::now(); }
	template <class ratio = std::chrono::milliseconds>
	decltype(auto) diffFromNow(steady_clock::time_point from) {
		return std::chrono::duration_cast<ratio>(steady_clock::now() - from).count();
	}

}