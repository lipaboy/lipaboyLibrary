#pragma once

#include <chrono>

namespace lipaboy_lib::extra {

	using std::chrono::steady_clock;

    inline steady_clock::time_point getCurrentTime() { return steady_clock::now(); }

    // return type == typename ratio::rep - is not beautiful
	template <class ratio = std::chrono::milliseconds>
    inline decltype(auto) diffFromNow(steady_clock::time_point from) {
		return std::chrono::duration_cast<ratio>(steady_clock::now() - from).count();
    }

}

