#include <algorithm>
#include <iterator>
#include <vector>

//#include "big_unsigned.h"

namespace lipaboy_lib {

	namespace long_numbers_space {

		// TODO: remove useless dependencies

		using std::vector;

		class BigUnsignedView {
		public:
			using BlockType = unsigned long;
			using ContainerType = std::vector<BlockType>;
			using IteratorType = typename ContainerType::const_iterator;
			using size_type = size_t;
			using ContainerTypePtr = const ContainerType * ;

		public:
			BigUnsignedView(ContainerType const & container) 
				: begin_(container.cbegin()), 
				end_(container.cend()),
				pContainer_(&container)
			{}
			BigUnsignedView(IteratorType begin, IteratorType end, ContainerType const & container) 
				: begin_(begin), 
				end_(end),
				pContainer_(&container)
			{}

			IteratorType begin() const { return begin_; }
			IteratorType end() const { return end_; }

			BlockType getBlockDirty(size_type index) const { return *std::next(begin_, index); }
			BlockType getBlock(size_type index) const { 
				return (index < static_cast<size_type>(std::distance(begin_, end_))) ? *std::next(begin_, index) : 0; 
			}
			size_type length() const { return static_cast<size_type>(std::distance(begin_, end_)); }
			bool isZero() const { return length() <= 1 && *begin_ == 0; }
			ContainerType const & container() { return *pContainer_; }

		private:
			IteratorType begin_;
			IteratorType end_;
			ContainerTypePtr pContainer_;
		};

	}

}