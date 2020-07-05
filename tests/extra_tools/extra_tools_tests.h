#ifndef EXTRA_TOOLS_TESTS_H
#define EXTRA_TOOLS_TESTS_H

#include "gtest/gtest.h"
#include <iostream>
#include <functional>

namespace lipaboy_lib_tests {

using std::cout;
using std::endl;

struct NoisyHash {};

struct NoisyCon : NoisyHash {
    NoisyCon() { cout << "Constructed" << endl; }
    NoisyCon(NoisyCon const &) {}
    NoisyCon(NoisyCon &&) noexcept {}
};

struct NoisyCopy : NoisyHash {
    NoisyCopy() {}
    NoisyCopy(NoisyCopy const &) { cout << "Copy-Constructed" << endl; }
};

struct NoisyMove : NoisyHash {
    NoisyMove() {}
    NoisyMove(NoisyMove &&) noexcept { cout << "Move-Constructed" << endl; }
};

// NoisyD - Noisy without notifying of destruction
struct NoisyD : 
	virtual NoisyCon, 
	virtual NoisyCopy, 
	virtual NoisyMove 
{
    NoisyD() {}
    NoisyD(NoisyD const & obj) : NoisyCopy(obj), NoisyCon(obj) {}
    NoisyD(NoisyD&& obj) noexcept : NoisyMove(std::move(obj)), NoisyCon(std::move(obj)) {}

    const NoisyD& operator= (const NoisyD&) {
        return *this;
    }
    const NoisyD& operator= (const NoisyD&&) noexcept {
        return *this;
    }
};

struct Noisy : NoisyD {
	Noisy() {}
	Noisy(Noisy const & obj) : NoisyD(obj) {}
	Noisy(Noisy&& obj) noexcept : NoisyD(std::move(obj)) {}

	bool operator==(Noisy const &) const { return false; }
	bool operator!=(Noisy const & other) const { return !((*this) == other); }

	const Noisy& operator= (const Noisy&) {
		return *this;
	}
	const Noisy& operator= (const Noisy&&) noexcept {
		return *this;
	}

    virtual ~Noisy() { cout << "Destructed" << endl; }
};

}

// std::hash specialization for NoisyHash
namespace std
{
	template<> 
	struct hash<lipaboy_lib_tests::NoisyHash>
	{
		using argument_type = lipaboy_lib_tests::NoisyHash;
		using result_type = std::size_t;

		result_type operator()(argument_type const&) const noexcept
		{
			return std::hash<int>{}(0);
		}
	};

	template<>
	struct hash<lipaboy_lib_tests::Noisy>
	{
		using argument_type = lipaboy_lib_tests::Noisy;
		using result_type = std::size_t;

		result_type operator()(argument_type const&) const noexcept
		{
			return std::hash<int>{}(0);
		}
	};
}

#endif // EXTRA_TOOLS_TESTS_H
