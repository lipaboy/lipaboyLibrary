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

namespace {
	static int id = 0;
}

// NoisyND - Noisy No notifying of Destruction
struct NoisyND
{
    NoisyND() : id_(++id) { cout << "Constructed " << id_ << endl; }
    NoisyND(NoisyND const & obj) : id_(++id) { cout << "Copy-Constructed " << id_ << endl; }
    NoisyND(NoisyND&& obj) noexcept : id_(++id) { cout << "Move-Constructed " << id_ << endl; }

    const NoisyND& operator= (const NoisyND&) {
        return *this;
    }
    const NoisyND& operator= (const NoisyND&&) noexcept {
        return *this;
    }

	int id_;
};

struct Noisy : 
	virtual NoisyND 
{
	Noisy() {}
	Noisy(Noisy const & obj) : NoisyND(obj) {}
	Noisy(Noisy&& obj) noexcept : NoisyND(std::move(obj)) {}

	bool operator==(Noisy const &) const { return false; }
	bool operator!=(Noisy const & other) const { return !((*this) == other); }

	const Noisy& operator= (const Noisy&) {
		return *this;
	}
	const Noisy& operator= (const Noisy&&) noexcept {
		return *this;
	}

    virtual ~Noisy() { cout << "Destructed " << id_ << endl; }
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
