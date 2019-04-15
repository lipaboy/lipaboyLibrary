#include "gtest/gtest.h"
#include <iostream>
#include <vector>
#include <string>

#include "nested_iterator_tests.h"

namespace stream_tests {

	using namespace lipaboy_lib;

	using std::cout;
	using std::endl;
	using std::vector;
	using std::string;

	using lipaboy_lib::NestedIterator;
	using ContainerType = std::list<vector<int> >;


	TEST(NestedIteratorTest, list_of_equal_by_length_containers) {
		ContainerType listOfVectors;
		listOfVectors.push_back(vector<int>({ 1, 2, 3 }));
		listOfVectors.push_back(vector<int>({ 4, 5, 6 }));
		listOfVectors.push_back(vector<int>({ 7, 8, 9 }));
		auto iter = NestedIterator<ContainerType>::begin(listOfVectors);
		auto endIter = NestedIterator<ContainerType>::end(listOfVectors);

		for (int i = 1; iter != endIter; iter++, i++) {
			ASSERT_EQ(*iter, i);
		}
	}

	TEST(NestedIteratorTest, list_of_unequal_by_length_containers) {
		ContainerType listOfVectors;
		listOfVectors.push_back(vector<int>({ 1 }));
		listOfVectors.push_back(vector<int>({ 2, 3 }));
		listOfVectors.push_back(vector<int>({ 4, 5, 6 }));
		auto iter = NestedIterator<ContainerType>::begin(listOfVectors);
		auto endIter = NestedIterator<ContainerType>::end(listOfVectors);

		for (int i = 1; iter != endIter; iter++, i++) {
			ASSERT_EQ(*iter, i);
		}
	}

	TEST(NestedIteratorTest, std_advance) {
		ContainerType listOfVectors;

		listOfVectors.push_back(vector<int>({ 1 }));
		listOfVectors.push_back(vector<int>({ 2, 3 }));
		listOfVectors.push_back(vector<int>({ 4, 5, 6 }));

		auto iter = NestedIterator<ContainerType>::begin(listOfVectors);
		std::advance(iter, 4);
		ASSERT_EQ(*iter, 5);
	}

	TEST(NestedIteratorTest, advance) {
		ContainerType listOfVectors;

		listOfVectors.push_back(vector<int>({ 1 }));
		listOfVectors.push_back(vector<int>({ 2, 3 }));
		listOfVectors.push_back(vector<int>({ 4, 5, 6 }));

		auto iter = NestedIterator<ContainerType>::begin(listOfVectors);
		iter.advance(4);
		ASSERT_EQ(*iter, 5);
		iter.advance(-4);
		ASSERT_EQ(*iter, 1);
		iter.advance(5);
		ASSERT_EQ(*iter, 6);
		iter.advance(-2);
		ASSERT_EQ(*iter, 4);
		iter.advance(-1);
		ASSERT_EQ(*iter, 3);
	}



}


