#include <gtest/gtest.h>
#include <iostream>
#include <array>
#include <time.h>

#include "containers/bit_vector.h"


namespace lipaboy_lib_tests {

	using lipaboy_lib::BitVector;

	TEST(BitVector, DISABLED_benchmark) {
		//TODO: Want I want to do: compare perfomance of vector<int>[i] and bitcon[i] (or maybe do it later?)

		constexpr int d = sizeof(int) * 8;	//It proves that sizeof(int) - compile-time function
		BitVector<> bitcon;
		std::vector<uint32_t> vect;

#define testSize static_cast<size_t>(2e3)
		clock_t start,
			end;
		//uint32_t arr[testSize] = { 0 };
		std::array<uint32_t, testSize> arr = { 0 };

		//Test 1 : push

		start = clock();
		for (int i = 0; i < testSize; i++)
			vect.push_back(rand() % 2);
		end = clock();
		std::cout << "vect time push: " << end - start << std::endl;

		start = clock();
		for (int i = 0; i < testSize; i++)
			bitcon.pushBack(rand() % 2);
		end = clock();
		std::cout << "bitcon time push: " << end - start << std::endl;

		//Test 2 : read
		int sum;

		start = clock();
		sum = 0;
		for (uint32_t i = 0; i < bitcon.size(); i++)
			sum += bitcon.get(i);
		end = clock();
		std::cout << "bitcon time read: " << end - start << std::endl;

		start = clock();
		sum = 0;
		for (uint32_t i = 0; i < vect.size(); i++)
			sum += vect[i];
		end = clock();
		std::cout << "vect time read: " << end - start << std::endl;

		start = clock();
		sum = 0;
		for (uint32_t i = 0; i < vect.size(); i++)
			sum += arr[i];
		end = clock();
		std::cout << "arr time read: " << end - start << std::endl;

		//Test 3 : write

		start = clock();
		for (uint32_t i = 0; i < bitcon.size(); i++)
			bitcon.set(i, rand() % 2);
		end = clock();
		std::cout << "bitcon time write: " << end - start << std::endl;

		start = clock();
		for (uint32_t i = 0; i < vect.size(); i++)
			vect[i] = rand() % 2;
		end = clock();
		std::cout << "vect time write: " << end - start << std::endl;

		start = clock();
		for (uint32_t i = 0; i < vect.size(); i++)
			arr[i] = rand() % 2;
		end = clock();
		std::cout << "arr time write: " << end - start << std::endl;


	}

}

