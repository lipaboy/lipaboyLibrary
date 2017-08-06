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

	typedef bool BitType;

	template <typename T = uint32_t, typename IndexType = uint32_t>
	class BitContainer {
	public:
		BitContainer(std::initializer_list<BitType> initList);
		//TODO: add in constexpr sizeof(T) * 8
		BitType get(IndexType index) const { 
			return container[index / sizeof(T) / 8] & (1 << (index % (sizeof(T) * 8))); 
		}

		IndexType size() const { return _size; }

	private:
		std::vector<T> container;
		IndexType _size;
	};

	template<typename T, typename IndexType>
	inline BitContainer<T, IndexType>::BitContainer(std::initializer_list<BitType> initList)
		: container((initList.size() - 1) / sizeof(T) / 8 + 1), _size(initList.size())
	{
		IndexType i = 0;
		IndexType step = sizeof(T) * 8;
		IndexType currInd = -1;
		for (std::initializer_list<BitType>::iterator it = initList.begin(); it != initList.end(); 
				it++, i++) 
		{	
			if (i % step == 0)	//for uint32_t it is 32
				currInd++;

			if (*it == true)
				container[currInd] |= 1 << (i % step);
		}
	}

}

#endif	//BIT_CONTAINER_H
