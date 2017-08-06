#ifndef BIT_CONTAINER_H
#define BIT_CONTAINER_H

#include <vector>
#include <algorithm>
#include <initializer_list>

#include <boost\range\any_range.hpp>

namespace LipaboyLib {

	typedef boost::any_range<
		int,
		boost::bidirectional_traversal_tag,
		int,
		std::ptrdiff_t
	> IntAnyRange;

	//TODO: think about generalizing true and false (etc. #define true BIT, false ZERO, but without define)
	typedef bool BitType;

	template <typename T>
	inline T addBitOne(T number, uint32_t position) { return number | (1u << position); }
	template <typename T>
	inline void setBitOne(T& number, uint32_t position) { number |= (1u << position); }
	// [ (sign * max) xor (((sign * max) xor number) | (1 << pos)) ] == [ (bit == 0) ? ~(~number | (1 << pos)) : number | (1 << pos) ]
	template <typename T>
	inline void setBit(T& number, uint32_t position, BitType bit) { 
		number = ((!bit) * (~static_cast<T>(0))) ^ (
			(
				(
					(!bit) * (~static_cast<T>(0))
				) ^ number
			) | (1 << position)
		); 
	}

	template <typename T = uint32_t, typename IndexType = uint32_t>
	class BitContainer {
	public:
		BitContainer() : container(), _size(0) {}
		BitContainer(std::initializer_list<BitType> initList);

		BitType get(IndexType index) const { 
			return container[index / bitsPerElem] & (static_cast<T>(1) << (index % bitsPerElem));
		}
		//TODO: I feel so upset that I need to write methods such as vector's
		void pushBack(BitType newElem) { ((++_size) % bitsPerElem == 1) 
			? container.push_back(static_cast<T>(newElem)) : set(_size - 1, newElem); }
		void set(IndexType index, BitType bit) {
			setBit(container[index / bitsPerElem], index % bitsPerElem, bit);
		}

		IndexType size() const { return _size; }

	private:
		//void setBitOne(IndexType index) { container[index / bitsPerElem] |= (static_cast<T>(1) << (index % bitsPerElem)); }
	private:
		std::vector<T> container;
		IndexType _size;
		//TODO: replate bitsPerElem on sizeof(T) * 8
		static const uint32_t bitsPerElem = sizeof(T) * 8;	//count bytes * count bits in one byte
	};

	template<typename T, typename IndexType>
	inline BitContainer<T, IndexType>::BitContainer(std::initializer_list<BitType> initList)
		: container(1 + (initList.size() - 1) / bitsPerElem), _size(initList.size())
	{
		IndexType i = 0;
		IndexType currInd = -1;
		for (std::initializer_list<BitType>::iterator it = initList.begin(); it != initList.end(); 
				it++, i++) 
		{	
			if (i % bitsPerElem == 0)	//for uint32_t it is 32
				currInd++;

			if (*it == true)
				setBitOne(container[currInd], i % bitsPerElem);
		}
	}

}

#endif	//BIT_CONTAINER_H
